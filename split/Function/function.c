/**********************************************************************

   Function.c
 
   An object embedding two Dvectors for the ease of manipulation as
   a function. 
   
   Copyright (C) 2006  Vincent Fourmond

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
   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
   
**********************************************************************/

#include <ruby.h>

#include "dvector.h"

/* the class we're defining */
static VALUE cFunction;
static VALUE cDvector;

static ID idSize;

#define IS_A_DVECTOR(x) RTEST(rb_obj_is_kind_of(x, cDvector))

#define X_VAL "@x_val"
#define Y_VAL "@y_val"
#define SPLINE_CACHE "@spline_cache"


/* basic functions for accessing the objects */
inline static VALUE get_x_vector(VALUE self) 
{
  return rb_iv_get(self, X_VAL);
}

inline static void set_x_vector(VALUE self, VALUE vector) 
{
  rb_iv_set(self, X_VAL, vector);
}

inline static VALUE get_y_vector(VALUE self) 
{
  return rb_iv_get(self, Y_VAL);
}

inline static void set_y_vector(VALUE self, VALUE vector) 
{
  rb_iv_set(self, Y_VAL, vector);
}


inline static VALUE get_spline_vector(VALUE self) 
{
  return rb_iv_get(self, SPLINE_CACHE);
}

inline static VALUE set_spline_vector(VALUE self, VALUE vector) 
{
  rb_iv_set(self, SPLINE_CACHE, vector);
}


/*
  call-seq:
    Function.new(x,y)

    Creates a Function object with given +x+ and +y+ values.
 */
static VALUE function_initialize(VALUE self, VALUE x, VALUE y)
{
  if(IS_A_DVECTOR(x) && IS_A_DVECTOR(y)) 
    {
      if(rb_funcall(x, idSize,0) == rb_funcall(y, idSize,0)) {
	set_x_vector(self, x);
	set_y_vector(self, y);
	/* fine, this could have been written in pure Ruby...*/
      }
      else
	rb_raise(rb_eArgError,"both vectors must have the same size");
    }
  else 
    rb_raise(rb_eArgError,"both arguments must be Dvector");
  return self;
}
  
/*
  Checks if the +x+ values of the Function are sorted.
*/
static VALUE function_is_sorted(VALUE self)
{
  long size;
  const double * x_data;
  double prev;
  x_data = Dvector_Data_for_Read(get_x_vector(self), &size);
  prev = x_data[0];
  while((--size) && prev <= *(++x_data))
    prev = *x_data;
  if(size)
    return Qfalse;
  else
    return Qtrue;
}

/* the function fort joint sorting...*/
extern void joint_quicksort (double *const x_values, double * const y_values,
			     size_t total_elems);

/* Dvector's lock */
#define DVEC_TMPLOCK  FL_USER1

/* 
   Sorts +x+, while ensuring that the corresponding +y+ values
   keep matching. Should be pretty fast.
*/

static VALUE function_joint_sort(VALUE self, VALUE x, VALUE y)
{
  long x_len, y_len;
  double * x_values = Dvector_Data_for_Write(x, &x_len);
  double * y_values = Dvector_Data_for_Write(y, &y_len);
  if(x_len != y_len)
    rb_raise(rb_eArgError,"both vectors must have the same size");
  else 
    {
      /* we temporarily freeze both Dvectors before sorting */
      FL_SET(x, DVEC_TMPLOCK);
      FL_SET(y, DVEC_TMPLOCK);
      joint_quicksort(x_values, y_values, (size_t) x_len);
      /* and unfreeze them */
      FL_UNSET(x, DVEC_TMPLOCK);
      FL_UNSET(y, DVEC_TMPLOCK);
    }
  return self; /* nothing interesting */
}

/*
  Sorts the X values while keeping the matching Y values. 
*/
static VALUE function_sort(VALUE self)
{
  function_joint_sort(self,get_x_vector(self), get_y_vector(self));
}

 
static void init_IDs()
{
  idSize = rb_intern("size");
}
 
void Init_Function() 
{
  init_IDs();
  
  rb_require("Dobjects/Dvector");
  VALUE mDobjects = rb_define_module("Dobjects");
  cFunction = rb_define_class_under(mDobjects, "Function", rb_cObject);

  /* get the Dvector class */
  cDvector = rb_const_get(mDobjects, rb_intern("Dvector"));

  rb_define_method(cFunction, "initialize", function_initialize, 2);
  rb_define_method(cFunction, "sorted?", function_is_sorted, 0);
  rb_define_alias(cFunction,  "is_sorted", "sorted?");

  rb_define_singleton_method(cFunction, "joint_sort", function_joint_sort, 2);
  rb_define_method(cFunction, "sort", function_sort, 0);

  /* now, we import the necessary symbols from Dvector */
  RB_IMPORT_SYMBOL(cDvector, Dvector_Data_for_Read);
  RB_IMPORT_SYMBOL(cDvector, Dvector_Data_for_Write);
}

IMPLEMENT_SYMBOL(Dvector_Data_for_Read);
IMPLEMENT_SYMBOL(Dvector_Data_for_Write);
