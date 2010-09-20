/* dtable.c */
/*
   Copyright (C) 2004  Bill Paxton

   This file is part of Dtable.

   Dtable is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Library Public License as published
   by the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.

   Dtable is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with Dtable; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
*/

#include <namespace.h>   
#include "dtable_intern.h"
#include <stdbool.h>
#include <stdlib.h>
#include <ctype.h>
#include <math.h>

/* Internal include files, from the ext/includes directory */
#include <symbols.h>
#include <symbols.c>

#include <defs.h>

/* safe storing of doubles */
#include <safe_double.h>
/* End of internal files */


#define is_a_dtable(d) ( TYPE(d) == T_DATA && RDATA(d)->dfree == (RUBY_DATA_FUNC)dtable_free )

#ifndef MAX
#define MAX(a,b)    (((a) > (b)) ? (a) : (b))
#endif
#ifndef MIN
#define MIN(a,b)    (((a) < (b)) ? (a) : (b))
#endif

typedef struct {
   long num_cols, num_rows; /* the dimensions */
   double **ptr; /* the data */
} Dtable;

/* prototypes */
static void dtable_free(Dtable *d);


PRIVATE bool Is_Dtable(VALUE obj) { return is_a_dtable(obj); }

// data stored in row order
// i.e., array arr of num_cols W and num_rows H is actually an array of H pointers
// each pointing to an array of W doubles.
// access row i and col j as arr[i][j] in c, as arr[i,j] in Ruby.

static void Alloc2dGrid(double ***f, int num_cols, int num_rows) {
   int i;
   if ((*f = (double **) calloc(num_rows, sizeof(double *)))==NULL)
      rb_raise(rb_eArgError, "Memory allocation error in \"Alloc2dGrid\"");
   for (i = 0; i < num_rows; i++) {
      if (((*f)[i] = (double *) calloc(num_cols ,sizeof(double)))==NULL)
         rb_raise(rb_eArgError, "Memory allocation error in \"Alloc2dGrid\"");
   }
}

static double Max2dGrid(double **f, int num_cols, int num_rows) {
   int i, j;
   double M;
   M = f[0][0];
   for (i = 0; i < num_rows; i++) {
      for (j = 0; j < num_cols; j++) {
         if (f[i][j] > M) M = f[i][j];
      }
   }
   return M;
}

static double Min2dGrid(double **f, int num_cols, int num_rows) {
   int i, j;
   double M;
   M = f[0][0];
   for (i = 0; i < num_rows; i++) {
      for (j = 0; j < num_cols; j++) {
         if (f[i][j] < M) M = f[i][j];
      }
   }
   return M;
}

static bool Max_Lt_2dGrid(double **f, double limit, int num_cols, int num_rows, double *fmax) {
   int i, j;
   double M=0, tmp;
   bool found = false;
   for (i = 0; i < num_rows; i++) {
      for (j = 0; j < num_cols; j++) {
         tmp = f[i][j];
         if (tmp >= limit) continue;
         if (!found) { found = true; M = tmp; }
         else if (tmp > M) M = tmp;
      }
   }
   *fmax = M;
   return found;
}

static bool Min_Gt_2dGrid(double **f, double limit, int num_cols, int num_rows, double *fmin) {
   int i, j;
   double M=0, tmp;
   bool found = false;
   for (i = 0; i < num_rows; i++) {
      for (j = 0; j < num_cols; j++) {
         tmp = f[i][j];
         if (tmp <= limit) continue;
         if (!found) { found = true; M = tmp; }
         else if (tmp < M) M = tmp;
      }
   }
   *fmin = M;
   return found;
}

static Dtable *Get_Dtable(VALUE obj) {
   Dtable *d;
   Data_Get_Struct(obj, Dtable, d);
   return d;
}

double **Dtable_Ptr(VALUE dtable, long *num_cols, long *num_rows) {
   Dtable *d;
   Data_Get_Struct(dtable, Dtable, d);
   if(num_cols)
     *num_cols = d->num_cols;
   if(num_rows)
     *num_rows = d->num_rows;
   return d->ptr;
   }

static VALUE cDtable; /* the Dtable class object */

static void dtable_free(Dtable *d) {
   double **array = d->ptr;
   int i;
   for (i = 0; i < d->num_rows; i++) free(array[i]);
   free(array);
   free(d);
}

static VALUE dtable_alloc(VALUE klass) {
   Dtable *d;
   VALUE ary = Data_Make_Struct(klass, Dtable, NULL, dtable_free, d);
   d->num_cols = d->num_rows = 0;
   d->ptr = NULL;
   return ary;
}

static VALUE dtable_init(VALUE ary, int num_cols, int num_rows) {
   Dtable *d = Get_Dtable(ary);
   if (num_cols <= 0 || num_rows <= 0)
      rb_raise(rb_eArgError, "bad args: Dtable.new(%d, %d)", num_cols, num_rows);
   Alloc2dGrid(&d->ptr, num_cols, num_rows);
   d->num_cols = num_cols;
   d->num_rows = num_rows;
   return ary;
}

PRIVATE
/*
 *  call-seq:
 *     dtable.dup  -> a_dtable
 *  
 *  Returns a copy of _dtable_.  For performance sensitive situations involving a series of operations,
 *  first make a copy using dup and then do "bang" operations to modify the result without further copying.
 */ 
VALUE dtable_dup(VALUE ary) {
   Dtable *d = Get_Dtable(ary);
   int i, j, num_cols = d->num_cols, num_rows = d->num_rows;
   VALUE new = dtable_init(dtable_alloc(cDtable), num_cols, num_rows);
   Dtable *d2 = Get_Dtable(new);
   double **src, **dest;
   src = d->ptr; dest = d2->ptr;
   for (i = 0; i < num_rows; i++) {
      for (j = 0; j < num_cols; j++) {
         dest[i][j] = src[i][j];
      }
   }
   return new;
}

PRIVATE
/*
 *  call-seq:
 *     dtable.reverse_rows  -> a_dtable
 *  
 *  Returns a copy of _dtable_ with the order of rows reversed.
 */ VALUE dtable_reverse_rows(VALUE ary)
{
   Dtable *d = Get_Dtable(ary);
   int i, j, num_cols = d->num_cols, num_rows = d->num_rows, last_row = num_rows - 1;
   VALUE new = dtable_init(dtable_alloc(cDtable), num_cols, num_rows);
   Dtable *d2 = Get_Dtable(new);
   double **src, **dest;
   src = d->ptr; dest = d2->ptr;
   for (i = 0; i < num_rows; i++) {
      for (j = 0; j < num_cols; j++) {
         dest[last_row-i][j] = src[i][j];
      }
   }
   return new;
}

PRIVATE
/*
 *  call-seq:
 *     dtable.reverse_cols  -> a_dtable
 *  
 *  Returns a copy of _dtable_ with the order of columns reversed.
 */ VALUE dtable_reverse_cols(VALUE ary)
{
   Dtable *d = Get_Dtable(ary);
   int i, j, num_cols = d->num_cols, num_rows = d->num_rows, last_col = num_cols - 1;
   VALUE new = dtable_init(dtable_alloc(cDtable), num_cols, num_rows);
   Dtable *d2 = Get_Dtable(new);
   double **src, **dest;
   src = d->ptr; dest = d2->ptr;
   for (i = 0; i < num_rows; i++) {
      for (j = 0; j < num_cols; j++) {
         dest[i][last_col-j] = src[i][j];
      }
   }
   return new;
}


PRIVATE
/*
 *  call-seq:
 *     dtable.rotate_cw90  -> a_dtable
 *  
 *  Returns a copy of _dtable_ rotated 90 degrees clockwise.
 */ VALUE dtable_rotate_cw90(VALUE ary)
{
   Dtable *d = Get_Dtable(ary);
   int i, j, num_cols = d->num_cols, num_rows = d->num_rows, last_row = num_rows - 1;
   VALUE new = dtable_init(dtable_alloc(cDtable), num_rows, num_cols);
   Dtable *d2 = Get_Dtable(new);
   double **src, **dest;
   src = d->ptr; dest = d2->ptr;
   for (i = 0; i < num_rows; i++) {
      for (j = 0; j < num_cols; j++) {
         dest[j][last_row-i] = src[i][j];
      }
   }
   return new;
}


PRIVATE
/*
 *  call-seq:
 *     dtable.rotate_ccw90  -> a_dtable
 *  
 *  Returns a copy of _dtable_ rotated 90 degrees counter-clockwise.
 */ VALUE dtable_rotate_ccw90(VALUE ary)
{
   Dtable *d = Get_Dtable(ary);
   int i, j, num_cols = d->num_cols, num_rows = d->num_rows, last_col = num_cols - 1;
   VALUE new = dtable_init(dtable_alloc(cDtable), num_rows, num_cols);
   Dtable *d2 = Get_Dtable(new);
   double **src, **dest;
   src = d->ptr; dest = d2->ptr;
   for (i = 0; i < num_rows; i++) {
      for (j = 0; j < num_cols; j++) {
         dest[last_col-j][i] = src[i][j];
      }
   }
   return new;
}

PRIVATE
/*
 *  call-seq:
 *     dtable.transpose  -> a_dtable
 *  
 *  Returns a transposed copy of _dtable_ (i.e., exchange rows and columns).
 */ VALUE dtable_transpose(VALUE ary) {
   Dtable *d = Get_Dtable(ary);
   int i, j, num_cols = d->num_cols, num_rows = d->num_rows;
   VALUE new = dtable_init(dtable_alloc(cDtable), num_rows, num_cols);
   Dtable *d2 = Get_Dtable(new);
   double **src, **dest;
   src = d->ptr; dest = d2->ptr;
   for (i = 0; i < num_rows; i++) {
      for (j = 0; j < num_cols; j++) {
         dest[j][i] = src[i][j];
      }
   }
   return new;
}

PRIVATE
/*
 *  call-seq:
 *     Dtable.new(num_cols, num_rows)      -> a_dtable
 *
 *  Returns a new Dtable with the requested dimensions.
 */ 
VALUE dtable_initialize(int argc, VALUE *argv, VALUE ary) {
   if (argc != 2) rb_raise(rb_eArgError, "need 2 args for Dtable.new(num_cols, num_rows)");
   int num_cols = NUM2INT(argv[0]), num_rows = NUM2INT(argv[1]);
   return dtable_init(ary, num_cols, num_rows);
}

PRIVATE
/*
 *  call-seq:
 *     dtable.num_cols  -> integer
 *  
 *  Returns the number of entries in the x dimension of _dtable_.
 */ 
VALUE dtable_num_cols(VALUE ary) {
   Dtable *d = Get_Dtable(ary);
   return LONG2NUM(d->num_cols);
}

PRIVATE
/*
 *  call-seq:
 *     dtable.num_rows  -> integer
 *  
 *  Returns the number of entries in the y dimension of _dtable_.
 */ VALUE dtable_num_rows(VALUE ary) {
   Dtable *d = Get_Dtable(ary);
   return LONG2NUM(d->num_rows);
}

PRIVATE
/*
 *  call-seq:
 *     dtable.min_gt(val)  -> float or nil
 *  
 *  Returns the minimum entry in _dtable_ which is greater than _val_, or <code>nil</code> if no such entry if found.
 */ VALUE dtable_min_gt(VALUE ary, VALUE val) {
   Dtable *d = Get_Dtable(ary);
   val = rb_Float(val);
   double zmin, z = NUM2DBL(val);
   if (Min_Gt_2dGrid(d->ptr, z, d->num_cols, d->num_rows, &zmin))
      return rb_float_new(zmin);
   return Qnil;
}

PRIVATE
/*
 *  call-seq:
 *     dtable.max_lt(val)  -> float or nil
 *  
 *  Returns the maximum entry in _dtable_ which is less than _val_, or <code>nil</code> if no such entry if found.
 */ VALUE dtable_max_lt(VALUE ary, VALUE val) {
   Dtable *d = Get_Dtable(ary);
   val = rb_Float(val);
   double zmax, z = NUM2DBL(val);
   if (Max_Lt_2dGrid(d->ptr, z, d->num_cols, d->num_rows, &zmax))
      return rb_float_new(zmax);
   return Qnil;
}

PRIVATE
/*
 *  call-seq:
 *     dtable.min  -> float
 *  
 *  Returns the minimum entry in _dtable_.
 */ VALUE dtable_min(VALUE ary) {
   Dtable *d = Get_Dtable(ary);
   double zmin = Min2dGrid(d->ptr, d->num_cols, d->num_rows);
   return rb_float_new(zmin);
}

PRIVATE
/*
 *  call-seq:
 *     dtable.max  -> float
 *  
 *  Returns the maximum entry in _dtable_.
 */ VALUE dtable_max(VALUE ary) {
   Dtable *d = Get_Dtable(ary);
   double zmax = Max2dGrid(d->ptr, d->num_cols, d->num_rows);
   return rb_float_new(zmax);
}

PRIVATE
/*
 *  call-seq:
 *     dtable.row(int)  -> a_dvec
 *  
 *  Creates a Dvector holding a copy of the contents of the requested row.
 */ VALUE dtable_row(VALUE ary, VALUE row_num) {
   Dtable *d = Get_Dtable(ary);
   row_num = rb_Integer(row_num);
   int row = NUM2INT(row_num);
   if (row < 0 || row >= d->num_rows)
      rb_raise(rb_eArgError, "Asking for row i = %i from array with only %li rows", row, d->num_rows);
   VALUE dvec = Dvector_Create();
   Dvector_Data_Replace(dvec, d->num_cols, d->ptr[row]);
   return dvec;
}

PRIVATE
/*
 *  call-seq:
 *     dtable.set_row(int,a_dvec) -> a_dvec
 *  
 *  Stores the contents of _a_dec_ in the specified row of the array.
 *  The length of the vector must equal the number of columns in the array.
 */ VALUE dtable_set_row(VALUE ary, VALUE row_num, VALUE dvec) {
   Dtable *d = Get_Dtable(ary);
   long len, j;
   double *data = Dvector_Data_for_Read(dvec, &len);
   row_num = rb_Integer(row_num);
   int row = NUM2INT(row_num);
   if (row < 0 || row >= d->num_rows)
      rb_raise(rb_eArgError, "Asking for row i = %i from array with only %li rows", row, d->num_rows);
   if (len != d->num_cols)
      rb_raise(rb_eArgError, "Length of vector (%li) does not match number of columns (%li)", len, d->num_cols);
   for (j=0; j < len; j++)
      d->ptr[row][j] = data[j];
   return dvec;
}

PRIVATE
/*
 *  call-seq:
 *    dtable.each_row{|row| }
 *
 *  Iterates over all rows and executes the given block
 */ VALUE dtable_each_row(VALUE ary){
   Dtable *d = Get_Dtable(ary);
   VALUE dvec = Dvector_Create();
   int i;
   for(i=0; i < d->num_rows; i++){
     Dvector_Data_Replace(dvec, d->num_cols, d->ptr[i]);
     rb_yield(dvec);
   }
   return ary;
}

PRIVATE
/*
 *  call-seq:
 *     dtable.set_column(int,a_dvec) -> a_dvec
 *  
 *  Stores the contents of _a_dec_ in the specified column of the array.
 *  The length of the vector must equal the number of rows in the array.
 */ VALUE dtable_set_column(VALUE ary, VALUE col_num, VALUE dvec) {
   Dtable *d = Get_Dtable(ary);
   long len, i;
   double *data = Dvector_Data_for_Read(dvec, &len);
   col_num = rb_Integer(col_num);
   int col = NUM2INT(col_num);
   if (col < 0 || col >= d->num_cols)
      rb_raise(rb_eArgError, "Asking for column i = %i from array with only %li columns", col, d->num_cols);
   if (len != d->num_rows)
      rb_raise(rb_eArgError, "Length of vector (%li) does not match number of rows (%li)", len, d->num_rows);
   for (i=0; i < len; i++)
      d->ptr[i][col] = data[i];
   return dvec;
}

PRIVATE
/*
 *  call-seq:
 *     dtable.column(int)  -> a_dvec
 *  
 *  Creates a Dvector holding a copy of the contents of the requested column.
 */ VALUE dtable_column(VALUE ary, VALUE column_num) {
   Dtable *d = Get_Dtable(ary);
   column_num = rb_Integer(column_num);
   int i, column = NUM2INT(column_num), len;
   if (column < 0 || column >= d->num_cols)
      rb_raise(rb_eArgError, "Asking for column i = %i from array with only %li columns", column, d->num_cols);
   VALUE dvec = Dvector_Create();
   len = d->num_rows;
   Dvector_Data_Resize(dvec, len);
   for (i=0; i < len; i++)
      Dvector_Store_Double(dvec, i, d->ptr[i][column]);
   return dvec;
}

PRIVATE
/*
 *  call-seq:
 *    dtable.each_column{|col| }
 *
 *  Iterates over all columns and executes the given block
 */ VALUE dtable_each_column(VALUE ary){
   Dtable *d = Get_Dtable(ary);
   VALUE dvec = Dvector_Create();
   int i,j;
   for(j=0; j < d->num_cols; j++){
     for(i=0; i < d->num_rows; i++){
       Dvector_Store_Double(dvec, i, d->ptr[i][j]);
     }
     rb_yield(dvec);
   }
   return ary;
}

static void set_dtable_vals(VALUE ary, double v) {
   Dtable *d = Get_Dtable(ary);
   int num_cols = d->num_cols, num_rows = d->num_rows, i, j;
   double **data = d->ptr;
   for (i = 0; i < num_rows; i++) {
      for (j = 0; j < num_cols; j++) {
         data[i][j] = v;
      }
   }
}

PRIVATE
/*
 *  call-seq:
 *     dtable.clear  -> dtable
 *  
 *  Sets the entries of _dtable_ array to zero.
 */ VALUE dtable_clear(VALUE ary, VALUE val) {
   set_dtable_vals(ary, 0.0);
   return ary;
}

PRIVATE
/*
 *  call-seq:
 *     dtable.set(float)  -> dtable
 *     dtable.set(a_dtable)  -> dtable
 *  
 *  Modifies the entries of _dtable_ array.  If the argument is a float, then all of the
 *  entries are set to that value.  If the argument is another Dtable array, then it must
 *  be the size as _dtable_, and its contents are copied to _dtable_.
 */ VALUE dtable_set(VALUE ary, VALUE val) {
   if (is_a_dtable(val)) {
      Dtable *d = Get_Dtable(ary);
      Dtable *d2 = Get_Dtable(val);
      int num_cols = d->num_cols, num_rows = d->num_rows, i, j;
      double **data = d->ptr;
      double **data2 = d2->ptr;
      if (d2->num_cols != num_cols || d2->num_rows != num_rows)
         rb_raise(rb_eArgError, "Arrays must be same size for Dtable set");
      for (i = 0; i < num_rows; i++) {
         for (j = 0; j < num_cols; j++) {
            data[i][j] = data2[i][j];
         }
      }
   } else {
      double v = NUM2DBL(val);
      set_dtable_vals(ary, v);
   }
   return ary;
}

PRIVATE
VALUE dtable_apply_math_op_bang(VALUE ary, double (*op)(double)) {
   Dtable *d = Get_Dtable(ary);
   double **p = d->ptr;
   int num_cols = d->num_cols, num_rows = d->num_rows, i, j;
      for (i = 0; i < num_rows; i++) {
         for (j = 0; j < num_cols; j++) {
         p[i][j] = (*op)(p[i][j]);
      }
   }
   return ary;
}

PRIVATE VALUE dtable_apply_math_op(VALUE source, double (*op)(double)) {
   return dtable_apply_math_op_bang(dtable_dup(source), op);
}

static double do_neg(double arg) { return -arg; }

PRIVATE
/*
 *  call-seq:
 *     dtable.neg   ->  a_dtable
 *  
 *  Returns of copy of _dtable_ with each entry x replaced by -x.
 */ VALUE dtable_neg(VALUE ary) {
   return dtable_apply_math_op(ary, do_neg);
}

PRIVATE
/*
 *  call-seq:
 *     dtable.abs   ->  a_dtable
 *  
 *  Returns of copy of _dtable_ with all entries replaced by their absolute values.
 */ VALUE dtable_abs(VALUE ary) {
   return dtable_apply_math_op(ary, fabs);
}

PRIVATE
/*
 *  call-seq:
 *     dtable.sin   ->  a_dtable
 *  
 *  Returns of copy of _dtable_ with entry x replaced by sin(x).
 */ VALUE dtable_sin(VALUE ary) {
   return dtable_apply_math_op(ary, sin);
}

PRIVATE
/*
 *  call-seq:
 *     dtable.cos   ->  a_dtable
 *  
 *  Returns of copy of _dtable_ with entry x replaced by cos(x).
 */ VALUE dtable_cos(VALUE ary) {
   return dtable_apply_math_op(ary, cos);
}

PRIVATE
/*
 *  call-seq:
 *     dtable.tan   ->  a_dtable
 *  
 *  Returns of copy of _dtable_ with entry x replaced by tan(x).
 */ VALUE dtable_tan(VALUE ary) {
   return dtable_apply_math_op(ary, tan);
}

PRIVATE
/*
 *  call-seq:
 *     dtable.asin   ->  a_dtable
 *  
 *  Returns of copy of _dtable_ with entry x replaced by asin(x).
 */ VALUE dtable_asin(VALUE ary) {
   return dtable_apply_math_op(ary, asin);
}

PRIVATE
/*
 *  call-seq:
 *     dtable.acos   ->  a_dtable
 *  
 *  Returns of copy of _dtable_ with entry x replaced by acos(x).
 */ VALUE dtable_acos(VALUE ary) {
   return dtable_apply_math_op(ary, acos);
}

PRIVATE
/*
 *  call-seq:
 *     dtable.atan   ->  a_dtable
 *  
 *  Returns of copy of _dtable_ with entry x replaced by atan(x).
 */ VALUE dtable_atan(VALUE ary) {
   return dtable_apply_math_op(ary, atan);
}

PRIVATE
/*
 *  call-seq:
 *     dtable.sinh   ->  a_dtable
 *  
 *  Returns of copy of _dtable_ with entry x replaced by sinh(x).
 */ VALUE dtable_sinh(VALUE ary) {
   return dtable_apply_math_op(ary, sinh);
}

PRIVATE
/*
 *  call-seq:
 *     dtable.cosh   ->  a_dtable
 *  
 *  Returns of copy of _dtable_ with entry x replaced by cosh(x).
 */ VALUE dtable_cosh(VALUE ary) {
   return dtable_apply_math_op(ary, cosh);
}

PRIVATE
/*
 *  call-seq:
 *     dtable.tanh   ->  a_dtable
 *  
 *  Returns of copy of _dtable_ with entry x replaced by tanh(x).
 */ VALUE dtable_tanh(VALUE ary) {
   return dtable_apply_math_op(ary, tanh);
}

static double do_asinh(double x) { return log(x + sqrt(x*x+1.0)); }

PRIVATE
/*
 *  call-seq:
 *     dtable.asinh   ->  a_dtable
 *  
 *  Returns of copy of _dtable_ with entry x replaced by asinh(x).
 */ VALUE dtable_asinh(VALUE ary) {
   return dtable_apply_math_op(ary, do_asinh);
}

static double do_acosh(double x) { return log(x + sqrt(x*x-1.0)); }

PRIVATE
/*
 *  call-seq:
 *     dtable.acosh   ->  a_dtable
 *  
 *  Returns of copy of _dtable_ with entry x replaced by acosh(x).
 */ VALUE dtable_acosh(VALUE ary) {
   return dtable_apply_math_op(ary, do_acosh);
}

static double do_atanh(double x) { return 0.5*log((1.0+x)/(1.0-x)); }

PRIVATE
/*
 *  call-seq:
 *     dtable.atanh   ->  a_dtable
 *  
 *  Returns of copy of _dtable_ with entry x replaced by atanh(x).
 */ VALUE dtable_atanh(VALUE ary) {
   return dtable_apply_math_op(ary, do_atanh);
}

PRIVATE
/*
 *  call-seq:
 *     dtable.ceil   ->  a_dtable
 *  
 *  Returns of copy of _dtable_ with entry x replaced by smallest integer not less than x.
 */ VALUE dtable_ceil(VALUE ary) {
   return dtable_apply_math_op(ary, ceil);
}

PRIVATE
/*
 *  call-seq:
 *     dtable.floor   ->  a_dtable
 *  
 *  Returns of copy of _dtable_ with each entry x replaced by largest integer not greater than x.
 */ VALUE dtable_floor(VALUE ary) {
   return dtable_apply_math_op(ary, floor);
}

static double do_round(double x) { return (x == 0.0)? 0.0 : (x > 0.0)? floor(x+0.5) : ceil(x-0.5); }

PRIVATE
/*
 *  call-seq:
 *     dtable.round   ->  a_dtable
 *  
 *  Returns of copy of _dtable_ with each entry x replaced by round(x).
 *  (Numbers midway between integers round away from zero.)
 */ VALUE dtable_round(VALUE ary) {
   return dtable_apply_math_op(ary, do_round);
}

PRIVATE
/*
 *  call-seq:
 *     dtable.exp   ->  a_dtable
 *  
 *  Returns of copy of _dtable_ with each entry x replaced by exp(x).
 */ VALUE dtable_exp(VALUE ary) {
   return dtable_apply_math_op(ary, exp);
}

static double do_exp10(double arg) { return pow(10.0, arg); }

PRIVATE
/*
 *  call-seq:
 *     dtable.exp10   ->  a_dtable
 *  
 *  Returns of copy of _dtable_ with each entry x replaced by 10**x.
 */ VALUE dtable_exp10(VALUE ary) {
   return dtable_apply_math_op(ary, do_exp10);
}

PRIVATE
/*
 *  call-seq:
 *     dtable.log   ->  a_dtable
 *  
 *  Returns of copy of _dtable_ with each entry x replaced by log(x).
 */ VALUE dtable_log(VALUE ary) {
   return dtable_apply_math_op(ary, log);
}

PRIVATE
/*
 *  call-seq:
 *     dtable.log10   ->  a_dtable
 *  
 *  Returns of copy of _dtable_ with each entry x replaced by log10(x).
 */ VALUE dtable_log10(VALUE ary) {
   return dtable_apply_math_op(ary, log10);
}

static double do_inv(double arg) { return 1.0/arg; }

PRIVATE
/*
 *  call-seq:
 *     dtable.inv   ->  a_dtable
 *  
 *  Returns of copy of _dtable_ with each entry x replaced by 1/x.
 */ VALUE dtable_inv(VALUE ary) {
   return dtable_apply_math_op(ary, do_inv);
}

PRIVATE
/*
 *  call-seq:
 *     dtable.sqrt   ->  a_dtable
 *  
 *  Returns of copy of _dtable_ with each entry x replaced by sqrt(x).
 */ VALUE dtable_sqrt(VALUE ary) {
   return dtable_apply_math_op(ary, sqrt);
}

PRIVATE
/*
 *  call-seq:
 *     dtable.neg!   ->  dtable
 *  
 *  Replace each entry x of _dtable_ with -x.
 */ VALUE dtable_neg_bang(VALUE ary) {
   return dtable_apply_math_op_bang(ary, do_neg);
}

PRIVATE
/*
 *  call-seq:
 *     dtable.abs!   ->  dtable
 *  
 *  Replace each entry x of _dtable_ with abs(x).
 */ VALUE dtable_abs_bang(VALUE ary) {
   return dtable_apply_math_op_bang(ary, fabs);
}

PRIVATE
/*
 *  call-seq:
 *     dtable.sin!   ->  dtable
 *  
 *  Replace each entry x of _dtable_ with sin(x).
 */ VALUE dtable_sin_bang(VALUE ary) {
   return dtable_apply_math_op_bang(ary, sin);
}

PRIVATE
/*
 *  call-seq:
 *     dtable.cos!   ->  dtable
 *  
 *  Replace each entry x of _dtable_ with cos(x).
 */ VALUE dtable_cos_bang(VALUE ary) {
   return dtable_apply_math_op_bang(ary, cos);
}

PRIVATE
/*
 *  call-seq:
 *     dtable.tan!   ->  dtable
 *  
 *  Replace each entry x of _dtable_ with tan(x).
 */ VALUE dtable_tan_bang(VALUE ary) {
   return dtable_apply_math_op_bang(ary, tan);
}

PRIVATE
/*
 *  call-seq:
 *     dtable.asin!   ->  dtable
 *  
 *  Replace each entry x of _dtable_ with asin(x).
 */ VALUE dtable_asin_bang(VALUE ary) {
   return dtable_apply_math_op_bang(ary, asin);
}

PRIVATE
/*
 *  call-seq:
 *     dtable.acos!   ->  dtable
 *  
 *  Replace each entry x of _dtable_ with acos(x).
 */ VALUE dtable_acos_bang(VALUE ary) {
   return dtable_apply_math_op_bang(ary, acos);
}

PRIVATE
/*
 *  call-seq:
 *     dtable.atan!   ->  dtable
 *  
 *  Replace each entry x of _dtable_ with atan(x).
 */ VALUE dtable_atan_bang(VALUE ary) {
   return dtable_apply_math_op_bang(ary, atan);
}

PRIVATE
/*
 *  call-seq:
 *     dtable.sinh!   ->  dtable
 *  
 *  Replace each entry x of _dtable_ with sinh(x).
 */ VALUE dtable_sinh_bang(VALUE ary) {
   return dtable_apply_math_op_bang(ary, sinh);
}

PRIVATE
/*
 *  call-seq:
 *     dtable.cosh!   ->  dtable
 *  
 *  Replace each entry x of _dtable_ with cosh(x).
 */ VALUE dtable_cosh_bang(VALUE ary) {
   return dtable_apply_math_op_bang(ary, cosh);
}

PRIVATE
/*
 *  call-seq:
 *     dtable.tanh!   ->  dtable
 *  
 *  Replace each entry x of _dtable_ with tanh(x).
 */ VALUE dtable_tanh_bang(VALUE ary) {
   return dtable_apply_math_op_bang(ary, tanh);
}

PRIVATE
/*
 *  call-seq:
 *     dtable.asinh!   ->  dtable
 *  
 *  Replace each entry x of _dtable_ with asinh(x).
 */ VALUE dtable_asinh_bang(VALUE ary) {
   return dtable_apply_math_op_bang(ary, do_asinh);
}

PRIVATE
/*
 *  call-seq:
 *     dtable.acosh!   ->  dtable
 *  
 *  Replace each entry x of _dtable_ with acosh(x).
 */ VALUE dtable_acosh_bang(VALUE ary) {
   return dtable_apply_math_op_bang(ary, do_acosh);
}

PRIVATE
/*
 *  call-seq:
 *     dtable.atanh!   ->  dtable
 *  
 *  Replace each entry x of _dtable_ with atanh(x).
 */ VALUE dtable_atanh_bang(VALUE ary) {
   return dtable_apply_math_op_bang(ary, do_atanh);
}

PRIVATE
/*
 *  call-seq:
 *     dtable.ceil!   ->  dtable
 *  
 *  Replace each entry x of _dtable_ with the smallest integer not less than x.
 */ VALUE dtable_ceil_bang(VALUE ary) {
   return dtable_apply_math_op_bang(ary, ceil);
}

PRIVATE
/*
 *  call-seq:
 *     dtable.floor!   ->  dtable
 *  
 *  Replace each entry x of _dtable_ with the largest integer not greater than x.
 */ VALUE dtable_floor_bang(VALUE ary) {
   return dtable_apply_math_op_bang(ary, floor);
}

PRIVATE
/*
 *  call-seq:
 *     dtable.round!   ->  dtable
 *  
 *  Replace each entry x of _dtable_ with the integer closest to x.
 *  (Numbers midway between integers round away from zero.)
 */ VALUE dtable_round_bang(VALUE ary) {
   return dtable_apply_math_op_bang(ary, do_round);
}

PRIVATE
/*
 *  call-seq:
 *     dtable.exp!   ->  dtable
 *  
 *  Replace each entry x of _dtable_ with exp(x).
 */ VALUE dtable_exp_bang(VALUE ary) {
   return dtable_apply_math_op_bang(ary, exp);
}

PRIVATE
/*
 *  call-seq:
 *     dtable.exp10!   ->  dtable
 *  
 *  Replace each entry x of _dtable_ with 10**x.
 */ VALUE dtable_exp10_bang(VALUE ary) {
   return dtable_apply_math_op_bang(ary, do_exp10);
}

PRIVATE
/*
 *  call-seq:
 *     dtable.log!   ->  dtable
 *  
 *  Replace each entry x of _dtable_ with log(x).
 */ VALUE dtable_log_bang(VALUE ary) {
   return dtable_apply_math_op_bang(ary, log);
}

PRIVATE
/*
 *  call-seq:
 *     dtable.log10!   ->  dtable
 *  
 *  Replace each entry x of _dtable_ with log10(x).
 */ VALUE dtable_log10_bang(VALUE ary) {
   return dtable_apply_math_op_bang(ary, log10);
}

PRIVATE
/*
 *  call-seq:
 *     dtable.inv!   ->  dtable
 *  
 *  Replace each entry x of _dtable_ with 1/x.
 */ VALUE dtable_inv_bang(VALUE ary) {
   return dtable_apply_math_op_bang(ary, do_inv);
}

PRIVATE
/*
 *  call-seq:
 *     dtable.sqrt!   ->  dtable
 *  
 *  Replace each entry x of _dtable_ with sqrt(x).
 */ VALUE dtable_sqrt_bang(VALUE ary) {
   return dtable_apply_math_op_bang(ary, sqrt);
}

PRIVATE VALUE dtable_apply_math_op1_bang(VALUE ary, VALUE arg, double (*op)(double, double)) {
   Dtable *d = Get_Dtable(ary);
   arg = rb_Float(arg);
   double y = NUM2DBL(arg), **p = d->ptr;
   int num_cols = d->num_cols, num_rows = d->num_rows, i, j;
      for (i = 0; i < num_rows; i++) {
         for (j = 0; j < num_cols; j++) {
         p[i][j] = (*op)(p[i][j], y);
      }
   }
   return ary;
}

PRIVATE VALUE dtable_apply_math_op1(VALUE source, VALUE arg, double (*op)(double, double)) {
   return dtable_apply_math_op1_bang(dtable_dup(source), arg, op);
}

static double do_trim(double x, double cutoff) { return (fabs(x) < cutoff)? 0.0 : x; }

PRIVATE
/*
 *  call-seq:
 *     dtable.trim(cutoff=1e-6)   ->  a_dtable
 *  
 *  Returns a copy of _dtable_ with any entry with absolute value less than _cutoff_ replaced by 0.
 */ VALUE dtable_trim(int argc, VALUE *argv, VALUE self) {
   VALUE arg1;
   if ((argc < 0) || (argc > 1))
      rb_raise(rb_eArgError, "wrong # of arguments(%d for 0 or 1)",argc);
   arg1 = (argc > 0)? argv[0] : rb_float_new(1e-6);
   return dtable_apply_math_op1(self, arg1, do_trim);
}

static double do_safe_log(double x, double y) { return log(MAX(x,y)); }

PRIVATE
/*
 *  call-seq:
 *     dtable.safe_log(cutoff=1e-99)   ->  a_dtable
 *  
 *  Returns a copy of _dtable_ with each entry x replaced by log(max(x,_cutoff_)).
 */ VALUE dtable_safe_log(int argc, VALUE *argv, VALUE self) {
   VALUE arg1;
   if ((argc < 0) || (argc > 1))
      rb_raise(rb_eArgError, "wrong # of arguments(%d for 0 or 1)",argc);
   arg1 = (argc > 0)? argv[0] : rb_float_new(1e-99);
   return dtable_apply_math_op1(self, arg1, do_safe_log);
}

static double do_safe_log10(double x, double y) { return log10(MAX(x,y)); }

PRIVATE
/*
 *  call-seq:
 *     dtable.safe_log10(cutoff=1e-99)   ->  a_dtable
 *  
 *  Returns a copy of _dtable_ with each entry x replaced by log10(max(x,_cutoff_)).
 *     
 */ VALUE dtable_safe_log10(int argc, VALUE *argv, VALUE self) {
   VALUE arg1;
   if ((argc < 0) || (argc > 1))
      rb_raise(rb_eArgError, "wrong # of arguments(%d for 0 or 1)",argc);
   arg1 = (argc > 0)? argv[0] : rb_float_new(1e-99);
   return dtable_apply_math_op1(self, arg1, do_safe_log10);
}

static double do_safe_inv(double x, double y) {
   return (fabs(x) >= y)? 1.0/x : (x > 0.0)? 1.0/y : -1.0/y; }

PRIVATE
/*
 *  call-seq:
 *     dtable.safe_inv(cutoff=1e-99)   ->  a_dtable
 *  
 *  Returns a copy of _dtable_ with each entry x replaced by sign(x)/_cutoff_ if abs(x) < _cutoff_, 1/x otherwise.
 *     
 */ VALUE dtable_safe_inv(int argc, VALUE *argv, VALUE self) {
   VALUE arg1;
   if ((argc < 0) || (argc > 1))
      rb_raise(rb_eArgError, "wrong # of arguments(%d for 0 or 1)",argc);
   arg1 = (argc > 0)? argv[0] : rb_float_new(1e-99);
   return dtable_apply_math_op1(self, arg1, do_safe_inv);
}

static double do_safe_asin(double x) { return asin(MAX(-1.0,MIN(1.0,x))); }

PRIVATE
/*
 *  call-seq:
 *     dtable.safe_asin   ->  a_dtable
 *  
 *  Returns a copy of _dtable_ with each entry x replaced by asin(max(-1,min(1,x))).
 *     
 */ VALUE dtable_safe_asin(VALUE ary) {
   return dtable_apply_math_op(ary, do_safe_asin);
}
   
static double do_safe_acos(double x) { return acos(MAX(-1.0,MIN(1.0,x))); }

PRIVATE
/*
 *  call-seq:
 *     dtable.safe_acos   ->  a_dtable
 *  
 *  Returns a copy of _dtable_ with each entry x replaced by acos(max(-1,min(1,x))).
 *     
 */ VALUE dtable_safe_acos(VALUE ary) {
   return dtable_apply_math_op(ary, do_safe_acos);
}
   
static double do_safe_sqrt(double x) { return sqrt(MAX(x,0.0)); }

PRIVATE
/*
 *  call-seq:
 *     dtable.safe_sqrt   ->  a_dtable
 *  
 *  Returns a copy of _dtable_ with each entry x replaced by sqrt(max(x,0)).
 *     
 */ VALUE dtable_safe_sqrt(VALUE ary) {
   return dtable_apply_math_op(ary, do_safe_sqrt);
}

PRIVATE
/*
 *  call-seq:
 *     dtable.atan2!(number)       ->  dtable
 *     dtable.atan2!(other)        ->  dtable
 *  
 *  When argument is a number, this operation replaces each entry x of _dtable_ by the angle whose tangent is x/_number_.
 *  When argument is a data array, this operation replaces each entry x of _dtable_ by the angle whose tangent is x divided
 *  by the corresponding entry in the _other_ data array.
 */ VALUE dtable_atan2_bang(VALUE ary, VALUE arg) {
   return dtable_apply_math_op2_bang(ary, arg, atan2);
}

static double do_mod(double x, double y) { return x - y * floor(x/y); }
PRIVATE
/*
 *  call-seq:
 *     dtable.modulo!(number)       ->  dtable
 *     dtable.mod!(number)          ->  dtable
 *     dtable.modulo!(other)        ->  dtable
 *     dtable.mod!(other)           ->  dtable
 *  
 *  When argument is a number, this operation returns a copy of _dtable_ with each entry x replaced by x % _number_.
 *  When argument is a data array, this operation returns a copy of _dtable_ with each entry x replaced
 *  by x % the corresponding entry in the _other_ data array.
 */ VALUE dtable_modulo_bang(VALUE ary, VALUE arg) {
   return dtable_apply_math_op2_bang(ary, arg, do_mod);
}

static double do_remainder(double x, double y) { return (x*y > 0.0)? do_mod(x,y) : do_mod(x,y)-y; }

PRIVATE
/*
 *  call-seq:
 *     dtable.remainder!(number)          ->  dtable
 *     dtable.remainder!(other)           ->  dtable
 *  
 *  When the argument is a number, this operation replaces with each entry x of _dtable_ by the remainder of x divided by _number_.
 *  When the argument is a data array, this operation replaces with each entry x of _dtable_
 *  by remainder of x divided by the corresponding entry in the _other_ data array.
 */ VALUE dtable_remainder_bang(VALUE ary, VALUE arg) {
   return dtable_apply_math_op2_bang(ary, arg, do_remainder);
}

PRIVATE
/*
 *  call-seq:
 *     dtable.trim!(cutoff=1e-6)   ->  dtable
 *  
 *  Each entry x in _dtable_ having absolute value less than _cutoff_ is replaced by 0.
 */ VALUE dtable_trim_bang(int argc, VALUE *argv, VALUE self) {
   VALUE arg1;
   if ((argc < 0) || (argc > 1))
      rb_raise(rb_eArgError, "wrong # of arguments(%d for 0 or 1)",argc);
   arg1 = (argc > 0)? argv[0] : rb_float_new(1e-6);
   return dtable_apply_math_op1_bang(self, arg1, do_trim);
}

PRIVATE
/*
 *  call-seq:
 *     dtable.pow!(number)                ->  dtable
 *     dtable.pow!(other)                 ->  dtable
 *     dtable.raised_to!(number)          ->  dtable
 *     dtable.raised_to!(other)           ->  dtable
 *  
 *  When argument is a number, this operation returns a copy of _dtable_ with each entry x replaced by x ** _number_.
 *  When argument is a data array, this operation returns a copy of _dtable_ with each entry x replaced
 *  by x ** the corresponding entry in the _other_ data array.
 */ VALUE dtable_pow_bang(VALUE ary, VALUE arg) {
   return dtable_apply_math_op1_bang(ary, arg, pow);
}

static double do_as_exponent_of(double x, double y) { return pow(y,x); }

PRIVATE
/*
 *  call-seq:
 *     dtable.as_exponent_of!(number)                ->  dtable
 *     dtable.as_exponent_of!(other)                 ->  dtable
 *  
 *  When argument is a number, this operation replaces each entry x of _dtable_ by _number_ ** x.
 *  When argument is a data array, this operation replaces each entry x of _dtable_
 *  by the corresponding entry in the _other_ data array raised to the power x.
 */ VALUE dtable_as_exponent_of_bang(VALUE ary, VALUE arg) {
   return dtable_apply_math_op2_bang(ary, arg, do_as_exponent_of);
}

PRIVATE
/*
 *  call-seq:
 *     dtable.safe_log!(cutoff=1e-99)   ->  dtable
 *  
 *  Replaces each entry x in _dtable_ by log(max(x,_cutoff_)).
 *     
 */ VALUE dtable_safe_log_bang(int argc, VALUE *argv, VALUE self) {
   VALUE arg1;
   if ((argc < 0) || (argc > 1))
      rb_raise(rb_eArgError, "wrong # of arguments(%d for 0 or 1)",argc);
   arg1 = (argc > 0)? argv[0] : rb_float_new(1e-99);
   return dtable_apply_math_op1_bang(self, arg1, do_safe_log);
}

PRIVATE
/*
 *  call-seq:
 *     dtable.safe_log10!(cutoff)   ->  dtable
 *  
 *  Replaces each entry x in _dtable_ by log10(max(x,_cutoff_)).
 *     
 */ VALUE dtable_safe_log10_bang(int argc, VALUE *argv, VALUE self) {
   VALUE arg1;
   if ((argc < 0) || (argc > 1))
      rb_raise(rb_eArgError, "wrong # of arguments(%d for 0 or 1)",argc);
   arg1 = (argc > 0)? argv[0] : rb_float_new(1e-99);
   return dtable_apply_math_op1_bang(self, arg1, do_safe_log10);
}

PRIVATE
/*
 *  call-seq:
 *     dtable.safe_inv!(cutoff)   ->  dtable
 *  
 *  Replaces each entry x in _dtable_ by sign(x)/_cutoff_ if abs(x) < _cutoff_, 1/x otherwise.
 *     
 */ VALUE dtable_safe_inv_bang(int argc, VALUE *argv, VALUE self) {
   VALUE arg1;
   if ((argc < 0) || (argc > 1))
      rb_raise(rb_eArgError, "wrong # of arguments(%d for 0 or 1)",argc);
   arg1 = (argc > 0)? argv[0] : rb_float_new(1e-99);
   return dtable_apply_math_op1_bang(self, arg1, do_safe_inv);
}

PRIVATE
/*
 *  call-seq:
 *     dtable.safe_sqrt!   ->  dtable
 *  
 *  Replaces each entry x in _dtable_ by sqrt(max(x,0)).
 *     
 */ VALUE dtable_safe_sqrt_bang(VALUE ary) {
   return dtable_apply_math_op_bang(ary, do_safe_sqrt);
}
   
PRIVATE
/*
 *  call-seq:
 *     dtable.safe_asin!   ->  dtable
 *  
 *  Replaces each entry x in _dtable_ by asin(max(-1,min(1,x)))..
 *     
 */ VALUE dtable_safe_asin_bang(VALUE ary) {
   return dtable_apply_math_op_bang(ary, do_safe_asin);
}
   
PRIVATE
/*
 *  call-seq:
 *     dtable.safe_acos!   ->  dtable
 *  
 *  Replaces each entry x in _dtable_ by acos(max(-1,min(1,x)))..
 *     
 */ VALUE dtable_safe_acos_bang(VALUE ary) {
   return dtable_apply_math_op_bang(ary, do_safe_acos);
}
   
PRIVATE VALUE dtable_apply_math_op2_bang(VALUE ary1, VALUE ary2, double (*op)(double, double)) {
   VALUE check = rb_obj_is_kind_of(ary2, rb_cNumeric);
   if (check != Qfalse) { return dtable_apply_math_op1_bang(ary1, ary2, op); }
   Dtable *d1 = Get_Dtable(ary1);
   Dtable *d2 = Get_Dtable(ary2);
   int num_cols = d1->num_cols, num_rows = d1->num_rows, i, j;
   if (num_cols != d2->num_cols || num_rows != d2->num_rows) 
      rb_raise(rb_eArgError, "Dtable arrays must be same dimension for math operation");
   double **p1, **p2;
   p1 = d1->ptr; p2 = d2->ptr;
   for (i = 0; i < num_rows; i++) {
      for (j = 0; j < num_cols; j++) {
         p1[i][j] = (*op)(p1[i][j], p2[i][j]);
      }
   }
   return ary1;
}

PRIVATE VALUE dtable_apply_math_op2(VALUE ary1, VALUE ary2, double (*op)(double, double)) {
   return dtable_apply_math_op2_bang(dtable_dup(ary1), ary2, op);
}

static double do_add(double x, double y) { return x + y; }
PRIVATE
/*
 *  call-seq:
 *     dtable.add(number)       ->  a_dtable
 *     dtable.add(other)        ->  a_dtable
 *     dtable + number          ->  a_dtable
 *     number + dtable          ->  a_dtable
 *     dtable + other           ->  a_dtable
 *  
 *  When argument is a number, this operation returns a copy of _dtable_ with each entry x replaced by x + _number_.
 *  When argument is a data array, this operation returns a copy of _dtable_ with each entry x replaced
 *  by x + the corresponding entry in the _other_ data array.
 */ VALUE dtable_add(VALUE ary, VALUE arg) {
   return dtable_apply_math_op2(ary, arg, do_add);
}

static double do_sub(double x, double y) { return x - y; }
PRIVATE
/*
 *  call-seq:
 *     dtable.sub(number)       ->  a_dtable
 *     dtable.sub(other)        ->  a_dtable
 *     dtable - number          ->  a_dtable
 *     number - dtable          ->  a_dtable
 *     dtable - other           ->  a_dtable
 *  
 *  When argument is a number, this operation returns a copy of _dtable_ with each entry x replaced by x - _number_.
 *  When argument is a data array, this operation returns a copy of _dtable_ with each entry x replaced
 *  by x - the corresponding entry in the _other_ data array.
 */ VALUE dtable_sub(VALUE ary, VALUE arg) {
   return dtable_apply_math_op2(ary, arg, do_sub);
}

static double do_mul(double x, double y) { return x * y; }
PRIVATE
/*
 *  call-seq:
 *     dtable.mul(number)       ->  a_dtable
 *     dtable.mul(other)        ->  a_dtable
 *     dtable - number          ->  a_dtable
 *     number - dtable          ->  a_dtable
 *     dtable - other           ->  a_dtable
 *  
 *  When argument is a number, this operation returns a copy of _dtable_ with each entry x replaced by x * _number_.
 *  When argument is a data array, this operation returns a copy of _dtable_ with each entry x replaced
 *  by x * the corresponding entry in the _other_ data array.
 */ VALUE dtable_mul(VALUE ary, VALUE arg) {
   return dtable_apply_math_op2(ary, arg, do_mul);
}

static double do_div(double x, double y) { return x / y; }
PRIVATE
/*
 *  call-seq:
 *     dtable.div(number)       ->  a_dtable
 *     dtable.div(other)        ->  a_dtable
 *     dtable - number          ->  a_dtable
 *     number - dtable          ->  a_dtable
 *     dtable - other           ->  a_dtable
 *  
 *  When argument is a number, this operation returns a copy of _dtable_ with each entry x replaced by x / _number_.
 *  When argument is a data array, this operation returns a copy of _dtable_ with each entry x replaced
 *  by x / the corresponding entry in the _other_ data array.
 */ VALUE dtable_div(VALUE ary, VALUE arg) {
   return dtable_apply_math_op2(ary, arg, do_div);
}

PRIVATE
/*
 *  call-seq:
 *     dtable.modulo(number)       ->  a_dtable
 *     dtable.mod(number)          ->  a_dtable
 *     dtable.modulo(other)        ->  a_dtable
 *     dtable.mod(other)           ->  a_dtable
 *     dtable % number             ->  a_dtable
 *     dtable % other              ->  a_dtable
 *  
 *  When argument is a number, this operation returns a copy of _dtable_ with each entry x replaced by x % _number_.
 *  When argument is a data array, this operation returns a copy of _dtable_ with each entry x replaced
 *  by x % the corresponding entry in the _other_ data array.
 *     
 */ VALUE dtable_mod(VALUE ary, VALUE arg) {
   return dtable_apply_math_op2(ary, arg, do_mod);
}

PRIVATE
/*
 *  call-seq:
 *     dtable.remainder(number)          ->  a_dtable
 *     dtable.remainder(other)           ->  a_dtable
 *  
 *  When the argument is a number, this operation returns a copy of _dtable_ with each entry x replaced by the remainder of x divided by _number_.
 *  When the argument is a data array, this operation returns a copy of _dtable_ with each entry x replaced
 *  by the remainder of x divided by the corresponding entry in the _other_ data array.
 */ VALUE dtable_remainder(VALUE ary, VALUE arg) {
   return dtable_apply_math_op2(ary, arg, do_remainder);
}

PRIVATE
/*
 *  call-seq:
 *     dtable.pow(number)                ->  a_dtable
 *     dtable.pow(other)                 ->  a_dtable
 *     dtable.raised_to(number)          ->  a_dtable
 *     dtable.raised_to(other)           ->  a_dtable
 *     dtable ** number                  ->  a_dtable
 *     dtable ** other                   ->  a_dtable
 *  
 *  When argument is a number, this operation returns a copy of _dtable_ with each entry x replaced by x ** _number_.
 *  When argument is a data array, this operation returns a copy of _dtable_ with each entry x replaced
 *  by x ** the corresponding entry in the _other_ data array.
 *     
 */ VALUE dtable_pow(VALUE ary, VALUE arg) {
   return dtable_apply_math_op2(ary, arg, pow);
}

PRIVATE
/*
 *  call-seq:
 *     dtable.as_exponent_of(number)                ->  a_dtable
 *     dtable.as_exponent_of(other)                 ->  a_dtable
 *  
 *  When argument is a number, this operation returns a copy of _dtable_ with each entry x replaced by _number_ ** x.
 *  When argument is a data array, this operation returns a copy of _dtable_ with each entry x replaced
 *  by the corresponding entry in the _other_ data array raised to the power x.
 */ VALUE dtable_as_exponent_of(VALUE ary, VALUE arg) {
   return dtable_apply_math_op2(ary, arg, do_as_exponent_of);
}

PRIVATE
/*
 *  call-seq:
 *     dtable.atan2(number)       ->  a_dtable
 *     dtable.atan2(other)        ->  a_dtable
 *  
 *  When argument is a number, this operation returns a copy of _dtable_ with each entry x replaced by the angle whose tangent is x/_number_.
 *  When argument is a data array, this operation returns a copy of _dtable_ with each entry x replaced
 *  by the angle whose tangent is x divided by the corresponding entry in the _other_ data array.
 */ VALUE dtable_atan2(VALUE ary, VALUE arg) {
   return dtable_apply_math_op2(ary, arg, atan2);
}

PRIVATE
/*
 *  call-seq:
 *     dtable.add!(number)       ->  dtable
 *     dtable.add!(other)        ->  dtable
 *  
 *  When argument is a number, each entry x in _dtable_ is replaced by x + _number_.
 *  When argument is a data array, each entry x in _dtable_ is replaced by x + 
 *  the corresponding entry in the _other_ data array.
 */ VALUE dtable_add_bang(VALUE ary, VALUE arg) {
   return dtable_apply_math_op2_bang(ary, arg, do_add);
}

PRIVATE
/*
 *  call-seq:
 *     dtable.sub!(number)       ->  dtable
 *     dtable.sub!(other)        ->  dtable
 *  
 *  When argument is a number, each entry x in _dtable_ is replaced by x - _number_.
 *  When argument is a data array, each entry x in _dtable_ is replaced by x - 
 *  the corresponding entry in the _other_ data array.
 */ VALUE dtable_sub_bang(VALUE ary, VALUE arg) {
   return dtable_apply_math_op2_bang(ary, arg, do_sub);
}

PRIVATE
/*
 *  call-seq:
 *     dtable.mul!(number)       ->  dtable
 *     dtable.mul!(other)        ->  dtable
 *  
 *  When argument is a number, each entry x in _dtable_ is replaced by x * _number_.
 *  When argument is a data array, each entry x in _dtable_ is replaced by x * 
 *  the corresponding entry in the _other_ data array.
 */ VALUE dtable_mul_bang(VALUE ary, VALUE arg) {
   return dtable_apply_math_op2_bang(ary, arg, do_mul);
}

PRIVATE
/*
 *  call-seq:
 *     dtable.div!(number)       ->  dtable
 *     dtable.div!(other)        ->  dtable
 *  
 *  When argument is a number, each entry x in _dtable_ is replaced by x / _number_.
 *  When argument is a data array, each entry x in _dtable_ is replaced by x / 
 *  the corresponding entry in the _other_ data array.
 */ VALUE dtable_div_bang(VALUE ary, VALUE arg) {
   return dtable_apply_math_op2_bang(ary, arg, do_div);
}

PRIVATE
/*======================================================================*/ VALUE Read_Dtable(VALUE dest, char *filename, int skip_lines) {
   FILE *file = NULL;
   long num_cols, num_rows;
   int i, j, k, len;
   const int buff_len = 10000;
   const int err_len = 100;
   char c, buff[buff_len], *p, *pend, err_str[err_len];
   double *data, **ptr = Dtable_Ptr(dest, &num_cols, &num_rows);
   if ((file=fopen(filename,"r")) == NULL)
      rb_raise(rb_eArgError, "failed to open %s", filename);
   for (i = 0; i < skip_lines; i++) { /* skip over initial lines */
      if (fgets(buff, buff_len, file)==NULL) {
         fclose(file);
         rb_raise(rb_eArgError, "ERROR: read reached end of file before reaching line %i in %s",
            skip_lines, filename);
      }
   }
   
   // rewrite to use strtod instead of fscanf to deal with numbers from fortran like 0.501-129 for 0.501E-129
   
   for (i = 0; i < num_rows; i++) {
      data = ptr[i];
      for (j = 0; j < num_cols; j++) {
         // skip over blanks (includes end-of-line and tab)
         p = buff; k = 0;
         while ((c=getc(file)) != EOF) {
            if (!isspace(c)) break;
         }
         // save the non-blanks
         *p++ = c;
         while ((c=getc(file)) != EOF) {
            if (isspace(c) || k > 1000) break;
            *p++ = c;
         }
         *p = ' ';
         data[j] = strtod(buff,&pend);
         if (pend != p) { // need to check to see if have a number like 0.501-129
            if (pend[0] == '+' || pend[0] == '-') { // insert 'E' and try again
                pend[5] = ' '; pend[4] = pend[3]; pend[3] = pend[2]; pend[2] = pend[1]; pend[1] = pend[0]; pend[0] = 'E';
                data[j] = strtod(buff,&pend);
            }
         }
         if (!is_okay_number(data[j])) {
            fclose(file);
            len = (pend-buff < err_len-1)? pend-buff : err_len-1;
            printf("len %i\n", len);
            strncpy(err_str,buff,len);
            rb_raise(rb_eArgError,
               "failed to read requested amount of data in %s (asked for %li xs and %li ys; found only %i and %i). last attempt to read got %g from string starting with: %s",
               filename, num_cols, num_rows, i+1, j, data[j], err_str);
         }
      }
   }
   fclose(file);
   return dest;
}


PRIVATE
/*
 *  call-seq:
 *     dtable.read(filename, skip_lines=0) ->  dtable
 *  
 *  The contents of _dtable_ are replaced by the contents of the file, starting after skipping
 *  the specified number of lines.  The values in the file are listed with row number 0 first.
 *  
 */ VALUE dtable_read(int argc, VALUE *argv, VALUE self) {
   if ((argc < 1) || (argc > 2))
      rb_raise(rb_eArgError, "wrong # of arguments(%d for 1 or 2)",argc);
   VALUE filename = argv[0];
   int skip_lines = (argc==2)? NUM2INT(rb_Integer(argv[1])) : 0;
   return Read_Dtable(self, StringValuePtr(filename), skip_lines);
}

PRIVATE VALUE dtable_entry(VALUE ary, long i, long j) {
   Dtable *d = Get_Dtable(ary);
   if (d->num_cols <= 0 || d->num_rows <= 0) return Qnil;
   if (i < 0) i += d->num_rows;
   if (j < 0) j += d->num_cols;
   if (i < 0 || d->num_rows <= i || j < 0 || d->num_cols <= j) return Qnil;
   return rb_float_new(d->ptr[i][j]);
}

PRIVATE
/* 
 *  call-seq:
 *     dtable[row,col]  ->  number or nil
 *     dtable.at(row,col)  ->  number or nil
 *
 *  Returns the element at location _row_, _col_.  Returns +nil+
 *  if the location is out of range.
 */ VALUE dtable_at(VALUE ary, VALUE xloc, VALUE yloc) {
   return dtable_entry(ary, NUM2LONG(xloc), NUM2LONG(yloc));
}

void dtable_store(VALUE ary, long i, long j, double v) {
   double **ptr;
   long num_cols, num_rows;
   ptr = Dtable_Ptr(ary, &num_cols, &num_rows);
   if (num_cols <= 0 || num_rows <= 0) {
      rb_raise(rb_eArgError, "bad args for setting entry in data array");
      }
   if (i < 0) i += num_rows;
   if (j < 0) j += num_cols;
   if (i < 0 || num_rows <= i || j < 0 || num_cols <= j) {
      rb_raise(rb_eArgError, "bad args for setting entry in data array");
      }
   ptr[i][j] = v;
}

PRIVATE
/* 
 *  call-seq:
 *     dtable[row,col] = number  ->  number
 *
 *  Replaces the element at location _row_, _col_ by the given _number_.
 */ 
VALUE dtable_aset(VALUE ary, VALUE xloc, VALUE yloc, VALUE val) {
   dtable_store(ary, NUM2LONG(xloc), NUM2LONG(yloc), NUM2DBL(val));
   return val;
}

#define DTABLE_DUMP_VERSION 1

PRIVATE
/*
  Called by the marshalling mechanism to store a permanent copy of a 
  Dtable. _limit_ is simply ignored.
 */
VALUE dtable_dump(VALUE ary, VALUE limit)
{
  int i; /* for STORE_UNSIGNED */
  long rows, cols;
  long x, y;
  double ** data = Dtable_Ptr(ary, &cols, &rows);
  double * col;
  long target_len = 1 /* first signature byte */
    + 8 /* 2 * length */
    + cols * rows * 8 ;
  unsigned u_len;
  VALUE str = rb_str_new2("");
  rb_str_resize(str,target_len); /* This seems to do the trick */
  /* \begin{playing with ruby's internals} */
  unsigned char * ptr = (unsigned char *) RSTRING_PTR(str);
  /* signature byte */
  (*ptr++) = DTABLE_DUMP_VERSION;
  u_len = (unsigned) rows; /* limits to 4 billions rows */
  STORE_UNSIGNED(u_len, ptr); /* destroys u_len */
  u_len = (unsigned) cols; /* limits to 4 billions columns */
  STORE_UNSIGNED(u_len, ptr); /* destroys u_len */
  for(x = 0; x < rows; x++)
    {
      col = data[x];
      for(y = 0; y < cols; y++)
	{
	  store_double(*(col++), ptr);
	  ptr += 8;
	}
    }
  /*  RSTRING_LEN(str) = target_len;*/
  return str;
  /* \end{playing with ruby's internals} */
}

PRIVATE
/*
  Called by the marshalling mechanism to retrieve a permanent copy of a 
  Dtable. 
 */
VALUE dtable_load(VALUE klass, VALUE str)
{
  VALUE ret = Qnil;
  VALUE s = StringValue(str);
  unsigned char * buf = (unsigned char *) StringValuePtr(s);
  unsigned char * dest = buf + RSTRING_LEN(s);
  unsigned i; /* for GET_UNSIGNED */
  unsigned tmp = 0;
  long rows, cols;
  long x,y;
  double ** data;
  double * col;
  /*  depending on the first byte, the decoding will be different */
  switch(*(buf++)) 
    {
    case 1:
      GET_UNSIGNED(tmp, buf);
      rows = tmp;
      GET_UNSIGNED(tmp, buf);
      cols = tmp;
      /* create a new Dtable with the right size */
      ret = dtable_init(dtable_alloc(cDtable), cols, rows);
      data = Dtable_Ptr(ret, NULL, NULL);
      for(x = 0; x < rows; x++) 
	{
	  col = data[x];
	  for(y = 0; y< cols; y++)
	    {
	      if(buf + 8 > dest)
		{
		  rb_raise(rb_eRuntimeError, 
			   "corrupted data given to Dtable._load");
		  break;
		}	
	      else 
		{
		  col[y] = get_double(buf);
		  buf += 8;
		}
	    }
	}
      break;
    default:
      rb_raise(rb_eRuntimeError, "corrupted data given to Dtable._load");
    }
  return ret;
}

/* The following function has been written by Benjamin ter Kuile <bterkuile@gmail.com> */

PRIVATE
/*
 *  call-seq:
 *     dtable.interpolate(Xs, Ys, nx, ny, x_start, x_end, y_start, y_end)  -> a_dtable
 *  
 *  Returns a copy of _dtable_ with the values interpolated given the proper X and Y axis to create a uniform spaced result in the X- and Y 
 *  direction consisting of nx- and ny values for each direction.
 */ VALUE dtable_interpolate(VALUE ary, VALUE x_vec, VALUE y_vec, VALUE nx_val, VALUE ny_val, VALUE xstart_val, VALUE xend_val, VALUE ystart_val, VALUE yend_val)
{
   Dtable *d = Get_Dtable(ary);
	int nx = NUM2DBL(rb_Integer(nx_val));
	int ny = NUM2DBL(rb_Integer(ny_val));
	int i, j, num_cols = d->num_cols, num_rows = d->num_rows/*, last_row = num_rows - 1*/;
	
   long xsrc_len, ysrc_len;
   double *xsrc = Dvector_Data_for_Read(x_vec, &xsrc_len);
   double *ysrc = Dvector_Data_for_Read(y_vec, &ysrc_len);
	if(xsrc_len != num_cols) rb_raise(rb_eArgError, "Number of x values (%ld) do not match the number of columns (%d)", xsrc_len, num_cols);
	if(ysrc_len != num_rows) rb_raise(rb_eArgError, "Number of y values (%ld) do not match the number of rows (%d)", ysrc_len, num_rows);
   VALUE new = dtable_init(dtable_alloc(cDtable), nx, ny);
   Dtable *d2 = Get_Dtable(new);
   double **src, **dest;
	xstart_val = rb_Float(xstart_val);
	double xstart = NUM2DBL(rb_Float(xstart_val));
	if(xstart < xsrc[0]) rb_raise(rb_eArgError, "The start x value %g is smaller than the bound (%g)", xstart, xsrc[0]);
	double xend = NUM2DBL(rb_Float(xend_val));
	if(xend > xsrc[xsrc_len-1]) rb_raise(rb_eArgError, "The end x value %g is bigger than the bound (%g)", xend, xsrc[xsrc_len-1]);
	double ystart = NUM2DBL(rb_Float(ystart_val));
	if(ystart < ysrc[0]) rb_raise(rb_eArgError, "The start y value %g is smaller than the bound (%g)", ystart, ysrc[0]);
	double yend = NUM2DBL(rb_Float(yend_val));
	if(yend > ysrc[ysrc_len-1]) rb_raise(rb_eArgError, "The end y value %g is bigger than the bound (%g)", yend, ysrc[ysrc_len-1]);
	double dx = (xend-xstart)/(nx-1);
	double dy = (yend-ystart)/(ny-1);
	double xcurrent = xstart;
	double ycurrent = ystart;
	double intvalue;
	int isrc = 1;
	int jsrc = 1;
   src = d->ptr; dest = d2->ptr;
   for (i = 1; i < ny+1; i++) {
		while(ysrc[isrc] < ycurrent && ycurrent < ysrc[ysrc_len-1]){
			isrc++;
		}
      for (j = 1; j < nx+1; j++) {
			while(xsrc[jsrc] < xcurrent && xcurrent < xsrc[xsrc_len-1]){
				jsrc++;
			}
			intvalue = (
				( src[isrc-1][jsrc-1]*(ysrc[isrc]-ycurrent)*(xsrc[jsrc]-xcurrent)) +
				( src[isrc][jsrc - 1] * (ycurrent - ysrc[isrc - 1]) * (xsrc[jsrc] - xcurrent) ) +
				( src[isrc - 1][jsrc] * (ysrc[isrc] - ycurrent) * (xcurrent - xsrc[jsrc - 1]) ) +
				( src[isrc][jsrc] * (ycurrent - ysrc[isrc - 1]) * (xcurrent - xsrc[jsrc - 1]) )
			);
			intvalue = intvalue / ( (ysrc[isrc] - ysrc[isrc - 1]) * (xsrc[jsrc] - xsrc[jsrc - 1]) );
         dest[i-1][j-1] = intvalue;
			xcurrent += dx;
      }
		xcurrent = xstart;
		jsrc = 1;
		ycurrent += dy;
   }
   return new;
}

PRIVATE 
/*
 *  call-seq:
 *     dtable.sum   ->  number
 *  
 *  Returns the sum of the entries in _dtable_. Returns 0.0 if
 *  _dtable_ is empty.
 *     
 *     a = Dtable.new(2,4)
 *     a.set_column(0, Dvector[1,2,3,4])
 *     a.set_column(1, Dvector[0, 1, 0, 1])
 *     a.sum        -> 12.0
 *     Dtable.new(2,2).sum   -> 0.0
 */ 
VALUE dtable_sum(VALUE tabl){
  int i,j;
  double sum=0.0;
  double **src;
  Dtable *d = Get_Dtable(tabl);
  src = d->ptr; 
  for(i=0; i < d->num_cols; i++){
    for(j=0; j < d->num_rows; j++){
      sum = sum + src[j][i];
    }
  }
  return rb_float_new(sum);
}

/* 
 * Document-class: Dobjects::Dtable
 *
 * Dtables are a specialized implementation of two-dimensional arrays of double precision floating point numbers. 
 * They are intended for use in applications needing efficient processing of large 2D tables of numeric data.
 * Essentially any of the operations you might do with a Ruby Array of numbers can also be done with a Dtable.
 * Dtables follow the same design philosophy as Dvector and uses Dvectors for several operations.
 */
PUBLIC void Init_Dtable() {
   /* modified by Vincent Fourmond, for splitting out the libraries */
  
   rb_require("Dobjects/Dvector");
   /* we first make sure Dvector is included */
   VALUE mDobjects = rb_define_module("Dobjects");
   cDtable = rb_define_class_under(mDobjects, "Dtable", rb_cObject);
   /* end of modification */
   
   rb_define_alloc_func(cDtable, dtable_alloc);
   rb_define_method(cDtable, "initialize", dtable_initialize, -1);
   
   rb_define_method(cDtable, "read", dtable_read, -1);

   rb_define_method(cDtable, "num_cols", dtable_num_cols, 0);
   rb_define_method(cDtable, "num_rows", dtable_num_rows, 0);

   rb_define_method(cDtable, "at", dtable_at, 2);
   rb_define_alias(cDtable,  "[]", "at");
   rb_define_method(cDtable, "[]=", dtable_aset, 3);

   rb_define_method(cDtable, "row", dtable_row, 1);
   rb_define_method(cDtable, "column", dtable_column, 1);
   rb_define_method(cDtable, "set_row", dtable_set_row, 2);
   rb_define_method(cDtable, "set_column", dtable_set_column, 2);

   rb_define_method(cDtable, "clear", dtable_clear, 0);
   rb_define_method(cDtable, "set", dtable_set, 1);
   rb_define_method(cDtable, "max", dtable_max, 0);
   rb_define_method(cDtable, "min", dtable_min, 0);
   rb_define_method(cDtable, "min_gt", dtable_min_gt, 1);
   rb_define_method(cDtable, "max_lt", dtable_max_lt, 1);
   
   rb_define_method(cDtable, "dup", dtable_dup, 0);
   rb_define_method(cDtable, "transpose", dtable_transpose, 0);
   rb_define_method(cDtable, "reverse_rows", dtable_reverse_rows, 0);
   rb_define_method(cDtable, "reverse_cols", dtable_reverse_cols, 0);
   rb_define_method(cDtable, "rotate_cw90", dtable_rotate_cw90, 0);
   rb_define_method(cDtable, "rotate_ccw90", dtable_rotate_ccw90, 0);
   
   /* math operations */
   rb_define_method(cDtable, "add", dtable_add, 1);
   rb_define_alias(cDtable,  "+", "add");
   rb_define_alias(cDtable,  "plus", "add");
   rb_define_method(cDtable, "sub", dtable_sub, 1);
   rb_define_alias(cDtable,  "-", "sub");
   rb_define_alias(cDtable,  "minus", "sub");
   rb_define_method(cDtable, "mul", dtable_mul, 1);
   rb_define_alias(cDtable,  "*", "mul");
   rb_define_alias(cDtable,  "times", "mul");
   rb_define_method(cDtable, "div", dtable_div, 1);
   rb_define_alias(cDtable,  "/", "div");
   rb_define_method(cDtable, "modulo", dtable_mod, 1);
   rb_define_alias(cDtable,  "mod", "modulo");
   rb_define_alias(cDtable,  "%", "modulo");
   rb_define_method(cDtable, "remainder", dtable_remainder, 1);
   rb_define_method(cDtable, "pow", dtable_pow, 1);
   rb_define_alias(cDtable,  "raised_to", "pow");
   rb_define_alias(cDtable,  "**", "pow");
   rb_define_method(cDtable, "as_exponent_of", dtable_as_exponent_of, 1);
   rb_define_method(cDtable, "atan2", dtable_atan2, 1);
   
   /* numeric methods */
   rb_define_method(cDtable, "abs", dtable_abs, 0);
   rb_define_method(cDtable, "ceil", dtable_ceil, 0);
   rb_define_method(cDtable, "floor", dtable_floor, 0);
   rb_define_method(cDtable, "round", dtable_round, 0);

   /* standard math functions */
   rb_define_method(cDtable, "acos", dtable_acos, 0);
   rb_define_method(cDtable, "acosh", dtable_acosh, 0);
   rb_define_method(cDtable, "asin", dtable_asin, 0);
   rb_define_method(cDtable, "asinh", dtable_asinh, 0);
   rb_define_method(cDtable, "atan", dtable_atan, 0);
   rb_define_method(cDtable, "atanh", dtable_atanh, 0);
   rb_define_method(cDtable, "cos", dtable_cos, 0);
   rb_define_method(cDtable, "cosh", dtable_cosh, 0);
   rb_define_method(cDtable, "exp", dtable_exp, 0);
   rb_define_method(cDtable, "log", dtable_log, 0);
   rb_define_method(cDtable, "log10", dtable_log10, 0);
   rb_define_method(cDtable, "sin", dtable_sin, 0);
   rb_define_method(cDtable, "sinh", dtable_sinh, 0);
   rb_define_method(cDtable, "sqrt", dtable_sqrt, 0);
   rb_define_method(cDtable, "tan", dtable_tan, 0);
   rb_define_method(cDtable, "tanh", dtable_tanh, 0);

   /* nonstandard math functions */
   rb_define_method(cDtable, "neg", dtable_neg, 0);
   rb_define_alias(cDtable, "-@", "neg");
   rb_define_method(cDtable, "exp10", dtable_exp10, 0);
   rb_define_method(cDtable, "inv", dtable_inv, 0);
   rb_define_method(cDtable, "trim", dtable_trim, -1);
   rb_define_method(cDtable, "safe_log", dtable_safe_log, -1);
   rb_define_method(cDtable, "safe_log10", dtable_safe_log10, -1);
   rb_define_method(cDtable, "safe_inv", dtable_safe_inv, -1);
   rb_define_method(cDtable, "safe_sqrt", dtable_safe_sqrt, 0);
   rb_define_method(cDtable, "safe_asin", dtable_safe_asin, 0);
   rb_define_method(cDtable, "safe_acos", dtable_safe_acos, 0);

   rb_define_method(cDtable, "add!", dtable_add_bang, 1);
   rb_define_alias(cDtable,  "plus!", "add!");
   rb_define_method(cDtable, "sub!", dtable_sub_bang, 1);
   rb_define_alias(cDtable,  "minus!", "sub!");
   rb_define_method(cDtable, "mul!", dtable_mul_bang, 1);
   rb_define_alias(cDtable,  "times!", "mul!");
   rb_define_method(cDtable, "div!", dtable_div_bang, 1);
   rb_define_method(cDtable, "modulo!", dtable_modulo_bang, 1);
   rb_define_alias(cDtable,  "mod!", "modulo!");
   rb_define_method(cDtable, "remainder!", dtable_remainder_bang, 1);
   rb_define_method(cDtable, "pow!", dtable_pow_bang, 1);
   rb_define_alias(cDtable,  "raised_to!", "pow!");
   rb_define_method(cDtable, "as_exponent_of!", dtable_as_exponent_of_bang, 1);
   rb_define_method(cDtable, "atan2!", dtable_atan2_bang, 1);

   rb_define_method(cDtable, "neg!", dtable_neg_bang, 0);
   rb_define_method(cDtable, "abs!", dtable_abs_bang, 0);
   rb_define_method(cDtable, "sin!", dtable_sin_bang, 0);
   rb_define_method(cDtable, "cos!", dtable_cos_bang, 0);
   rb_define_method(cDtable, "tan!", dtable_tan_bang, 0);
   rb_define_method(cDtable, "asin!", dtable_asin_bang, 0);
   rb_define_method(cDtable, "acos!", dtable_acos_bang, 0);
   rb_define_method(cDtable, "atan!", dtable_atan_bang, 0);
   rb_define_method(cDtable, "sinh!", dtable_sinh_bang, 0);
   rb_define_method(cDtable, "cosh!", dtable_cosh_bang, 0);
   rb_define_method(cDtable, "tanh!", dtable_tanh_bang, 0);
   rb_define_method(cDtable, "asinh!", dtable_asinh_bang, 0);
   rb_define_method(cDtable, "acosh!", dtable_acosh_bang, 0);
   rb_define_method(cDtable, "atanh!", dtable_atanh_bang, 0);
   rb_define_method(cDtable, "ceil!", dtable_ceil_bang, 0);
   rb_define_method(cDtable, "floor!", dtable_floor_bang, 0);
   rb_define_method(cDtable, "round!", dtable_round_bang, 0);
   rb_define_method(cDtable, "exp!", dtable_exp_bang, 0);
   rb_define_method(cDtable, "exp10!", dtable_exp10_bang, 0);
   rb_define_method(cDtable, "log!", dtable_log_bang, 0);
   rb_define_method(cDtable, "log10!", dtable_log10_bang, 0);
   rb_define_method(cDtable, "inv!", dtable_inv_bang, 0);
   rb_define_method(cDtable, "sqrt!", dtable_sqrt_bang, 0);
   
   rb_define_method(cDtable, "trim!", dtable_trim_bang, -1);
   rb_define_method(cDtable, "safe_log!", dtable_safe_log_bang, -1);
   rb_define_method(cDtable, "safe_log10!", dtable_safe_log10_bang, -1);
   rb_define_method(cDtable, "safe_inv!", dtable_safe_inv_bang, -1);
   rb_define_method(cDtable, "safe_sqrt!", dtable_safe_sqrt_bang, 0);
   rb_define_method(cDtable, "safe_asin!", dtable_safe_asin_bang, 0);
   rb_define_method(cDtable, "safe_acos!", dtable_safe_acos_bang, 0);

   rb_define_method(cDtable, "interpolate", dtable_interpolate, 8);
   rb_define_method(cDtable, "sum", dtable_sum, 0);
   rb_define_method(cDtable, "each_row", dtable_each_row, 0);
   rb_define_method(cDtable, "each_column", dtable_each_column, 0);

   /* Marshal : */
   rb_define_method(cDtable, "_dump", dtable_dump, 1);
   rb_define_singleton_method(cDtable, "_load", dtable_load, 1);
   /* modified by Vincent Fourmond, for splitting out the libraries */
   rb_require("Dobjects/Dtable_extras.rb");
   /* end of modification */

   /* first, we export the symbols needed by other libraries
      see include/dtable.h for their description 
   */
   RB_EXPORT_SYMBOL(cDtable, Read_Dtable);
   RB_EXPORT_SYMBOL(cDtable, Dtable_Ptr);

   /* now we import the symbols from Dvector */
   VALUE cDvector = rb_const_get(mDobjects, rb_intern("Dvector"));
   RB_IMPORT_SYMBOL(cDvector, Dvector_Create);
   RB_IMPORT_SYMBOL(cDvector, Dvector_Data_Resize);
   RB_IMPORT_SYMBOL(cDvector, Dvector_Data_Replace);
   RB_IMPORT_SYMBOL(cDvector, Dvector_Data_for_Read);
   RB_IMPORT_SYMBOL(cDvector, Dvector_Store_Double);

}

/* implementing the symnbols just means that the location
   of the global variable with the pointer function will be here 
*/
IMPLEMENT_SYMBOL(Dvector_Create);
IMPLEMENT_SYMBOL(Dvector_Data_Resize);
IMPLEMENT_SYMBOL(Dvector_Data_Replace);
IMPLEMENT_SYMBOL(Dvector_Data_for_Read);
IMPLEMENT_SYMBOL(Dvector_Store_Double);


