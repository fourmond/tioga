/**********************************************************************

   Dvector.c
   based on Ruby's array.c (Copyright (C) Yukihiro Matsumoto)
   specialized for 1D arrays that only contain doubles
   
   Copyright (C) 2005  Bill Paxton

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Library Public License as published
   by the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.
   
   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU Library General Public License for more details.
   
   You should have received a copy of the GNU Library General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
   
**********************************************************************/

#include <stdbool.h>
#include <stdlib.h>
#include <ctype.h>
#include <math.h>
#include "ruby.h"
#include "dvector_intern.h"


/* Internal files that are defined in the ext/includes directory */
#include <symbols.h>
#include <symbols.c>

/* compiler-dependent definitions, such as is_okay_number */
#include <defs.h>

/* safe storing of doubles */
#include <safe_double.h>

/* End of internal files */

#define is_a_dvector(d) ( TYPE(d) == T_DATA && RDATA(d)->dfree == (RUBY_DATA_FUNC)dvector_free )

#ifndef MAX
#define MAX(a,b)    (((a) > (b)) ? (a) : (b))
#endif
#ifndef MIN
#define MIN(a,b)    (((a) < (b)) ? (a) : (b))
#endif
#ifndef SIGN
#define SIGN(a)    (((a) > 0) ? 1 : -1)
#endif



typedef struct {
   long len; /* current number of doubles in this vector */
   long capa; /* current capacity which must be >= len */
   VALUE shared; /* if not Qnil, then is the base object we're sharing with others */
   double *ptr; /* the data */
   int dirty; 	/* set to 1 if data has been modified since the last time
		   it was cleared
		*/
} Dvector;

static VALUE dvector_make_shared(VALUE ary);
static VALUE dvector_alloc(VALUE klass);
static VALUE dvector_s_create(int argc, VALUE *argv, VALUE klass);
static double *dvector_replace_dbls(VALUE ary, long len, double *data);
static void dvector_store(VALUE ary, long idx, VALUE val);
static VALUE dvector_to_dvector(VALUE obj);
static Dvector *Get_Dvector(VALUE obj);
static Dvector *dvector_modify(VALUE ary);
static void dvector_splice(VALUE ary, long beg, long len, VALUE rpl);
static void dvector_mark(Dvector *d);
static void dvector_free(Dvector *d);

#define DVEC_DEFAULT_SIZE 16

PRIVATE bool Is_Dvector(VALUE obj) { return is_a_dvector(obj); }

PRIVATE 
/* Checks if the given object is a Dvector. Mainly here for testing
   purposes, as it corresponds to the internal +is_a_dvector+.
*/
VALUE dvector_is_a_dvector(VALUE self, VALUE obj)
{
  if(Is_Dvector(obj))
    return Qtrue;
  return Qfalse;
}

static inline void dvector_mem_clear(double *mem, int size) {
   while (size--) {
      *mem++ = 0.0;
   }
}

static inline void dvector_memfill(double *mem, long size, double val) {
   while (size--) {
      *mem++ = val;
   }
}

static VALUE cDvector; /* the Dvector class object */

static void dvector_mark(Dvector *d) {
   if (d->shared != Qnil) rb_gc_mark(d->shared);
}

static void dvector_free(Dvector *d) {
   if (d->ptr != NULL && d->shared == Qnil) free(d->ptr);
   free(d);
}

static VALUE dvector_alloc(VALUE klass) {
   Dvector *d;
   VALUE ary = Data_Make_Struct(klass, Dvector, dvector_mark, dvector_free, d);
   d->len = 0;
   d->ptr = NULL;
   d->capa = 0;
   d->shared = Qnil;
   return ary;
}

static void ary_to_dvector(VALUE *data, long len, double *p) {
   long i;
   VALUE v;
   for (i = 0; i < len; i++) {
      v = rb_Float(data[i]);
      p[i] = NUM2DBL(v);
   }
}

/* 
* Returns a new Dvector populated with the given objects. 
*
*   Dvector.[]( 1, 2, 3, 4 )      -> a_dvector
*   Dvector[ 1, 2, 3, 4 ]         -> a_dvector
*
*/

static VALUE dvector_s_create(int argc, VALUE *argv, VALUE klass) {
   VALUE ary = make_new_dvector(klass, argc, argc);
   Dvector *d = Get_Dvector(ary);
   if (argc < 0) {
      rb_raise(rb_eArgError, "negative number of arguments");
   }
   ary_to_dvector(argv, argc, d->ptr);
   return ary;
}

void Dvector_Store_Double(VALUE ary, long idx, double val) {
   Dvector *d;
   d = dvector_modify(ary);
   if (idx < 0) {
      idx += d->len;
      if (idx < 0) {
         rb_raise(rb_eIndexError, "index %ld out of array", idx - d->len);
      }
   }
   if (idx >= d->capa) {
      long new_capa = d->capa / 2;
      if (new_capa < DVEC_DEFAULT_SIZE) {
         new_capa = DVEC_DEFAULT_SIZE;
      }
      new_capa += idx;
      REALLOC_N(d->ptr, double, new_capa);
      d->capa = new_capa;
   }
   if (idx > d->len) {
      dvector_mem_clear(d->ptr + d->len, idx - d->len + 1);
   }
   if (idx >= d->len) {
      d->len = idx + 1;
   }
   d->ptr[idx] = val;
}

static void dvector_store(VALUE ary, long idx, VALUE val) {
   val = rb_Float(val);
   Dvector_Store_Double(ary, idx, NUM2DBL(val));
}

static VALUE dvector_to_dvector(VALUE obj) {
   if ( is_a_dvector(obj) ) return obj;
   obj = rb_Array(obj);
   return dvector_s_create(RARRAY_LEN(obj), RARRAY_PTR(obj), cDvector);
}

static Dvector *Get_Dvector(VALUE obj) {
   Dvector *d;
   obj = dvector_to_dvector(obj);
   Data_Get_Struct(obj, Dvector, d);
   return d;
}

#define DVEC_TMPLOCK  FL_USER1

static inline void dvector_modify_check(VALUE ary) {
   if (OBJ_FROZEN(ary)) rb_error_frozen("dvector");
   if (FL_TEST(ary, DVEC_TMPLOCK))
      rb_raise(rb_eRuntimeError, "can't modify dvector during iteration");
   if (!OBJ_TAINTED(ary) && rb_safe_level() >= 4)
      rb_raise(rb_eSecurityError, "Insecure: can't modify dvector");
}

static Dvector *dvector_modify(VALUE ary) {
   double *ptr;
   Dvector *d;
   dvector_modify_check(ary);
   d = Get_Dvector(ary);
   /* we set the dirty bit */
   d->dirty = 1;
   if (d->shared != Qnil) {
      ptr = ALLOC_N(double, d->len);
      d->shared = Qnil;
      d->capa = d->len;
      MEMCPY(ptr, d->ptr, double, d->len);
      d->ptr = ptr;
   }
   if (d->ptr == NULL) {
      d->ptr = ALLOC_N(double, DVEC_DEFAULT_SIZE);
      d->capa = DVEC_DEFAULT_SIZE;
   }
   return d;
}

PRIVATE
/*
 *  call-seq:
 *    dvector.dirty? -> _true_ or _false_
 *
 *  Returns _true_ if the vector has been modified since the last time
 *  dirty was cleared. When a Dvector is created or copied, dirty is set
 *  to false. It is set to true whenever the vector is modified. You need
 *  to reset it manually using dirty=.
 */ VALUE dvector_is_dirty(VALUE ary) {
  Dvector *d;
  d = Get_Dvector(ary);
  if(d->dirty)
    return Qtrue;
  else
    return Qfalse;
}

PRIVATE
/*
 *  call-seq:
 *    dvector.clean? -> _true_ or _false_
 *
 *  Returns _true_ if the vector hasn't been modified since the last time
 *  dirty was cleared. See dirty?.
 */ VALUE dvector_is_clean(VALUE ary) {
  if(RTEST(dvector_is_dirty(ary)))
    return Qfalse;
  return Qtrue;
}


PRIVATE
/*
 *  call-seq:
 *    dvector.dirty= _true_ or _false_ -> dvector
 *
 *  Sets (or unsets) the _dirty_ flag. Returns _dvector_.
 */ VALUE dvector_set_dirty(VALUE ary, VALUE b) {
  Dvector *d;
  d = Get_Dvector(ary);
  d->dirty = RTEST(b);
  return ary;
}

PRIVATE
/*
 *  call-seq:
 *     dvector.freeze  -> _dvector_
 *
 *  Prevents further modifications.  A TypeError will be raised if modification is attempted.
 */ VALUE dvector_freeze(VALUE ary) {
   return rb_obj_freeze(ary);
}

PRIVATE
/*
 *  call-seq:
 *     dvector.frozen?  -> true or false
 *
 *  Return <code>true</code> if this vector is frozen (or temporarily frozen
 *  while being sorted).
 */ VALUE dvector_frozen_p(VALUE ary) {
   if (OBJ_FROZEN(ary)) return Qtrue;
   if (FL_TEST(ary, DVEC_TMPLOCK)) return Qtrue;
   return Qfalse;
}

PRIVATE VALUE make_new_dvector(VALUE klass, long len, long capa) {
   VALUE ary = dvector_alloc(klass);
   Dvector *d = Get_Dvector(ary);
   if (len < 0) {
      rb_raise(rb_eArgError, "negative dvector size (or size too big)");
   }
   d->len = len;
   if (len == 0) len++;
   if (capa < len) capa = len;
   d->ptr = ALLOC_N(double, capa);
   d->capa = capa;
   dvector_mem_clear(d->ptr, capa);
   /* we set dirty to 0 */
   d->dirty = 0;
   return ary;
}

/* Makes a Dvector with the given data. No additional capacity. */
PRIVATE VALUE make_dvector_from_data(VALUE klass, long len, double * data) {
  VALUE ary = dvector_alloc(klass);
  Dvector *d = Get_Dvector(ary);
  if (len < 0) {
    rb_raise(rb_eArgError, "negative dvector size (or size too big)");
  }
  d->len = len;
  if (len == 0) len++;
  d->ptr = ALLOC_N(double, len);
  MEMCPY(d->ptr, data, double, len);
  d->capa = len;
  /* we set dirty to 0 */
  d->dirty = 0;
  return ary;
}

PRIVATE VALUE dvector_new2(long len, long capa) {
   return make_new_dvector(cDvector, len, capa);
}

PRIVATE VALUE dvector_new() {
   return dvector_new2(0, DVEC_DEFAULT_SIZE);
   
}

PRIVATE VALUE dvector_new4(long len, VALUE *data) {
   VALUE ary = make_new_dvector(cDvector, len, len);
   Dvector *d = Get_Dvector(ary);
   if (d->len < len) {
      Dvector_Store_Double(ary, len-1, 0.0);
   }
   ary_to_dvector(data, len, d->ptr);
   return ary;
}

PRIVATE VALUE dvector_new4_dbl(long len, double *data) {
   VALUE ary = make_new_dvector(cDvector, len, len);
   Dvector *d = Get_Dvector(ary);
   if (d->len < len) {
      Dvector_Store_Double(ary, len-1, 0.0);
   }
   MEMCPY(d->ptr, data, double, len);
   return ary;
}

PRIVATE VALUE dvector_check_array_type(VALUE ary) {
   if ( is_a_dvector(ary) ) return ary;
   return rb_check_convert_type(ary, T_ARRAY, "Array", "to_ary");
}

PRIVATE
/*
 *  call-seq:
 *     Dvector.new                          -> a_dvector
 *     Dvector.new(size=0, value=0)         -> a_dvector
 *     Dvector.new(other)                   -> a_dvector
 *     Dvector.new(size) {|index| block }   -> a_dvector
 *
 *  Returns a new Dvector. In the first form, the new vector is
 *  empty. In the second it is created with _size_ copies of _value_
 *  The third form creates a copy of the _other_ vector
 *  passed as a parameter (this can also be an Array).
 *  In the last form, a vector of the given size is created. Each element in this vector is
 *  calculated by passing the element's index to the given block and
 *  storing the return value.
 *
 *     Dvector.new                       -> Dvector[]
 *     Dvector.new(2)                    -> Dvector[ 0, 0 ]
 *     Dvector.new(3, -1)                -> Dvector[ -1, -1, -1 ]
 *     Dvector.new(3) {|i| i**2 + 1}     -> Dvector[ 1, 2, 5 ]
 * 
 */ VALUE dvector_initialize(int argc, VALUE *argv, VALUE ary) {
   long len;
   VALUE size, val;
   Dvector *d = dvector_modify(ary);
   if (rb_scan_args(argc, argv, "02", &size, &val) == 0) {
      d->len = 0;
      if (rb_block_given_p()) {
         rb_warning("given block not used");
      }
      return ary;
   }
   if (argc == 1 && !FIXNUM_P(size)) {
      val = dvector_check_array_type(size);
      if (!NIL_P(val)) {
         dvector_replace(ary, val);
         return ary;
      }
   }
   len = NUM2LONG(size);
   if (len < 0) {
      rb_raise(rb_eArgError, "negative array size");
   }
   if (len > 0 && len * (long)sizeof(VALUE) <= len) {
      rb_raise(rb_eArgError, "array size too big");
   }
   if (len > d->capa) {
      REALLOC_N(d->ptr, double, len);
      d->capa = len;
   }
   if (rb_block_given_p()) {
      long i;
      if (argc == 2) {
         rb_warn("block supersedes default value argument");
      }
      for (i=0; i<len; i++) {
         dvector_store(ary, i, rb_yield(LONG2NUM(i)));
         d->len = i + 1;
      }
   } else if (val == Qnil) {
      dvector_memfill(d->ptr, len, 0.0);
      d->len = len;
   } else {
      val = rb_Float(val);
      dvector_memfill(d->ptr, len, NUM2DBL(val));
      d->len = len;
   }
   /* we ensure that the vector is clean on exit of initialize */
   d->dirty = 0;
   return ary;
}

PRIVATE void Dvector_Push_Double(VALUE ary, double val) {
   Dvector *d = Get_Dvector(ary);
   Dvector_Store_Double(ary, d->len, val);
}

PRIVATE
/*
 *  call-seq:
 *     dvector << number  -> dvector
 *  
 *  Append---Pushes the given number on to the end of this vector. This
 *  expression returns the vector itself, so several appends
 *  may be chained together.
 *
 *     Dvector[ 1, 2 ] << -3.3 << 1e3    ->  Dvector[ 1, 2, 3.3, 1000.0 ]
 *
 */ VALUE dvector_push(VALUE ary, VALUE item) {
   item = rb_Float(item);
   Dvector_Push_Double(ary, NUM2DBL(item));
   return ary;
}

PRIVATE
/* 
 *  call-seq:
 *     dvector.push(number, ... )  -> dvector
 *  
 *  Append---Pushes the given number(s) on to the end of this vector.
 *
 *     a = Dvector[ 1, 2, 3 ]
 *     a.push(4, 5, 6)  -> Dvector[1, 2, 3, 4, 5, 6]
 */ VALUE dvector_push_m(int argc, VALUE *argv, VALUE ary) {
   while (argc--) {
      dvector_push(ary, *argv++);
   }
   return ary;
}

PRIVATE
/*
 *  call-seq:
 *     dvector.pop -> number or nil
 *  
 *  Removes the last element from _dvector_ and returns it, or
 *  <code>nil</code> if the vector is empty.
 *     
 *     a = Dvector[ 1, 2, 3 ]
 *     a.pop   -> 3
 *     a       -> Dvector[ 1, 2 ]
 */ VALUE dvector_pop(VALUE ary) {
   Dvector *d = dvector_modify(ary);
   if (d->len == 0) return Qnil;
   if (d->shared == Qnil && d->len * 2 < d->capa && d->capa > DVEC_DEFAULT_SIZE) {
      d->capa = d->len * 2;
      REALLOC_N(d->ptr, double, d->capa);
   }
   return rb_float_new(d->ptr[--d->len]);
}

static VALUE dvector_make_shared(VALUE ary) {
   Dvector *d = Get_Dvector(ary);
   if (d->shared != Qnil) return d->shared;
   VALUE shared = dvector_alloc(cDvector);
   Dvector *s = Get_Dvector(shared);
   s->len = d->len;
   s->ptr = d->ptr;
   s->capa = d->capa;
   d->shared = shared;
   OBJ_FREEZE(shared);
   return shared;
}

PRIVATE
/*
 *  call-seq:
 *     dvector.shift   ->   number or nil
 *  
 *  Returns the first element of _dvector_ and removes it (shifting all
 *  other elements down by one). Returns <code>nil</code> if the vector
 *  is empty.
 *     
 *     args = Dvector[ 1, 2, 3 ]
 *     args.shift   -> 1
 *     args         -> Dvector[ 2, 3 ]
 */ VALUE dvector_shift(VALUE ary) {
   double top;
   Dvector *d = dvector_modify(ary);
   if (d->len == 0) return Qnil;
   top = d->ptr[0];
   dvector_make_shared(ary);
   d->ptr++;		/* shift ptr */
   d->len--;
   return rb_float_new(top);
}

PRIVATE
/*
 *  call-seq:
 *     dvector.unshift(number, ...)  -> dvector
 *  
 *  Prepends objects to the front of _dvector_, moving other elements up one.
 *     
 *     a = [ 2, 3, 4 ]
 *     a.unshift(1)      -> Dvector[ 1, 2, 3, 4 ]
 *     a.unshift(-1, 0)  -> Dvector[ -1, 0, 1, 2, 3, 4 ]
 */ VALUE dvector_unshift_m(int argc, VALUE *argv, VALUE ary) {
   Dvector *d = Get_Dvector(ary);
   long len = d->len, i;
   if (argc < 0) {
      rb_raise(rb_eArgError, "negative number of arguments");
   }
   if (argc == 0) return ary;
   /* make room by setting the last item */
   Dvector_Store_Double(ary, len + argc - 1, 0.0);
   /* slide items */
   MEMMOVE(d->ptr + argc, d->ptr, double, len);
   for (i=0; i < argc; i++)
      dvector_store(ary, i, argv[i]);
   return ary;
}

PRIVATE VALUE dvector_entry(VALUE ary, long offset) {
   Dvector *d = Get_Dvector(ary);
   if (d->len == 0) return Qnil;
   if (offset < 0) {
      offset += d->len;
   }
   if (offset < 0 || d->len <= offset) {
      return Qnil;
   }
   return rb_float_new(d->ptr[offset]);
}

PRIVATE VALUE dvector_subseq(VALUE ary, long beg, long len) {
   VALUE klass, ary2, shared;
   double *ptr;
   Dvector *d, *d2;
   d = Get_Dvector(ary);
   if (beg > d->len) return Qnil;
   if (beg < 0 || len < 0) return Qnil;
   if (beg + len > d->len) {
      len = d->len - beg;
      if (len < 0)
         len = 0;
   }
   klass = rb_obj_class(ary);
   if (len == 0) return make_new_dvector(klass,0,DVEC_DEFAULT_SIZE);
   shared = dvector_make_shared(ary);
   ptr = d->ptr;
   ary2 = dvector_alloc(klass);
   d2 = Get_Dvector(ary2);
   d2->ptr = ptr + beg;
   d2->len = len;
   d2->shared = shared;
   return ary2;
}

PRIVATE
/* 
 *  call-seq:
 *     dvector[int]                  -> number or nil
 *     dvector[start, length]        -> dvector or nil
 *     dvector[range]                -> dvector or nil
 *     dvector.slice(index)          -> number or nil
 *     dvector.slice(start, length)  -> dvector or nil
 *     dvector.slice(range)          -> dvector or nil
 *
 *  Element Reference---Returns the element at index _int_,
 *  or returns a subvector starting at _start_ and
 *  continuing for _length_ elements, or returns a subvector
 *  specified by _range_.
 *  Negative indices count backward from the end of the
 *  vector (-1 is the last element). Returns <code>nil</code> if the index
 *  (or starting index) are out of range.  If the start index equals the
 *  vector size and a _length_ or _range_ parameter is given, an
 *  empty vector is returned.
 *
 *     a = Dvector[ 1, 2, 3, 4, 5 ]
 *     a[2] + a[0] + a[1]     -> 6
 *     a[6]                   -> nil
 *     a[1, 2]                -> Dvector[ 2, 3 ]
 *     a[1..3]                -> Dvector[ 2, 3, 4 ]
 *     a[4..7]                -> Dvector[ 5 ]
 *     a[6..10]               -> nil
 *     a[-3, 3]               -> Dvector[ 3, 4, 5 ]
 *     # special cases
 *     a[5]                   -> nil
 *     a[5, 1]                -> Dvector[]
 *     a[5..10]               -> Dvector[]
 *
 */ VALUE dvector_aref(int argc, VALUE *argv, VALUE ary) {
   VALUE arg;
   long beg, len;
   Dvector *d = Get_Dvector(ary);
   if (argc == 2) {
      if (SYMBOL_P(argv[0])) {
         rb_raise(rb_eTypeError, "Symbol as array index");
      }
      beg = NUM2LONG(argv[0]);
      len = NUM2LONG(argv[1]);
      if (beg < 0) {
         beg += d->len;
      }
      return dvector_subseq(ary, beg, len);
   }
   if (argc != 1) {
      rb_scan_args(argc, argv, "11", 0, 0);
   }
   arg = argv[0];
   /* special case - speeding up */
   if (FIXNUM_P(arg)) {
      return dvector_entry(ary, FIX2LONG(arg));
   }
   if (SYMBOL_P(arg)) {
      rb_raise(rb_eTypeError, "Symbol as array index");
   }
   /* check if idx is Range */
   switch (rb_range_beg_len(arg, &beg, &len, d->len, 0)) {
      case Qfalse:
         break;
      case Qnil:
         return Qnil;
      default:
         return dvector_subseq(ary, beg, len);
   }
   return dvector_entry(ary, NUM2LONG(arg));
}

PRIVATE
/* 
 *  call-seq:
 *     dvector.at(int)  ->  number or nil
 *
 *  Returns the element at index _int_. A
 *  negative index counts from the end of _dvector_.  Returns +nil+
 *  if the index is out of range.
 *
 *     a = Dvector[ 1, 2, 3, 4, 5 ]
 *     a.at(0)     -> 1
 *     a.at(-1)    -> 5
 */ VALUE dvector_at(VALUE ary, VALUE pos) {
   return dvector_entry(ary, NUM2LONG(pos));
}

PRIVATE
/*
 *  call-seq:
 *     dvector.first          ->  number or nil
 *     dvector.first(count)   ->  a_dvector
 *  
 *  Returns the first element, or the first _count_ elements, of _dvector_. If the vector is empty,
 *  the first form returns <code>nil</code>, and the second returns an empty vector.
 *     
 *     a = Dvector[ 1, 2, 3, 4, 5 ]
 *     a.first   -> 1
 *     a.first(1)   -> Dvector[ 1 ]
 *     a.first(3)   -> Dvector[ 1, 2, 3 ]
 */ 
VALUE dvector_first(int argc, VALUE *argv, VALUE ary) {
   VALUE nv, result;
   long n, i;
   Dvector *d = Get_Dvector(ary);
   if (argc == 0) {
      if (d->len == 0) return Qnil;
      return rb_float_new(d->ptr[0]);
   }
   rb_scan_args(argc, argv, "01", &nv);
   n = NUM2LONG(nv);
   if (n > d->len) n = d->len;
   result = dvector_new2(n,n);
   for (i=0; i<n; i++) {
      Dvector_Store_Double(result, i, d->ptr[i]);
   }
   return result;
}

PRIVATE
/*
 *  call-seq:
 *     dvector.last     ->  number or nil
 *     dvector.last(count)  ->  a_dvector
 *  
 *  Returns the last element, or the last _count_ elements, of _dvector_. If the vector is empty,
 *  the first form returns <code>nil</code>, and the second returns an empty vector.
 *     
 *     a = Dvector[ 1, 2, 3, 4, 5 ]
 *     a.last   -> 5
 *     a.last(1)   -> Dvector[ 5 ]
 *     a.last(3)   -> Dvector[ 3, 4, 5 ]
*/ VALUE dvector_last(int argc, VALUE *argv, VALUE ary) {
   VALUE nv, result;
   long n, i, beg;
   Dvector *d = Get_Dvector(ary);
   if (argc == 0) {
      if (d->len == 0) return Qnil;
      return rb_float_new(d->ptr[d->len-1]);
   }
   rb_scan_args(argc, argv, "01", &nv);
   n = NUM2LONG(nv);
   if (n > d->len) n = d->len;
   result = dvector_new2(n,n);
   beg = d->len - n;
   for (i=0; i < n; i++) {
      Dvector_Store_Double(result, i, d->ptr[beg+i]);
   }
   return result;
}

PRIVATE
/*
 *  call-seq:
 *     dvector.uniq! -> dvector
 *  
 * Same as Dvector#uniq, but modifies the receiver in place.  Returns nil if no changes are made
 * (that is, no duplicates are found).
 *  Removes duplicate elements from dvector.
 *  Deletes the element if there is a later one in the vector that is equal to it.
 *     
 *     a = Dvector[ 1.1, 3.8, 1.7, 3.8, 5 ]
 *     a.uniq!              ->   Dvector[1.1, 1.7, 3.8, 5]
 *     b = Dvector[ 1.1, 3.8, 1.7, 5 ]
 *     b.uniq!              ->   nil
 */ VALUE dvector_uniq_bang(VALUE ary) {
   double v;
   Dvector *d = dvector_modify(ary);
   long i, j, k;
   int uniq;
   for (i=j=0; i < d->len; i++) {
      v = d->ptr[i];
      uniq = true;
      for (k=i+1; k < d->len; k++) {
         if (d->ptr[k] == v) {
            uniq = false;
            break;
         }
      } 
      if (uniq) d->ptr[j++] = v;
   }
   if (d->len == j) return Qnil;
   d->len = j;
   return ary;
}

PRIVATE
/*
 *  call-seq:
 *     dvector.uniq -> a_dvector
 *  
 *  Returns a new vector by removing duplicate elements from _dvector_.
 *  Remove the element if there is a later one in the vector that is equal to it.
 *     
 *     a = Dvector[ 1.1, 3.8, 1.7, 3.8, 5 ]
 *     a.uniq              ->   Dvector[1.1, 1.7, 3.8, 5]
 */ VALUE dvector_uniq(VALUE ary) {
   VALUE new = dvector_uniq_bang(dvector_dup(ary));
   if (new == Qnil) new = ary;
   return new;
}

PRIVATE
/*
 *  call-seq:
 *     dvector.fetch(int)                    -> number
 *     dvector.fetch(int, default )          -> number
 *     dvector.fetch(int) {|index| block }   -> number
 *  
 *  Tries to return the element at index <i>int</i>. If the index
 *  lies outside the vector, the first form throws an
 *  <code>IndexError</code> exception, the second form returns
 *  <i>default</i>, and the third form returns the value of invoking
 *  the block, passing in the index. Negative values of the index
 *  count from the end of the vector.
 *     
 *     a = Dvector[ 11, 22, 33, 44 ]
 *     a.fetch(1)               -> 22
 *     a.fetch(-1)              -> 44
 *     a.fetch(4, 0)            -> 0
 *     a.fetch(4) { |i| i*i }   -> 16
 */ VALUE dvector_fetch(int argc, VALUE *argv, VALUE ary) {
   VALUE pos, ifnone;
   long block_given;
   long idx;
   Dvector *d = Get_Dvector(ary);
   rb_scan_args(argc, argv, "11", &pos, &ifnone);
   block_given = rb_block_given_p();
   if (block_given && argc == 2) {
      rb_warn("block supersedes default value argument");
   }
   idx = NUM2LONG(pos);
   if (idx < 0) {
      idx += d->len;
   }
   if (idx < 0 || d->len <= idx) {
      if (block_given) return rb_yield(pos);
      if (argc == 1) {
         rb_raise(rb_eIndexError, "index %ld out of dvector", idx);
      }
      return ifnone;
   }
   return rb_float_new(d->ptr[idx]);
}

PRIVATE
/*
 *  call-seq:
 *     dvector.index(number)   ->  int or nil
 *  
 *  Returns the index of the first object in _dvector_  
 *  <code>==</code> to _number_. Returns <code>nil</code> if
 *  no match is found.
 *     
 *     a = Dvector[ 1, 2, 3, 4, 5, 4, 3, 2 ]
 *     a.index(3)   -> 2
 *     a.index(0)   -> nil
 */ VALUE dvector_index(VALUE ary, VALUE val) {
   Dvector *d = Get_Dvector(ary);
   double v;
   long i = d->len;
   val = rb_Float(val);
   v = NUM2DBL(val);
   for (i=0; i < d->len; i++) {
      if (d->ptr[i] == v)
         return LONG2NUM(i);
   }
   return Qnil;
}

PRIVATE
/*
 *  call-seq:
 *     dvector.rindex(number)    ->  int or nil
 *  
 *  Returns the index of the last object in _dvector_ 
 *  <code>==</code> to _number_. Returns <code>nil</code> if
 *  no match is found.
 *     
 *     a = Dvector[ 1, 2, 3, 4, 5, 4, 3, 2 ]
 *     a.rindex(3)   -> 6
 *     a.rindex(0)   -> nil
 */ VALUE dvector_rindex(VALUE ary, VALUE val) {
   Dvector *d = Get_Dvector(ary);
   double v;
   long i = d->len;
   val = rb_Float(val);
   v = NUM2DBL(val);
   while (i--) {
      if (i > d->len) {
         i = d->len;
         continue;
      }
      if (d->ptr[i] == v)
         return LONG2NUM(i);
   }
   return Qnil;
}

static void dvector_splice(VALUE ary, long beg, long len, VALUE rpl) {
   long rlen;
   Dvector *d = dvector_modify(ary), *r = NULL;
   if (len < 0) rb_raise(rb_eIndexError, "negative length (%ld)", len);
   if (beg < 0) {
      beg += d->len;
      if (beg < 0) {
         beg -= d->len;
         rb_raise(rb_eIndexError, "index %ld out of array", beg);
      }
   }
   if (beg + len > d->len) {
      len = d->len - beg;
   }
   if (NIL_P(rpl)) {
      rlen = 0;
   } else if (rb_obj_is_kind_of(rpl,rb_cArray)) {
      rpl = rb_Array(rpl);
      rlen = RARRAY_LEN(rpl);
   } else {
      rpl = dvector_to_dvector(rpl);
      r = Get_Dvector(rpl);
      rlen = r->len;
   }
   if (beg >= d->len) {
      len = beg + rlen;
      if (len >= d->capa) {
         REALLOC_N(d->ptr, double, len);
         d->capa = len;
      }
      dvector_mem_clear(d->ptr + d->len, beg - d->len);
      if (rlen > 0) {
         if (r != NULL) MEMCPY(d->ptr + beg, r->ptr, double, rlen);
         else ary_to_dvector(RARRAY_PTR(rpl), rlen, d->ptr);
      }
      d->len = len;
   } else {
      long alen;
      if (beg + len > d->len) {
         len = d->len - beg;
      }
      alen = d->len + rlen - len;
      if (alen >= d->capa) {
         REALLOC_N(d->ptr, double, alen);
         d->capa = alen;
      }
      if (len != rlen) {
         MEMMOVE(d->ptr + beg + rlen, d->ptr + beg + len, double, d->len - (beg + len));
         d->len = alen;
      }
      if (rlen > 0) {
         if (r != NULL) MEMMOVE(d->ptr + beg, r->ptr, double, rlen);
         else ary_to_dvector(RARRAY_PTR(rpl), rlen, d->ptr + beg);
      }
   }
}

PRIVATE
/* 
 *  call-seq:
 *     dvector[int]           = number
 *     dvector[start, length] = number or a_dvector or an_array or nil
 *     dvector[range]         = number or a_dvector or an_array or nil
 *
 *  Element Assignment---Sets the element at index _int_,
 *  or replaces a subvector starting at _start_ and
 *  continuing for _length_ elements, or replaces a subvector
 *  specified by _range_.  Returns the assigned object as value.
 *  If indices are greater than
 *  the current capacity of the vector, the vector grows
 *  automatically by adding zeros. Negative indices will count backward
 *  from the end of the vector. Inserts elements if _length_ is
 *  zero. If +nil+ is used in the second and third forms,
 *  deletes elements from _dvector_. A 1D Array of numbers can be used on the right in the second
 *  and third forms in place of a Dvector. An +IndexError+ is raised if a
 *  negative index points past the beginning of the vector. See also
 *  <code>Dvector#push</code>, and <code>Dvector#unshift</code>.
 * 
 *     a = Dvector.new
 *     a[4] = 4;                      -> Dvector[ 0, 0, 0, 0, 4 ]
 *     a[0, 3] = [ 1, 2, 3 ]          -> Dvector[ 1, 2, 3, 0, 4 ]
 *     a[1..2] = [ 1, 2 ]             -> Dvector[ 1, 1, 2, 0, 4 ]
 *     a[0, 2] = -1                   -> Dvector[ -1, 2, 0, 4 ]
 *     a[0..2] = 1                    -> Dvector[ 1, 4 ]
 *     a[-1]   = 5                    -> Dvector[ 1, 5 ]
 *     a[1..-1] = nil                 -> Dvector[ 1 ]
 */ VALUE dvector_aset(int argc, VALUE *argv, VALUE ary) {
   long offset, beg, len;
   Dvector *d = Get_Dvector(ary);
   VALUE arg1;
   if (argc == 3) {
      if (SYMBOL_P(argv[0])) {
         rb_raise(rb_eTypeError, "Symbol as vector index");
      }
      if (SYMBOL_P(argv[1])) {
         rb_raise(rb_eTypeError, "Symbol as subvector length");
      }
      dvector_splice(ary, NUM2LONG(argv[0]), NUM2LONG(argv[1]), argv[2]);
      return argv[2];
   }
   if (argc != 2) {
      rb_raise(rb_eArgError, "wrong number of arguments (%d for 2)", argc);
   }
   if (FIXNUM_P(argv[0])) {
      offset = FIX2LONG(argv[0]);
      goto fixnum;
   }
   if (SYMBOL_P(argv[0])) {
      rb_raise(rb_eTypeError, "Symbol as vector index");
   }
   if (rb_range_beg_len(argv[0], &beg, &len, d->len, 1)) {
      /* check if idx is Range */
      arg1 = argv[1];
      if (arg1 != Qnil && !rb_obj_is_kind_of(arg1, rb_cNumeric) && !is_a_dvector(arg1))
         arg1 = dvector_to_dvector(arg1);
      dvector_splice(ary, beg, len, arg1);
      return arg1;
   }
   offset = NUM2LONG(argv[0]);
  fixnum:
   dvector_store(ary, offset, argv[1]);
   return argv[1];
}


PRIVATE
/* 
 *  call-seq:
 *     dvector.clear  ->  dvector
 *
 *  Removes all elements from _dvector_.
 *
 *     a = Dvector[ 1, 2, 3, 4, 5 ]
 *     a.clear    -> Dvector[]
 */ VALUE dvector_clear(VALUE ary) {
   Dvector *d = dvector_modify(ary);
   d->len = 0;
   if (DVEC_DEFAULT_SIZE * 2 < d->capa) {
      REALLOC_N(d->ptr, double, DVEC_DEFAULT_SIZE * 2);
      d->capa = DVEC_DEFAULT_SIZE * 2;
   }
   return ary;
}

PRIVATE
/*
 *  call-seq:
 *     dvector.set(float)  -> dvector
 *     dvector.set(a_dvector)  -> dvector
 *  
 *  Modifies the entries of _dvector_ array.  If the argument is a float, then all of the
 *  entries are set to that value.  If the argument is another Dvector, then it must
 *  be the same length as _dvector_, and its contents are copied to _dvector_.
 */ VALUE dvector_set(VALUE ary, VALUE val) {
   Dvector *d = dvector_modify(ary);
   int len = d->len, i;
   double *data = d->ptr;
   if (is_a_dvector(val)) {
      Dvector *d2 = Get_Dvector(val);
      double *data2 = d2->ptr;
      if (d2->len != len)
         rb_raise(rb_eArgError, "Vectors must be same length for Dvector set");
      for (i = 0; i < len; i++) {
         data[i] = data2[i];
      }
   } else {
      double v = NUM2DBL(val);
      for (i = 0; i < len; i++) {
         data[i] = v;
      }
   }
   return ary;
}

PRIVATE
/*
 *  call-seq:
 *     dvector.min_gt(val)  -> float or nil
 *  
 *  Returns the minimum entry in _dvector_ which is greater than _val_,
 *  or <code>nil</code> if no such entry if found.
 */ VALUE dvector_min_gt(VALUE ary, VALUE val) {
   Dvector *d = Get_Dvector(ary);
   val = rb_Float(val);
   double zmin = 0, z = NUM2DBL(val), x; /* it doesn't matter
					    what is zmin's initial value 
					 */
   double *data = d->ptr;
   int len = d->len, i, imin;
   imin = -1;
   for (i = 0; i < len; i++) {
       x = data[i];
       if (x > z && (imin < 0 || x < zmin)) { imin = i; zmin = x; }
   }
   if (imin >= 0)
      return rb_float_new(zmin);
   return Qnil;
}

PRIVATE
/*
 *  call-seq:
 *     dvector.max_lt(val)  -> float or nil
 *  
 *  Returns the maximum entry in _dvector_ which is less than _val_,
 *  or <code>nil</code> if no such entry if found.
 */ VALUE dvector_max_lt(VALUE ary, VALUE val) {
   Dvector *d = Get_Dvector(ary);
   val = rb_Float(val);
   double zmax = 0, z = NUM2DBL(val), x;
   double *data = d->ptr;
   int len = d->len, i, imax;
   imax = -1;
   for (i = 0; i < len; i++) {
       x = data[i];
       if (x < z && (imax < 0 || x > zmax)) { imax = i; zmax = x; }
   }
   if (imax >= 0)
      return rb_float_new(zmax);
   return Qnil;
}

PRIVATE
/*
 *  call-seq:
 *     dvector.insert(int, number...)  -> dvector
 *  
 *  If the index is not negative, insert the given values before
 *     the element with the index _int_.  If the index is -1, appends the
 *     values to _dvector_.  Otherwise inserts the values after the element
 *    with the given index.
 *
 *     a = Dvector[ 1, 2, 3 ]
 *     a.insert(2, 99)         -> Dvector[ 1, 2, 99, 3 ]
 *     a.insert(-2, 1, 2, 3)   -> Dvector[ 1, 2, 99, 1, 2, 3, 3 ]
 *     a.insert(-1, 0)         -> Dvector[ 1, 2, 99, 1, 2, 3, 3, 0 ]
 */ VALUE dvector_insert(int argc, VALUE *argv, VALUE ary) {
   long pos;
   if (argc < 1) {
      rb_raise(rb_eArgError, "wrong number of arguments (at least 1)");
   }
   pos = NUM2LONG(argv[0]);
   if (pos == -1) {
      Dvector *d = Get_Dvector(ary);
      pos = d->len;
   }
   else if (pos < 0) {
      pos++;
   }
   if (argc == 1) return ary;
   dvector_splice(ary, pos, 0, dvector_new4(argc - 1, argv + 1));
   return ary;
}

PRIVATE
/*
 *  call-seq:
 *     dvector.each {|x| block }   ->   dvector
 *  
 *  Calls <i>block</i> once for each element in _dvector_, passing that
 *  element as a parameter.
 *     
 *     a = Dvector[ 1, 0, -1 ]
 *     a.each {|x| print x, " -- " }
 *     
 *  produces:
 *     
 *      1 -- 0 -- -1 --
 */ VALUE dvector_each(VALUE ary) {
   Dvector *d = Get_Dvector(ary);
   long i;
   for (i=0; i < d->len; i++) {
      rb_yield(rb_float_new(d->ptr[i]));
   }
   return ary;
}

PRIVATE
/*
 *  call-seq:
 *     dvector.each2(other) {|x,y| block } 
 *  
 *  Calls <i>block</i> once for each element in _dvector_, passing that
 *  element as a parameter along with the corresponding element from the _other_ vector.
 *  The two vectors must be the same length.
 *     
 *     a = Dvector[ 1, 0, -1 ]
 *     b = Dvector[ 3, 4, 5 ]
 *     a.each2(b) {|x,y| print "(", x ",", y, ") " }
 *     
 *  produces:
 *     
 *     (1,3) (0,4) (-1,5) 
 */ VALUE dvector_each2(VALUE ary, VALUE ary2) {
   Dvector *d = Get_Dvector(ary);
   Dvector *d2 = Get_Dvector(ary2);
   long i;
   if (d->len != d2->len) {
      rb_raise(rb_eArgError, "vectors with different lengths (%ld vs %ld) for each2", d->len, d2->len);
   }
   for (i=0; i < d->len; i++) {
      rb_yield_values(2, rb_float_new(d->ptr[i]), rb_float_new(d2->ptr[i]));
   }
   return ary;
}

PRIVATE
/*
 *  call-seq:
 *     dvector.each3(other1, other2) {|x,y, z| block } 
 *  
 *  Calls <i>block</i> once for each element in _dvector_, passing that
 *  element as a parameter along with the corresponding element from the _other1_ and _other2_ vectors.
 *  The three vectors must be the same length.
 *     
 *     a = Dvector[ 1, 0, -1 ]
 *     b = Dvector[ 3, 4, 5 ]
 *     c = Dvector[ 6, 9, 2 ]
 *     a.each3(b, c) {|x,y,z| print "(", x ",", y, ", ", z, ") " }
 *     
 *  produces:
 *     
 *     (1,3,6) (0,4,9) (-1,5,2) 
 */ VALUE dvector_each3(VALUE ary, VALUE ary2, VALUE ary3) {
   Dvector *d = Get_Dvector(ary);
   Dvector *d2 = Get_Dvector(ary2);
   Dvector *d3 = Get_Dvector(ary3);
   long i;
   if (d->len != d2->len) {
      rb_raise(rb_eArgError, "vectors with different lengths (%ld vs %ld) for each3", d->len, d2->len);
   }
   if (d->len != d3->len) {
      rb_raise(rb_eArgError, "vectors with different lengths (%ld vs %ld) for each3", d->len, d3->len);
   }
   for (i=0; i < d->len; i++) {
      rb_yield_values(3, rb_float_new(d->ptr[i]), rb_float_new(d2->ptr[i]), rb_float_new(d3->ptr[i]));
   }
   return ary;
}

PRIVATE
/*
 *  call-seq:
 *     dvector.each_index {|index| block }  ->  dvector
 *  
 *  Same as <code>Dvector#each</code>, but passes the index of the element
 *  instead of the element itself.
 *     
 *     a = Dvector[ 1, 0, -1 ]
 *     a.each_index {|x| print x, " -- " }
 *     
 *  produces:
 *     
 *     0 -- 1 -- 2 --
 */ VALUE dvector_each_index(VALUE ary) {
   Dvector *d = Get_Dvector(ary);
   long i;
   for (i=0; i < d->len; i++) {
      rb_yield(LONG2NUM(i));
   }
   return ary;
}

PRIVATE
/*
 *  call-seq:
 *     dvector.each_with_index {|x,index| block }  ->  dvector
 *  
 *  Same as <code>Dvector#each</code>, but passes the index of the element
 *  in addition to the element itself.
 *     
 */ VALUE dvector_each_with_index(VALUE ary) {
   Dvector *d = Get_Dvector(ary);
   long i;
   for (i=0; i < d->len; i++) {
      rb_yield_values(2, rb_float_new(d->ptr[i]), LONG2NUM(i));
   }
   return ary;
}

PRIVATE
/*
 *  call-seq:
 *     dvector.each2_with_index(other) {|x,y,index| block } 
 *  
 *  Calls <i>block</i> once for each element in _dvector_, passing the 
 *  element, the corresponding element from the _other_ vector, and the index.
 *     
 *     a = Dvector[ 1, 0, -1 ]
 *     b = Dvector[ 3, 4, 5 ]
 *     a.each2_with_index(b) {|x,y,i| print "(", x ",", y, ",", i, ") " }
 *     
 *  produces:
 *     
 *     (1,3,0) (0,4,1) (-1,5,2) 
 */ VALUE dvector_each2_with_index(VALUE ary, VALUE ary2) {
   Dvector *d = Get_Dvector(ary);
   Dvector *d2 = Get_Dvector(ary2);
   long i;
   if (d->len != d2->len) {
      rb_raise(rb_eArgError, "vectors with different lengths (%ld vs %ld) for each2_with_index", d->len, d2->len);
   }
   for (i=0; i < d->len; i++) {
      rb_yield_values(3, rb_float_new(d->ptr[i]), rb_float_new(d2->ptr[i]), LONG2NUM(i));
   }
   return ary;
}

PRIVATE
/*
 *  call-seq:
 *     dvector.each3_with_index(other1, other2) {|x,y,z,i| block } 
 *  
 *  Calls <i>block</i> once for each element in _dvector_, passing that
 *  element as a parameter along with the corresponding element from the _other1_ and _other2_ vectors and the index.
 *  The three vectors must be the same length.
 *     
 *     a = Dvector[ 1, 0, -1 ]
 *     b = Dvector[ 3, 4, 5 ]
 *     c = Dvector[ 6, 9, 2 ]
 *     a.each3(b, c) {|x,y,z,i| print "(", x ",", y, ", ", z, ",", i, ") " }
 *     
 *  produces:
 *     
 *     (1,3,6,0) (0,4,9,1) (-1,5,2,2) 
 */ VALUE dvector_each3_with_index(VALUE ary, VALUE ary2, VALUE ary3) {
   Dvector *d = Get_Dvector(ary);
   Dvector *d2 = Get_Dvector(ary2);
   Dvector *d3 = Get_Dvector(ary3);
   long i;
   if (d->len != d2->len) {
      rb_raise(rb_eArgError, "vectors with different lengths (%ld vs %ld) for each3", d->len, d2->len);
   }
   if (d->len != d3->len) {
      rb_raise(rb_eArgError, "vectors with different lengths (%ld vs %ld) for each3", d->len, d3->len);
   }
   for (i=0; i < d->len; i++) {
      rb_yield_values(4, rb_float_new(d->ptr[i]), rb_float_new(d2->ptr[i]), rb_float_new(d3->ptr[i]), LONG2NUM(i));
   }
   return ary;
}

PRIVATE
/*
 *  call-seq:
 *     dvector.reverse_each {|x| block } 
 *  
 *  Same as <code>Dvector#each</code>, but traverses _dvector_ in reverse
 *  order.
 *     
 *     a = Dvector[ 1, 0, -1 ]
 *     a.each {|x| print x, " " }
 *     
 *  produces:
 *     
 *      -1 0 1
 */ VALUE dvector_reverse_each(VALUE ary) {
   Dvector *d = Get_Dvector(ary);
   long len = d->len;
   while (len--) {
      rb_yield(rb_float_new(d->ptr[len]));
      if (d->len < len) {
         len = d->len;
      }
   }
   return ary;
}

PRIVATE
/*
 *  call-seq:
 *     dvector.reverse_each2(other) {|x,y| block } 
 *  
 *  Same as <code>Dvector#each2</code>, but traverses vectors in reverse
 *  order.  The vectors must have the same size.
 *     
 *     a = Dvector[ 1, 0, -1 ]
 *     b = Dvector[ 3, 4, 5 ]
 *     a.reverse_each2(b) {|x,y| print "(", x ",", y, ") " }
 *     
 *  produces:
 *     
 *      (-1,5) (0,4) (1,3)
 */ VALUE dvector_reverse_each2(VALUE ary, VALUE ary2) {
   Dvector *d = Get_Dvector(ary);
   Dvector *d2 = Get_Dvector(ary2);
   long len = d->len;
   if (len != d2->len) {
      rb_raise(rb_eArgError, "vectors with different lengths (%ld vs %ld) for reverse_each2", len, d2->len);
   }
   while (len--) {
      rb_yield_values(2, rb_float_new(d->ptr[len]), rb_float_new(d2->ptr[len]));
      if (d->len < len) {
         len = d->len;
      }
   }
   return ary;
}

PRIVATE
/*
 *  call-seq:
 *     dvector.reverse_each3(other1, other2) {|x,y,z| block } 
 *  
 *  Same as <code>Dvector#each3</code>, but traverses vectors in reverse
 *  order.  The vectors must have the same size.
 *     
 *     a = Dvector[ 1, 0, -1 ]
 *     b = Dvector[ 3, 4, 5 ]
 *     c = Dvector[ 6, 9, 2 ]
 *     a.reverse_each3(b, c) {|x,y,z| print "(", x ",", y, ", ", z, ") " }
 *     
 *  produces:
 *     
 *      (-1,5,2) (0,4,9) (1,3,6)
 */ VALUE dvector_reverse_each3(VALUE ary, VALUE ary2, VALUE ary3) {
   Dvector *d = Get_Dvector(ary);
   Dvector *d2 = Get_Dvector(ary2);
   Dvector *d3 = Get_Dvector(ary3);
   long len = d->len;
   if (len != d2->len) {
      rb_raise(rb_eArgError, "vectors with different lengths (%ld vs %ld) for reverse_each3", len, d2->len);
   }
   if (len != d3->len) {
      rb_raise(rb_eArgError, "vectors with different lengths (%ld vs %ld) for reverse_each3", len, d3->len);
   }
   while (len--) {
      rb_yield_values(3, rb_float_new(d->ptr[len]), rb_float_new(d2->ptr[len]), rb_float_new(d3->ptr[len]));
      if (d->len < len) {
         len = d->len;
      }
   }
   return ary;
}

PRIVATE
/*
 *  call-seq:
 *     dvector.reverse_each_index {|index| block }  ->  array
 *  
 *  Same as <code>Dvector#reverse_each</code>, but passes the index of the element
 *  instead of the element itself.
 *     
 *     a = Dvector[ 1, 0, -1 ]
 *     a.reverse_each_index {|i| print i, " -- " }
 *     
 *  produces:
 *     
 *     2 -- 1 -- 0 --
 */ VALUE dvector_reverse_each_index(VALUE ary) {
   Dvector *d = Get_Dvector(ary);
   long len = d->len;
   while (len--) {
      rb_yield(LONG2NUM(len));
      if (d->len < len) {
         len = d->len;
      }
   }
   return ary;
}

PRIVATE
/*
 *  call-seq:
 *     dvector.reverse_each_with_index {|x,index| block }  ->  array
 *  
 *  Same as <code>Dvector#each_with_index</code>, but traverses _dvector_ in reverse
 *  order.
 *     
 *     a = Dvector[ 1, 0, -1 ]
 *     a.reverse_each_with_index {|x,i| print "(", x, ",", i, ") " }
 *     
 *  produces:
 *     
 *     (-1,2) (0,1) (1,0) 
 */ VALUE dvector_reverse_each_with_index(VALUE ary) {
   Dvector *d = Get_Dvector(ary);
   long len = d->len;
   while (len--) {
      rb_yield_values(2, rb_float_new(d->ptr[len]), LONG2NUM(len));
      if (d->len < len) {
         len = d->len;
      }
   }
   return ary;
}

PRIVATE
/*
 *  call-seq:
 *     dvector.reverse_each2_with_index {|x,y,index| block }  ->  dvector
 *  
 *  Same as <code>Dvector#each2_with_index</code>, but traverses the vectors in reverse
 *  order.
 *     
 *     a = Dvector[ 1, 0, -1 ]
 *     b = Dvector[ 3, 4, 5 ]
 *     a.reverse_each2_with_index(b) {|x,y,i| print "(", x ",", y, "," i, ") " }
 *     
 *  produces:
 *     
 *      (-1,5,2) (0,4,1) (1,3,0)
 */ VALUE dvector_reverse_each2_with_index(VALUE ary, VALUE ary2) {
   Dvector *d = Get_Dvector(ary);
   Dvector *d2 = Get_Dvector(ary2);
   long len = d->len;
   if (len != d2->len) {
      rb_raise(rb_eArgError, "vectors with different lengths (%ld vs %ld) for reverse_each2_with_index", 
         len, d2->len);
   }
   while (len--) {
      rb_yield_values(3, rb_float_new(d->ptr[len]), rb_float_new(d2->ptr[len]), LONG2NUM(len));
      if (d->len < len) {
         len = d->len;
      }
   }
   return ary;
}

PRIVATE
/*
 *  call-seq:
 *     dvector.reverse_each3_with_index {|x,y,z,index| block }  ->  dvector
 *  
 *  Same as <code>Dvector#each3_with_index</code>, but traverses the vectors in reverse
 *  order.
 *     
 *     a = Dvector[ 1, 0, -1 ]
 *     b = Dvector[ 3, 4, 5 ]
 *     c = Dvector[ 6, 9, 2 ]
 *     a.reverse_each3_with_index(b,c) {|x,y,i| print "(", x ",", y, "," i, ") " }
 *     a.each3(b, c) {|x,y,z,i| print "(", x ",", y, ", ", z, ",", i, ") " }
 *     
 *  produces:
 *     
 *      (-1,5,2,2) (0,4,9,1) (1,3,6,0)
 */ VALUE dvector_reverse_each3_with_index(VALUE ary, VALUE ary2, VALUE ary3) {
   Dvector *d = Get_Dvector(ary);
   Dvector *d2 = Get_Dvector(ary2);
   Dvector *d3 = Get_Dvector(ary3);
   long len = d->len;
   if (len != d2->len) {
      rb_raise(rb_eArgError, "vectors with different lengths (%ld vs %ld) for reverse_each3_with_index", 
         len, d3->len);
   }
   if (len != d3->len) {
      rb_raise(rb_eArgError, "vectors with different lengths (%ld vs %ld) for reverse_each3_with_index", 
         len, d3->len);
   }
   while (len--) {
      rb_yield_values(4, rb_float_new(d->ptr[len]), rb_float_new(d2->ptr[len]), rb_float_new(d3->ptr[len]), LONG2NUM(len));
      if (d->len < len) {
         len = d->len;
      }
   }
   return ary;
}

PRIVATE
/*
 *  call-seq:
 *     dvector.length -> int
 *  
 *  Returns the number of elements in _dvector_.
 *     
 *     Dvector[ 0, -1, 2, -3, 4 ].length   -> 5
 *     Dvector[].length                    -> 0
 */ VALUE dvector_length(VALUE ary) {
   Dvector *d = Get_Dvector(ary);
   return LONG2NUM(d->len);
}

PRIVATE
/*
 *  call-seq:
 *     dvector.empty?   -> true or false
 *  
 *  Returns <code>true</code> if _dvector_ vector contains no elements.
 *     
 *     Dvector[].empty?   -> true
 */ VALUE dvector_empty_p(VALUE ary) {
   Dvector *d = Get_Dvector(ary);
   if (d->len == 0)
      return Qtrue;
   return Qfalse;
}

static VALUE dvector_output_fs;

PRIVATE VALUE dvector_join(VALUE ary, VALUE sep) {
   long len = 1, i;
   int taint = Qfalse;
   VALUE result, tmp;
   char buff[100];
   Dvector *d = Get_Dvector(ary);
   if (d->len == 0) return rb_str_new(0, 0);
   if (OBJ_TAINTED(ary) || OBJ_TAINTED(sep)) taint = Qtrue;
   len += d->len * 10;
   if (!NIL_P(sep)) {
      StringValue(sep);
      len += RSTRING_LEN(sep) * (d->len - 1); /* So it works for ruby 1.9 */
   }
   result = rb_str_buf_new(len);
   for (i=0; i < d->len; i++) {
      snprintf(buff,sizeof(buff), "%g", d->ptr[i]);
      tmp = rb_str_new2(buff);
      if (i > 0 && !NIL_P(sep)) rb_str_buf_append(result, sep);
      rb_str_buf_append(result, tmp);
   }
   if (taint) OBJ_TAINT(result);
   return result;
}

PRIVATE
/*
 *  call-seq:
 *     dvector.join(sep=" ")    -> a_string
 *  
 *  Returns a string created by converting each element of the vector to
 *  a string, separated by <i>sep</i>.
 *     
 *     Dvector[ 1, 2, 3 ].join        -> "1 2 3"
 *     Dvector[ 1, 2, 3 ].join("-")   -> "1-2-3"
 */ 
VALUE dvector_join_m(int argc, VALUE *argv, VALUE ary) {
   VALUE sep;
   rb_scan_args(argc, argv, "01", &sep);
   if (NIL_P(sep)) sep = dvector_output_fs;
   return dvector_join(ary, sep);
}

PRIVATE
/*
 *  call-seq:
 *     dvector.tridag(a,b,c,r) -> dvector
 *  
 *  Sets contents of _dvector_ to solution vector _u_ of the following tri-diagonal matrix problem.
 *     
 *     |  b[0]  c[0]    0    ...                         |   |  u[0]  |   |  r[0]  |
 *     |  a[1]  b[1]  c[1]   ...                         |   |  u[1]  |   |  r[1]  |
 *     |                     ...                         | * |  ...   | = |  ...   |
 *     |                     ... a[n-2]  b[n-2]  c[n-2]  |   | u[n-2] |   | r[n-2] |
 *     |                     ...   0     a[n-1]  b[n-1]  |   | u[n-1] |   | r[n-1] |
 *
 *  This corresponds to solving difference equations of the form
 *		 a[j] * u[j-1] + b[j] * u[j] + c[j] * u[j+1] = r[j], for 0 < j < n,
 *  with boundary conditions
 *     u[0]   = (r[0] - c[0] * u[1]) / b[0], and
 *     u[n-1] = (r[n-1] - a[n-1] * u[n-2]) / b[n-1].
 *
 *  See Numerical Recipes for more details.  
 */ VALUE dvector_tridag(VALUE uVec, VALUE aVec, VALUE bVec, VALUE cVec, VALUE rVec) {
    double *u, *a, *b, *c, *r, *gam, bet;
    long j, n, u_len, a_len, b_len, c_len, r_len;
    u = Dvector_Data_for_Read(uVec, &u_len);
    a = Dvector_Data_for_Read(aVec, &a_len);
    b = Dvector_Data_for_Read(bVec, &b_len);
    c = Dvector_Data_for_Read(cVec, &c_len);
    r = Dvector_Data_for_Read(rVec, &r_len);
    if (a_len != b_len || b_len != c_len || c_len != r_len) {
        rb_raise(rb_eArgError, "vectors with different lengths for tridag");
    }
    n = a_len;
    if (u_len != a_len) u = Dvector_Data_Resize(uVec, a_len);
    gam = (double *)ALLOC_N(double, n);
    if (b[0] == 0.0) {
        rb_raise(rb_eArgError, "b[0] is zero in tridag");
    }
    u[0] = r[0] / (bet = b[0]);
    for (j = 1; j < n; j++) {  // decomposition and forward substitution
        gam[j] = c[j-1] / bet;
        bet = b[j] - a[j]*gam[j];
        if (bet == 0.0) {
            rb_raise(rb_eArgError, "zero divisor in tridag (j=%ld)", j);
        }
        u[j] = (r[j] - a[j]*u[j-1]) / bet;
    }
    for (j = n-2; j >= 0; j--) { // backsubstitution
        u[j] -= gam[j+1]*u[j+1];
    }
    free(gam);
    return uVec;
}

PRIVATE
/*
 *  call-seq:
 *     dvector.to_s -> a_string
 *  
 *  Returns _dvector_<code>.join</code>.
 *     
 *     Dvector[ 1, 2, 3 ].to_s        -> "1 2 3"
 *
 */ VALUE dvector_to_s(VALUE ary) {
   Dvector *d = Get_Dvector(ary);
   if (d->len == 0) return rb_str_new(0, 0);
   return dvector_join(ary, dvector_output_fs);
}

PRIVATE
/*
 *  call-seq:
 *     dvector.to_a     -> an_array
 *
 *     Returns an Array with the same contents as _dvector_.
 *  
 */ VALUE dvector_to_a(VALUE dvector) {
   Dvector *d = Get_Dvector(dvector);
   long len = d->len, i;
   double *ptr = d->ptr;
   VALUE ary = rb_ary_new2(len);
   for (i = 0; i < len; i++) {
      rb_ary_store(ary, i, rb_float_new(*ptr++));
   }
   return ary;
}

PRIVATE VALUE dvector_reverse(VALUE ary) {
   Dvector *d = dvector_modify(ary);
   double tmp, *p1, *p2;
   if (d->len > 1) {
      p1 = d->ptr;
      p2 = p1 + d->len - 1;	/* points last item */
      while (p1 < p2) {
         tmp = *p1;
         *p1++ = *p2;
         *p2-- = tmp;
      }
   }
   return ary;
}

PRIVATE
/*
 *  call-seq:
 *     dvector.reverse!   -> dvector 
 *  
 *  Reverses _dvector_ in place.
 *     
 *     a = Dvector[ 1, 2, 3 ]
 *     a.reverse!       -> Dvector[ 3, 2, 1 ]
 *     a                -> Dvector[ 3, 2, 1 ]
 */ VALUE dvector_reverse_bang(VALUE ary) {
   return dvector_reverse(ary);
}

PRIVATE
/*
 *  call-seq:
 *     dvector.reverse -> a_dvector
 *  
 *  Returns a new vector containing _dvector_'s elements in reverse order.
 *     
 *     Dvector[ 1, 2, 3 ].reverse         -> Dvector[ 3, 2, 1 ]
 *     Dvector[ 1 ].reverse               -> Dvector[ 1 ]
 */ VALUE dvector_reverse_m(VALUE ary) {
   return dvector_reverse(dvector_dup(ary));
}

struct dvector_sort_data {
   Dvector *d;
   double *ptr;
   long len;
};
 
static int sort_2(const void *ap, const void *bp) {
   double a = *(double *)ap, b = *(double *)bp;
   if (a < b) return -1;
   if (a > b) return 1;
   return 0;
}

extern VALUE rb_ary_sort_bang(VALUE);

static VALUE dvector_sort_internal(VALUE ary) {
   struct dvector_sort_data data;
   Dvector *d = Get_Dvector(ary);
   VALUE ary2;
   data.d = d; data.ptr = d->ptr; data.len = d->len;
   if (!rb_block_given_p()) qsort(d->ptr, d->len, sizeof(double), sort_2);
   else {
      ary2 = dvector_to_a(ary);
      ary2 = rb_ary_sort_bang(ary2);
      ary_to_dvector(RARRAY_PTR(ary2), d->len, d->ptr);
   }
   return ary;
}

static VALUE dvector_sort_unlock(VALUE ary) {
   FL_UNSET(ary, DVEC_TMPLOCK);
   return ary;
}

PRIVATE
/*
 *  call-seq:
 *     dvector.sort!                   -> dvector
 *     dvector.sort! {| a,b | block }  -> dvector 
 *  
 *  Sorts _dvector_ in place. _dvev_ is effectively frozen while a sort is in progress.
 *  Comparisons for the sort will be done using the <code><=></code> operator or using
 *  an optional code block. The block implements a comparison between
 *  <i>a</i> and <i>b</i>, returning -1, 0, or +1.
 *     
 *     a = Dvector[ 4, 1, 2, 5, 3 ]
 *     a.sort!                    -> Dvector[ 1, 2, 3, 4, 5 ]
 *     a                          -> Dvector[ 1, 2, 3, 4, 5 ]
 *     a.sort! {|x,y| y <=> x }   -> Dvector[ 5, 4, 3, 2, 1 ]
 *     a                          -> Dvector[ 5, 4, 3, 2, 1 ]
 */ VALUE dvector_sort_bang(VALUE ary) {
   Dvector *d = dvector_modify(ary); /* force "unshared" before start the sort */
   if (d->len > 1) {
      FL_SET(ary, DVEC_TMPLOCK);	/* prohibit modification during sort */
      rb_ensure(dvector_sort_internal, ary, dvector_sort_unlock, ary);
   }
   return ary;
}

PRIVATE
/*
 *  call-seq:
 *     dvector.sort                   -> a_dvector 
 *     dvector.sort {| a,b | block }  -> a_dvector 
 *  
 *  Returns a new vector created by sorting _dvector_. Comparisons for
 *  the sort will be done using the <code><=></code> operator or using
 *  an optional code block. The block implements a comparison between
 *  <i>a</i> and <i>b</i>, returning -1, 0, or +1.
 *     
 *     a = Dvector[ 4, 1, 2, 5, 3 ]
 *     a.sort                    -> Dvector[ 1, 2, 3, 4, 5 ]
 *     a                         -> Dvector[ 4, 1, 2, 5, 3 ]
 *     a.sort {|x,y| y <=> x }   -> Dvector[ 5, 4, 3, 2, 1 ]
 */ VALUE dvector_sort(VALUE ary) {
    ary = dvector_dup(ary);
    dvector_sort_bang(ary);
    return ary;
}

PRIVATE
/*
 *  call-seq:
 *     dvector.dup  -> a_dvector
 *  
 *  Returns a copy of _dvector_.
 *  For performance sensitive situations involving a series of vector operations,
 *  first make a copy using dup and then do "bang" operations to modify the result
 *  without further copying.
 */ VALUE dvector_dup(VALUE ary) {
   Dvector *d = Get_Dvector(ary);
   return dvector_new4_dbl(d->len, d->ptr);
}

PRIVATE
/*
 *  call-seq:
 *     dvector.collect {|x| block }  -> a_dvector
 *     dvector.map     {|x| block }  -> a_dvector
 *  
 *  Invokes <i>block</i> once for each element of _dvector_.
 *  Returns a new vector holding the values returned by _block_.
 *  Note that for numeric operations on long vectors, it is more efficient to
 *  apply the operator directly to the vector rather than using map or collect.
 *     
 *     a = Dvector[ 1, 2, 3, 4 ]
 *     a.map {|x| x**2 + 1 }      -> Dvector[ 2, 5, 10, 17 ]
 *    A better way:
 *     a = Dvector[ 1, 2, 3, 4 ]
 *     a**2 + 1                   -> Dvector[ 2, 5, 10, 17 ]
 */ VALUE dvector_collect(VALUE ary) {
   long i;
   VALUE collect;
   Dvector *d = Get_Dvector(ary);
   if (!rb_block_given_p()) {
      if ( is_a_dvector(ary) ) {
         return dvector_new4_dbl(d->len, d->ptr);
      }
      ary = rb_Array(ary);
      return dvector_new4(d->len, RARRAY_PTR(ary));
   }
   collect = dvector_new2(0,d->len);
   for (i = 0; i < d->len; i++) {
      dvector_push(collect, rb_yield(rb_float_new(d->ptr[i])));
   }
   return collect;
}

PRIVATE
/*
 *  call-seq:
 *     dvector.collect2(other) {|x,y| block }  -> dvector
 *     dvector.map2(other)     {|x,y| block }  -> dvector
 *  
 *  Calls <i>block</i> for each element of _dvector_ along with the corresponding element in _other_. Creates a 
 *  new vector containing the values returned by _block_.  The vectors must be the same size.
 *     
 *     a = Dvector[ 1, 0, -1 ]
 *     b = Dvector[ 3, 4, 5 ]
 *     a.map2(b) {|x,y| x**2 + y**2 }  -> Dvector[ 10, 16, 26 ]
 *
 */ VALUE dvector_collect2(VALUE ary, VALUE ary2) {
   long i;
   VALUE collect;
   Dvector *d = Get_Dvector(ary);
   Dvector *d2 = Get_Dvector(ary2);
   if (d->len != d2->len) {
      rb_raise(rb_eArgError, "vectors with different lengths (%ld vs %ld) for collect2", d->len, d2->len);
   }
   if (!rb_block_given_p()) {
      return dvector_collect(ary);
   }
   collect = dvector_new2(0,d->len);
   for (i = 0; i < d->len; i++) {
      dvector_push(collect, rb_yield_values(2, rb_float_new(d->ptr[i]), rb_float_new(d2->ptr[i])));
   }
   return collect;
}

PRIVATE
/* 
 *  call-seq:
 *     dvector.collect! {|x| block }   ->   dvector
 *     dvector.map!     {|x| block }   ->   dvector
 *
 *  Invokes  _block_ once for each element of _dvector_, replacing the
 *  element with the value returned by _block_.  
 *  Note that for numeric operations on long vectors, it is more efficient to
 *  apply the operator directly to the vector rather than using these operators.
 *   
 *     a = Dvector[ 2, -3, 7 ]
 *     a.map! {|x| x**2 + 1 }      -> Dvector[ 5, 10, 50 ]
 *     a                           -> Dvector[ 5, 10, 50 ]
 *  A better way:
 *     a.mul!(a).add!(1)           -> Dvector[ 5, 10, 50 ]
 *
*/ VALUE dvector_collect_bang(VALUE ary) {
   long i;
   Dvector *d= dvector_modify(ary);
   for (i = 0; i < d->len; i++) {
      dvector_store(ary, i, rb_yield(rb_float_new(d->ptr[i])));
   }
   return ary;
}

PRIVATE
/* 
 *  call-seq:
 *     dvector.collect2! {|x,y| block }   ->   dvector
 *     dvector.map2!     {|x,y| block }   ->   dvector
 *
 *  Invokes _block_ once for each element of _dvector_, replacing the
 *  element with the value returned by _block_.
 *   
 *     a = Dvector[ 1, 0, -1 ]
 *     b = Dvector[ 3, 4, 5 ]
 *     a.map2!(b) {|x,y| x**2 + y**2 }  -> Dvector[ 10, 16, 26 ]
 *     a                                -> Dvector[ 10, 16, 26 ]
 *
*/ 
VALUE dvector_collect2_bang(VALUE ary, VALUE ary2) {
   long i;
   Dvector *d = dvector_modify(ary);
   Dvector *d2 = Get_Dvector(ary2);
   if (d->len != d2->len) {
      rb_raise(rb_eArgError, "vectors with different lengths (%ld vs %ld) for collect2!", d->len, d2->len);
   }
   for (i = 0; i < d->len; i++) {
      dvector_store(ary, i, rb_yield_values(2, rb_float_new(d->ptr[i]), rb_float_new(d2->ptr[i])));
   }
   return ary;
}

PRIVATE
/* 
 *  call-seq:
 *     dvector.values_at(selector,... )   -> a_dvector
 *
 *  Returns a new vector containing the elements in
 *  _dvector_ corresponding to the given selector(s). The selectors
 *  may be either integer indices or ranges. 
 * 
 *     a = Dvector[ 1, 2, 3, 4, 5, 6 ]
 *     a.values_at(1, 3, 5)           -> Dvector[ 2, 4, 6 ]
 *     a.values_at(1, 3, 5, 7)        -> Dvector[ 2, 4, 6 ]
 *     a.values_at(-1, -3, -5, -7)    -> Dvector[ 6, 4, 2 ]
 *     a.values_at(1..3, 2...5)       -> Dvector[ 2, 3, 4, 3, 4, 5 ]
 */ 
VALUE dvector_values_at(int argc, VALUE *argv, VALUE ary) {
   Dvector *d = Get_Dvector(ary);
   long olen = d->len;
   VALUE result = dvector_new();
   long beg, len, i, j;
   for (i=0; i<argc; i++) {
      if (FIXNUM_P(argv[i])) {
         j = FIX2LONG(argv[i]);
         if (j < 0) j += d->len;
         if (j >= 0 && j < d->len) Dvector_Push_Double(result, d->ptr[j]);
         continue;
      }
      /* check if idx is Range */
      switch (rb_range_beg_len(argv[i], &beg, &len, olen, 0)) {
         case Qfalse:
            break;
         case Qnil:
            continue;
         default:
         for (j=0; j<len; j++) {
            if (j+beg >= 0 && j+beg < d->len) Dvector_Push_Double(result, d->ptr[j+beg]);
         }
         continue;
      }
      j = NUM2LONG(argv[i]);
      if (j < 0) j += d->len;
      if (j >= 0 && j < d->len) Dvector_Push_Double(result, d->ptr[j]);
   }
   return result;
}

PRIVATE
/*
 *  call-seq:
 *     dvector.select {|x| block } -> dvector
 *  
 *  Invokes the block passing in successive elements from <i>dvector</i>,
 *  returning a vector containing those elements for which the block
 *  returns a true value (equivalent to <code>Enumerable#select</code>).
 *     
 *     a = Dvector[ 1, 2, 3, 4, 5, 6 ]
 *     a.select {|x| x.modulo(2) == 0 }   -> Dvector[2, 4, 6]
 */ 
VALUE dvector_select(VALUE ary) {
   VALUE result, el;
   long i;
   Dvector *d;
   d = Get_Dvector(ary);
   result = dvector_new2(0,d->len);
   for (i = 0; i < d->len; i++) {
      el = rb_float_new(d->ptr[i]);
      if (RTEST(rb_yield(el))) {
         dvector_push(result, el);
      }
   }
   return result;
}

PRIVATE
/*
 *  call-seq:
 *     dvector.delete(number)            -> number or nil 
 *     dvector.delete(number) { block }  -> number or nil
 *  
 *  Deletes items from _dvector_ that are equal to _number_. If
 *  _number_ is not found, returns <code>nil</code>. If the optional
 *  code block is given, returns the result of <i>block</i> if the item
 *  is not found.
 *     
 *     a = Dvector.new(5) {|i| i*3 }
 *     a.delete(6)                     -> 6
 *     a                               -> Dvector[0, 3, 9, 12]
 *     a.delete(2)                     -> nil
 *     a.delete(2) { "not found" }     -> "not found"
 */ 
VALUE dvector_delete(VALUE ary, VALUE item) {
   long len, i1, i2;
   Dvector *d;
   double val, e;
   item = rb_Float(item);
   val = NUM2DBL(item);
   d = dvector_modify(ary);
   len = d->len;
   for (i1 = i2 = 0; i1 < d->len; i1++) {
      e = d->ptr[i1];
      if (e == val) continue;
      if (i1 != i2) {
         d->ptr[i2] = e;
      }
      i2++;
   }
   if (len == i2) {
      if (rb_block_given_p()) {
         return rb_yield(item);
      }
      return Qnil;
   }
   if (len > i2) {
      d->len = i2;
      if (i2 * 2 < d->capa && d->capa > DVEC_DEFAULT_SIZE) {
         REALLOC_N(d->ptr, double, i2 * 2);
         d->capa = i2 * 2;
      }
   }
   return item;
}
PRIVATE 
VALUE dvector_delete_at(VALUE ary, long pos) {
   long i, len;
   double del;
   Dvector *d;
   d = Get_Dvector(ary);
   len = d->len;
   if (pos >= len) return Qnil;
   if (pos < 0) {
      pos += len;
      if (pos < 0) return Qnil;
   }
   dvector_modify(ary);
   del = d->ptr[pos];
   for (i = pos + 1; i < len; i++, pos++) {
      d->ptr[pos] = d->ptr[i];
   }
   d->len = pos;
   return rb_float_new(del);
}

PRIVATE
/*
 *  call-seq:
 *     dvector.delete_at(int)  -> number or nil
 *  
 *  Deletes the element at the specified index _int_, returning that element,
 *  or <code>nil</code> if the index is out of range.
 *     
 *     a = Dvector.new(5) {|i| i*3 }
 *     a.delete_at(2)                   -> 6
 *     a                                -> Dvector[0, 3, 9, 12]
 *     a.delete_at(6)                   -> nil
 */ 
VALUE dvector_delete_at_m(VALUE ary, VALUE pos) {
    return dvector_delete_at(ary, NUM2LONG(pos));
}

PRIVATE
/*
 *  call-seq:
 *     dvector.prune(lst)  -> dvector
 *  
 *  Creates a new dvector without the entries given by the indexes in _lst_.
 *     
 *     a = Dvector.new(5) {|i| i*3 }    -> [0, 3, 6, 9, 12]
 *     a.prune([0, 2])                  -> [3, 9, 12]
 *     a                                -> [0, 3, 6, 9, 12]
 */ 
VALUE dvector_prune(VALUE ary, VALUE lst) {
   ary = dvector_dup(ary);
   dvector_prune_bang(ary, lst);
   return ary;
}

PRIVATE
/*
 *  call-seq:
 *     dvector.prune!(lst)  -> dvector
 *  
 *  Modifies the dvector by removing the entries given by the indexes in _lst_.
 *     
 *     a = Dvector.new(5) {|i| i*3 }    -> [0, 3, 6, 9, 12]
 *     a.prune!([0, 2])                 -> [3, 9, 12]
 *     a                                -> [3, 9, 12]
 */ 
VALUE dvector_prune_bang(VALUE ary, VALUE lst) {
   Dvector *d;
   d = dvector_modify(ary);
   int i, lst_len, ary_len, pos, j;
   VALUE *lst_ptr;
   lst = rb_Array(lst);
   lst_ptr = RARRAY_PTR(lst);
   lst_len = RARRAY_LEN(lst);
   for (i = lst_len-1; i >= 0; i--) {
      ary_len = d->len;
      pos = NUM2INT(lst_ptr[i]);  // remove this one from ary
      for (j = pos+1; j < ary_len; j++, pos++) {
         d->ptr[pos] = d->ptr[j];
      }
      d->len = pos;
   }
   return ary;
}

PRIVATE
/*
 *  call-seq:
 *     dvector.slice!(int)         -> number or nil
 *     dvector.slice!(start, length) -> sub_vector or nil
 *     dvector.slice!(range)         -> sub_vector or nil 
 *  
 *  Deletes the element(s) given by an index (optionally with a length)
 *  or by a range. Returns the deleted object, subvector, or
 *  <code>nil</code> if the index is out of range.
 *     
 *     a = Dvector.new(5) {|i| i*3 }
 *     a.slice!(1)               -> 3
 *     a                         -> Dvector[0, 6, 9, 12]
 *     a.slice!(-1)              -> 12
 *     a                         -> Dvector[0, 6, 9]
 *     a.slice!(100)             -> nil
 *     a                         -> Dvector[0, 6, 9]
 *     a.slice!(1..2)            -> Dvector[6, 9]
 *     a                         -> Dvector[0]
 *     a.slice!(1..2)            -> Dvector[]
 *     a                         -> Dvector[0]
 *     a.slice!(0..2)            -> Dvector[0]
 *     a                         -> Dvector[]
*/ 
VALUE dvector_slice_bang(int argc, VALUE *argv, VALUE ary) {
   VALUE arg1, arg2;
   long pos, len;
   Dvector *d;
   d = dvector_modify(ary);
   if (rb_scan_args(argc, argv, "11", &arg1, &arg2) == 2) {
      pos = NUM2LONG(arg1);
      len = NUM2LONG(arg2);
    delete_pos_len:
      if (pos < 0) {
         pos = d->len + pos;
      }
      arg2 = dvector_subseq(ary, pos, len);
      dvector_splice(ary, pos, len, Qnil);
      return arg2;
   }
   if (!FIXNUM_P(arg1) && rb_range_beg_len(arg1, &pos, &len, d->len, 1)) {
      goto delete_pos_len;
   }
   return dvector_delete_at(ary, NUM2LONG(arg1));
}

PRIVATE
/*
 *  call-seq:
 *     dvector.reject! {|x| block }  -> dvector or nil
 *  
 *  Equivalent to <code>Dvector#delete_if</code>, deleting elements from
 *  _dvector_ for which the block evaluates to true, but returns
 *  <code>nil</code> if no changes were made. Also see
 *  <code>Enumerable#reject</code>.
 */ 
VALUE dvector_reject_bang(VALUE ary) {
   long i1, i2;
   Dvector *d;
   d = dvector_modify(ary);
   for (i1 = i2 = 0; i1 < d->len; i1++) {
      double val = d->ptr[i1];
      VALUE v = rb_float_new(val);
      if (RTEST(rb_yield(v))) continue;
      if (i1 != i2) {
         d->ptr[i2] = val;
      }
      i2++;
   }
   if (d->len == i2) return Qnil;
   if (i2 < d->len)
      d->len = i2;
   return ary;
}

PRIVATE
/*
 *  call-seq:
 *     dvector.reject {|x| block }      -> a_dvector
 *  
 *  Returns a new vector containing the items in _dvector_,
 *  except those for which the block is <code>true</code>.
 *     
 *     a = Dvector[ 1, 2, 3, 4 ]
 *     a.reject {|x| x.modulo(2) == 0 }   -> Dvector[1, 3]
 */ 
VALUE dvector_reject(VALUE ary) {
   ary = dvector_dup(ary);
   dvector_reject_bang(ary);
   return ary;
}

PRIVATE
/*
 *  call-seq:
 *     dvector.delete_if {|x| block }  -> dvector
 *  
 *  Deletes every element of _dvector_ for which <i>block</i> evaluates
 *  to <code>true</code>.
 *     
 *     a = Dvector[ 1, 2, 3, 4 ]
 *     a.delete_if {|x| x.modulo(2) == 0 }   -> Dvector[1, 3]
 *     a                                     -> Dvector[ 1, 3 ]
 */ 
VALUE dvector_delete_if(VALUE ary) {
   dvector_reject_bang(ary);
   return ary;
}

static double *dvector_replace_dbls(VALUE ary, long len, double *data) {
   Dvector *d;
   long i;
   if (len < 0) len = 0;
   d = dvector_modify(ary);
   if (len > d->capa) {
      long new_capa = d->capa / 2;
      if (new_capa < DVEC_DEFAULT_SIZE) {
         new_capa = DVEC_DEFAULT_SIZE;
      }
      new_capa += len;
      REALLOC_N(d->ptr, double, new_capa);
      d->capa = new_capa;
   }
   d->len = len;
   for (i = 0; i < len; i++) d->ptr[i] = data[i];
   return d->ptr;
}

PRIVATE
/*
 *  call-seq:
 *     dvector.replace(other)  -> dvector
 *  
 *  Replaces the contents of _dvector_ with the contents of
 *  <i>other</i>, truncating or expanding if necessary.
 *     
 *     a = Dvector[ 1, 2, 3, 4, 5 ]
 *     a.replace(Dvector[ -1, -2, -3 ])   -> Dvector[ -1, -2, -3 ]
 *     a                               -> Dvector[ -1, -2, -3 ]
 */ 
VALUE dvector_replace(VALUE dest, VALUE orig) {
   VALUE shared;
   Dvector *org, *d;
   dvector_modify(dest); // take care of any sharing issues.
   orig = dvector_to_dvector(orig); /* it might be some kind of Array rather than a Dvector */
   if (dest == orig) return dest;
   org = Get_Dvector(orig);
   d = Get_Dvector(dest);
   if (d->ptr) free(d->ptr); // we know it isn't shared because we did dvector_modify above
   shared = dvector_make_shared(orig);
   org = Get_Dvector(shared);
   d->ptr = org->ptr;
   d->len = org->len;
   d->shared = shared;
   return dest;
}

PRIVATE
/*
 *  call-seq:
 *     dvector.fill(number)                             -> dvector
 *     dvector.fill(number, start <, length>)           -> dvector
 *     dvector.fill(number, range )                     -> dvector
 *     dvector.fill {|index| block }                    -> dvector
 *     dvector.fill(start <, length> ) {|index| block } -> dvector
 *     dvector.fill(range) {|index| block }             -> dvector
 *  
 *  The first three forms set the selected elements of _dvector_ (which
 *  may be the entire vector) to <i>number</i>. A <i>start</i> of
 *  <code>nil</code> is equivalent to zero. A <i>length</i> of
 *  <code>nil</code> is equivalent to _dvector_.<code>length</code>. The last three
 *  forms fill the vector with the value of the block. The block is
 *  passed the absolute index of each element to be filled.
 *     
 *     a = Dvector[ 1, 2, 3, 4, 5 ]
 *     a.fill(-1)               -> Dvector[ -1, -1, -1, -1, -1 ]
 *     a.fill(7, 2, 2)          -> Dvector[ -1, -1, 7, 7, -1 ]
 *     a.fill(8, 0..1)          -> Dvector[ 8, 8, 7, 7, -1 ]
 *     a.fill {|i| i*i}         -> Dvector[ 0, 1, 4, 9, 16 ]
 *     a.fill(-2) {|i| i*i*i}   -> Dvector[ 0, 1, 4, 27, 64 ]
 *     a                        -> Dvector[ 0, 1, 4, 27, 64 ]
 */ 
VALUE dvector_fill(int argc, VALUE *argv, VALUE ary) {
   Dvector *d;
   VALUE item, arg1, arg2;
   long beg, end, len, i;
   double *p, *pend;
   int block_p = Qfalse;
   double v;
   d = dvector_modify(ary);
   if (rb_block_given_p()) {
      block_p = Qtrue;
      rb_scan_args(argc, argv, "02", &arg1, &arg2);
      argc += 1;		/* hackish */
   } else {
      rb_scan_args(argc, argv, "12", &item, &arg1, &arg2);
   }
   switch (argc) {
      case 1:
         beg = 0;
         len = d->len;
         break;
      case 2:
         if (rb_range_beg_len(arg1, &beg, &len, d->len, 1)) {
            break;
         }
      /* fall through */
      case 3:
         beg = NIL_P(arg1) ? 0 : NUM2LONG(arg1);
         if (beg < 0) {
            beg = d->len + beg;
            if (beg < 0) beg = 0;
         }
         len = NIL_P(arg2) ? d->len - beg : NUM2LONG(arg2);
         break;
   }
   end = beg + len;
   if (end > d->len) {
      if (end >= d->capa) {
         REALLOC_N(d->ptr, double, end);
         d->capa = end;
      }
      if (beg > d->len) {
         dvector_mem_clear(d->ptr + d->len, end - d->len);
      }
      d->len = end;
   }
   if (block_p) {
      for (i=beg; i<end; i++) {
         item = rb_Float(rb_yield(LONG2NUM(i)));
         d->ptr[i] = NUM2DBL(item);
      }
   }
   else {
      p = d->ptr + beg;
      pend = p + len;
      item = rb_Float(item);
      v = NUM2DBL(item);
      while (p < pend) {
         *p++ = v;
      }
   }
   return ary;
}

static VALUE c_Resize(VALUE ary, long new_len) {
   Dvector *d = dvector_modify(ary);
   if (new_len > d->capa) {
      Dvector_Store_Double(ary, new_len-1, 0.0);
   } else {
      d->len = new_len;
      if (new_len < DVEC_DEFAULT_SIZE) new_len = DVEC_DEFAULT_SIZE;
      if (new_len * 2 < d->capa) {
         REALLOC_N(d->ptr, double, new_len * 2);
         d->capa = new_len * 2;
      }
   }
   return ary;
}

PRIVATE
/*
 *  call-seq:
 *     dvector.resize(new_size)  -> dvector
 *
 *  Modifies _dvector_ to have the requested size by truncating or expanding with trailing zeros.
 *
 *  a = Dvector[1, 5, -3]
 *  a.resize(5)            -> Dvector[ 1, 5, -3, 0, 0 ]  
 *  a                      -> Dvector[ 1, 5, -3, 0, 0 ]  
 *  a.resize(2)            -> Dvector[ 1, 5 ]  
 *  a                      -> Dvector[ 1, 5 ]  
 */ 
VALUE dvector_resize(VALUE ary, VALUE len) {
   len = rb_Integer(len);
   long new_size = NUM2INT(len);
   return c_Resize(ary, new_size);
}

PRIVATE
/*
 *  call-seq:
 *     dvector.concat(other)  -> dvector
 *
 *  Appends the elements in _other_ to dvector.  _other_  can either be a Dvector or a 1D Array of numbers.
 *
 *  a = Dvector[1, 5, -3]
 *  a.concat([6, 7])       -> Dvector[ 1, 5, -3, 6, 7 ]  
 *  a                      -> Dvector[ 1, 5, -3, 6, 7 ]  
 */ 
VALUE dvector_concat(VALUE x, VALUE y) {
   Dvector *c, *d;
   y = dvector_to_dvector(y);
   c = Get_Dvector(x);
   d = Get_Dvector(y);
   if (d->len > 0) {
      dvector_splice(x, c->len, 0, y);
   }
   return x;
}

PRIVATE
/*
 *  call-seq:
 *     dvector.eql?(other)  -> true or false
 *
 *  Returns <code>true</code> if _dvector_ and _other_ have the same content.  _other_  can either be a Dvector or a 1D Array of numbers.
 */ 
VALUE dvector_eql(VALUE ary1, VALUE ary2) {
   long i, len;
   Dvector *d1, *d2;
   double *p1, *p2;
   if (ary1 == ary2) return Qtrue;
   if (ary1 == Qnil || ary2 == Qnil) return Qfalse;
   d1 = Get_Dvector(ary1);
   d2 = Get_Dvector(ary2);
   len = d1->len;
   if (len != d2->len) return Qfalse;
   p1 = d1->ptr; p2 = d2->ptr;
   for (i=0; i < len; i++) {
      if (*p1++ != *p2++) return Qfalse;
   }
   return Qtrue;
}

PRIVATE
/*
 *  call-seq:
 *  dvector.include?(number)   -> true or false
 *  
 *  Returns <code>true</code> if _number_ is present in
 *  _dvector_, <code>false</code> otherwise.
 *     
 *     a = Dvector[ 1, 2, 3 ]
 *     a.include?(2)   -> true
 *     a.include?(0)   -> false
 */ 
VALUE dvector_includes(VALUE ary, VALUE item) {
   Dvector *d = Get_Dvector(ary);
   long i, len = d->len;
   double x, *p = d->ptr;
   item = rb_Float(item);
   x = NUM2DBL(item);
   for (i=0; i < len; i++) {
      if (*p++ == x) return Qtrue;
   }
   return Qfalse;
}

PRIVATE
/* 
 *  call-seq:
 *     dvector <=> other   ->  -1, 0, +1
 *
 *  Comparison---Returns an integer (-1, 0,
 *  or +1) if this vector is less than, equal to, or greater than
 *  _other_.  Two vectors are
 *  ``equal'' according to <code>Dvector#<=></code> if and only if they have
 *  the same length and contain exactly the same values.
 *  
 *  
 *     Dvector[ 1, 1, 2 ] <=> Dvector[ 1, 2, 3 ]        -> -1
 *     Dvector[ 1, 1, 2 ] <=> Dvector[ 1, 0, 3 ]        -> +1
 *     Dvector[ 1, 1, 2 ] <=> Dvector[ 1, 1, 2, 3 ]     -> -1
 *     Dvector[ 1, 2, 3, 4, 5, 6 ] <=> Dvector[ 1, 2 ]  -> +1
 *
 */ 
VALUE dvector_cmp(VALUE ary1, VALUE ary2) {
   long i, len;
   Dvector *d1, *d2;
   double *p1, *p2, v1, v2;
   d1 = Get_Dvector(ary1);
   d2 = Get_Dvector(ary2);
   len =d1->len;
   if (len > d2->len) {
      len = d2->len;
   }
   p1 = d1->ptr; p2 = d2->ptr;
   for (i=0; i<len; i++) {
      v1 = *p1++; v2 = *p2++;
      if (v1 < v2) return INT2FIX(-1);
      if (v1 > v2) return INT2FIX(1);
   }
   len = d1->len - d2->len;
   if (len == 0) return INT2FIX(0);
   if (len > 0) return INT2FIX(1);
   return INT2FIX(-1);
}

PRIVATE
/*
 *  call-seq:
 *     dvector.where_max         ->  int or nil
 *     dvector.where_first_max   ->  int or nil
 *  
 *  Returns the index of the first entry with the maximum value in _dvector_. Returns <code>nil</code> if
 *  _dvector_ is empty.
 *     
 *     a = Dvector[ 1, 2, 3, 4, 5, 4, 3, 5, 2 ]
 *     a.where_max        -> 4
 *     Dvector[].where_max   -> nil
 */ 
VALUE dvector_where_max(VALUE ary) {
   Dvector *d = Get_Dvector(ary);
   double *p = d->ptr, bst;
   long len = d->len, i, bst_i;
   if (len <= 0) return Qnil;
   bst = p[0];
   bst_i = 0;
   for (i=1; i<len; i++) {
      if (p[i] > bst) { bst_i = i; bst = p[i]; }
   }
   return INT2FIX(bst_i);   
}

PRIVATE
/*
 *  call-seq:
 *     dvector.where_last_max   ->  int or nil
 *  
 *  Returns the index of the last entry with the maximum value in _dvector_. Returns <code>nil</code> if
 *  _dvector_ is empty.
 *     
 *     a = Dvector[ 1, 2, 3, 4, 5, 4, 3, 5, 2 ]
 *     a.where_last_max        -> 7
 *     Dvector[].where_last_max   -> nil
 */ 
VALUE dvector_where_last_max(VALUE ary) {
   Dvector *d = Get_Dvector(ary);
   double *p = d->ptr, bst;
   long len = d->len, i, bst_i;
   if (len <= 0) return Qnil;
   bst_i = len-1;
   bst = p[bst_i];
   for (i=bst_i-1; i>=0; i--) {
      if (p[i] > bst) { bst_i = i; bst = p[i]; }
   }
   return INT2FIX(bst_i);   
}

static double c_dvector_max(Dvector *d)
{
   double bst, *p = d->ptr;
   long len = d->len, i;
   if (len <= 0) return 0.0;
   bst = p[0];
   for (i=1; i<len; i++) {
      if (p[i] > bst) bst = p[i];
   }
   return bst;   
}

PRIVATE
/*
 *  call-seq:
 *     dvector.max   ->  number or nil
 *     dvector.max(dvector2, ...)   ->  number or nil
 *  
 *  First form returns the entry with the maximum value in _dvector_, <code>nil</code> if
 *  _dvector_ is empty.  Second form returns maximum of all the vectors (or <code>nil</code> if all are empty).
 *     
 *     a = Dvector[ 1, 2, 3, 4, 5, 4, 3, 5, 2 ]
 *     a.max               -> 5
 *     Dvector[].max          -> nil
 *     b = Dvector[ 8, 3, 0, 7 ]
 *     a.max(b)            -> 8
 */ 
VALUE dvector_max(int argc, VALUE *argv, VALUE self) {
   VALUE ary, index;
   int i, got_one = false;
   double mx=0, tmp;
   Dvector *d;
   for (i = 0; i <= argc; i++) {
      ary = (i < argc)? argv[i] : self;
      index = dvector_where_max(ary);
      if (index == Qnil) continue;
      d = Get_Dvector(ary);
      tmp = d->ptr[NUM2INT(index)];
      if (!got_one || tmp > mx) { mx = tmp; got_one = true; }
   }
   if (!got_one) return Qnil;
   return rb_float_new(mx);
}

PRIVATE
/*
 *  call-seq:
 *     dvector.where_min         ->  int or nil
 *     dvector.where_first_min   ->  int or nil
 *  
 *  Returns the index of the first entry with the minimum value in _dvector_, <code>nil</code> if
 *  _dvector_ is empty.
 *     
 *     a = Dvector[ 1, 2, -3, 4, -5, 4, 3, -5, 2 ]
 *     a.where_min        -> 4
 *     Dvector[].where_min   -> nil
 */ 
VALUE dvector_where_min(VALUE ary) {
   Dvector *d = Get_Dvector(ary);
   double bst, *p = d->ptr;
   long len = d->len, i, bst_i;
   if (len <= 0) return Qnil;
   bst = p[0];
   bst_i = 0;
   for (i=1; i<len; i++) {
      if (p[i] < bst) { bst_i = i; bst = p[i]; }
   }
   return INT2FIX(bst_i);   
}

static double c_dvector_min(Dvector *d)
{
   double bst, *p = d->ptr;
   long len = d->len, i;
   if (len <= 0) return 0.0;
   bst = p[0];
   for (i=1; i<len; i++) {
      if (p[i] < bst) bst = p[i];
   }
   return bst;   
}

PRIVATE
/*
 *  call-seq:
 *     dvector.min   ->  number or nil
 *     dvector.min(dvector2, ...)   ->  number or nil
 *  
 *  First form returns the entry with the minimum value in _dvector_, <code>nil</code> if
 *  _dvector_ is empty.  Second form returns minimum of all the vectors (or <code>nil</code> if all are empty).
 *     
 *     a = Dvector[ 1, 2, 3, 4, 5, 4, 3, 5, 2 ]
 *     a.min               -> 1
 *     Dvector[].min          -> nil
 *     b = Dvector[ 8, 3, 0, 7 ]
 *     a.min(b)            -> 0
 */ 
VALUE dvector_min(int argc, VALUE *argv, VALUE self) {
   VALUE ary, index;
   int i, got_one = false;
   double mn=0, tmp;
   Dvector *d;
   for (i = 0; i <= argc; i++) {
      ary = (i < argc)? argv[i] : self;
      index = dvector_where_min(ary);
      if (index == Qnil) continue;
      d = Get_Dvector(ary);
      tmp = d->ptr[NUM2INT(index)];
      if (!got_one || tmp < mn) { mn = tmp; got_one = true; }
   }
   if (!got_one) return Qnil;
   return rb_float_new(mn);
}

PRIVATE
/*
 *  call-seq:
 *     dvector.where_last_min   ->  int or nil
 *  
 *  Returns the index of the last entry with the minimum value in _dvector_, <code>nil</code> if
 *  _dvector_ is empty.
 *     
 *     a = Dvector[ 1, 2, -3, 4, -5, 4, 3, -5, 2 ]
 *     a.where_last_min         -> 7
 *     Dvector[].where_last_min    -> nil
 */ 
VALUE dvector_where_last_min(VALUE ary) {
   Dvector *d = Get_Dvector(ary);
   double *p = d->ptr, bst;
   long len = d->len, i, bst_i;
   if (len <= 0) return Qnil;
   bst_i = len-1;
   bst = p[bst_i];
   for (i=bst_i-1; i>=0; i--) {
      if (p[i] < bst) { bst_i = i; bst = p[i]; }
   }
   return INT2FIX(bst_i);   
}

PRIVATE
/*
 *  call-seq:
 *     dvector.where_closest(number)         ->  int or nil
 *     dvector.where_first_closest(number)   ->  int or nil
 *  
 *  Returns the index of the first entry in _dvector_ with value closest to _number_, <code>nil</code> if
 *  _dvector_ is empty.
 *     
 *     a = Dvector[ 1, 2, -3, 4, -5, 4, 3, -5, 2 ]
 *     a.where_closest(3.9)        -> 3
 *     Dvector[].where_closest(3.9)   -> nil
 */ 
VALUE dvector_where_closest(VALUE ary, VALUE item) {
   Dvector *d = Get_Dvector(ary);
   item = rb_Float(item);
   double x = NUM2DBL(item), *p = d->ptr, tmp, bst;
   long len = d->len, i, bst_i;
   if (len <= 0) return Qnil;
   bst = fabs(p[0]-x);
   if (bst == 0.0) return INT2FIX(0);
   bst_i = 0;
   for (i=1; i<len; i++) {
      tmp = p[i] - x;
      if (tmp == 0.0) return INT2FIX(i);
      if (tmp < 0.0) tmp = -tmp;
      if (tmp < bst) { bst_i = i; bst = tmp; }
   }
   return INT2FIX(bst_i);   
}

PRIVATE
/*
 *  call-seq:
 *     dvector.where_last_closest(number)   ->  int or nil
 *  
 *  Returns the index of the last entry in _dvector_ with value closest to _number_, <code>nil</code> if
 *  _dvector_ is empty.
 *     
 *     a = Dvector[ 1, 2, -3, 4, -5, 4, 3, -5, 2 ]
 *     a.where_last_closest(3.9)         -> 5
 *     Dvector[].where_last_closest(3.9)    -> nil
 */ 
VALUE dvector_where_last_closest(VALUE ary, VALUE item) {
   Dvector *d = Get_Dvector(ary);
   item = rb_Float(item);
   double x = NUM2DBL(item), *p = d->ptr, tmp, bst;
   long len = d->len, i, bst_i;
   if (len <= 0) return Qnil;
   bst_i = len-1;
   bst = fabs(p[bst_i]-x);
   if (bst == 0.0) return INT2FIX(bst_i);
   for (i=bst_i-1; i>=0; i--) {
      tmp = p[i] - x;
      if (tmp == 0.0) return INT2FIX(i);
      if (tmp < 0.0) tmp = -tmp;
      if (tmp < bst) { bst_i = i; bst = tmp; }
   }
   return INT2FIX(bst_i);   
}

PRIVATE
/*
 *  call-seq:
 *     dvector.where_eq(number)         ->  int or nil
 *     dvector.where_first_eq(number)   ->  int or nil
 *  
 *  Returns the index of the first entry in _dvector_ with value equal to _number_, <code>nil</code> if
 *  _dvector_ has no such entry.
 *     
 *     a = Dvector[ 1, 2, -3, 4, -5, 4, 3, -5, 2 ]
 *     a.where_eq(4)        -> 3
 *     a.where_eq(6)        -> nil
 *     Dvector[].where_eq(4)   -> nil
 */ 
VALUE dvector_where_first_eq(VALUE ary, VALUE item) {
   Dvector *d = Get_Dvector(ary);
   item = rb_Float(item);
   double x = NUM2DBL(item), *p = d->ptr;
   long len = d->len, i;
   if (len <= 0) return Qnil;
   for (i=0; i<len; i++) {
      if (p[i] == x) return INT2FIX(i);
   }
   return Qnil;   
}

PRIVATE
/*
 *  call-seq:
 *     dvector.where_ne(number)         ->  int or nil
 *     dvector.where_first_ne(number)   ->  int or nil
 *  
 *  Returns the index of the first entry in _dvector_ with value not equal to _number_, <code>nil</code> if
 *  _dvector_ has no such entry.
 *     
 *     a = Dvector[ 1, 2, -3, 4, -5, 4, 3, -5, 2 ]
 *     a.where_ne(1)        -> 1
 *     Dvector[].where_ne(4)   -> nil
 */ 
VALUE dvector_where_first_ne(VALUE ary, VALUE item) {
   Dvector *d = Get_Dvector(ary);
   item = rb_Float(item);
   double x = NUM2DBL(item), *p = d->ptr;
   long len = d->len, i;
   if (len <= 0) return Qnil;
   for (i=0; i<len; i++) {
      if (p[i] != x) return INT2FIX(i);
   }
   return Qnil;   
}

PRIVATE
/*
 *  call-seq:
 *     dvector.where_lt(number)          ->  int or nil
 *     dvector.where_first_lt(number)    ->  int or nil
 *  
 *  Returns the index of the first entry in _dvector_ with value less than _number_, <code>nil</code> if
 *  _dvector_ has no such entry.
 *     
 *     a = Dvector[ 1, 2, -3, 4, -5, 4, 3, -5, 2 ]
 *     a.where_lt(1)        -> 2
 *     a.where_lt(-5)       -> nil
 *     Dvector[].where_lt(4)   -> nil
 */ 
VALUE dvector_where_first_lt(VALUE ary, VALUE item) {
   Dvector *d = Get_Dvector(ary);
   item = rb_Float(item);
   double x = NUM2DBL(item), *p = d->ptr;
   long len = d->len, i;
   if (len <= 0) return Qnil;
   for (i=0; i<len; i++) {
      if (p[i] < x) return INT2FIX(i);
   }
   return Qnil;   
}

PRIVATE
/*
 *  call-seq:
 *     dvector.where_le(number)         ->  int or nil
 *     dvector.where_first_le(number)   ->  int or nil
 *  
 *  Returns the index of the first entry in _dvector_ with value less than or equal _number_, <code>nil</code> if
 *  _dvector_ has no such entry.
 *     
 *     a = Dvector[ 1, 2, -3, 4, -5, 4, 3, -5, 2 ]
 *     a.where_le(0)        -> 2
 *     a.where_le(-5.5)     -> nil
 *     Dvector[].where_le(4)   -> nil
 */ 
VALUE dvector_where_first_le(VALUE ary, VALUE item) {
   Dvector *d = Get_Dvector(ary);
   item = rb_Float(item);
   double x = NUM2DBL(item), *p = d->ptr;
   long len = d->len, i;
   if (len <= 0) return Qnil;
   for (i=0; i<len; i++) {
      if (p[i] <= x) return INT2FIX(i);
   }
   return Qnil;   
}

PRIVATE
/*
 *  call-seq:
 *     dvector.where_gt(number)         ->  int or nil
 *     dvector.where_first_gt(number)   ->  int or nil
 *  
 *  Returns the index of the first entry in _dvector_ with value greater than _number_, <code>nil</code> if
 *  _dvector_ has no such entry.
 *     
 *     a = Dvector[ 1, 2, -3, 4, -5, 4, 3, -5, 2 ]
 *     a.where_gt(3)        -> 3
 *     a.where_gt(4)        -> nil
 *     Dvector[].where_gt(0)   -> nil
 */ 
VALUE dvector_where_first_gt(VALUE ary, VALUE item) {
   Dvector *d = Get_Dvector(ary);
   item = rb_Float(item);
   double x = NUM2DBL(item), *p = d->ptr;
   long len = d->len, i;
   if (len <= 0) return Qnil;
   for (i=0; i<len; i++) {
      if (p[i] > x) return INT2FIX(i);
   }
   return Qnil;   
}

PRIVATE
/*
 *  call-seq:
 *     dvector.where_ge(number)         ->  int or nil
 *     dvector.where_first_ge(number)   ->  int or nil
 *  
 *  Returns the index of the first entry in _dvector_ with value greater than or equal _number_, <code>nil</code> if
 *  _dvector_ has no such entry.
 *     
 *     a = Dvector[ 1, 2, -3, 4, -5, 4, 3, -5, 2 ]
 *     a.where_ge(3)        -> 3
 *     a.where_ge(5)        -> nil
 *     Dvector[].where_ge(0)   -> nil
 */ 
VALUE dvector_where_first_ge(VALUE ary, VALUE item) {
   Dvector *d = Get_Dvector(ary);
   item = rb_Float(item);
   double x = NUM2DBL(item), *p = d->ptr;
   long len = d->len, i;
   if (len <= 0) return Qnil;
   for (i=0; i<len; i++) {
      if (p[i] >= x) return INT2FIX(i);
   }
   return Qnil;   
}

PRIVATE
/*
 *  call-seq:
 *     dvector.where_last_eq(number)   ->  int or nil
 *  
 *  Returns the index of the last entry in _dvector_ with value equal _number_, <code>nil</code> if
 *  _dvector_ has no such entry.
 *     
 *     a = Dvector[ 1, 2, -3, 4, -5, 4, 3, -5, 2 ]
 *     a.where_last_eq(2)         -> 8
 *     a.where_last_eq(5)         -> nil
 *     Dvector[].where_last_eq(0)    -> nil
 */ 
VALUE dvector_where_last_eq(VALUE ary, VALUE item) {
   Dvector *d = Get_Dvector(ary);
   item = rb_Float(item);
   double x = NUM2DBL(item), *p = d->ptr;
   long len = d->len, i;
   if (len <= 0) return Qnil;
   for (i=len-1; i>=0; i--) {
      if (p[i] == x) return INT2FIX(i);
   }
   return Qnil;   
}

PRIVATE
/*
 *  call-seq:
 *     dvector.where_last_ne(number)   ->  int or nil
 *  
 *  Returns the index of the last entry in _dvector_ with value not equal _number_, <code>nil</code> if
 *  _dvector_ has no such entry.
 *     
 *     a = Dvector[ 1, 2, -3, 4, -5, 4, 3, -5, 2 ]
 *     a.where_last_ne(2)        -> 7
 *     Dvector[0].where_last_ne(0)  -> nil
 *     Dvector[].where_last_ne(0)   -> nil
 */ 
VALUE dvector_where_last_ne(VALUE ary, VALUE item) {
   Dvector *d = Get_Dvector(ary);
   item = rb_Float(item);
   double x = NUM2DBL(item), *p = d->ptr;
   long len = d->len, i;
   if (len <= 0) return Qnil;
   for (i=len-1; i>=0; i--) {
      if (p[i] != x) return INT2FIX(i);
   }
   return Qnil;   
}

PRIVATE
/*
 *  call-seq:
 *     dvector.where_last_lt(number)   ->  int or nil
 *  
 *  Returns the index of the last entry in _dvector_ with value less than _number_, <code>nil</code> if
 *  _dvector_ has no such entry.
 *     
 *     a = Dvector[ 1, 2, -3, 4, -5, 4, 3, -5, 2 ]
 *     a.where_last_lt(2)        -> 7
 *     a.where_last_lt(-5)       -> nil
 *     Dvector[].where_last_lt(0)   -> nil
 */ 
VALUE dvector_where_last_lt(VALUE ary, VALUE item) {
   Dvector *d = Get_Dvector(ary);
   item = rb_Float(item);
   double x = NUM2DBL(item), *p = d->ptr;
   long len = d->len, i;
   if (len <= 0) return Qnil;
   for (i=len-1; i>=0; i--) {
      if (p[i] < x) return INT2FIX(i);
   }
   return Qnil;   
}

PRIVATE
/*
 *  call-seq:
 *     dvector.where_last_le(number)   ->  int or nil
 *  
 *  Returns the index of the last entry in _dvector_ with value less than or equal _number_, <code>nil</code> if
 *  _dvector_ has no such entry.
 *     
 *     a = Dvector[ 1, 2, -3, 4, -5, 4, 3, -5, 2 ]
 *     a.where_last_le(1)        -> 7
 *     a.where_last_le(-6)       -> nil
 *     Dvector[].where_last_le(0)   -> nil
 */ 
VALUE dvector_where_last_le(VALUE ary, VALUE item) {
   Dvector *d = Get_Dvector(ary);
   item = rb_Float(item);
   double x = NUM2DBL(item), *p = d->ptr;
   long len = d->len, i;
   if (len <= 0) return Qnil;
   for (i=len-1; i>=0; i--) {
      if (p[i] <= x) return INT2FIX(i);
   }
   return Qnil;   
}

PRIVATE
/*
 *  call-seq:
 *     dvector.where_last_gt(number)   ->  int or nil
 *  
 *  Returns the index of the last entry in _dvector_ with value greater than _number_, <code>nil</code> if
 *  _dvector_ has no such entry.
 *     
 *     a = Dvector[ 1, 2, -3, 4, -5, 4, 3, -5, 2 ]
 *     a.where_last_gt(2)        -> 6
 *     a.where_last_gt(4)        -> nil
 *     Dvector[].where_last_gt(0)   -> nil
 */ 
VALUE dvector_where_last_gt(VALUE ary, VALUE item) {
   Dvector *d = Get_Dvector(ary);
   item = rb_Float(item);
   double x = NUM2DBL(item), *p = d->ptr;
   long len = d->len, i;
   if (len <= 0) return Qnil;
   for (i=len-1; i>=0; i--) {
      if (p[i] > x) return INT2FIX(i);
   }
   return Qnil;   
}

PRIVATE
/*
 *  call-seq:
 *     dvector.where_last_ge(number)   ->  int or nil
 *  
 *  Returns the index of the last entry in _dvector_ with value greater than or equal _number_, <code>nil</code> if
 *  _dvector_ has no such entry.
 *     
 *     a = Dvector[ 1, 2, -3, 4, -5, 4, 3, -5, 2 ]
 *     a.where_last_ge(4)        -> 5
 *     a.where_last_ge(5)        -> nil
 *     Dvector[].where_last_ge(0)   -> nil
 */ 
VALUE dvector_where_last_ge(VALUE ary, VALUE item) {
   Dvector *d = Get_Dvector(ary);
   item = rb_Float(item);
   double x = NUM2DBL(item), *p = d->ptr;
   long len = d->len, i;
   if (len <= 0) return Qnil;
   for (i=len-1; i>=0; i--) {
      if (p[i] >= x) return INT2FIX(i);
   }
   return Qnil;   
}

PRIVATE
/*
 *  call-seq:
 *     dvector.sum   ->  number
 *  
 *  Returns the sum of the entries in _dvector_. Returns 0.0 if
 *  _dvector_ is empty.
 *     
 *     a = Dvector[ 1, 2, 3, 4 ]
 *     a.sum        -> 10
 *     Dvector[].sum   -> 0
 */ 
VALUE dvector_sum(VALUE ary) {
   Dvector *d = Get_Dvector(ary);
   double *p = d->ptr, sum = 0.0;
   long len = d->len, i;
   for (i=0; i<len; i++) sum += p[i];
   return rb_float_new(sum);
}

PRIVATE
/*
 *  call-seq:
 *     dvector.dot(other)   ->  number
 *  
 *  Returns the sum of the products of entries in _dvector_ and _other_. Returns 0.0 if
 *  _dvector_ is empty.  The vectors must be the same length.
 *     
 *     a = Dvector[ 1, 2, 3, 4 ]
 *     b = Dvector[ 1, -3, 3, 0 ]
 *     a.dot(b)        -> 4
 *     Dvector[].dot(b)   -> 0
 */ 
VALUE dvector_dot(VALUE ary1, VALUE ary2) {
   Dvector *d1 = Get_Dvector(ary1), *d2 = Get_Dvector(ary2);
   double *p1 = d1->ptr, *p2 = d2->ptr, sum = 0.0;
   long len = d1->len, i;
   if (len != d2->len) {
      rb_raise(rb_eArgError, "vectors with different lengths (%ld vs %ld) for dot", d1->len, d2->len);
   }
   for (i=0; i<len; i++) sum += p1[i] * p2[i];
   return rb_float_new(sum);
}

PRIVATE
/*
 *  call-seq:
 *     dvector.vector_length   ->  number
 *  
 *  Returns square root of the dot product of the vector with itself.
 *     
 *     a = Dvector[ 3, 4 ]
 *     a.vector_length -> 5.0
 */ 
VALUE dvector_vector_length(VALUE ary) {
   Dvector *d = Get_Dvector(ary);
   double *p = d->ptr, sum = 0.0;
   long len = d->len, i;
   for (i=0; i<len; i++) sum += p[i] * p[i];
   return rb_float_new(sqrt(sum));
}
PRIVATE 
VALUE dvector_apply_math_op_bang(VALUE ary, double (*op)(double)) {
   Dvector *d= dvector_modify(ary);
   double *p = d->ptr;
   long len = d->len, i;
   for (i=0; i<len; i++) p[i] = (*op)(p[i]);
   return ary;
}
PRIVATE 
VALUE dvector_apply_math_op(VALUE source, double (*op)(double)) {
   return dvector_apply_math_op_bang(dvector_dup(source), op);
}

static double do_neg(double arg) { return -arg; }

PRIVATE
/*
 *  call-seq:
 *     dvector.neg   ->  a_dvector
 *  
 *  Returns of copy of _dvector_ with each entry x replaced by -x.
 *     
 *     a = Dvector[ 1, 2, 3, 4 ]
 *     a.neg   -> Dvector[ -1, -2, -3, -4 ]
 */ 
VALUE dvector_neg(VALUE ary) {
   return dvector_apply_math_op(ary, do_neg);
}

PRIVATE
/*
 *  call-seq:
 *     dvector.abs   ->  a_dvector
 *  
 *  Returns of copy of _dvector_ with all entries replaced by their absolute values.
 *     
 *     a = Dvector[ 1, -2, -3, 4 ]
 *     a.abs   -> Dvector[ 1, 2, 3, 4 ]
 */ 
VALUE dvector_abs(VALUE ary) {
   return dvector_apply_math_op(ary, fabs);
}

PRIVATE
/*
 *  call-seq:
 *     dvector.sin   ->  a_dvector
 *  
 *  Returns of copy of _dvector_ with entry x replaced by sin(x).
 *     
 *     a = Dvector[ 1, -2, -3, 4 ]
 *     a.sin   -> Dvector[ sin(1), sin(-2), sin(-3), sin(4) ]
 */ 
VALUE dvector_sin(VALUE ary) {
   return dvector_apply_math_op(ary, sin);
}

PRIVATE
/*
 *  call-seq:
 *     dvector.cos   ->  a_dvector
 *  
 *  Returns of copy of _dvector_ with entry x replaced by cos(x).
 *     
 *     a = Dvector[ 1, -2, -3, 4 ]
 *     a.cos   -> Dvector[ cos(1), cos(-2), cos(-3), cos(4) ]
 */ 
VALUE dvector_cos(VALUE ary) {
   return dvector_apply_math_op(ary, cos);
}

PRIVATE
/*
 *  call-seq:
 *     dvector.tan   ->  a_dvector
 *  
 *  Returns of copy of _dvector_ with entry x replaced by tan(x).
 *     
 *     a = Dvector[ 1, -2, -3, 4 ]
 *     a.tan   -> Dvector[ tan(1), tan(-2), tan(-3), tan(4) ]
 */ 
VALUE dvector_tan(VALUE ary) {
   return dvector_apply_math_op(ary, tan);
}

PRIVATE
/*
 *  call-seq:
 *     dvector.asin   ->  a_dvector
 *  
 *  Returns of copy of _dvector_ with entry x replaced by asin(x).
 *     
 *     a = Dvector[ 0.1, -0.2, -0.3, 0.4 ]
 *     a.asin   -> Dvector[ asin(0.1), asin(-0.2), asin(-0.3), asin(0.4) ]
 */ 
VALUE dvector_asin(VALUE ary) {
   return dvector_apply_math_op(ary, asin);
}

PRIVATE
/*
 *  call-seq:
 *     dvector.acos   ->  a_dvector
 *  
 *  Returns of copy of _dvector_ with entry x replaced by acos(x).
 *     
 *     a = Dvector[ 0.1, -0.2, -0.3, 0.4 ]
 *     a.acos   -> Dvector[ acos(0.1), acos(-0.2), acos(-0.3), acos(0.4) ]
 */ 
VALUE dvector_acos(VALUE ary) {
   return dvector_apply_math_op(ary, acos);
}

PRIVATE
/*
 *  call-seq:
 *     dvector.atan   ->  a_dvector
 *  
 *  Returns of copy of _dvector_ with entry x replaced by atan(x).
 *     
 *     a = Dvector[ 0.1, -0.2, -0.3, 0.4 ]
 *     a.atan   -> Dvector[ atan(0.1), atan(-0.2), atan(-0.3), atan(0.4) ]
 */ 
VALUE dvector_atan(VALUE ary) {
   return dvector_apply_math_op(ary, atan);
}

PRIVATE
/*
 *  call-seq:
 *     dvector.sinh   ->  a_dvector
 *  
 *  Returns of copy of _dvector_ with entry x replaced by sinh(x).
 *     
 *     a = Dvector[ 0.1, -0.2, 0.3 ]
 *     a.sinh   -> Dvector[ sinh(0.1), sinh(-0.2), sinh(0.3) ]
 */ 
VALUE dvector_sinh(VALUE ary) {
   return dvector_apply_math_op(ary, sinh);
}

PRIVATE
/*
 *  call-seq:
 *     dvector.cosh   ->  a_dvector
 *  
 *  Returns of copy of _dvector_ with entry x replaced by cosh(x).
 *     
 *     a = Dvector[ 0.1, -0.2, 0.3 ]
 *     a.cosh   -> Dvector[ cosh(0.1), cosh(-0.2), cosh(0.3) ]
 */ 
VALUE dvector_cosh(VALUE ary) {
   return dvector_apply_math_op(ary, cosh);
}

PRIVATE
/*
 *  call-seq:
 *     dvector.tanh   ->  a_dvector
 *  
 *  Returns of copy of _dvector_ with entry x replaced by tanh(x).
 *     
 *     a = Dvector[ 0.1, -0.2, 0.3 ]
 *     a.tanh   -> Dvector[ tanh(0.1), tanh(-0.2), tanh(0.3) ]
 */ 
VALUE dvector_tanh(VALUE ary) {
   return dvector_apply_math_op(ary, tanh);
}

static double do_asinh(double x) { return log(x + sqrt(x*x+1.0)); }

PRIVATE
/*
 *  call-seq:
 *     dvector.asinh   ->  a_dvector
 *  
 *  Returns of copy of _dvector_ with entry x replaced by asinh(x).
 *     
 *     a = Dvector[ 0.1, -0.2, 0.3 ]
 *     a.asinh   -> Dvector[ asinh(0.1), asinh(-0.2), asinh(0.3) ]
 */ 
VALUE dvector_asinh(VALUE ary) {
   return dvector_apply_math_op(ary, do_asinh);
}

static double do_acosh(double x) { return log(x + sqrt(x*x-1.0)); }

PRIVATE
/*
 *  call-seq:
 *     dvector.acosh   ->  a_dvector
 *  
 *  Returns of copy of _dvector_ with entry x replaced by acosh(x).
 *     
 *     a = Dvector[ 5.1, 2.2, 1.3 ]
 *     a.acosh   -> Dvector[ acosh(5.1), acosh(2.2), acosh(1.3) ]
 */ 
VALUE dvector_acosh(VALUE ary) {
   return dvector_apply_math_op(ary, do_acosh);
}

static double do_atanh(double x) { return 0.5*log((1.0+x)/(1.0-x)); }

PRIVATE
/*
 *  call-seq:
 *     dvector.atanh   ->  a_dvector
 *  
 *  Returns of copy of _dvector_ with entry x replaced by atanh(x).
 *     
 *     a = Dvector[ 0.1, -0.2, 0.3 ]
 *     a.atanh   -> Dvector[ atanh(0.1), atanh(-0.2), atanh(0.3) ]
 */ 
VALUE dvector_atanh(VALUE ary) {
   return dvector_apply_math_op(ary, do_atanh);
}

PRIVATE
/*
 *  call-seq:
 *     dvector.ceil   ->  a_dvector
 *  
 *  Returns of copy of _dvector_ with entry x replaced by smallest integer not less than x.
 *     
 *     a = Dvector[ 1.1, -2.2, 5.3 ]
 *     a.ceil   -> Dvector[ 2, -2, 6 ]
 */ 
VALUE dvector_ceil(VALUE ary) {
   return dvector_apply_math_op(ary, ceil);
}

PRIVATE
/*
 *  call-seq:
 *     dvector.floor   ->  a_dvector
 *  
 *  Returns of copy of _dvector_ with each entry x replaced by largest integer not greater than x.
 *     
 *     a = Dvector[ 1.1, -2.2, 5.3 ]
 *     a.floor   -> Dvector[ 1, -3, 5 ]
 */ 
VALUE dvector_floor(VALUE ary) {
   return dvector_apply_math_op(ary, floor);
}

static double do_round(double x) { return (x == 0.0)? 0.0 : (x > 0.0)? floor(x+0.5) : ceil(x-0.5); }

PRIVATE
/*
 *  call-seq:
 *     dvector.round   ->  a_dvector
 *  
 *  Returns of copy of _dvector_ with each entry x replaced by round(x).
 *  (Numbers midway between integers round away from zero.)
 *     
 *     a = Dvector[ 1.1, -2.2, 5.3 ]
 *     a.round   -> Dvector[ 1, -2, 5 ]
 */ 
VALUE dvector_round(VALUE ary) {
   return dvector_apply_math_op(ary, do_round);
}

PRIVATE
/*
 *  call-seq:
 *     dvector.exp   ->  a_dvector
 *  
 *  Returns of copy of _dvector_ with each entry x replaced by exp(x).
 *     
 *     a = Dvector[ 1.1, -2.2, 5.3 ]
 *     a.exp   -> Dvector[ exp(1.1), exp(-2.2), exp(5.3) ]
 */ 
VALUE dvector_exp(VALUE ary) {
   return dvector_apply_math_op(ary, exp);
}

static double do_exp10(double arg) { return pow(10.0, arg); }

PRIVATE
/*
 *  call-seq:
 *     dvector.exp10   ->  a_dvector
 *  
 *  Returns of copy of _dvector_ with each entry x replaced by 10**x.
 *     
 *     a = Dvector[ 1.1, -2.2, 5.3 ]
 *     a.exp10   -> Dvector[ 10**(1.1), 10**(-2.2), 10**(5.3) ]
 */ 
VALUE dvector_exp10(VALUE ary) {
   return dvector_apply_math_op(ary, do_exp10);
}

PRIVATE
/*
 *  call-seq:
 *     dvector.log   ->  a_dvector
 *  
 *  Returns of copy of _dvector_ with each entry x replaced by log(x).
 *     
 *     a = Dvector[ 1.1, -2.2, 5.3 ]
 *     a.log   -> Dvector[ log(1.1), log(-2.2), log(5.3) ]
 */ 
VALUE dvector_log(VALUE ary) {
   return dvector_apply_math_op(ary, log);
}

PRIVATE
/*
 *  call-seq:
 *     dvector.log10   ->  a_dvector
 *  
 *  Returns of copy of _dvector_ with each entry x replaced by log10(x).
 *     
 *     a = Dvector[ 1.1, -2.2, 5.3 ]
 *     a.log10   -> Dvector[ log10(1.1), log10(-2.2), log10(5.3) ]
 */ 
VALUE dvector_log10(VALUE ary) {
   return dvector_apply_math_op(ary, log10);
}

static double do_inv(double arg) { return 1.0/arg; }

PRIVATE
/*
 *  call-seq:
 *     dvector.inv   ->  a_dvector
 *  
 *  Returns of copy of _dvector_ with each entry x replaced by 1/x.
 *     
 *     a = Dvector[ 1.1, -2.2, 5.3 ]
 *     a.inv   -> Dvector[ 1 / 1.1, -1 / 2.2, 1 / 5.3 ]
 */ 
VALUE dvector_inv(VALUE ary) {
   return dvector_apply_math_op(ary, do_inv);
}

PRIVATE
/*
 *  call-seq:
 *     dvector.sqrt   ->  a_dvector
 *  
 *  Returns of copy of _dvector_ with each entry x replaced by sqrt(x).
 *     
 *     a = Dvector[ 1.1, 2.2, 5.3 ]
 *     a.sqrt   -> Dvector[ sqrt(1.1), sqrt(2.2), sqrt(5.3) ]
 */ 
VALUE dvector_sqrt(VALUE ary) {
   return dvector_apply_math_op(ary, sqrt);
}

PRIVATE
/*
 *  call-seq:
 *     dvector.neg!   ->  dvector
 *  
 *  Replace each entry x of _dvector_ with -x.
 *     
 *     a = Dvector[ 1.1, -2.2, 5.3 ]
 *     a.neg!   -> Dvector[ -1.1, 2.2, -5.3 ]
 *     a        -> Dvector[ -1.1, 2.2, -5.3 ]
 */ 
VALUE dvector_neg_bang(VALUE ary) {
   return dvector_apply_math_op_bang(ary, do_neg);
}

PRIVATE
/*
 *  call-seq:
 *     dvector.abs!   ->  dvector
 *  
 *  Replace each entry x of _dvector_ with abs(x).
 *     
 *     a = Dvector[ 1.1, -2.2, 5.3 ]
 *     a.abs!   -> Dvector[ 1.1, 2.2, 5.3 ]
 *     a        -> Dvector[ 1.1, 2.2, 5.3 ]
 */ 
VALUE dvector_abs_bang(VALUE ary) {
   return dvector_apply_math_op_bang(ary, fabs);
}

PRIVATE
/*
 *  call-seq:
 *     dvector.sin!   ->  dvector
 *  
 *  Replace each entry x of _dvector_ with sin(x).
 *     
 *     a = Dvector[ 1.1, -2.2, 5.3 ]
 *     a.sin!   -> Dvector[ sin(1.1), sin(-2.2), sin(5.3) ]
 *     a        -> Dvector[ sin(1.1), sin(-2.2), sin(5.3) ]
 */ 
VALUE dvector_sin_bang(VALUE ary) {
   return dvector_apply_math_op_bang(ary, sin);
}

PRIVATE
/*
 *  call-seq:
 *     dvector.cos!   ->  dvector
 *  
 *  Replace each entry x of _dvector_ with cos(x).
 *     
 *     a = Dvector[ 1.1, -2.2, 5.3 ]
 *     a.cos!   -> Dvector[ cos(1.1), cos(-2.2), cos(5.3) ]
 *     a        -> Dvector[ cos(1.1), cos(-2.2), cos(5.3) ]
 */ 
VALUE dvector_cos_bang(VALUE ary) {
   return dvector_apply_math_op_bang(ary, cos);
}

PRIVATE
/*
 *  call-seq:
 *     dvector.tan!   ->  dvector
 *  
 *  Replace each entry x of _dvector_ with tan(x).
 *     
 *     a = Dvector[ 1.1, -2.2, 5.3 ]
 *     a.tan!   -> Dvector[ tan(1.1), tan(-2.2), tan(5.3) ]
 *     a        -> Dvector[ tan(1.1), tan(-2.2), tan(5.3) ]
 */ 
VALUE dvector_tan_bang(VALUE ary) {
   return dvector_apply_math_op_bang(ary, tan);
}

PRIVATE
/*
 *  call-seq:
 *     dvector.asin!   ->  dvector
 *  
 *  Replace each entry x of _dvector_ with asin(x).
 *     
 *     a = Dvector[ 0.1, -0.2, 0.3 ]
 *     a.asin!   -> Dvector[ asin(0.1), asin(-0.2), asin(0.3) ]
 *     a         -> Dvector[ asin(0.1), asin(-0.2), asin(0.3) ]
 */ 
VALUE dvector_asin_bang(VALUE ary) {
   return dvector_apply_math_op_bang(ary, asin);
}

PRIVATE
/*
 *  call-seq:
 *     dvector.acos!   ->  dvector
 *  
 *  Replace each entry x of _dvector_ with acos(x).
 *     
 *     a = Dvector[ 0.1, -0.2, 0.3 ]
 *     a.acos!   -> Dvector[ acos(0.1), acos(-0.2), acos(0.3) ]
 *     a         -> Dvector[ acos(0.1), acos(-0.2), acos(0.3) ]
 */ 
VALUE dvector_acos_bang(VALUE ary) {
   return dvector_apply_math_op_bang(ary, acos);
}

PRIVATE
/*
 *  call-seq:
 *     dvector.atan!   ->  dvector
 *  
 *  Replace each entry x of _dvector_ with atan(x).
 *     
 *     a = Dvector[ 1.1, -2.2, 5.3 ]
 *     a.atan!   -> Dvector[ atan(1.1), atan(-2.2), atan(5.3) ]
 *     a         -> Dvector[ atan(1.1), atan(-2.2), atan(5.3) ]
 */ 
VALUE dvector_atan_bang(VALUE ary) {
   return dvector_apply_math_op_bang(ary, atan);
}

PRIVATE
/*
 *  call-seq:
 *     dvector.sinh!   ->  dvector
 *  
 *  Replace each entry x of _dvector_ with sinh(x).
 *     
 *     a = Dvector[ 1.1, -2.2, 5.3 ]
 *     a.sinh!   -> Dvector[ sinh(1.1), sinh(-2.2), sinh(5.3) ]
 *     a         -> Dvector[ sinh(1.1), sinh(-2.2), sinh(5.3) ]
 */ 
VALUE dvector_sinh_bang(VALUE ary) {
   return dvector_apply_math_op_bang(ary, sinh);
}

PRIVATE
/*
 *  call-seq:
 *     dvector.cosh!   ->  dvector
 *  
 *  Replace each entry x of _dvector_ with cosh(x).
 *     
 *     a = Dvector[ 1.1, -2.2, 5.3 ]
 *     a.cosh!   -> Dvector[ cosh(1.1), cosh(-2.2), cosh(5.3) ]
 *     a         -> Dvector[ cosh(1.1), cosh(-2.2), cosh(5.3) ]
 */ 
VALUE dvector_cosh_bang(VALUE ary) {
   return dvector_apply_math_op_bang(ary, cosh);
}

PRIVATE
/*
 *  call-seq:
 *     dvector.tanh!   ->  dvector
 *  
 *  Replace each entry x of _dvector_ with tanh(x).
 *     
 *     a = Dvector[ 1.1, -2.2, 5.3 ]
 *     a.tanh!   -> Dvector[ tanh(1.1), tanh(-2.2), tanh(5.3) ]
 *     a         -> Dvector[ tanh(1.1), tanh(-2.2), tanh(5.3) ]
 */ 
VALUE dvector_tanh_bang(VALUE ary) {
   return dvector_apply_math_op_bang(ary, tanh);
}

PRIVATE
/*
 *  call-seq:
 *     dvector.asinh!   ->  dvector
 *  
 *  Replace each entry x of _dvector_ with asinh(x).
 *     
 *     a = Dvector[ 1.1, 2.2, 5.3 ]
 *     a.asinh!   -> Dvector[ asinh(1.1), asinh(2.2), asinh(5.3) ]
 *     a          -> Dvector[ asinh(1.1), asinh(2.2), asinh(5.3) ]
 */ 
VALUE dvector_asinh_bang(VALUE ary) {
   return dvector_apply_math_op_bang(ary, do_asinh);
}

PRIVATE
/*
 *  call-seq:
 *     dvector.acosh!   ->  dvector
 *  
 *  Replace each entry x of _dvector_ with acosh(x).
 *     
 *     a = Dvector[ 1.1, 2.2, 5.3 ]
 *     a.acosh!   -> Dvector[ acosh(1.1), acosh(2.2), acosh(5.3) ]
 *     a          -> Dvector[ acosh(1.1), acosh(2.2), acosh(5.3) ]
 */ 
VALUE dvector_acosh_bang(VALUE ary) {
   return dvector_apply_math_op_bang(ary, do_acosh);
}

PRIVATE
/*
 *  call-seq:
 *     dvector.atanh!   ->  dvector
 *  
 *  Replace each entry x of _dvector_ with atanh(x).
 *     
 *     a = Dvector[ 1.1, -2.2, 5.3 ]
 *     a.atanh!   -> Dvector[ atanh(1.1), atanh(-2.2), atanh(5.3) ]
 *     a          -> Dvector[ atanh(1.1), atanh(-2.2), atanh(5.3) ]
 */ 
VALUE dvector_atanh_bang(VALUE ary) {
   return dvector_apply_math_op_bang(ary, do_atanh);
}

PRIVATE
/*
 *  call-seq:
 *     dvector.ceil!   ->  dvector
 *  
 *  Replace each entry x of _dvector_ with the smallest integer not less than x.
 *     
 *     a = Dvector[ 1.1, -2.2, 5.3 ]
 *     a.ceil!   -> Dvector[ 2, -2, 6 ]
 *     a         -> Dvector[ 2, -2, 6 ]
 */ 
VALUE dvector_ceil_bang(VALUE ary) {
   return dvector_apply_math_op_bang(ary, ceil);
}

PRIVATE
/*
 *  call-seq:
 *     dvector.floor!   ->  dvector
 *  
 *  Replace each entry x of _dvector_ with the largest integer not greater than x.
 *     
 *     a = Dvector[ 1.1, -2.2, 5.3 ]
 *     a.floor!   -> Dvector[ 1, -3, 5 ]
 *     a          -> Dvector[ 1, -3, 5 ]
 */ 
VALUE dvector_floor_bang(VALUE ary) {
   return dvector_apply_math_op_bang(ary, floor);
}

PRIVATE
/*
 *  call-seq:
 *     dvector.round!   ->  dvector
 *  
 *  Replace each entry x of _dvector_ with the integer closest to x.
 *  (Numbers midway between integers round away from zero.)
 *     
 *     a = Dvector[ 1.1, -2.2, 5.3 ]
 *     a.round!   -> Dvector[ 1, -2, 5 ]
 *     a          -> Dvector[ 1, -2, 5 ]
 */ 
VALUE dvector_round_bang(VALUE ary) {
   return dvector_apply_math_op_bang(ary, do_round);
}

PRIVATE
/*
 *  call-seq:
 *     dvector.exp!   ->  dvector
 *  
 *  Replace each entry x of _dvector_ with exp(x).
 *     
 *     a = Dvector[ 1.1, -2.2, 5.3 ]
 *     a.exp!   -> Dvector[ exp(1.1), exp(-2.2), exp(5.3) ]
 *     a        -> Dvector[ exp(1.1), exp(-2.2), exp(5.3) ]
 */ 
VALUE dvector_exp_bang(VALUE ary) {
   return dvector_apply_math_op_bang(ary, exp);
}

PRIVATE
/*
 *  call-seq:
 *     dvector.exp10!   ->  dvector
 *  
 *  Replace each entry x of _dvector_ with 10**x.
 *     
 *     a = Dvector[ 1.1, -2.2, 5.3 ]
 *     a.exp10!   -> Dvector[ 10**(1.1), 10**(-2.2), 10**(5.3) ]
 *     a          -> Dvector[ 10**(1.1), 10**(-2.2), 10**(5.3) ]
 */ 
VALUE dvector_exp10_bang(VALUE ary) {
   return dvector_apply_math_op_bang(ary, do_exp10);
}

PRIVATE
/*
 *  call-seq:
 *     dvector.log!   ->  dvector
 *  
 *  Replace each entry x of _dvector_ with log(x).
 *     
 *     a = Dvector[ 1.1, -2.2, 5.3 ]
 *     a.log!   -> Dvector[ log(1.1), log(-2.2), log(5.3) ]
 *     a        -> Dvector[ log(1.1), log(-2.2), log(5.3) ]
 */ 
VALUE dvector_log_bang(VALUE ary) {
   return dvector_apply_math_op_bang(ary, log);
}

PRIVATE
/*
 *  call-seq:
 *     dvector.log10!   ->  dvector
 *  
 *  Replace each entry x of _dvector_ with log10(x).
 *     
 *     a = Dvector[ 1.1, -2.2, 5.3 ]
 *     a.log10!   -> Dvector[ log10(1.1), log10(-2.2), log10(5.3) ]
 *     a          -> Dvector[ log10(1.1), log10(-2.2), log10(5.3) ]
 */ 
VALUE dvector_log10_bang(VALUE ary) {
   return dvector_apply_math_op_bang(ary, log10);
}

PRIVATE
/*
 *  call-seq:
 *     dvector.inv!   ->  dvector
 *  
 *  Replace each entry x of _dvector_ with 1/x.
 *     
 *     a = Dvector[ 1.1, -2.2, 5.3 ]
 *     a.inv!   -> Dvector[ 1 / 1.1, -1 / 2.2, 1 / 5.3 ]
 *     a        -> Dvector[ 1 / 1.1, -1 / 2.2, 1 / 5.3 ]
 */ 
VALUE dvector_inv_bang(VALUE ary) {
   return dvector_apply_math_op_bang(ary, do_inv);
}

PRIVATE
/*
 *  call-seq:
 *     dvector.sqrt!   ->  dvector
 *  
 *  Replace each entry x of _dvector_ with sqrt(x).
 *     
 *     a = Dvector[ 1.1, 2.2, 5.3 ]
 *     a.sqrt!   -> Dvector[ sqrt(1.1), sqrt(2.2), sqrt(5.3) ]
 *     a         -> Dvector[ sqrt(1.1), sqrt(2.2), sqrt(5.3) ]
 */ 
VALUE dvector_sqrt_bang(VALUE ary) {
   return dvector_apply_math_op_bang(ary, sqrt);
}

static VALUE dvector_apply_math_op1_bang(VALUE ary, VALUE arg, double (*op)(double, double)) {
   Dvector *d = dvector_modify(ary);
   arg = rb_Float(arg);
   double y = NUM2DBL(arg), *p = d->ptr;
   long len = d->len, i;
   for (i=0; i<len; i++) p[i] = (*op)(p[i], y);
   return ary;
}

static VALUE dvector_apply_math_op1(VALUE source, VALUE arg, double (*op)(double, double)) {
   return dvector_apply_math_op1_bang(dvector_dup(source), arg, op);
}

static double do_trim(double x, double cutoff) { return (fabs(x) < cutoff)? 0.0 : x; }

PRIVATE
/*
 *  call-seq:
 *     dvector.trim(cutoff=1e-6)   ->  a_dvector
 *  
 *  Returns a copy of _dvector_ with any entry with absolute value less than _cutoff_ replaced by 0.
 *     
 *     a = Dvector[ 1.1, 1e-20, -5.3 ]
 *     a.trim  -> Dvector[ 1.1, 0, -5.3 ]
 */ 
VALUE dvector_trim(int argc, VALUE *argv, VALUE self) {
   VALUE arg1;
   if ((argc < 0) || (argc > 1))
      rb_raise(rb_eArgError, "wrong # of arguments(%d for 0 or 1)",argc);
   arg1 = (argc > 0)? argv[0] : rb_float_new(1e-6);
   return dvector_apply_math_op1(self, arg1, do_trim);
}

static double do_safe_log(double x, double y) { return log(MAX(x,y)); }

PRIVATE
/*
 *  call-seq:
 *     dvector.safe_log(cutoff=1e-99)   ->  a_dvector
 *  
 *  Returns a copy of _dvector_ with each entry x replaced by log(max(x,_cutoff_)).
 *     
 */ 
VALUE dvector_safe_log(int argc, VALUE *argv, VALUE self) {
   VALUE arg1;
   if ((argc < 0) || (argc > 1))
      rb_raise(rb_eArgError, "wrong # of arguments(%d for 0 or 1)",argc);
   arg1 = (argc > 0)? argv[0] : rb_float_new(1e-99);
   return dvector_apply_math_op1(self, arg1, do_safe_log);
}

static double do_safe_log10(double x, double y) { return log10(MAX(x,y)); }

PRIVATE
/*
 *  call-seq:
 *     dvector.safe_log10(cutoff=1e-99)   ->  a_dvector
 *  
 *  Returns a copy of _dvector_ with each entry x replaced by log10(max(x,_cutoff_)).
 *     
 */ 
VALUE dvector_safe_log10(int argc, VALUE *argv, VALUE self) {
   VALUE arg1;
   if ((argc < 0) || (argc > 1))
      rb_raise(rb_eArgError, "wrong # of arguments(%d for 0 or 1)",argc);
   arg1 = (argc > 0)? argv[0] : rb_float_new(1e-99);
   return dvector_apply_math_op1(self, arg1, do_safe_log10);
}

static double do_safe_inv(double x, double y) {
   return (fabs(x) >= y)? 1.0/x : (x > 0.0)? 1.0/y : -1.0/y; }

PRIVATE
/*
 *  call-seq:
 *     dvector.safe_inv(cutoff)   ->  a_dvector
 *  
 *  Returns a copy of _dvector_ with each entry x replaced by sign(x)/_cutoff_ if abs(x) < _cutoff_, 1/x otherwise.
 *     
 */ 
VALUE dvector_safe_inv(VALUE ary, VALUE arg) {
   return dvector_apply_math_op1(ary, arg, do_safe_inv);
}

static double do_safe_asin(double x) { return asin(MAX(-1.0,MIN(1.0,x))); }

PRIVATE
/*
 *  call-seq:
 *     dvector.safe_asin   ->  a_dvector
 *  
 *  Returns a copy of _dvector_ with each entry x replaced by asin(max(-1,min(1,x))).
 *     
 */ 
VALUE dvector_safe_asin(VALUE ary) {
   return dvector_apply_math_op(ary, do_safe_asin);
}
   
static double do_safe_acos(double x) { return acos(MAX(-1.0,MIN(1.0,x))); }

PRIVATE
/*
 *  call-seq:
 *     dvector.safe_acos   ->  a_dvector
 *  
 *  Returns a copy of _dvector_ with each entry x replaced by acos(max(-1,min(1,x))).
 *     
 */ 
VALUE dvector_safe_acos(VALUE ary) {
   return dvector_apply_math_op(ary, do_safe_acos);
}
   
static double do_safe_sqrt(double x) { return sqrt(MAX(x,0.0)); }

PRIVATE
/*
 *  call-seq:
 *     dvector.safe_sqrt   ->  a_dvector
 *  
 *  Returns a copy of _dvector_ with each entry x replaced by sqrt(max(x,0)).
 *     
 */ 
VALUE dvector_safe_sqrt(VALUE ary) {
   return dvector_apply_math_op(ary, do_safe_sqrt);
}

PRIVATE
/*
 *  call-seq:
 *     dvector.atan2!(number)       ->  dvector
 *     dvector.atan2!(other)        ->  dvector
 *  
 *  When argument is a number, this operation replaces each entry x of _dvector_ by the angle whose tangent is x/_number_.
 *  When argument is a vector, this operation replaces each entry x of _dvector_ by the angle whose tangent is x divided
 *  by the corresponding entry in the _other_ vector.
 *     
 *     a = Dvector[ 1.1, -5.7, 12.7 ]
 *     a.atan2!(3.8)              -> Dvector[ atan2(1.1, 3.8), atan2(-5.7,3.8), atan2(12.7,3.8) ]
 *     a = Dvector[ 1.1, -5.7, 12.7 ]
 *     b = Dvector[ 7.1, 4.9, -10.1 ]
 *     a.atan2!(b)                -> Dvector[ atan2(1.1,7.1), atan2(-5.7,4.9), atan2(12.7,-10.1) ]
 */ 
VALUE dvector_atan2_bang(VALUE ary, VALUE arg) {
   return dvector_apply_math_op2_bang(ary, arg, atan2);
}

static double do_mod(double x, double y) { return x - y * floor(x/y); }
PRIVATE
/*
 *  call-seq:
 *     dvector.modulo!(number)       ->  dvector
 *     dvector.mod!(number)          ->  dvector
 *     dvector.modulo!(other)        ->  dvector
 *     dvector.mod!(other)           ->  dvector
 *  
 *  When argument is a number, this operation returns a copy of _dvector_ with each entry x replaced by x % _number_.
 *  When argument is a vector, this operation returns a copy of _dvector_ with each entry x replaced
 *  by x % the corresponding entry in the _other_ vector.
 *     
 *     a = Dvector[ 1.1, -5.7, 12.7 ]
 *     a.mod!(3.8)              -> Dvector[ 1.1, 1.9, 1.3 ]
 *     a                        -> Dvector[ 1.1, 1.9, 1.3 ]
 *     b = Dvector[ 7.1, 4.9, -10.1 ]
 *     a.mod!(b)                -> Dvector[ 1.1, 4.1, -7.5 ]
 *     a                        -> Dvector[ 1.1, 4.1, -7.5 ]
 */ 
VALUE dvector_modulo_bang(VALUE ary, VALUE arg) {
   return dvector_apply_math_op2_bang(ary, arg, do_mod);
}

static double do_remainder(double x, double y) { return (x*y > 0.0)? do_mod(x,y) : do_mod(x,y)-y; }

PRIVATE
/*
 *  call-seq:
 *     dvector.remainder!(number)          ->  dvector
 *     dvector.remainder!(other)           ->  dvector
 *  
 *  When the argument is a number, this operation replaces with each entry x of _dvector_ by the remainder of x divided by _number_.
 *  When the argument is a vector, this operation replaces with each entry x of _dvector_
 *  by remainder of x divided by the corresponding entry in the _other_ vector.
 *     
 *     a = Dvector[ 11, -5, 2 ]
 *     a.remainder!(3)                -> Dvector[ 2, -2, 2 ]
 *     a                              -> Dvector[ 2, -2, 2 ]
 *     a = Dvector[ 11, -5, 2 ]
 *     b = Dvector[ 2, 3, 5 ]
 *     a.remainder!(b)                -> Dvector[ 1, -2, 2 ]
 *     a                              -> Dvector[ 1, -2, 2 ]
 */ 
VALUE dvector_remainder_bang(VALUE ary, VALUE arg) {
   return dvector_apply_math_op2_bang(ary, arg, do_remainder);
}

PRIVATE
/*
 *  call-seq:
 *     dvector.trim!(cutoff=1e-6)   ->  dvector
 *  
 *  Each entry x in _dvector_ having absolute value less than _cutoff_ is replaced by 0.
 *     
 *     a = Dvector[ 1.1, 1e-20, -5.3 ]
 *     a.trim!  -> Dvector[ 1.1, 0, -5.3 ]
 */ 
VALUE dvector_trim_bang(int argc, VALUE *argv, VALUE self) {
   VALUE arg1;
   if ((argc < 0) || (argc > 1))
      rb_raise(rb_eArgError, "wrong # of arguments(%d for 0 or 1)",argc);
   arg1 = (argc > 0)? argv[0] : rb_float_new(1e-6);
   return dvector_apply_math_op1_bang(self, arg1, do_trim);
}

PRIVATE
/*
 *  call-seq:
 *     dvector.pow!(number)                ->  dvector
 *     dvector.pow!(other)                 ->  dvector
 *     dvector.raised_to!(number)          ->  dvector
 *     dvector.raised_to!(other)           ->  dvector
 *  
 *  When argument is a number, this operation returns a copy of _dvector_ with each entry x replaced by x ** _number_.
 *  When argument is a vector, this operation returns a copy of _dvector_ with each entry x replaced
 *  by x ** the corresponding entry in the _other_ vector.
 *     
 *     a = Dvector[ 1.1, -5.7, 12.7 ]
 *     a.raised_to!(3)                -> Dvector[ 1.1 ** 3, (-5.7) ** 3, 12.7 ** 3 ]
 *     a                              -> Dvector[ 1.1 ** 3, (-5.7) ** 3, 12.7 ** 3 ]
 *     b = Dvector[ 7, 4, -2 ]
 *     a.raised_to!(b)                -> Dvector[ 1.1 ** 7, (-5.7) ** 4, 12.7 ** (-2) ]
 *     a                              -> Dvector[ 1.1 ** 7, (-5.7) ** 4, 12.7 ** (-2) ]
 */ 
VALUE dvector_pow_bang(VALUE ary, VALUE arg) {
   return dvector_apply_math_op1_bang(ary, arg, pow);
}

static double do_as_exponent_of(double x, double y) { return pow(y,x); }

PRIVATE
/*
 *  call-seq:
 *     dvector.as_exponent_of!(number)                ->  dvector
 *     dvector.as_exponent_of!(other)                 ->  dvector
 *  
 *  When argument is a number, this operation replaces each entry x of _dvector_ by _number_ ** x.
 *  When argument is a vector, this operation replaces each entry x of _dvector_
 *  by the corresponding entry in the _other_ vector raised to the power x.
 *     
 *     a = Dvector[ 2, -5, 12 ]
 *     a.as_exponent_of!(3.8)              -> Dvector[ 3.8 ** 2, 3.8 ** (-5), 3.8 ** 12 ]
 *     a                                   -> Dvector[ 3.8 ** 2, 3.8 ** (-5), 3.8 ** 12 ]
 *     b = Dvector[ 7.1, 4.9, -10 ]
 *     a.as_exponent_of!(b)                -> Dvector[ 7.1 ** 2, 4.9 ** (-5), (-10) ** 12 ]
 *     a                                   -> Dvector[ 7.1 ** 2, 4.9 ** (-5), (-10) ** 12 ]
 */ 
VALUE dvector_as_exponent_of_bang(VALUE ary, VALUE arg) {
   return dvector_apply_math_op2_bang(ary, arg, do_as_exponent_of);
}

PRIVATE
/*
 *  call-seq:
 *     dvector.safe_log!(cutoff=1e-99)   ->  dvector
 *  
 *  Replaces each entry x in _dvector_ by log(max(x,_cutoff_)).
 *     
 */ 
VALUE dvector_safe_log_bang(int argc, VALUE *argv, VALUE self) {
   VALUE arg1;
   if ((argc < 0) || (argc > 1))
      rb_raise(rb_eArgError, "wrong # of arguments(%d for 0 or 1)",argc);
   arg1 = (argc > 0)? argv[0] : rb_float_new(1e-99);
   return dvector_apply_math_op1_bang(self, arg1, do_safe_log);
}

PRIVATE
/*
 *  call-seq:
 *     dvector.safe_log10!(cutoff=1e-99)   ->  dvector
 *  
 *  Replaces each entry x in _dvector_ by log10(max(x,_cutoff_)).
 *     
 */ 
VALUE dvector_safe_log10_bang(int argc, VALUE *argv, VALUE self) {
   VALUE arg1;
   if ((argc < 0) || (argc > 1))
      rb_raise(rb_eArgError, "wrong # of arguments(%d for 0 or 1)",argc);
   arg1 = (argc > 0)? argv[0] : rb_float_new(1e-99);
   return dvector_apply_math_op1_bang(self, arg1, do_safe_log10);
}

PRIVATE
/*
 *  call-seq:
 *     dvector.safe_inv!(cutoff=1e-99)   ->  dvector
 *  
 *  Replaces each entry x in _dvector_ by sign(x)/_cutoff_ if abs(x) < _cutoff_, 1/x otherwise.
 *     
 */ 
VALUE dvector_safe_inv_bang(int argc, VALUE *argv, VALUE self) {
   VALUE arg1;
   if ((argc < 0) || (argc > 1))
      rb_raise(rb_eArgError, "wrong # of arguments(%d for 0 or 1)",argc);
   arg1 = (argc > 0)? argv[0] : rb_float_new(1e-99);
   return dvector_apply_math_op1_bang(self, arg1, do_safe_inv);
}

PRIVATE
/*
 *  call-seq:
 *     dvector.safe_sqrt!   ->  dvector
 *  
 *  Replaces each entry x in _dvector_ by sqrt(max(x,0)).
 *     
 */ 
VALUE dvector_safe_sqrt_bang(VALUE ary) {
   return dvector_apply_math_op_bang(ary, do_safe_sqrt);
}
   
PRIVATE
/*
 *  call-seq:
 *     dvector.safe_asin!   ->  dvector
 *  
 *  Replaces each entry x in _dvector_ by asin(max(-1,min(1,x))).
 *     
 */ 
VALUE dvector_safe_asin_bang(VALUE ary) {
   return dvector_apply_math_op_bang(ary, do_safe_asin);
}
   
PRIVATE
/*
 *  call-seq:
 *     dvector.safe_acos!   ->  dvector
 *  
 *  Replaces each entry x in _dvector_ by acos(max(-1,min(1,x))).
 *     
 */ 
VALUE dvector_safe_acos_bang(VALUE ary) {
   return dvector_apply_math_op_bang(ary, do_safe_acos);
}
PRIVATE    
VALUE dvector_apply_math_op2_bang(VALUE ary1, VALUE ary2, double (*op)(double, double)) {
   VALUE check = rb_obj_is_kind_of(ary2, rb_cNumeric);
   if (check != Qfalse) { return dvector_apply_math_op1_bang(ary1, ary2, op); }
   Dvector *d1 = dvector_modify(ary1), *d2 = Get_Dvector(ary2);
   double *p1 = d1->ptr, *p2 = d2->ptr;
   long len = d1->len, i;
   if (len != d2->len) {
      rb_raise(rb_eArgError, "vectors with different lengths (%ld vs %ld) math operation", d1->len, d2->len);
   }
   for (i=0; i<len; i++) p1[i] = (*op)(p1[i], p2[i]);
   return ary1;
}
PRIVATE 
VALUE dvector_apply_math_op2(VALUE ary1, VALUE ary2, double (*op)(double, double)) {
   return dvector_apply_math_op2_bang(dvector_dup(ary1), ary2, op);
}

static double do_add(double x, double y) { return x + y; }
PRIVATE
/*
 *  call-seq:
 *     dvector.add(number)       ->  a_dvector
 *     dvector.add(other)        ->  a_dvector
 *     dvector + number          ->  a_dvector
 *     number + dvector          ->  a_dvector
 *     dvector + other           ->  a_dvector
 *  
 *  When argument is a number, this operation returns a copy of _dvector_ with each entry x replaced by x + _number_.
 *  When argument is a vector, this operation returns a copy of _dvector_ with each entry x replaced
 *  by x + the corresponding entry in the _other_ vector.
 *     
 *     a = Dvector[ 11, -5, 2 ]
 *     a.add(3)              -> Dvector[ 14, -2, 5 ]
 *     a + 3                 -> Dvector[ 14, -2, 5 ]
 *     3 + a                 -> Dvector[ 14, -2, 5 ]
 *     b = Dvector[ 7, 4, -10 ]
 *     a.add(b)              -> Dvector[ 18, -1, -8 ]
 *     a + b                 -> Dvector[ 18, -1, -8 ]
 */ 
VALUE dvector_add(VALUE ary, VALUE arg) {
   return dvector_apply_math_op2(ary, arg, do_add);
}

static double do_sub(double x, double y) { return x - y; }
PRIVATE
/*
 *  call-seq:
 *     dvector.sub(number)       ->  a_dvector
 *     dvector.sub(other)        ->  a_dvector
 *     dvector - number          ->  a_dvector
 *     number - dvector          ->  a_dvector
 *     dvector - other           ->  a_dvector
 *  
 *  When argument is a number, this operation returns a copy of _dvector_ with each entry x replaced by x - _number_.
 *  When argument is a vector, this operation returns a copy of _dvector_ with each entry x replaced
 *  by x - the corresponding entry in the _other_ vector.
 *     
 *     a = Dvector[ 11, -5, 2 ]
 *     a.sub(3)               -> Dvector[ 8, -8, -1 ]
 *     a - 3                  -> Dvector[ 8, -8, -1 ]
 *     3 - a                  -> Dvector[ -8, 8, 1 ]
 *     b = Dvector[ 7, 4, -10 ]
 *     a.sub(b)               -> Dvector[ 4, -9, 12 ]
 *     a - b                  -> Dvector[ 4, -9, 12 ]
 */ 
VALUE dvector_sub(VALUE ary, VALUE arg) {
   return dvector_apply_math_op2(ary, arg, do_sub);
}

static double do_mul(double x, double y) { return x * y; }
PRIVATE
/*
 *  call-seq:
 *     dvector.mul(number)       ->  a_dvector
 *     dvector.mul(other)        ->  a_dvector
 *     dvector - number          ->  a_dvector
 *     number - dvector          ->  a_dvector
 *     dvector - other           ->  a_dvector
 *  
 *  When argument is a number, this operation returns a copy of _dvector_ with each entry x replaced by x * _number_.
 *  When argument is a vector, this operation returns a copy of _dvector_ with each entry x replaced
 *  by x * the corresponding entry in the _other_ vector.
 *     
 *     a = Dvector[ 11, -5, 2 ]
 *     a.mul(3)               -> Dvector[ 33, -15, 6 ]
 *     a * 3                  -> Dvector[ 33, -15, 6 ]
 *     3 * a                  -> Dvector[ 33, -15, 6 ]
 *     b = Dvector[ 7, 4, -10 ]
 *     a.mul(b)               -> Dvector[ 77, -20, -20 ]
 *     a * b                  -> Dvector[ 77, -20, -20 ]
 */ 
VALUE dvector_mul(VALUE ary, VALUE arg) {
   return dvector_apply_math_op2(ary, arg, do_mul);
}

static double do_div(double x, double y) { return x / y; }
PRIVATE
/*
 *  call-seq:
 *     dvector.div(number)       ->  a_dvector
 *     dvector.div(other)        ->  a_dvector
 *     dvector - number          ->  a_dvector
 *     number - dvector          ->  a_dvector
 *     dvector - other           ->  a_dvector
 *  
 *  When argument is a number, this operation returns a copy of _dvector_ with each entry x replaced by x / _number_.
 *  When argument is a vector, this operation returns a copy of _dvector_ with each entry x replaced
 *  by x / the corresponding entry in the _other_ vector.
 *     
 *     a = Dvector[ 1.1, -5.7, 2.5 ]
 *     a.div(3.8)              -> Dvector[ 1.1/3.8, -5.7/3.8, 2.5/3.8 ]
 *     a / 3.8                 -> Dvector[ 1.1/3.8, -5.7/3.8, 2.5/3.8 ]
 *     3 / a                   -> Dvector[ 3.8/1.1, -3.8/5.7, 3.8/2.5 ]
 *     b = Dvector[ 7.1, 4.9, -10.1 ]
 *     a.div(b)                -> Dvector[ 1.1/7.1, -5.7/4.9, 2.5/10.1 ]
 *     a / b                   -> Dvector[ 1.1/7.1, -5.7/4.9, 2.5/10.1 ]
 */ 
VALUE dvector_div(VALUE ary, VALUE arg) {
   return dvector_apply_math_op2(ary, arg, do_div);
}

PRIVATE
/*
 *  call-seq:
 *     dvector.modulo(number)       ->  a_dvector
 *     dvector.mod(number)          ->  a_dvector
 *     dvector.modulo(other)        ->  a_dvector
 *     dvector.mod(other)           ->  a_dvector
 *     dvector % number             ->  a_dvector
 *     dvector % other              ->  a_dvector
 *  
 *  When argument is a number, this operation returns a copy of _dvector_ with each entry x replaced by x % _number_.
 *  When argument is a vector, this operation returns a copy of _dvector_ with each entry x replaced
 *  by x % the corresponding entry in the _other_ vector.
 *     
 *     a = Dvector[ 1.1, -5.7, 12.7 ]
 *     a.mod(3.8)              -> Dvector[ 1.1, 1.9, 1.3 ]
 *     a % 3.8                 -> Dvector[ 1.1, 1.9, 1.3 ]
 *     b = Dvector[ 7.1, 4.9, -10.1 ]
 *     a.mod(b)                -> Dvector[ 1.1, 4.1, -7.5 ]
 *     a % b                   -> Dvector[ 1.1, 4.1, -7.5 ]
 */ 
VALUE dvector_mod(VALUE ary, VALUE arg) {
   return dvector_apply_math_op2(ary, arg, do_mod);
}

PRIVATE
/*
 *  call-seq:
 *     dvector.remainder(number)          ->  a_dvector
 *     dvector.remainder(other)           ->  a_dvector
 *  
 *  When the argument is a number, this operation returns a copy of _dvector_ with each entry x replaced by the remainder of x divided by _number_.
 *  When the argument is a vector, this operation returns a copy of _dvector_ with each entry x replaced
 *  by the remainder of x divided by the corresponding entry in the _other_ vector.
 *     
 *     a = Dvector[ 11, -5, 2 ]
 *     a.remainder(3)  -> Dvector[ 2, -2, 2 ]
 *     b = Dvector[ 2, 3, 5 ]
 *     a.remainder(b)                -> Dvector[ 1, -2, 2 ]
 */ 
VALUE dvector_remainder(VALUE ary, VALUE arg) {
   return dvector_apply_math_op2(ary, arg, do_remainder);
}

PRIVATE
/*
 *  call-seq:
 *     dvector.pow(number)                ->  a_dvector
 *     dvector.pow(other)                 ->  a_dvector
 *     dvector.raised_to(number)          ->  a_dvector
 *     dvector.raised_to(other)           ->  a_dvector
 *     dvector ** number                  ->  a_dvector
 *     dvector ** other                   ->  a_dvector
 *  
 *  When argument is a number, this operation returns a copy of _dvector_ with each entry x replaced by x ** _number_.
 *  When argument is a vector, this operation returns a copy of _dvector_ with each entry x replaced
 *  by x ** the corresponding entry in the _other_ vector.
 *     
 *     a = Dvector[ 1.1, -5.7, 12.7 ]
 *     a.raised_to(3)              -> Dvector[ 1.1 ** 3, (-5.7) ** 3, 12.7 ** 3 ]
 *     a ** 3                      -> Dvector[ 1.1 ** 3, (-5.7) ** 3, 12.7 ** 3 ]
 *     b = Dvector[ 7, 4, -2 ]
 *     a.raised_to(b)                -> Dvector[ 1.1 ** 7, (-5.7) ** 4, 12.7 ** (-2) ]
 *     a ** b                        -> Dvector[ 1.1 ** 7, (-5.7) ** 4, 12.7 ** (-2) ]
 */ 
VALUE dvector_pow(VALUE ary, VALUE arg) {
   return dvector_apply_math_op2(ary, arg, pow);
}

PRIVATE
/*
 *  call-seq:
 *     dvector.as_exponent_of(number)                ->  a_dvector
 *     dvector.as_exponent_of(other)                 ->  a_dvector
 *  
 *  When argument is a number, this operation returns a copy of _dvector_ with each entry x replaced by _number_ ** x.
 *  When argument is a vector, this operation returns a copy of _dvector_ with each entry x replaced
 *  by the corresponding entry in the _other_ vector raised to the power x.
 *     
 *     a = Dvector[ 2, -5, 12 ]
 *     a.as_exponent_of(3.8)              -> Dvector[ 3.8 ** 2, 3.8 ** (-5), 3.8 ** 12 ]
 *     b = Dvector[ 7.1, 4.9, -10 ]
 *     a.as_exponent_of(b)                -> Dvector[ 7.1 ** 2, 4.9 ** (-5), (-10) ** 12 ]
 */ 
VALUE dvector_as_exponent_of(VALUE ary, VALUE arg) {
   return dvector_apply_math_op2(ary, arg, do_as_exponent_of);
}

PRIVATE
/*
 *  call-seq:
 *     dvector.atan2(number)       ->  a_dvector
 *     dvector.atan2(other)        ->  a_dvector
 *  
 *  When argument is a number, this operation returns a copy of _dvector_ with each entry x replaced by the angle whose tangent is x/_number_.
 *  When argument is a vector, this operation returns a copy of _dvector_ with each entry x replaced
 *  by the angle whose tangent is x divided by the corresponding entry in the _other_ vector.
 *     
 *     a = Dvector[ 1.1, -5.7, 12.7 ]
 *     a.atan2(3.8)              -> Dvector[ atan2(1.1, 3.8), atan2(-5.7,3.8), atan2(12.7,3.8) ]
 *     b = Dvector[ 7.1, 4.9, -10.1 ]
 *     a.atan2(b)                -> Dvector[ atan2(1.1,7.1), atan2(-5.7,4.9), atan2(12.7,-10.1) ]
 */ 
VALUE dvector_atan2(VALUE ary, VALUE arg) {
   return dvector_apply_math_op2(ary, arg, atan2);
}

PRIVATE
/*
 *  call-seq:
 *     dvector.add!(number)       ->  dvector
 *     dvector.add!(other)        ->  dvector
 *  
 *  When argument is a number, each entry x in _dvector_ is replaced by x + _number_.
 *  When argument is a vector, each entry x in _dvector_ is replaced by x + 
 *  the corresponding entry in the _other_ vector.
 *     
 *     a = Dvector[ 11, -5, 2 ]
 *     a.add!(3)              -> Dvector[ 14, -2, 5 ]
 *     a                      -> Dvector[ 14, -2, 5 ]
 *     a = Dvector[ 11, -5, 2 ]
 *     b = Dvector[ 7, 4, -10 ]
 *     a.add!(b)              -> Dvector[ 18, -1, -8 ]
 *     a                      -> Dvector[ 18, -1, -8 ]
 */ 
VALUE dvector_add_bang(VALUE ary, VALUE arg) {
   return dvector_apply_math_op2_bang(ary, arg, do_add);
}

PRIVATE
/*
 *  call-seq:
 *     dvector.sub!(number)       ->  dvector
 *     dvector.sub!(other)        ->  dvector
 *  
 *  When argument is a number, each entry x in _dvector_ is replaced by x - _number_.
 *  When argument is a vector, each entry x in _dvector_ is replaced by x - 
 *  the corresponding entry in the _other_ vector.
 *     
 *     a = Dvector[ 11, -5, 2 ]
 *     a.sub!(3)               -> Dvector[ 8, -8, -1 ]
 *     a                       -> Dvector[ 8, -8, -1 ]
 *     b = Dvector[ 7, 4, -10 ]
 *     a = Dvector[ 11, -5, 2 ]
 *     a.sub!(b)               -> Dvector[ 4, -9, 12 ]
 *     a                       -> Dvector[ 4, -9, 12 ]
 */ 
VALUE dvector_sub_bang(VALUE ary, VALUE arg) {
   return dvector_apply_math_op2_bang(ary, arg, do_sub);
}

PRIVATE
/*
 *  call-seq:
 *     dvector.mul!(number)       ->  dvector
 *     dvector.mul!(other)        ->  dvector
 *  
 *  When argument is a number, each entry x in _dvector_ is replaced by x * _number_.
 *  When argument is a vector, each entry x in _dvector_ is replaced by x * 
 *  the corresponding entry in the _other_ vector.
 *     
 *     a = Dvector[ 11, -5, 2 ]
 *     a.mul!(3)               -> Dvector[ 33, -15, 6 ]
 *     a                       -> Dvector[ 33, -15, 6 ]
 *     a = Dvector[ 11, -5, 2 ]
 *     b = Dvector[ 7, 4, -10 ]
 *     a.mul!(b)               -> Dvector[ 77, -20, -20 ]
 *     a                       -> Dvector[ 77, -20, -20 ]
 */ 
VALUE dvector_mul_bang(VALUE ary, VALUE arg) {
   return dvector_apply_math_op2_bang(ary, arg, do_mul);
}

PRIVATE
/*
 *  call-seq:
 *     dvector.div!(number)       ->  dvector
 *     dvector.div!(other)        ->  dvector
 *  
 *  When argument is a number, each entry x in _dvector_ is replaced by x / _number_.
 *  When argument is a vector, each entry x in _dvector_ is replaced by x / 
 *  the corresponding entry in the _other_ vector.
 *     
 *     a = Dvector[ 1.1, -5.7, 2.5 ]
 *     a.div!(3.8)             -> Dvector[ 1.1/3.8, -5.7/3.8, 2.5/3.8 ]
 *     a                       -> Dvector[ 1.1/3.8, -5.7/3.8, 2.5/3.8 ]
 *     a = Dvector[ 1.1, -5.7, 2.5 ]
 *     b = Dvector[ 7.1, 4.9, -10.1 ]
 *     a.div!(b)               -> Dvector[ 1.1/7.1, -5.7/4.9, 2.5/10.1 ]
 *     a                       -> Dvector[ 1.1/7.1, -5.7/4.9, 2.5/10.1 ]
 */ 
VALUE dvector_div_bang(VALUE ary, VALUE arg) {
   return dvector_apply_math_op2_bang(ary, arg, do_div);
}

static char *fill_read_buffer(char **buff_ptr, int *len_ptr, FILE *file) {
   char *buff, *new_buff;
   buff = *buff_ptr;
   int len, i, max_tries, line_len;
   long filepos = ftell(file);
   if (filepos == -1) {
      printf("ftell failed\n");
      return NULL;
   }
   max_tries = 10;
   for (i = 0; i < max_tries; i++) {
      len = *len_ptr;
      buff[len-1] = '1'; // mark the last character position
      buff = fgets(buff, len, file);
      if (buff == NULL) return NULL; // end of file
      if (buff[len-1] != '\0') {
         if (0) {
            line_len = strlen(buff);
            printf("len %i line_len %i\n", len, line_len);
            if (line_len < 80) {
               printf("line buff contains: %s\n", buff);
            } else {
               printf("line buff ends with: %s\n", buff+line_len-80);
            }
         }
         return buff;
      }
      // ran out of room -- make buffer larger and try again
      len = 10*len + 100;
      //printf("fill_read_buffer ran out of room -- increase buffer len to %i and try again\n", len);
      *len_ptr = len;
      new_buff = (char *)realloc(buff, len);
      if (new_buff == NULL) break;
      buff = new_buff;
      *buff_ptr = buff;
      if (fseek(file, filepos, SEEK_SET) != 0) {
         printf("fseek failed\n");
         return NULL;
      }
   }
   return NULL;
}

PRIVATE
/*======================================================================*/ 
VALUE Read_Dvectors(char *filename, VALUE destinations, int first_row_of_file, int number_of_rows) {
   FILE *file = NULL;
   VALUE col_obj, cols_obj, *cols_ptr = NULL;
   Dvector *d;
   double v;
   int last_row_of_file;
   int buff_len = 100;
   char *buff, *num_str, *pend, c, *cptr;
   int num_cols = 0, i, row, col, buff_loc, skip = first_row_of_file - 1;
   last_row_of_file = (number_of_rows == -1)? -1 : first_row_of_file + number_of_rows - 1;
   if ((last_row_of_file != -1 && last_row_of_file < first_row_of_file) || filename == NULL) return false;
   if (destinations != Qnil) {
      cols_obj = rb_Array(destinations);
      num_cols = RARRAY_LEN(cols_obj);
      cols_ptr = RARRAY_PTR(cols_obj);
      for (i = 0; i < num_cols; i++) { /* first pass to check validity */
         col_obj = cols_ptr[i];
         if (col_obj == Qnil) continue;
         if (!is_a_dvector(col_obj)) {
            rb_raise(rb_eArgError, "ERROR: destinations for read must be Dvectors");
         }
      }
      for (i = 0; i < num_cols; i++) { /* second pass to clear destination dvectors */
         col_obj = cols_ptr[i];
         if (col_obj == Qnil) continue;
         d = dvector_modify(col_obj);
         d->len = 0;
      }
   }
   if ((file=fopen(filename,"r")) == NULL) {
      rb_raise(rb_eArgError, "ERROR: read cannot open %s", filename);
   }
   buff = (char *)malloc(buff_len);
   if (buff == NULL) {
      fclose(file);
      rb_raise(rb_eArgError, "ERROR: allocation of read buffer failed");
   }
   for (i = 0; i < skip; i++) { /* skip over initial lines */
      if (fill_read_buffer(&buff, &buff_len, file)==NULL) {
         fclose(file);
         free(buff);
         rb_raise(rb_eArgError, "ERROR: read reached end of file before reaching line %i in %s",
            first_row_of_file, filename);
      }
   }
   for (row = 0, i = first_row_of_file; last_row_of_file == -1 || i <= last_row_of_file; row++, i++) {
      if (fill_read_buffer(&buff, &buff_len, file)==NULL) break; /* have reached end of file */
      if (destinations == Qnil) { /* create destinations */
         buff_loc = 0;
         while (true) {
            while (isspace(buff[buff_loc])) buff_loc++; /* skip leading blanks */
            if (buff[buff_loc] == '\0') break;
            while (isgraph(buff[buff_loc])) buff_loc++; /* include non-blanks */
            if (buff[buff_loc] == '\0') break;
            col_obj = dvector_new();
            if (destinations == Qnil) destinations = rb_ary_new3(1, col_obj);
            else destinations = rb_ary_push(destinations, col_obj);
         }
      }
      if (row == 0) {
         cols_obj = rb_Array(destinations);
         num_cols = RARRAY_LEN(cols_obj);
         cols_ptr = RARRAY_PTR(cols_obj);
      }
      buff_loc = 0;
      for (col = 0; col < num_cols; col++) {
         while (isspace(buff[buff_loc])) buff_loc++; /* skip leading blanks */
         if (buff[buff_loc] == '\0') {
            fclose(file);
            free(buff);
            rb_raise(rb_eArgError, "read reached end of line looking for column %i in line %i of %s", col+1, i, filename);
         }
         num_str = buff+buff_loc;
         while (isgraph(buff[buff_loc])) buff_loc++; /* include non-blanks */
         if (buff[buff_loc] == '\0') {
            fclose(file);
            free(buff);
            rb_raise(rb_eArgError, "ERROR: read reached end of line looking for column %i in line %i of %s", col+1, i, filename);
         }
         col_obj = cols_ptr[col];
         if (col_obj == Qnil) continue;
         Data_Get_Struct(col_obj, Dvector, d);
         
         buff[buff_loc] = ' ';
         // rewrite to use strtod instead of fscanf to deal with numbers from fortran like 0.501-129 for 0.501E-129
         v = strtod(num_str,&pend);
         
         if (pend != buff+buff_loc) {
         if (pend != buff+buff_loc) {
            if ((buff+buff_loc - pend >= 4) && (pend[0] == '+' || pend[0] == '-')) { // insert 'E' and try again
                cptr = pend+5; c = *cptr; pend[5] = ' '; pend[4] = pend[3]; pend[3] = pend[2]; 
                pend[2] = pend[1]; pend[1] = pend[0]; pend[0] = 'E';
                v = strtod(num_str,&pend); *cptr = c; buff_loc = pend - buff;
            } else {
                fclose(file);
                free(buff);
                pend[0] = 0;
                rb_raise(rb_eArgError, "ERROR: unreadable value in file %s in line %i: %s", filename, i , buff+buff_loc);
            }
         }
         }
         
         if (!is_okay_number(v)) {
            fclose(file);
            free(buff);
            rb_raise(rb_eArgError, "ERROR: bad value %g in line %i of %s -- %s", v, i, filename, num_str);
         }
         if (row >= d->capa) 
            Dvector_Store_Double(col_obj, row, v);
         else {
            if (row > d->len) dvector_mem_clear(d->ptr + d->len, row - d->len + 1);
            if (row >= d->len) d->len = row + 1;
            d->ptr[row] = v;
         }
      }
   }
   fclose(file);
   free(buff);
   return destinations;
}

PRIVATE 
/*
 *  call-seq:
 *     Dvector.read(filename, dest=nil, start=1, length=-1)  ->  array of dvectors
 *
 * The data on the file should be organized in columns of numbers, with one row per line.
 * The _start_ parameter determines the starting line and defaults to 1 meaning start at the first line of the file.
 * The _length_ parameter determines the number of lines to be read and defaults to -1 meaning read to the end of the file.
 * Each column of data is stored in a Dvector.  If the _dest_ argument is <code>nil</code>, the result array holds the newly created vectors
 * with the leftmost column in array entry 0, the second column in entry 1, and so on.
 * If _dest_ is not <code>nil</code>, it must be an array with entries either Dvectors or <code>nil</code>.  For entries that are Dvectors, the
 * contents of the vector are replaced by the column of data from the file.  If the entry is <code>nil</code>, that column of the file is skipped.
 *
 */
VALUE dvector_read(int argc, VALUE *argv, VALUE klass) {
   char *arg1 ;
   VALUE arg2 = Qnil;
   int arg3 = (int) 1 ;
   int arg4 = (int) -1 ;
   if ((argc < 1) || (argc > 4))
      rb_raise(rb_eArgError, "wrong # of arguments(%d) for read", argc);
   arg1 = StringValueCStr(argv[0]);
   if (argc > 1) arg2 = argv[1];
   if (argc > 2) arg3 = NUM2INT(argv[2]);
   if (argc > 3) arg4 = NUM2INT(argv[3]);
   return Read_Dvectors(arg1,arg2,arg3,arg4);
   klass = Qnil;
}
PRIVATE 
VALUE Read_Rows_of_Dvectors(char *filename, VALUE destinations, int first_row_of_file) {
   FILE *file = NULL;
   VALUE row_obj, rows_obj, *rows_ptr = NULL;
   Dvector *d;
   double v, *row_data;
   int buff_len = 1000;
   char *buff, *num_str, *pend, c, *cptr;
   int num_rows = 0, i, row, col, buff_loc, skip = first_row_of_file - 1;
   rows_obj = rb_Array(destinations);
   num_rows = RARRAY_LEN(rows_obj);
   rows_ptr = RARRAY_PTR(rows_obj);
   for (i = 0; i < num_rows; i++) { /* first pass to check validity */
      row_obj = rows_ptr[i];
      if (row_obj == Qnil) continue;
      if (!is_a_dvector(row_obj)) {
         rb_raise(rb_eArgError, "ERROR: destinations for read_rows must be Dvectors");
      }
   }
   for (i = 0; i < num_rows; i++) { /* second pass to clear destination dvectors */
      row_obj = rows_ptr[i];
      if (row_obj == Qnil) continue;
      d = dvector_modify(row_obj);
      d->len = 0;
   }
   if ((file=fopen(filename,"r")) == NULL) {
      rb_raise(rb_eArgError, "ERROR: read_rows cannot open %s", filename);
   }
   buff = (char *)malloc(buff_len);
   if (buff == NULL) {
      fclose(file);
      rb_raise(rb_eArgError, "ERROR: allocation of read buffer failed");
   }
   for (i = 0; i < skip; i++) { /* skip over initial lines */
      if (fill_read_buffer(&buff, &buff_len, file)==NULL) {
         fclose(file);
         free(buff);
         rb_raise(rb_eArgError, "ERROR: read_rows reached end of file before reaching line %i in %s",
            first_row_of_file, filename);
      }
   }
   for (row = 0, i = first_row_of_file; row < num_rows; row++, i++) {
      if (fill_read_buffer(&buff, &buff_len, file)==NULL) {
         fclose(file);
         free(buff);
         rb_raise(rb_eArgError, "ERROR: read_rows reached end of file at line %i in %s", i, filename);
      }
      row_obj = rows_ptr[row];
      if (row_obj == Qnil) continue;
      d = Get_Dvector(row_obj);
      row_data = d->ptr;
      buff_loc = 0;
      for (col = 0; ; col++) {
         while (isspace(buff[buff_loc])) buff_loc++; /* skip leading blanks */
         if (buff[buff_loc] == '\0') break;
         num_str = buff+buff_loc;
         while (isgraph(buff[buff_loc])) buff_loc++; /* include non-blanks */

         buff[buff_loc] = ' ';
         // use strtod instead of fscanf to deal with numbers from fortran like 0.501-129 for 0.501E-129
         v = strtod(num_str,&pend);
         if (pend != buff+buff_loc) {
            if ((buff+buff_loc - pend >= 4) && (pend[0] == '+' || pend[0] == '-')) { // insert 'E' and try again
                cptr = pend+5; c = *cptr; pend[5] = ' '; pend[4] = pend[3]; pend[3] = pend[2]; 
                pend[2] = pend[1]; pend[1] = pend[0]; pend[0] = 'E';
                v = strtod(num_str,&pend); *cptr = c; buff_loc = pend - buff;
            } else {
                fclose(file);
                pend[0] = 0;
                rb_raise(rb_eArgError, "ERROR: unreadable value in file %s in line %i: %s", filename, i , buff+buff_loc);
            }
         }

         if (!is_okay_number(v)) {
            fclose(file);
            free(buff);
            rb_raise(rb_eArgError, "ERROR: bad value %g in line %i of file %s", v, i, filename);
         }
         if (col < d->capa) { row_data[col] = v; d->len = col+1; }
         else {
            Dvector_Store_Double(row_obj, col, v);
            d = Get_Dvector(row_obj);
            row_data = d->ptr;
         }
      }
      if (col+10 < d->capa) {
         REALLOC_N(d->ptr, double, col);
         d->capa = col;
      }
   }
   fclose(file);
   free(buff);
   return destinations;
}


PRIVATE
/*
 *  call-seq:
 *     Dvector.read_rows(filename, dest, start=1)  ->  array of dvectors
 *
 * The data on the file should be organized in rows of numbers, with one row per line.
 * The rows need not all have the same number of entries since each row is placed in its own Dvector.
 * The _start_ parameter determines the starting line and defaults to 1 meaning start at the first line of the file.
 * The _dest_ must be an array with entries either Dvectors or <code>nil</code>.  For entries that are Dvectors, the
 * contents of the vector are replaced by the row of data from the file.  The _start_ row is placed in the
 * first entry in _dest_, the second row in the next, and so on.  If the entry in _dest_ is <code>nil</code>, that row of the file is skipped.
 *
 */ 
VALUE dvector_read_rows(int argc, VALUE *argv, VALUE klass) {
   int arg3 = (int) 1 ;
   if ((argc < 2) || (argc > 3))
      rb_raise(rb_eArgError, "wrong # of arguments(%d) for read_rows", argc);
   if (argc > 2) arg3 = NUM2INT(argv[2]);
   return Read_Rows_of_Dvectors(StringValueCStr(argv[0]),argv[1],arg3);
   klass = Qnil;
}

PRIVATE 
VALUE Read_Row(char *filename, int row, VALUE row_ary) {
   FILE *file = NULL;
   int buff_len = 1000;
   char *buff, *num_str, *pend, c, *cptr;
   int i, col, buff_loc;
   double v;
   if (row <= 0) {
      rb_raise(rb_eArgError, "ERROR: read_row line must be positive (not %i) for file %s", row, filename);
   }
   if (filename == NULL || (file=fopen(filename,"r")) == NULL) {
      rb_raise(rb_eArgError, "ERROR: read_row cannot open %s", filename);
   }
   buff = (char *)malloc(buff_len);
   if (buff == NULL) {
      fclose(file);
      rb_raise(rb_eArgError, "ERROR: allocation of read buffer failed");
   }
   for (i = 0; i < row; i++) { /* read lines until reach desired row */
      if (fill_read_buffer(&buff, &buff_len, file)==NULL) {
         fclose(file);
         free(buff);
         rb_raise(rb_eArgError, "ERROR: read_row reached end of file before reaching line %i in %s",
            row, filename);
      }
   }
   if (row_ary == Qnil) row_ary = dvector_new();
   else if (is_a_dvector(row_ary)) dvector_clear(row_ary);
   else {
      fclose(file);
      free(buff);
      rb_raise(rb_eArgError, "ERROR: destination for read_row must be a Dvector");
   }
   buff_loc = 0;
   for (col = 0; ; col++) {
      while (isspace(buff[buff_loc])) buff_loc++; /* skip leading blanks */
      if (buff[buff_loc] == '\0') break;
      num_str = buff+buff_loc;
      while (isgraph(buff[buff_loc])) buff_loc++; /* include non-blanks */
      if (buff[buff_loc] == '\0') break;

      buff[buff_loc] = ' ';
      // rewrite to use strtod instead of fscanf to deal with numbers from fortran like 0.501-129 for 0.501E-129
      v = strtod(num_str,&pend);
      if (pend != buff+buff_loc) {
         if (pend != buff+buff_loc) {
            if ((buff+buff_loc - pend >= 4) && (pend[0] == '+' || pend[0] == '-')) { // insert 'E' and try again
                cptr = pend+5; c = *cptr; pend[5] = ' '; pend[4] = pend[3]; pend[3] = pend[2]; 
                pend[2] = pend[1]; pend[1] = pend[0]; pend[0] = 'E';
                v = strtod(num_str,&pend); *cptr = c; buff_loc = pend - buff;
            } else {
                fclose(file);
                free(buff);
                pend[0] = 0;
                rb_raise(rb_eArgError, "ERROR: unreadable value in file %s in line %i: %s", filename, i , buff+buff_loc);
            }
         }
      }

      if (!is_okay_number(v)) {
         fclose(file);
         free(buff);
         rb_raise(rb_eArgError, "ERROR: bad value %g in line %i of file %s", v, i, filename);
      }
      Dvector_Store_Double(row_ary, col, v);
   }
   fclose(file);
   free(buff);
   return row_ary;
}

PRIVATE
/*
 *  call-seq:
 *     Dvector.read_row(filename, row=1, dvector=nil)  ->  dvector
 *
 *  This routine reads a row of numbers from the named file.
 *  The _row_ argument determines which line of the file to read, starting at 1 for the first line.
 *  If the _dvector_ argument is <code>nil</code>, a new Dvector is allocated to hold the row of numbers.
 *  Otherwise, the contents of _dvector_ are replaced by the numbers from the line in the file.
 *  
 */ 
VALUE dvector_read_row(int argc, VALUE *argv, VALUE klass) {
   char *arg1 ;
   int arg2 = (int) 1 ;
   VALUE arg3 = Qnil;
   if ((argc < 1) || (argc > 3))
      rb_raise(rb_eArgError, "wrong # of arguments(%d) for read_row", argc);
   arg1 = StringValueCStr(argv[0]);
   if (argc > 1) arg2 = NUM2INT(argv[1]);
   if (argc > 2) arg3 = argv[2];
   return Read_Row(arg1,arg2,arg3);
   klass = Qnil;
}

/* C API functions */

bool isa_Dvector(VALUE dvector) {
   return Is_Dvector(dvector); 
   }

long len_Dvector(VALUE dvector) {
   Dvector *d = Get_Dvector(dvector);
   return d->len;
   }

double access_Dvector(VALUE dvector, long offset) {
   Dvector *d = Get_Dvector(dvector);
   if (d->len == 0) return 0.0;
   if (offset < 0) {
      offset += d->len;
   }
   if (offset < 0 || d->len <= offset) {
      return 0.0;
   }
   return d->ptr[offset];
   } 

double *Dvector_Data_for_Read(VALUE dvector, long *len_ptr) { /* returns pointer to the dvector's data (which may be shared) */
   Dvector *d = Get_Dvector(dvector);
   if(len_ptr)
     *len_ptr = d->len;
   return d->ptr;
   }

double *Dvector_Data_Copy(VALUE dvector, long *len_ptr) { /* like Dvector_Data_for_Read, but returns pointer to a copy of the data */
   Dvector *d = Get_Dvector(dvector);
   double *data;
   if(len_ptr)
     *len_ptr = d->len;
   data = (double *)ALLOC_N(double, d->len);
   MEMCPY(data, d->ptr, double, d->len);
   return data;
   }

double *Dvector_Data_for_Write(VALUE dvector, long *len_ptr) {
   if (!is_a_dvector(dvector)) rb_raise(rb_eArgError, "arg must be a Dvector");
   Dvector *d = dvector_modify(dvector);
   if(len_ptr)
     *len_ptr = d->len;
   return d->ptr;
   }

double *Dvector_Data_Resize(VALUE dvector, long new_len) {
   if (!is_a_dvector(dvector)) rb_raise(rb_eArgError, "arg must be a Dvector");
   Dvector *d = Get_Dvector(c_Resize(dvector, new_len));
   return d->ptr;
   }

double *Dvector_Data_Replace(VALUE dvector, long len, double *data) { /* copies the data into the dvector */
   if (!is_a_dvector(dvector)) rb_raise(rb_eArgError, "arg must be a Dvector");
   return dvector_replace_dbls(dvector, len, data); }
PRIVATE    
VALUE Dvector_Create(void) { return dvector_new(); }
/*
PRIVATE int Find_First_Both_Greater(VALUE Xs, VALUE Ys, double x, double y) {
      int i;
      long xlen, ylen;
      double *X_data = Dvector_Data_for_Read(Xs, &xlen);
      double *Y_data = Dvector_Data_for_Read(Ys, &ylen);
      if (X_data == NULL || Y_data == NULL || xlen != ylen) {
         rb_raise(rb_eArgError, "ERROR: Find_First_Both_Greater args must be Dvectors of same length");
      }
      for (i=0; i < xlen; i++) {
         if (X_data[i] > x && Y_data[i] > y) return i;
         }
      return -1;
      }
   
PRIVATE int Find_First_Both_Smaller(VALUE Xs, VALUE Ys, double x, double y) {
      int i;
      long xlen, ylen;
      double *X_data = Dvector_Data_for_Read(Xs, &xlen);
      double *Y_data = Dvector_Data_for_Read(Ys, &ylen);
      if (X_data == NULL || Y_data == NULL || xlen != ylen) {
         rb_raise(rb_eArgError, "ERROR: Find_First_Both_Smaller args must be Dvectors of same length");
      }
      for (i=0; i < xlen; i++) {
         if (X_data[i] < x && Y_data[i] < y) return i;
         }
      return -1;
      }
*/
PRIVATE    
VALUE c_make_bezier_control_points_for_cubic_in_x(VALUE dest, double x0, double y0, double delta_x, double a, double b, double c)
{
   double x1, y1, x2, y2, x3, y3, *data;
   if (!Is_Dvector(dest))
      rb_raise(rb_eArgError, "Sorry: invalid 'dest' for get_control_points_for_cubic_in_x: must be a Dvector");
   double cx = delta_x, ay = a * delta_x * delta_x * delta_x, by = b * delta_x * delta_x, cy = c * delta_x;
   x1 = x0 + cx/3.0; x2 = x1 + cx/3.0; x3 = x0 + delta_x;
   y1 = y0 + cy/3.0; y2 = y1 + (cy + by)/3.0; y3 = y0 + ay + by + cy;
   data = Dvector_Data_Resize(dest, 6);
   data[0] = x1; data[1] = y1; data[2] = x2; data[3] = y2; data[4] = x3; data[5] = y3;
   return dest;
}
   
PRIVATE
/*
 *  call-seq:
 *     dvector.make_bezier_control_points_for_cubic_in_x(x0, y0, delta_x, a, b, c)
 *  
 *  Replaces contents of _dvector_ by control points for Bezier curve.
 *  The cubic, y(x), is defined from x0 to x0+delta_x.
 *  At location x = x0 + dx, with dx between 0 and delta_x, define y = a*dx^3 + b*dx^2 + c*dx + y0.
 *  This routine replaces the contents of _dest_ by [x1, y1, x2, y2, x3, y3],
 *  the Bezier control points to match this cubic.
 *     
 */ 
VALUE dvector_make_bezier_control_points_for_cubic_in_x(VALUE dest, VALUE x0, VALUE y0, VALUE delta_x, VALUE a, VALUE b, VALUE c)
{
   x0 = rb_Float(x0);
   y0 = rb_Float(y0);
   delta_x = rb_Float(delta_x);
   a = rb_Float(a);
   b = rb_Float(b);
   c = rb_Float(c);
   return c_make_bezier_control_points_for_cubic_in_x(dest,
      NUM2DBL(x0), NUM2DBL(y0), NUM2DBL(delta_x), NUM2DBL(a), NUM2DBL(b), NUM2DBL(c));
}





void c_dvector_create_pm_cubic_interpolant(int nx, double *x, double *f,
    double *As, double *Bs, double *Cs)
{
   double *h = (double *)ALLOC_N(double, nx);
   double *s = (double *)ALLOC_N(double, nx);
   double *p = (double *)ALLOC_N(double, nx);
   double as00, asm1, ap00, sm1, s00;
   int n = nx-1, i;
   
   
   for (i=0; i < n; i++) {
      h[i] = x[i+1] - x[i];
      s[i] = (f[i+1] - f[i])/h[i];
   }
   
   /* slope at i of parabola through i-1, i, and i+1 */
   for (i=1; i < n; i++) {
      p[i] = (s[i-1]*h[i] + s[i]*h[i-1])/(h[i]+h[i-1]);
   }
   
   /* "safe" slope at i to ensure monotonic -- see Steffen paper for explanation. */
   for (i=1; i < n; i++) {
      asm1 = fabs(s[i-1]);
      as00 = fabs(s[i]);
      ap00 = fabs(p[i]);
      sm1 = (s[i-1] > 0)? 1.0 : -1.0;
      s00 = (s[i] > 0)? 1.0 : -1.0;
      Cs[i] = (sm1+s00)*MIN(asm1, MIN(as00, 0.5*ap00));
   }
      
   /* slope at 1st point of parabola through 1st 3 points */
   p[0] = s[0]*(1 + h[0] / (h[0] + h[1])) - s[1] * h[0] / (h[0] + h[1]);
   
   /* safe slope at 1st point */
   if (p[0]*s[0] <= 0) {
      Cs[0] = 0;
   } else if (fabs(p[0]) > 2.0*fabs(s[0])) {
      Cs[0] = 2.0*s[0];
   } else {
      Cs[0] = p[0];
   }
   
   /* slope at last point of parabola through last 3 points */   
   p[n] = s[n-1]*(1 + h[n-1] / (h[n-1] + h[n-2])) - s[n-2]*h[n-1] / (h[n-1] + h[n-2]);

   /* safe slope at last point */    
   if (p[n]*s[n-1] <= 0) {
      Cs[n] = 0;
   } else if (fabs(p[n]) > 2.0*fabs(s[n-1])) {
      Cs[n] = 2.0*s[n-1];
   } else {
      Cs[n] = p[n];
   }
         
   for (i=0; i < n; i++) {
      Bs[i] = (3.0*s[i] - 2.0*Cs[i] - Cs[i+1]) / h[i];
   }
   Bs[n] = 0;
         
   for (i=1; i < n; i++) {
      As[i] = (Cs[i] + Cs[i+1] - 2.0*s[i]) / (h[i]*h[i]);
   }
   As[n] = 0;
      
   free(p); free(s); free(h);
}

PRIVATE
/*
 *  call-seq:
 *     Dvector.create_pm_cubic_interpolant(xs, ys) ->  interpolant
 *
 *  Uses Dvectors _xs_ and _ys_ to create a cubic pm_cubic interpolant.  The _xs_ must be given in ascending order.
 *  The interpolant is an array of Dvectors: [Xs, Ys, As, Bs, Cs].
 *  For x between Xs[j] and Xs[j+1], let dx = x - Xs[j], and find interpolated y for x by
 *  y = As[j]*dx^3 + Bs[j]*dx^2 + Cs[j]*dx + Ys[j].
 *  pm_cubic algorithms derived from Steffen, M., "A simple method for monotonic interpolation in one dimension", 
 *        Astron. Astrophys., (239) 1990, 443-450.
 *  
 */ 
VALUE dvector_create_pm_cubic_interpolant(int argc, VALUE *argv, VALUE klass) {
   if (argc != 2)
      rb_raise(rb_eArgError, "wrong # of arguments(%d) for create_pm_cubic_interpolant", argc);
   klass = Qnil;
   VALUE Xs = argv[0], Ys = argv[1];
   long xdlen, ydlen;
   double *X_data = Dvector_Data_for_Read(Xs, &xdlen);
   double *Y_data = Dvector_Data_for_Read(Ys, &ydlen);
   if (X_data == NULL || Y_data == NULL || xdlen != ydlen)
      rb_raise(rb_eArgError, "Data for create_pm_cubic_interpolant must be equal length Dvectors");
   int nx = xdlen;
   VALUE As = Dvector_Create(), Bs = Dvector_Create(), Cs = Dvector_Create();
   double *As_data = Dvector_Data_Resize(As, nx);
   double *Bs_data = Dvector_Data_Resize(Bs, nx);
   double *Cs_data = Dvector_Data_Resize(Cs, nx);
   c_dvector_create_pm_cubic_interpolant(nx, X_data, Y_data, As_data, Bs_data, Cs_data);
   VALUE result = rb_ary_new2(5);
   rb_ary_store(result, 0, Xs);
   rb_ary_store(result, 1, Ys);
   rb_ary_store(result, 2, As);
   rb_ary_store(result, 3, Bs);
   rb_ary_store(result, 4, Cs);
   return result;
}

double c_dvector_pm_cubic_interpolate(double x, int nx, 
    double *Xs, double *Ys, double *As, double *Bs, double *Cs)
{
   int j;
   for (j = 0; j < nx && x >= Xs[j]; j++);
   if (j == nx) return Ys[j-1];
   if (j == 0) return Ys[0];
   j--;
   double dx = x - Xs[j];
   return Ys[j] + dx*(Cs[j] + dx*(Bs[j] + dx*As[j]));
}

PRIVATE
/*
 *  call-seq:
 *     Dvector.pm_cubic_interpolate(x, interpolant)  ->  y
 *
 *  Returns the _y_ corresponding to _x_ by pm_cubic interpolation using the _interpolant_
 *  which was previously created by calling _create_pm_cubic_interpolant_.
 *  
 */ 
VALUE dvector_pm_cubic_interpolate(int argc, VALUE *argv, VALUE klass) {
   if (argc != 2)
      rb_raise(rb_eArgError, "wrong # of arguments(%d) for pm_cubic_interpolate", argc);
   klass = Qnil;
   VALUE x = argv[0];
   VALUE interpolant = argv[1];
   x = rb_Float(x);
   interpolant = rb_Array(interpolant);
   if (RARRAY_LEN(interpolant) != 5)
      rb_raise(rb_eArgError, "interpolant must be array of length 5 from create_pm_cubic_interpolant");
   Dvector *Xs = Get_Dvector(rb_ary_entry(interpolant,0));
   Dvector *Ys = Get_Dvector(rb_ary_entry(interpolant,1));
   Dvector *As = Get_Dvector(rb_ary_entry(interpolant,2));
   Dvector *Bs = Get_Dvector(rb_ary_entry(interpolant,3));
   Dvector *Cs = Get_Dvector(rb_ary_entry(interpolant,4));
   if (Xs->len <= 0 || Xs->len != Ys->len || Xs->len != Bs->len || Xs->len != Cs->len || Xs->len != As->len)
      rb_raise(rb_eArgError, "interpolant must be from create_pm_cubic_interpolant");
   double y = c_dvector_pm_cubic_interpolate(NUM2DBL(x), Xs->len, Xs->ptr, Ys->ptr, As->ptr, Bs->ptr, Cs->ptr);
   return rb_float_new(y);
}





void c_dvector_create_spline_interpolant(int n_pts_data, double *Xs, double *Ys,
    bool start_clamped, double start_slope, bool end_clamped, double end_slope,
    double *As, double *Bs, double *Cs)
{
   double *Hs = (double *)ALLOC_N(double, n_pts_data);
   double *alphas = (double *)ALLOC_N(double, n_pts_data);
   double *Ls = (double *)ALLOC_N(double, n_pts_data);
   double *mu_s = (double *)ALLOC_N(double, n_pts_data);
   double *Zs = (double *)ALLOC_N(double, n_pts_data);
   int n = n_pts_data-1, i, j;
   for (i=0; i < n; i++)
      Hs[i] = Xs[i+1] - Xs[i];
   if (start_clamped) alphas[0] = 3.0*(Ys[1]-Ys[0])/Hs[0] - 3.0*start_slope;
   if (end_clamped) alphas[n] = 3.0*end_slope - 3.0*(Ys[n]-Ys[n-1])/Hs[n-1];
   for (i=1; i < n; i++)
      alphas[i] = 3.0*(Ys[i+1]*Hs[i-1]-Ys[i]*(Xs[i+1]-Xs[i-1])+Ys[i-1]*Hs[i])/(Hs[i-1]*Hs[i]);
   if (start_clamped) { Ls[0] = 2.0*Hs[0]; mu_s[0] = 0.5; Zs[0] = alphas[0]/Ls[0]; }
   else { Ls[0] = 1.0; mu_s[0] = 0.0; Zs[0] = 0.0; }
   for (i = 1; i < n; i++) {
      Ls[i] = 2.0*(Xs[i+1]-Xs[i-1]) - Hs[i-1]*mu_s[i-1];
      mu_s[i] = Hs[i]/Ls[i];
      Zs[i] = (alphas[i] - Hs[i-1]*Zs[i-1])/Ls[i];
      }
   if (end_clamped) { 
      Ls[n] = Hs[n-1]*(2.0-mu_s[n-1]);
      Bs[n] = Zs[n] = (alphas[n]-Hs[n-1]*Zs[n-1])/Ls[n];
      }
   else { Ls[n] = 1.0; Zs[n] = 0.0; Bs[n] = 0.0; }
   for (j = n-1; j >= 0; j--) {
      Bs[j] = Zs[j] - mu_s[j]*Bs[j+1];
      Cs[j] = (Ys[j+1]-Ys[j])/Hs[j] - Hs[j]*(Bs[j+1]+2.0*Bs[j])/3.0;
      As[j] = (Bs[j+1]-Bs[j])/(3.0*Hs[j]);
      }
   free(Zs); free(mu_s); free(Ls); free(alphas); free(Hs);
}

PRIVATE
/*
 *  call-seq:
 *     Dvector.create_spline_interpolant(xs, ys, start_clamped, start_slope, end_clamped, end_slope) ->  interpolant
 *
 *  Uses Dvectors _xs_ and _ys_ to create a cubic spline interpolant.  The _xs_ must be given in ascending order.
 *  There is a boundary condition choice to be made for each end concerning the slope.  If clamped is true, the
 *  correspdoning slope argument value sets the slope.  If clamped is false (known as a "free" or "natural" spline),
 *  the 2nd derivative is set to 0 and the slope is determined by the fit.  In this case, the corresponding slope
 *  argument is ignored.  The interpolant is an array of Dvectors: [Xs, Ys, As, Bs, Cs].
 *  For x between Xs[j] and Xs[j+1], let dx = x - Xs[j], and find interpolated y for x by
 *  y = As[j]*dx^3 + Bs[j]*dx^2 + Cs[j]*dx + Ys[j].
 *  (Spline algorithms derived from Burden & Faires, Numerical Analysis, 4th edition, pp 131 and following.)
 *  
 */ 
VALUE dvector_create_spline_interpolant(int argc, VALUE *argv, VALUE klass) {
   if (argc != 6)
      rb_raise(rb_eArgError, "wrong # of arguments(%d) for create_spline_interpolant", argc);
   klass = Qnil;
   VALUE Xs = argv[0], Ys = argv[1];
   VALUE start_clamped = argv[2], start_slope = argv[3], end_clamped = argv[4], end_slope = argv[5];
   long xdlen, ydlen;
   double start = 0.0, end = 0.0;
   double *X_data = Dvector_Data_for_Read(Xs, &xdlen);
   double *Y_data = Dvector_Data_for_Read(Ys, &ydlen);
   if (X_data == NULL || Y_data == NULL || xdlen != ydlen)
      rb_raise(rb_eArgError, "Data for create_spline_interpolant must be equal length Dvectors");
   bool start_flg = (start_clamped == Qtrue);
   bool end_flg = (end_clamped == Qtrue);
   if (start_flg) { start_slope = rb_Float(start_slope); start = NUM2DBL(start_slope); }
   if (end_flg) { end_slope = rb_Float(end_slope); end = NUM2DBL(end_slope); }
   int n_pts_data = xdlen;
   VALUE As = Dvector_Create(), Bs = Dvector_Create(), Cs = Dvector_Create();
   double *As_data = Dvector_Data_Resize(As, n_pts_data);
   double *Bs_data = Dvector_Data_Resize(Bs, n_pts_data);
   double *Cs_data = Dvector_Data_Resize(Cs, n_pts_data);
   c_dvector_create_spline_interpolant(n_pts_data, X_data, Y_data,
      start_flg, start, end_flg, end, As_data, Bs_data, Cs_data);
   VALUE result = rb_ary_new2(5);
   rb_ary_store(result, 0, Xs);
   rb_ary_store(result, 1, Ys);
   rb_ary_store(result, 2, As);
   rb_ary_store(result, 3, Bs);
   rb_ary_store(result, 4, Cs);
   return result;
}

double c_dvector_spline_interpolate(double x, int n_pts_data, 
    double *Xs, double *Ys, double *As, double *Bs, double *Cs)
{
   int j;
   for (j = 0; j < n_pts_data && x >= Xs[j]; j++);
   if (j == n_pts_data) return Ys[j-1];
   if (j == 0) return Ys[0];
   j--;
   double dx = x - Xs[j];
   return Ys[j] + dx*(Cs[j] + dx*(Bs[j] + dx*As[j]));
}

PRIVATE
/*
 *  call-seq:
 *     Dvector.spline_interpolate(x, interpolant)  ->  y
 *
 *  Returns the _y_ corresponding to _x_ by spline interpolation using the _interpolant_
 *  which was previously created by calling _create_spline_interpolant_.
 *  
 */ 
VALUE dvector_spline_interpolate(int argc, VALUE *argv, VALUE klass) {
   if (argc != 2)
      rb_raise(rb_eArgError, "wrong # of arguments(%d) for spline_interpolate", argc);
   klass = Qnil;
   VALUE x = argv[0];
   VALUE interpolant = argv[1];
   x = rb_Float(x);
   interpolant = rb_Array(interpolant);
   if (RARRAY_LEN(interpolant) != 5)
      rb_raise(rb_eArgError, "Spline interpolant must be array of length 5 from create_spline_interpolant");
   Dvector *Xs = Get_Dvector(rb_ary_entry(interpolant,0));
   Dvector *Ys = Get_Dvector(rb_ary_entry(interpolant,1));
   Dvector *As = Get_Dvector(rb_ary_entry(interpolant,2));
   Dvector *Bs = Get_Dvector(rb_ary_entry(interpolant,3));
   Dvector *Cs = Get_Dvector(rb_ary_entry(interpolant,4));
   if (Xs->len <= 0 || Xs->len != Ys->len || Xs->len != Bs->len || Xs->len != Cs->len || Xs->len != As->len)
      rb_raise(rb_eArgError, "Spline interpolant must be from create_spline_interpolant");
   double y = c_dvector_spline_interpolate(NUM2DBL(x), Xs->len, Xs->ptr, Ys->ptr, As->ptr, Bs->ptr, Cs->ptr);
   return rb_float_new(y);
}

double c_dvector_linear_interpolate(int num_pts, double *xs, double *ys, double x)
{
   int i;
   if (num_pts == 1) return ys[0];
   for (i = 0; i < num_pts; i++) {
      if (xs[i] <= x && x < xs[i+1]) {
         return ys[i] + (ys[i+1]-ys[i])*(x-xs[i])/(xs[i+1]-xs[i]);
      }
   }
   return ys[num_pts-1];
}

PRIVATE
/*
 *  call-seq:
 *     Dvector.linear_interpolate(x, xs, ys)  ->  y
 *
 *  Returns the _y_ corresponding to _x_ by linear interpolation using the Dvectors _xs_ and _ys_.
 *  
 */ 
VALUE dvector_linear_interpolate(int argc, VALUE *argv, VALUE klass) {
   if (argc != 3)
      rb_raise(rb_eArgError, "wrong # of arguments(%d) for linear_interpolate", argc);
   klass = Qnil;
   VALUE x = argv[0];
   VALUE Xs = argv[1];
   VALUE Ys = argv[2];
   Dvector *X_data = Get_Dvector(Xs);
   Dvector *Y_data = Get_Dvector(Ys);
   if (X_data->len <= 0 || X_data->len != Y_data->len)
      rb_raise(rb_eArgError, "Xs and Ys for linear_interpolate must be equal length Dvectors: xlen %ld ylen %ld.", 
         X_data->len, Y_data->len);
   x = rb_Float(x);
   double y = c_dvector_linear_interpolate(X_data->len, X_data->ptr, Y_data->ptr, NUM2DBL(x));
   return rb_float_new(y);
}

PRIVATE
/*
 *  call-seq:
 *     Dvector.min_of_many(ary)  ->  number or nil
 *
 *  Returns the minimum value held in the array of Dvectors (or <code>nil</code> if _ary_ is empty).
 *  Any +nil+ entries in _ary_ are ignored.
 *  
 */ 
VALUE dvector_min_of_many(VALUE klass, VALUE ary) {
   VALUE *ary_ptr;
   long ary_len, i;
   Dvector *d;
   double m=0.0, tmp;
   bool found_one = false;
   ary = rb_Array(ary);
   ary_ptr = RARRAY_PTR(ary);
   ary_len = RARRAY_LEN(ary);
   if (ary_len == 0) return Qnil;
   for (i = 0; i < ary_len; i++) {
      if (ary_ptr[i] == Qnil) continue;
      d = Get_Dvector(ary_ptr[i]);
      if (d->len == 0) continue;
      if (!found_one) { m = c_dvector_min(d); found_one = true; }
      else {
         tmp = c_dvector_min(d);
         if (tmp < m) m = tmp;
      }
   }
   if (!found_one) return Qnil;
   return rb_float_new(m);
}


PRIVATE
/*
 *  call-seq:
 *     Dvector.max_of_many(ary)  ->  number or nil
 *
 *  Returns the maximum value held in the array of Dvectors (or <code>nil</code> if _ary_ is empty).
 *  Any +nil+ entries in _ary_ are ignored.
 *  
 */ 
VALUE dvector_max_of_many(VALUE klass, VALUE ary) {
   VALUE *ary_ptr;
   long ary_len, i;
   Dvector *d;
   double m=0.0, tmp;
   bool found_one = false;
   ary = rb_Array(ary);
   ary_ptr = RARRAY_PTR(ary);
   ary_len = RARRAY_LEN(ary);
   if (ary_len == 0) return Qnil;
   for (i = 0; i < ary_len; i++) {
      if (ary_ptr[i] == Qnil) continue;
      d = Get_Dvector(ary_ptr[i]);
      if (d->len == 0) continue;
      if (!found_one) { m = c_dvector_max(d); found_one = true; }
      else {
         tmp = c_dvector_max(d);
         if (tmp > m) m = tmp;
      }
   }
   if (!found_one) return Qnil;
   return rb_float_new(m);
}

#define DVECTOR_DUMP_VERSION 1

PRIVATE
/*
  Called by the marshalling mechanism to store a permanent copy of a 
  Dvector. _limit_ is simply ignored.
 */
VALUE dvector_dump(VALUE ary, VALUE limit)
{
  int i; /* for STORE_UNSIGNED */
  long len;
  double * data = Dvector_Data_for_Read(ary, &len);
  long target_len = 1 /* first signature byte */
    + 4 /* length */
    + len * 8 ;
  unsigned u_len = (unsigned) len; /* this is bad, I know, but it
				      won't hurt before it is common
				      that computers have 32 GB of RAM...
				   */

  VALUE str = rb_str_new2("");
  rb_str_resize(str,target_len); /* This seems to do the trick */
  /* \begin{playing with ruby's internals} */
  unsigned char * ptr = (unsigned char *) RSTRING_PTR(str);
  /* signature byte */
  (*ptr++) = DVECTOR_DUMP_VERSION;
  STORE_UNSIGNED(u_len, ptr); /* destroys u_len */
  while(len-- > 0)
    {
      store_double(*(data++), ptr);
      ptr += 8;
    }
  /*   RSTRING_LEN(str) = target_len; */
  return str;
  /* \end{playing with ruby's internals} */
}

PRIVATE
/*
  Called by the marshalling mechanism to retrieve a permanent copy of a 
  Dvector. 
 */
VALUE dvector_load(VALUE klass, VALUE str)
{
  VALUE ret = Qnil;
  VALUE s = StringValue(str);
  unsigned char * buf = (unsigned char *) StringValuePtr(s);
  unsigned char * dest = buf + RSTRING_LEN(s);
  unsigned i; /* for GET_UNSIGNED */
  unsigned tmp = 0;
  double * data;
  /*  depending on the first byte, the decoding will be different */
  switch(*(buf++)) 
    {
    case 1:
      GET_UNSIGNED(tmp, buf);
      /* create a new Dvector with the right size */
      ret = rb_funcall(cDvector, rb_intern("new"), 1, UINT2NUM(tmp));
      data = Dvector_Data_for_Write(ret, NULL);
      for(i = 0; i< tmp; i++)
	{
	  if(buf + 8 > dest)
	    {
	      rb_raise(rb_eRuntimeError, 
		       "corrupted data given to Dvector._load");
	      break;
	    }	
	  else 
	    {
	      data[i] = get_double(buf);
	      buf += 8;
	    }
	}
      break;
    default:
      rb_raise(rb_eRuntimeError, "corrupted data given to Dvector._load");
    }
  return ret;
}

/* Returns the boundaries of a Dvector, that is [min, max]. It ignores
   NaN and will complain if the Dvector contains only NaN.
   
    v = Dvector[0.0/0.0, 0.0/0.0, 1,2,4,5,9,0.0/0.0,0.1]
    v.bounds -> [0.1, 9]

*/
static VALUE dvector_bounds(VALUE self)
{
  double min, max;
  VALUE ret;
  long len;
  double * data = Dvector_Data_for_Read(self, &len);
  /* skip all NaNs at the beginning */
  while(len-- > 0)
    if(!isnan(*data++))
       break;
  if(len>=0)
    {
      min = max = *(data-1);
      while(len-- > 0)
	{
	  if(! isnan(*data))
	    {
	      if(*data < min)
		min = *data;
	      if(*data > max)
		max = *data;
	    }
	  data++;
	}
      ret = rb_ary_new2(2);
      rb_ary_store(ret, 0, rb_float_new(min));
      rb_ary_store(ret, 1, rb_float_new(max));
      return ret;
    }
  else
    rb_raise(rb_eRuntimeError, 
	     "bounds called on an array containing only NaN");
  return Qnil;
}

/*
  :call-seq:
    vector.convolve(kernel, middle)

  convolve applies a simple convolution to the vector using kernel centered
  at the point middle. (0 is the leftmost point of the kernel).
*/

static VALUE dvector_convolve(VALUE self, VALUE kernel, VALUE middle)
{
  long len;
  const double * values = Dvector_Data_for_Read(self, &len);
  VALUE retval = dvector_new2(len,len);
  double * ret = Dvector_Data_for_Write(retval,NULL);
  long kernel_len;
  const double * ker = Dvector_Data_for_Read(kernel, &kernel_len);
  /* I guess */
  long mid = NUM2LONG(middle);
  if(mid > kernel_len)
    rb_raise(rb_eArgError, "middle should be within kernel's range");
  else
    {
      long i,j,k;
      for(i = 0; i < len; i++) 
	{
	  double sum = 0, k_sum = 0;
	  for(j = 0; j < kernel_len; j++) 
	    {
	      /* check that we are within the vector */
	      k = i - mid + j; 	/* The current index inside the vector */
	      /* This code is equivalent to saying that the vector is
		 prolongated until infinity with values at the boundaries
		 -> no, obnoxious, I think. Simply don't take care
		 of these points
		 -> yes, finally ?
	      */
	      if( k < 0)
/* 		continue; */
		k = 0;
	      if( k >= len)
/* 		continue; */
		k = len - 1;
	      sum += ker[j] * values[k];
	      k_sum += ker[j];
	    }
	  sum/= k_sum;
	  ret[i] = sum;
	}
    }
  return retval;
}

static VALUE marked_array()
{
  VALUE v = rb_ary_new();
#if defined(RUBY_VERSION_MINOR) && (RUBY_VERSION_MINOR != 8)
  rb_gc_register_mark_object(v);
#endif
  return v;
}

/*
  :call-seq:
  Dvector.fast_fancy_read(stream, options) => Array_of_Dvectors
  
  Reads data from an IO stream (or anything that supports a gets method)
  and separate it into columns of data
  according to the _options_, a hash holding the following elements
  (compulsory, but you can use FANCY_READ_DEFAULTS):
  * 'sep': a regular expression that separate the entries
  * 'comments': any line matching this will be skipped
  * 'skip_first': skips that many lines before reading anything
  * 'index_col': if true, the first column returned contains the
    number of the line read
  * 'remove_space': whether to remove spaces at the beginning of a line.
  * 'comment_out': this should be an array into which the comments
     will be dumped one by one.
  * 'default':  what to put when nothing was found but a number must be used
  * 'last_col': when this is specified, it represents the last column which
     is read and parsed as numbers (0-based, so the 3rd column is 2). 
     The remaining is returned as text in one n+1 column
  * 'text_columns': if provided, it is an array of integers containing the 
    0-based indices of columns to be parsed as text rather than numbers.
    
  In addition to these options that control the output, here are a few
  others to tune memory allocation; these can strongly improve the
  performance (or make it worse if you wish):

  * 'initial_size': the initial size of the memory buffers: if there
    are not more lines than that, no additional memory allocation/copy
    occurs.
*/
static VALUE dvector_fast_fancy_read(VALUE self, VALUE stream, VALUE options)
{
  /* First, we read up options: */
  double def = rb_num2dbl(rb_hash_aref(options, 
				       rb_str_new2("default")));
  int remove_space = RTEST(rb_hash_aref(options, 
					rb_str_new2("remove_space")));
//  int index_col = RTEST(rb_hash_aref(options, 
//				     rb_str_new2("index_col")));
  long skip_first = FIX2LONG(rb_hash_aref(options, 
					  rb_str_new2("skip_first")));
  VALUE sep = rb_hash_aref(options, rb_str_new2("sep"));
  VALUE comments = rb_hash_aref(options, rb_str_new2("comments"));
  VALUE comment_out = rb_hash_aref(options, rb_str_new2("comment_out"));

  /* Elements after that many columns  */
  VALUE lc = rb_hash_aref(options, rb_str_new2("last_col"));
  long last_col = RTEST(lc) ? FIX2LONG(lc) : -1;
  VALUE text_columns = rb_hash_aref(options, rb_str_new2("text_columns"));

  /* Then, some various variables: */
  VALUE line;

  ID chomp_id = rb_intern("chomp!");
  ID gets_id = rb_intern("gets");
  ID max_id = rb_intern("max");
  ID size_id = rb_intern("size");
  
  /* We compute the maximum number of text columns */
  long last_text_col = last_col+1;
  VALUE mx = RTEST(text_columns) ? rb_funcall(text_columns, max_id, 0) : Qnil;
  if(RTEST(mx) && last_text_col < 0) { /* Only taking the max into
                                          account if the last col
                                          stuff is not on */
    long d = FIX2LONG(mx);
    last_text_col = d;
  }


  /* array of Ruby arrays containing the text objects of interest */
  VALUE * text_cols = NULL;

  /*
    Handling of text columns.

    The number and position of text columns has to be known in
    advance. For each of those, the value of text_columns isn't Qnil,
    and the corresponding column is NULL.

   */
  if(last_text_col >= 0) {
    text_cols = ALLOC_N(VALUE, last_text_col + 1);
    int i;
    for(i = 0; i < last_text_col + 1; i++)
      text_cols[i] = Qnil;
    if(last_col >= 0) {
      text_cols[last_col+1] = marked_array();
    }
    if(RTEST(mx)) {
      /* Todo */
      int sz = 
#ifdef RARRAY_LENINT
      RARRAY_LENINT(text_columns);
#else
      RARRAY_LEN(text_columns);
#endif
      int i;
      for(i = 0; i <  sz; i++) {
        long idx = FIX2LONG(rb_ary_entry(text_columns, i));
        if(idx >= 0 && (last_col < 0 || idx < last_col)) {
          text_cols[idx] = marked_array();
        }
      }
    }
  }




  long line_number = 0;

  /* 
     Now come the fun part - rudimentary vectors management

     TODO: if the stream provides functionality to get its total size,
     it could be interesting to estimate the total number of lines
     based on some small heuristics
   */
  int nb_vectors = 0;		/* The number of vectors currently created */
  int current_size = 10;	/* The number of slots available */
  double ** vectors = ALLOC_N(double *, current_size);
  long index = 0;		/* The current index in the vectors */
  /* The size available in the vectors */
  int allocated_size = 
    FIX2LONG(rb_hash_aref(options, rb_str_new2("initial_size"))); 


  int i;
  for(i = 0; i < current_size; i++)
    vectors[i] = NULL;

  /* The return value */
  VALUE ary;

  /* We use a real gets so we can also use StringIO, for instance */
  while(RTEST(line = rb_funcall(stream, gets_id, 0))) {
    VALUE pre, post, match;
    const char * line_ptr;
    int col = 0;
    line_number++;
    /* Whether we should skip the line... */
    if(skip_first >= line_number)
      continue;

    /* We check for a blank line using isspace: */
    line_ptr = StringValueCStr(line);
    while(line_ptr && *line_ptr) {
      if(! isspace(*line_ptr))
	break;
      line_ptr++;
    }
    if(! *line_ptr)
      continue;			/* We found a blank line  */
    if(remove_space)		/* We replace the contents of the line  */
      line = rb_str_new2(line_ptr);

    /* ... or a comment line */
    if(RTEST(comments) && RTEST(rb_reg_match(comments, line))) {
      if(RTEST(comment_out))
	rb_ary_push(comment_out, line);
      continue;
    }

    /* Then, we remove the newline: */
    post = line;
    rb_funcall(post, chomp_id, 0);

    /* We iterate over the different portions between
       matches
    */
    while(RTEST(post)) {
      const char * a;
      char * b;
      if(RTEST(rb_reg_match(sep, post))) {
	match = rb_gv_get("$~");
	pre = rb_reg_match_pre(match);
	post = rb_reg_match_post(match);
      }
      else {
	pre = post;
	post = Qnil;
      }
      if(text_cols && col <= last_text_col && RTEST(text_cols[col])) {
        rb_ary_push(text_cols[col], pre);
        if(col >= nb_vectors) {
          nb_vectors ++;
          if(col < current_size)
            vectors[col] = NULL;
        }
      }
      else {
        a = StringValueCStr(pre);
        double c = strtod(a, &b);
        if(b == a) 
          c = def;
        if(col >= nb_vectors) {
          /* We need to create a new vector */
          if(col >= current_size) { /* Increase the available size */
            current_size = col + 5;
            REALLOC_N(vectors, double * , current_size);
          }
          for(; nb_vectors <= col; nb_vectors++)
            vectors[nb_vectors] = NULL; /* default to NULL */
          
          double * vals = vectors[col] = ALLOC_N(double, allocated_size);
          /* Filling it with the default value */
          for(i = 0; i < index; i++) {
            vals[i] = def;
          }
        }
        vectors[col][index] = c;
      }
      col++;
      if(last_col >= 0 && col > last_col) {
        rb_ary_push(text_cols[last_col + 1], post);
        nb_vectors = col + 1;
        col++;
        break;
      }
    }
    /* Now, we finish the line */
    for(; col < nb_vectors; col++) {
      if(text_cols && col <= last_text_col && RTEST(text_cols[col]))
        rb_ary_push(text_cols[col], Qnil);
      else
        vectors[col][index] = def;
    }
    index++;
    /* Now, we reallocate memory if necessary */
    if(index >= allocated_size) {
      allocated_size *= 2;	/* We double the size */
      for(col = 0; col < nb_vectors; col++) {
        if(col < current_size && vectors[col])
          REALLOC_N(vectors[col], double, allocated_size);
      }
    }
  }
  /* Now, we make up the array */
  ary = rb_ary_new();
  for(i = 0; i < nb_vectors; i++) {
    /* We create a vector */
    if(text_cols && i <= last_text_col && RTEST(text_cols[i]))
      rb_ary_store(ary, i, text_cols[i]);
    else {
      rb_ary_store(ary, i, make_dvector_from_data(cDvector, index, vectors[i]));
      /* And free the memory */
      free(vectors[i]);
    }
  }
  free(vectors);
  if(text_cols)
    free(text_cols);
  return ary;
}

/*
  Returns a list of local extrema of the vector, organized thus:
  
   [ [:min, idmin1], [:max, idmax1], ...]
  
  The values are pushed in the order in which they are found. It works
  thus: it scans the vector and looks around the current point in a
  given window. If the current point is the maximum or the minimum, it
  is considered as a local maximum/minimum. Control over which extrema
  are included is given to the used through threshold mechanisms.

  The _options_ hash controls how the peaks are detected:
  * _window_: the number of elements on which we look on
    both sides (default 5, ie the local maximum is over 11 points)
  * _threshold_: the minimum amplitude the extrema must have to
    be considered (default 0)
  * _dthreshold_: how much over/under the average an extremum must be
    (default 0) 
  * _or_: whether the _threshold_ and _dthreshold_ tests are both
    necessary or if only one is (default false: both tests are
    necessary)

    *Note:* beware of NANs ! They *will* screw up peak detection, as
  they are neither bigger nor smaller than anything...  
*/
static VALUE dvector_extrema(int argc, VALUE *argv, VALUE self)
{
  long window = 5;
  double threshold = 0;
  double dthreshold = 0;
  int inclusive = 1;
  
  if(argc == 1) {
    VALUE t;
    t = rb_hash_aref(argv[0], rb_str_new2("window"));
    if(RTEST(t)) {
      window = FIX2LONG(t);
    }
    t = rb_hash_aref(argv[0], rb_str_new2("threshold"));
    if(RTEST(t)) {
      threshold = rb_num2dbl(t);
    }
    t = rb_hash_aref(argv[0], rb_str_new2("dthreshold"));
    if(RTEST(t)) {
      dthreshold = rb_num2dbl(t);
    }
    
    t = rb_hash_aref(argv[0], rb_str_new2("or"));
    inclusive = ! RTEST(t);
  } else if(argc > 1)
    rb_raise(rb_eArgError, "Dvector.extrema only takes 0 or 1 argument");

  /* Handling of the vector */
  long len, i,j;
  double * data = Dvector_Data_for_Read(self, &len);
  VALUE s_min = ID2SYM(rb_intern("min"));
  VALUE s_max = ID2SYM(rb_intern("max"));

  

  VALUE ret = rb_ary_new();
		       
  for(i = 0; i < len; i++) {

    /* This is stupid and will need decent optimization when I have
       time */
    long first = i > window ? i - window : 0;
    double cur_min = data[first];
    long cur_min_idx = first;
    double cur_max = data[first];
    long cur_max_idx = first;
    double average = 0;
    long nb = 0;
    
    for(j = first; (j < i+window) && (j < len); j++,nb++) {
      average += data[j];
      if(data[j] <= cur_min) {
	cur_min = data[j];
	cur_min_idx = j;
      }
      if(data[j] >= cur_max) {
	cur_max = data[j];
	cur_max_idx = j;
      }
    }
    average /= nb;

    if(cur_min_idx == i) {
      /* This is a potential minimum */
      if((inclusive && 
	  (fabs(cur_min) >= threshold) && 
	  (fabs(cur_min - average) >= dthreshold))
	 || (!inclusive && 
	     ((fabs(cur_min) >= threshold) ||
	      (fabs(cur_min - average) >= dthreshold))
	     )) {
	VALUE min = rb_ary_new();
	rb_ary_push(min, s_min);
	rb_ary_push(min, LONG2FIX(i));
	rb_ary_push(ret, min);
      }
    }
    else if(cur_max_idx == i) {
      /* A potential maximum */
      if((inclusive && 
	  (fabs(cur_max) >= threshold) && 
	  (fabs(cur_max - average) >= dthreshold))
	 || (!inclusive && 
	     ((fabs(cur_max) >= threshold) ||
	      (fabs(cur_max - average) >= dthreshold))
	     )) {
	VALUE max = rb_ary_new();
	rb_ary_push(max, s_max);
	rb_ary_push(max, LONG2FIX(i));
	rb_ary_push(ret, max);
      }
    }
  }
  return ret;
}

/* This is the FFTW-based part of the game */
#ifdef HAVE_FFTW3_H
#include <fftw3.h>

/* 
   Performs an in-place Fourier transform of the vector. The results
   is stored in the so-called "half-complex" format (see
   http://www.fftw.org/fftw3_doc/The-Halfcomplex_002dformat-DFT.html
   for more information).

*/
static VALUE dvector_fft(VALUE self)
{
  long len;
  double * values = Dvector_Data_for_Write(self, &len);
  fftw_plan plan = fftw_plan_r2r_1d(len, values, values,
				    FFTW_R2HC, FFTW_ESTIMATE);
  fftw_execute(plan);
  fftw_destroy_plan(plan);
  return self;
}

/* 
   Performs a reverse in-place Fourier transform of the vector. The
   original data must have been stored in the so called "half-complex"
   format (see #fft!).
*/

static VALUE dvector_rfft(VALUE self)
{
  long len;
  double * values = Dvector_Data_for_Write(self, &len);
  fftw_plan plan = fftw_plan_r2r_1d(len, values, values,
				    FFTW_HC2R, FFTW_ESTIMATE);
  fftw_execute(plan);
  fftw_destroy_plan(plan);
  return self;
}

/* 
   Now, small functions to manipulate the FFTed data:
   * multiply them
   * divide them
   * get (the square of) their module
*/


/* 
   Returns the power spectra of the ffted data (ie the square of the
   norm of the complex fourier coefficients.
   
   The returned value is a new Dvector of size about two times smaller
   than the original (precisely size/2 + 1)

   For some reasons, convolutions don't work for now.
*/
static VALUE dvector_fft_spectrum(VALUE self)
{
  long len;
  const double * values = Dvector_Data_for_Read(self, &len);
  /* First compute the size of the target: */
  long target_size = len/2+1;
  long i;
  VALUE retval = dvector_new2(target_size,target_size);
  double * ret = Dvector_Data_for_Write(retval,NULL);

  /* Pointer to real and imaginary parts */
  const double * real;
  const double * img;
  ret[0] = values[0] * values[0];


  /* The Nyquist frequency */
  if(len % 2 == 0)
    ret[target_size - 1] = values[target_size-1] * values[target_size-1];
  for(i = 1, real = values + 1, img = values + len-1; i < len/2;
      i++, real++, img--)
    ret[i] = *real * *real + *img * *img;
  return retval;
}

/*
  Converts the FFTed data in the complex conjugate
*/
static VALUE dvector_fft_conj(VALUE self)
{
  long len;
  double * v1 = Dvector_Data_for_Write(self, &len);
  double * img;
  long i;
  for(i = 1, img = v1 + len-1; i < (len+1)/2;
      i++, img--)
    *img = -*img;
  return self;
}


/* 
   Multiplies the FFTed data held in the vector by another vector. The
   behaviour depends on the size of the target vector:
   
   * if it is the same size, it is assumed to be FFTed data
   * if it is the same size of a power spectrum, then it is assumed that it
     is multiplication by real values
   * anything else won't make this function happy.

   As a side note, if you only want multiplication by a scalar, the
   standard #mul! should be what you look for.
 */
static VALUE dvector_fft_mul(VALUE self, VALUE m)
{
  long len;
  double * v1 = Dvector_Data_for_Write(self, &len);
  long len2;
  const double * v2 = Dvector_Data_for_Write(m, &len2);
  if(len2 == len) {		/* Full complex multiplication */
    const double * m_img;
    const double * m_real;
    double * v_img;
    double * v_real;
    long i;
    /* First, special cases */
    v1[0] *= v2[0];
    if(len % 2 == 0)
      v1[len/2] *= v2[len/2];
    
    for(i = 1, m_real = v2 + 1, m_img = v2 + len-1,
	  v_real = v1 + 1, v_img = v1 + len-1; i < (len+1)/2;
	i++, m_real++, v_real++, m_img--, v_img--) {
      double r = *m_real * *v_real - *m_img * *v_img;
      *v_img = *m_real * *v_img + *v_real * *m_img;
      *v_real = r;
    }
    return self;
  }
  else if(len2 == len/2+1) {		/* Complex * real*/
    const double * val;
    double * v_img;
    double * v_real;
    long i;
    /* First, special cases */
    v1[0] *= v2[0];
    if(len % 2 == 0)
      v1[len/2] *= v2[len/2];
    
    for(i = 1, val = v2 + 1,
	  v_real = v1 + 1, v_img = v1 + len-1; i < (len+1)/2;
	i++, val++, v_real++, v_img--) {
      *v_real *= *val;
      *v_img *= *val;
    }
    return self;
  }
  else {
    rb_raise(rb_eArgError, "incorrect Dvector size for fft_mul!");
  }
}


#endif


/* 
 * Document-class: Dobjects::Dvector
 *
 * Dvectors are a specialized implementation of one-dimensional arrays of double precision floating point numbers. 
 * They are intended for use in applications needing efficient processing of large vectors of numeric data.
 * Essentially any of the operations you might do with a 1D Ruby Array of numbers can also be done with a Dvector.
 * Just like Arrays, Dvector indexing starts at 0.  A negative index is 
 * assumed to be relative to the end of the vector---that is, an index of -1 
 * indicates the last element of the vector, -2 is the next to last 
 * element in the vector, and so on.  Element reference and element assignment are the same as for Arrays,
 * allowing for (start,length) or (range) as well as for simple indexing.  All of the other Array operations that
 * make sense for a 1D array of numbers are also provided for Dvectors.  For example, you can "fetch" or "fill"
 * with Dvectors, but there is no "assoc" or "compact" for them since one looks for arrays as elements
 * and the other looks for +nil+ elements, neither of which are found in Dvectors.
 *
 * In addition to the usual Array methods, there are a variety of
 * others that operate on the contents of the entire vector without the use of
 * explicit iterators.  These routines are crucial for efficient processing of large vectors. 
 * For example, "_vec_.sqrt" will create a vector of square roots of entries in  _vec_
 * more efficiently than the semantically equivalent form "_vec_.collect { |x| sqrt(x) }".
 *
 * All of the numeric methods also have 
 * 'bang' versions that modify the contents of the vector in place; for example, "_vec_.sqrt!"
 * is more efficient than "_vec_ = _vec_.sqrt".  By providing implicit iterators and in-place modification,
 * Dvectors make it possible to operate on large vectors of doubles in Ruby at speeds
 * closely approaching a C implementation.
 *
 * As a final example, for diehards only, say we have large vectors holding values of abundances of total helium
 * (<i>xhe</i>), singly ionized helium (<i>xhe1</i>), and doubly ionized helium (<i>xhe2</i>).
 * We're missing the values for neutral helium, but that is just what's left of total helium
 * after you subtract the two ionized forms, so it is easy to compute it.  Finally, we need to
 * calculate the log of the abundance of neutral helium and store it in another vector (<i>log_xhe0</i>).  
 * If we don't care about creating work for the garbage collector, we can simply do this
 *          log_xhe0 = (xhe - xhe1 - xhe2).log10
 * This works, but it creates multiple temporary vectors for intermediate results.
 * If, like me, you're compulsive about efficiency, you can do the whole thing with no garbage created at all:
 *          log_xhe0 = Dvector.new
 *          log_xhe0.replace(xhe).sub!(xhe1).sub!(xhe2).log10!
 * This copies <i>xhe</i> to <i>log_xhe0</i>, subtracts <i>xhe1</i> and <i>xhe2</i> from <i>log_xhe0</i> in place,
 * and then takes the +log+, also in place.  It's not pretty, but it is efficient -- use if needed.
 *
 * Please report problems with the Dvector extension to the <tt>tioga-users</tt> at <tt>rubyforge.org</tt> mailing list.
 * [Note: for N-dimensional arrays or arrays of complex numbers or integers as well as doubles,
 * along with a variety of matrix operations,
 * check out the NArray[http://www.ir.isas.ac.jp/~masa/ruby/index-e.html] extension.]
 *
 * Dvector now also prides itselfs with a _dump and a _load function, which
 * means you can Marshal them.
 */


void Init_Dvector() {
  
  /* modified by Vincent Fourmond for the splitting out:
     we use the Dvector module. I don't know if it is a good idea...
  */
   VALUE mDobjects = rb_define_module("Dobjects");
   cDvector = rb_define_class_under(mDobjects, "Dvector", rb_cObject);
   rb_include_module(cDvector, rb_mEnumerable);
   
   rb_define_alloc_func(cDvector, dvector_alloc);
   rb_define_singleton_method(cDvector, "[]", dvector_s_create, -1);
   rb_define_singleton_method(cDvector, "read", dvector_read, -1);
   rb_define_singleton_method(cDvector, "read_columns", dvector_read, -1);
   rb_define_singleton_method(cDvector, "read_rows", dvector_read_rows, -1);
   rb_define_singleton_method(cDvector, "read_row", dvector_read_row, -1);
   rb_define_singleton_method(cDvector, "create_spline_interpolant", dvector_create_spline_interpolant, -1);
   rb_define_singleton_method(cDvector, "spline_interpolate", dvector_spline_interpolate, -1);

   rb_define_singleton_method(cDvector, "create_pm_cubic_interpolant", dvector_create_pm_cubic_interpolant, -1);
   rb_define_singleton_method(cDvector, "pm_cubic_interpolate", dvector_pm_cubic_interpolate, -1);

   rb_define_singleton_method(cDvector, "linear_interpolate", dvector_linear_interpolate, -1);
   rb_define_singleton_method(cDvector, "min_of_many", dvector_min_of_many, 1);
   rb_define_singleton_method(cDvector, "max_of_many", dvector_max_of_many, 1);

   rb_define_singleton_method(cDvector, "is_a_dvector", dvector_is_a_dvector, 1);

   
   rb_define_method(cDvector, "make_bezier_control_points_for_cubic_in_x", 
      dvector_make_bezier_control_points_for_cubic_in_x, 6);
   
   rb_define_method(cDvector, "initialize", dvector_initialize, -1);
   rb_define_method(cDvector, "initialize_copy", dvector_replace, 1);
   
   rb_define_method(cDvector, "tridag", dvector_tridag, 4);
   
   rb_define_method(cDvector, "to_s", dvector_to_s, 0);
   rb_define_alias(cDvector,  "inspect", "to_s");
   rb_define_method(cDvector, "to_a", dvector_to_a, 0);
   rb_define_alias(cDvector,  "to_ary", "to_a");
   rb_define_method(cDvector, "freeze",  dvector_freeze, 0);
   rb_define_method(cDvector, "frozen?",  dvector_frozen_p, 0);
   
   rb_define_method(cDvector, "eql?", dvector_eql, 1);
   rb_define_alias(cDvector,  "==", "eql?");
   
   rb_define_method(cDvector, "[]", dvector_aref, -1);
   rb_define_alias(cDvector,  "slice", "[]");
   rb_define_method(cDvector, "[]=", dvector_aset, -1);
   rb_define_method(cDvector, "at", dvector_at, 1);
   rb_define_method(cDvector, "fetch", dvector_fetch, -1);
   rb_define_method(cDvector, "first", dvector_first, -1);
   rb_define_method(cDvector, "last", dvector_last, -1);
   rb_define_method(cDvector, "resize", dvector_resize, 1);
   rb_define_method(cDvector, "concat", dvector_concat, 1);
   rb_define_method(cDvector, "<<", dvector_push, 1);
   rb_define_method(cDvector, "push", dvector_push_m, -1);
   rb_define_method(cDvector, "pop", dvector_pop, 0);
   rb_define_method(cDvector, "shift", dvector_shift, 0);
   rb_define_method(cDvector, "unshift", dvector_unshift_m, -1);
   rb_define_method(cDvector, "insert", dvector_insert, -1);
   rb_define_method(cDvector, "each", dvector_each, 0);
   rb_define_method(cDvector, "each_index", dvector_each_index, 0);
   rb_define_method(cDvector, "each_with_index", dvector_each_with_index, 0);
   rb_define_method(cDvector, "reverse_each", dvector_reverse_each, 0);
   rb_define_method(cDvector, "length", dvector_length, 0);
   rb_define_alias(cDvector,  "size", "length");
   rb_define_alias(cDvector,  "nitems", "length");
   rb_define_method(cDvector, "empty?", dvector_empty_p, 0);
   rb_define_method(cDvector, "index", dvector_index, 1);
   rb_define_method(cDvector, "rindex", dvector_rindex, 1);
   rb_define_method(cDvector, "join", dvector_join_m, -1);
   rb_define_method(cDvector, "reverse", dvector_reverse_m, 0);
   rb_define_method(cDvector, "reverse!", dvector_reverse_bang, 0);
   rb_define_method(cDvector, "sort", dvector_sort, 0);
   rb_define_method(cDvector, "sort!", dvector_sort_bang, 0);
   rb_define_method(cDvector, "collect", dvector_collect, 0);
   rb_define_method(cDvector, "collect!", dvector_collect_bang, 0);
   rb_define_method(cDvector, "prune", dvector_prune, 1);
   rb_define_method(cDvector, "prune!", dvector_prune_bang, 1);
   rb_define_alias(cDvector,  "map", "collect");
   rb_define_alias(cDvector,  "map!", "collect!");
   rb_define_method(cDvector, "dup", dvector_dup, 0);
   rb_define_method(cDvector, "select", dvector_select, 0);
   rb_define_method(cDvector, "values_at", dvector_values_at, -1);
   rb_define_method(cDvector, "delete", dvector_delete, 1);
   rb_define_method(cDvector, "delete_at", dvector_delete_at_m, 1);
   rb_define_method(cDvector, "delete_if", dvector_delete_if, 0);
   rb_define_method(cDvector, "reject", dvector_reject, 0);
   rb_define_method(cDvector, "reject!", dvector_reject_bang, 0);
   rb_define_method(cDvector, "replace", dvector_replace, 1);
   rb_define_method(cDvector, "fill", dvector_fill, -1);
   rb_define_method(cDvector, "include?", dvector_includes, 1);
   rb_define_method(cDvector, "<=>", dvector_cmp, 1);
   rb_define_method(cDvector, "slice!", dvector_slice_bang, -1);
   rb_define_method(cDvector, "uniq", dvector_uniq, 0);
   rb_define_method(cDvector, "uniq!", dvector_uniq_bang, 0);
   rb_define_method(cDvector, "reverse_each_index", dvector_reverse_each_index, 0);
   rb_define_method(cDvector, "reverse_each_with_index", dvector_reverse_each_with_index, 0);
   rb_define_method(cDvector, "each2", dvector_each2, 1);
   rb_define_method(cDvector, "each2_with_index", dvector_each2_with_index, 1);
   rb_define_method(cDvector, "reverse_each2", dvector_reverse_each2, 1);
   rb_define_method(cDvector, "reverse_each2_with_index", dvector_reverse_each2_with_index, 1);
   rb_define_method(cDvector, "each3", dvector_each3, 2);
   rb_define_method(cDvector, "each3_with_index", dvector_each3_with_index, 2);
   rb_define_method(cDvector, "reverse_each3", dvector_reverse_each3, 2);
   rb_define_method(cDvector, "reverse_each3_with_index", dvector_reverse_each3_with_index, 2);
   
   rb_define_method(cDvector, "collect2", dvector_collect2, 1);
   rb_define_method(cDvector, "collect2!", dvector_collect2_bang, 1);
   rb_define_alias(cDvector,  "map2", "collect2");
   rb_define_alias(cDvector,  "map2!", "collect2!");

   rb_define_method(cDvector, "where_max", dvector_where_max, 0);
   rb_define_alias(cDvector,  "where_first_max", "where_max");
   rb_define_method(cDvector, "where_last_max", dvector_where_last_max, 0);
   rb_define_method(cDvector, "where_min", dvector_where_min, 0);
   rb_define_alias(cDvector,  "where_first_min", "where_min");
   rb_define_method(cDvector, "where_last_min", dvector_where_last_min, 0);
   rb_define_method(cDvector, "where_closest", dvector_where_closest, 1);
   rb_define_alias(cDvector,  "where_first_closest", "where_closest");
   rb_define_method(cDvector, "where_last_closest", dvector_where_last_closest, 0);
   rb_define_method(cDvector, "where_first_eq", dvector_where_first_eq, 1);
   rb_define_alias(cDvector,  "where_eq", "where_first_eq");
   rb_define_method(cDvector, "where_first_ne", dvector_where_first_ne, 1);
   rb_define_alias(cDvector,  "where_ne", "where_first_ne");
   rb_define_method(cDvector, "where_first_lt", dvector_where_first_lt, 1);
   rb_define_alias(cDvector,  "where_lt", "where_first_lt");
   rb_define_method(cDvector, "where_first_le", dvector_where_first_le, 1);
   rb_define_alias(cDvector,  "where_le", "where_first_le");
   rb_define_method(cDvector, "where_first_gt", dvector_where_first_gt, 1);
   rb_define_alias(cDvector,  "where_gt", "where_first_gt");
   rb_define_method(cDvector, "where_first_ge", dvector_where_first_ge, 1);
   rb_define_alias(cDvector,  "where_ge", "where_first_ge");
   rb_define_method(cDvector, "where_last_eq", dvector_where_last_eq, 1);
   rb_define_method(cDvector, "where_last_ne", dvector_where_last_ne, 1);
   rb_define_method(cDvector, "where_last_lt", dvector_where_last_lt, 1);
   rb_define_method(cDvector, "where_last_le", dvector_where_last_le, 1);
   rb_define_method(cDvector, "where_last_gt", dvector_where_last_gt, 1);
   rb_define_method(cDvector, "where_last_ge", dvector_where_last_ge, 1);
   
   rb_define_method(cDvector, "max", dvector_max, -1);
   rb_define_method(cDvector, "min", dvector_min, -1);
   rb_define_method(cDvector, "clear", dvector_clear, 0);
   rb_define_method(cDvector, "set", dvector_set, 1);
   rb_define_method(cDvector, "min_gt", dvector_min_gt, 1);
   rb_define_method(cDvector, "max_lt", dvector_max_lt, 1);
   rb_define_method(cDvector, "bounds", dvector_bounds, 0);

   
   rb_define_method(cDvector, "sum", dvector_sum, 0);
   rb_define_method(cDvector, "dot", dvector_dot, 1);
   rb_define_method(cDvector, "vector_length", dvector_vector_length, 0);
   
   rb_define_method(cDvector, "add", dvector_add, 1);
   rb_define_alias(cDvector,  "+", "add");
   rb_define_alias(cDvector,  "plus", "add");
   rb_define_method(cDvector, "sub", dvector_sub, 1);
   rb_define_alias(cDvector,  "-", "sub");
   rb_define_alias(cDvector,  "minus", "sub");
   rb_define_method(cDvector, "mul", dvector_mul, 1);
   rb_define_alias(cDvector,  "*", "mul");
   rb_define_alias(cDvector,  "times", "mul");
   
   rb_define_method(cDvector, "div", dvector_div, 1);
   rb_define_alias(cDvector,  "/", "div");
   rb_define_method(cDvector, "modulo", dvector_mod, 1);
   rb_define_alias(cDvector,  "mod", "modulo");
   rb_define_alias(cDvector,  "%", "modulo");
   rb_define_method(cDvector, "remainder", dvector_remainder, 1);
   rb_define_method(cDvector, "pow", dvector_pow, 1);
   rb_define_alias(cDvector,  "raised_to", "pow");
   rb_define_alias(cDvector,  "**", "pow");
   rb_define_method(cDvector, "as_exponent_of", dvector_as_exponent_of, 1);
   rb_define_method(cDvector, "atan2", dvector_atan2, 1);
   
   /* numeric methods */
   rb_define_method(cDvector, "abs", dvector_abs, 0);
   rb_define_method(cDvector, "ceil", dvector_ceil, 0);
   rb_define_method(cDvector, "floor", dvector_floor, 0);
   rb_define_method(cDvector, "round", dvector_round, 0);

   /* standard math functions */
   rb_define_method(cDvector, "acos", dvector_acos, 0);
   rb_define_method(cDvector, "acosh", dvector_acosh, 0);
   rb_define_method(cDvector, "asin", dvector_asin, 0);
   rb_define_method(cDvector, "asinh", dvector_asinh, 0);
   rb_define_method(cDvector, "atan", dvector_atan, 0);
   rb_define_method(cDvector, "atanh", dvector_atanh, 0);
   rb_define_method(cDvector, "cos", dvector_cos, 0);
   rb_define_method(cDvector, "cosh", dvector_cosh, 0);
   rb_define_method(cDvector, "exp", dvector_exp, 0);
   rb_define_method(cDvector, "log", dvector_log, 0);
   rb_define_method(cDvector, "log10", dvector_log10, 0);
   rb_define_method(cDvector, "sin", dvector_sin, 0);
   rb_define_method(cDvector, "sinh", dvector_sinh, 0);
   rb_define_method(cDvector, "sqrt", dvector_sqrt, 0);
   rb_define_method(cDvector, "tan", dvector_tan, 0);
   rb_define_method(cDvector, "tanh", dvector_tanh, 0);

   /* nonstandard math functions */
   rb_define_method(cDvector, "neg", dvector_neg, 0);
   rb_define_alias(cDvector, "-@", "neg");
   rb_define_method(cDvector, "exp10", dvector_exp10, 0);
   rb_define_method(cDvector, "inv", dvector_inv, 0);
   rb_define_method(cDvector, "trim", dvector_trim, -1);
   rb_define_method(cDvector, "safe_log", dvector_safe_log, -1);
   rb_define_method(cDvector, "safe_log10", dvector_safe_log10, -1);
   rb_define_method(cDvector, "safe_inv", dvector_safe_inv, 1);
   rb_define_method(cDvector, "safe_sqrt", dvector_safe_sqrt, 0);
   rb_define_method(cDvector, "safe_asin", dvector_safe_asin, 0);
   rb_define_method(cDvector, "safe_acos", dvector_safe_acos, 0);

   rb_define_method(cDvector, "add!", dvector_add_bang, 1);
   rb_define_method(cDvector, "sub!", dvector_sub_bang, 1);
   rb_define_method(cDvector, "mul!", dvector_mul_bang, 1);
   
   rb_define_alias(cDvector,  "plus!", "add!");
   rb_define_alias(cDvector,  "minus!", "sub!");
   rb_define_alias(cDvector,  "times!", "mul!");
   
   rb_define_method(cDvector, "div!", dvector_div_bang, 1);
   rb_define_method(cDvector, "modulo!", dvector_modulo_bang, 1);
   rb_define_alias(cDvector,  "mod!", "modulo!");
   rb_define_method(cDvector, "remainder!", dvector_remainder_bang, 1);
   rb_define_method(cDvector, "pow!", dvector_pow_bang, 1);
   rb_define_alias(cDvector,  "raised_to!", "pow!");
   rb_define_method(cDvector, "as_exponent_of!", dvector_as_exponent_of_bang, 1);
   rb_define_method(cDvector, "atan2!", dvector_atan2_bang, 1);

   rb_define_method(cDvector, "neg!", dvector_neg_bang, 0);
   rb_define_method(cDvector, "abs!", dvector_abs_bang, 0);
   rb_define_method(cDvector, "sin!", dvector_sin_bang, 0);
   rb_define_method(cDvector, "cos!", dvector_cos_bang, 0);
   rb_define_method(cDvector, "tan!", dvector_tan_bang, 0);
   rb_define_method(cDvector, "asin!", dvector_asin_bang, 0);
   rb_define_method(cDvector, "acos!", dvector_acos_bang, 0);
   rb_define_method(cDvector, "atan!", dvector_atan_bang, 0);
   rb_define_method(cDvector, "sinh!", dvector_sinh_bang, 0);
   rb_define_method(cDvector, "cosh!", dvector_cosh_bang, 0);
   rb_define_method(cDvector, "tanh!", dvector_tanh_bang, 0);
   rb_define_method(cDvector, "asinh!", dvector_asinh_bang, 0);
   rb_define_method(cDvector, "acosh!", dvector_acosh_bang, 0);
   rb_define_method(cDvector, "atanh!", dvector_atanh_bang, 0);
   rb_define_method(cDvector, "ceil!", dvector_ceil_bang, 0);
   rb_define_method(cDvector, "floor!", dvector_floor_bang, 0);
   rb_define_method(cDvector, "round!", dvector_round_bang, 0);
   rb_define_method(cDvector, "exp!", dvector_exp_bang, 0);
   rb_define_method(cDvector, "exp10!", dvector_exp10_bang, 0);
   rb_define_method(cDvector, "log!", dvector_log_bang, 0);
   rb_define_method(cDvector, "log10!", dvector_log10_bang, 0);
   rb_define_method(cDvector, "inv!", dvector_inv_bang, 0);
   rb_define_method(cDvector, "sqrt!", dvector_sqrt_bang, 0);
   
   rb_define_method(cDvector, "trim!", dvector_trim_bang, -1);
   rb_define_method(cDvector, "safe_log!", dvector_safe_log_bang, -1);
   rb_define_method(cDvector, "safe_log10!", dvector_safe_log10_bang, -1);
   rb_define_method(cDvector, "safe_inv!", dvector_safe_inv_bang, -1);
   rb_define_method(cDvector, "safe_sqrt!", dvector_safe_sqrt_bang, 0);
   rb_define_method(cDvector, "safe_asin!", dvector_safe_asin_bang, 0);
   rb_define_method(cDvector, "safe_acos!", dvector_safe_acos_bang, 0);

   /* dirty flag related methods */
   rb_define_method(cDvector, "dirty?", dvector_is_dirty, 0);
   rb_define_method(cDvector, "clean?", dvector_is_clean, 0);
   rb_define_alias(cDvector,  "dirty", "dirty?");
   rb_define_method(cDvector, "dirty=", dvector_set_dirty, 1);

   /* dvector marshalling */
   rb_define_method(cDvector, "_dump", dvector_dump, 1);
   rb_define_singleton_method(cDvector, "_load", dvector_load, 1);

   /* simple convolution */
   rb_define_method(cDvector, "convolve", dvector_convolve, 2);

   /* Fast fancy read: */
   rb_define_singleton_method(cDvector, "fast_fancy_read", 
			      dvector_fast_fancy_read, 2);


   /* Local extrema */
   rb_define_method(cDvector, "extrema", dvector_extrema, -1);

   /* FFT functions */
#ifdef HAVE_FFTW3_H

   rb_define_method(cDvector, "fft!", dvector_fft, 0);
   rb_define_method(cDvector, "rfft!", dvector_rfft, 0);
   rb_define_method(cDvector, "fft_spectrum", dvector_fft_spectrum, 0);
   rb_define_method(cDvector, "fft_mul!", dvector_fft_mul, 1);
   rb_define_method(cDvector, "fft_conj!", dvector_fft_conj, 0);

#endif


   dvector_output_fs = Qnil;
   rb_global_variable(&dvector_output_fs);
   dvector_output_fs = rb_str_new2(" ");

   /* modified by Vincent Fourmond 22/3/2006 for the splitting out */
   rb_require("Dobjects/Dvector_extras.rb");
   rb_require("Dobjects/Numeric_extras.rb");
   /* end of modifications */

   /* now, the fun part: exporting binary symbols.
      see include/dvector.h for their meaning 
   */
   RB_EXPORT_SYMBOL(cDvector, isa_Dvector);
   RB_EXPORT_SYMBOL(cDvector, len_Dvector);
   RB_EXPORT_SYMBOL(cDvector, access_Dvector);
   RB_EXPORT_SYMBOL(cDvector, Dvector_Data_for_Read);
   RB_EXPORT_SYMBOL(cDvector, Dvector_Data_Copy);
   RB_EXPORT_SYMBOL(cDvector, Dvector_Data_for_Write);
   RB_EXPORT_SYMBOL(cDvector, Dvector_Data_for_Read);
   RB_EXPORT_SYMBOL(cDvector, Dvector_Data_Resize);
   RB_EXPORT_SYMBOL(cDvector, Dvector_Data_Replace);
   RB_EXPORT_SYMBOL(cDvector, Dvector_Create);
   RB_EXPORT_SYMBOL(cDvector, Dvector_Store_Double);
   RB_EXPORT_SYMBOL(cDvector, Dvector_Push_Double);
   RB_EXPORT_SYMBOL(cDvector, c_dvector_spline_interpolate);
   RB_EXPORT_SYMBOL(cDvector, c_dvector_linear_interpolate);
   RB_EXPORT_SYMBOL(cDvector, c_dvector_create_spline_interpolant);
   /* I guess that this should be all */
}


