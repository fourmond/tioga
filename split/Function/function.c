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
static ID idSetDirty;
static ID idDirty;

/* a few macros to work with Dvectors */
#define IS_A_DVECTOR(x) RTEST(rb_obj_is_kind_of(x, cDvector))

/* returns the size of a Dvector object */
#define DVECTOR_SIZE(x) (NUM2LONG(rb_funcall(x, idSize,0)))

#define DVECTOR_IS_DIRTY(x) (RTEST(rb_funcall(x, idDirty,0)))
#define DVECTOR_CLEAR(x) (rb_funcall(x, idSetDirty,1, Qfalse))


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

inline static void set_spline_vector(VALUE self, VALUE vector) 
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
      if(DVECTOR_SIZE(x) == DVECTOR_SIZE(y)) {
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

/* test if the number if finite */
#define is_okay_number(x) ((x) - (x) == 0.0)

/* small macros to make the code a little more clear */
#define FIXED_BOUNDARY(n, slope) (3.0/(x_vals[n+1] - x_vals[n])) *\
((y_vals[n+1] - y_vals[n])/(x_vals[n+1] - x_vals[n]) - slope)


/* This code is greatly inspired by what can be found in the book
   Numerical Recipes in C. It fills the y2_vals values with computed
   second derivatives. left_der and right_der are boundary conditions.
   If not finite, use natural spline.
*/
static void function_fill_second_derivatives(long nb_points,
					     const double *x_vals,
					     const double *y_vals,
					     double * y2_vals,
					     double left_slope,
					     double right_slope)
{
   if(nb_points < 1)
     return;
   double *tmp = (double *)ALLOC_N(double, nb_points);
   long i;
   double piv;
   double ratio;

   if(is_okay_number(left_slope)) /* slope is defined */
     {
       y2_vals[0] = -0.5;
       tmp[0] = FIXED_BOUNDARY(0,left_slope);
     }
   else
     y2_vals[0] = tmp[0] = 0; /* natural spline */
   
   /* forward decomposition */
   for(i = 1; i < nb_points - 1; i++) 
     {
       ratio = (x_vals[i] - x_vals[i-1])/(x_vals[i+1] - x_vals[i-1]);
       piv = 1/(ratio * y2_vals[i-1] + 2.0);
       y2_vals[i] = (ratio - 1.0) * piv;
       tmp[i] = (6.0 * 
		 ((y_vals[i+1] - y_vals[i]  )/
		  (x_vals[i+1] - x_vals[i]  ) - 
		  (y_vals[i]   - y_vals[i-1])/
		  (x_vals[i]   - x_vals[i-1])
		  )/
		 (x_vals[i+1]  - x_vals[i-1]) 
		 - ratio * tmp[i-1]) * piv;
     }
   /* then, the right boundary condition */
   if(is_okay_number(right_slope)) /* slope is defined */
     {
       y2_vals[nb_points - 1] = 0.5;
       tmp[nb_points - 1] = - FIXED_BOUNDARY(nb_points - 2,right_slope);
     }
   else
     y2_vals[nb_points - 1] = tmp[nb_points - 1] = 0; /* natural spline */

   /* then, backward substitution */
   y2_vals[nb_points - 1] = (tmp[nb_points - 1] - 
			     y2_vals[nb_points - 1] * tmp[nb_points - 2])/
     (y2_vals[nb_points - 1] * y2_vals[nb_points - 2] + 1.0);
   for(i = nb_points - 2; i >= 0; i--)
     y2_vals[i] = y2_vals[i]*y2_vals[i+1] + tmp[i];
   /* done, we free the allocated buffer */
   xfree(tmp);
}

/* Computes spline data and caches it inside the object. Both X and Y vectors
   are cleared (see Dvector#clear) to make sure the cache is kept up-to-date.
*/
static VALUE function_compute_spline_data(VALUE self)
{
  VALUE x_vec = get_x_vector(self);
  VALUE y_vec = get_y_vector(self);
  VALUE cache = get_spline_vector(self);
  long size = DVECTOR_SIZE(x_vec);

  if(DVECTOR_SIZE(y_vec) != size)
    rb_raise(rb_eRuntimeError, 
	     "x and y should have the same size !");
  if(! IS_A_DVECTOR(cache))    /* create it -- and silently ignores
				  its previous values */
      cache = rb_funcall(cDvector, rb_intern("new"),
			 1, LONG2NUM(size));
  if(DVECTOR_SIZE(y_vec) != size) /* switch to the required size */
    Dvector_Data_Resize(cache, size);
  
  double * x, *y, *spline;
  x = Dvector_Data_for_Read(x_vec, NULL);
  y = Dvector_Data_for_Read(y_vec, NULL);
  spline = Dvector_Data_for_Write(cache, NULL);

  function_fill_second_derivatives(size, x, y, spline,1.0/0.0, 1.0/0.0);
  set_spline_vector(self, cache);

  /* now, we clear both X and Y */
  DVECTOR_CLEAR(x_vec);
  DVECTOR_CLEAR(y_vec);
  return self;
}

/* Computes the results of spline interpolation for the given set
   of x points. It assumes that x points are sorted and within range ...
*/
static void function_compute_spline_interpolation(long dat_size,
						  const double * x_dat,
						  const double * y_dat,
						  const double * y2_dat,
						  long dest_size,
						  const double * x,
						  double * y)
{
  long low,hi,mid;
  double h;
  double a,b;
  low = 0;
  hi = dat_size - 1;
  if(dest_size <= 1) /* nothing interesting to be done here...*/
    return;
  if(x[0] < x_dat[0] || x[dest_size - 1] > x_dat[dat_size - 1])
    rb_raise(rb_eRuntimeError, "x range should be within x_dat range");
  /* first, we seek the first point by bisection */
  while(low - hi >  1) 
    {
      mid = (low + hi) >> 1;
      if(x[0] > x_dat[mid])
	low = mid;
      else
	hi = mid;
    }
  
  for(hi = 0; hi < dest_size; hi++)
    {
      while(x_dat[low + 1] < x[hi])
	low++; /* seek forward - shouldn't be too long ? */
      h = x_dat[low + 1] - x_dat[low];
      /* should hopefully not be zero */
      if(h <= 0.0)
	rb_raise(rb_eRuntimeError, 
		 "x_dat must be striclty growing");
      a = (x_dat[low + 1] - x[hi])/h;
      b = - (x_dat[low] - x[hi])/h;
      /* spline evaluation */
      y[hi] = a * y_dat[low] +
	b * y_dat[low + 1] +
	( (a*a*a - a) * y2_dat[low] +
	  (b*b*b - b) * y2_dat[low + 1]
	  ) * (h * h)/6.0;
    }
}

/* Interpolates the value of the function at the points given.
   Returns a brand new Dvector. The X values must be sorted ! 
 */
static VALUE function_compute_spline(VALUE self, VALUE x_values)
{
  VALUE x_vec = get_x_vector(self);
  VALUE y_vec = get_y_vector(self);
  VALUE cache = get_spline_vector(self);
  VALUE ret_val;
  long dat_size = DVECTOR_SIZE(x_vec);
  long size = DVECTOR_SIZE(x_values);
  

  /* check that the cache is here and up-to-date */
  if(! IS_A_DVECTOR(cache) || 
     DVECTOR_IS_DIRTY(x_vec) || 
     DVECTOR_IS_DIRTY(y_vec))
    function_compute_spline_data(self);

  ret_val = rb_funcall(cDvector, rb_intern("new"),
		       1, LONG2NUM(size));
  double * x_dat = Dvector_Data_for_Read(x_vec,NULL);
  double * y_dat = Dvector_Data_for_Read(y_vec,NULL);
  double * spline = Dvector_Data_for_Read(cache,NULL);
  double * x = Dvector_Data_for_Read(x_values,NULL);
  double * y = Dvector_Data_for_Write(ret_val,NULL);
  
  function_compute_spline_interpolation(dat_size,
					x_dat,
					y_dat,
					spline,
					size,
					x,
					y);
  return ret_val;
}
						     

/* the function fort joint sorting...*/
extern void joint_quicksort(double *const x_values, double * const y_values,
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


/* Yields X and Y */
static VALUE function_each(VALUE self)
{
  long x_len, y_len;
  VALUE x = get_x_vector(self);
  VALUE y = get_y_vector(self);
  double * x_values = Dvector_Data_for_Write(x, &x_len);
  double * y_values = Dvector_Data_for_Write(y, &y_len);
  if(x_len != y_len)
    rb_raise(rb_eRuntimeError,"X and Y must have the same size");
  else 
    {
      /* we temporarily freeze both Dvectors during iteration */
      FL_SET(x, DVEC_TMPLOCK);
      FL_SET(y, DVEC_TMPLOCK);
      while(x_len--)
	{
	  VALUE flt_x = rb_float_new(*x_values++);
	  VALUE flt_y = rb_float_new(*y_values++);
	  rb_yield_values(2, flt_x, flt_y);
	}
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
  return function_joint_sort(self,get_x_vector(self), get_y_vector(self));
}

 
static void init_IDs()
{
  idSize  = rb_intern("size");
  idSetDirty = rb_intern("dirty=");
  idDirty = rb_intern("dirty?");
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

  /* spline stuff :*/
  rb_define_method(cFunction, "compute_spline_data", 
		   function_compute_spline_data, 0);
  rb_define_method(cFunction, "compute_spline", 
		   function_compute_spline, 1);

  /* iterator */
  rb_define_method(cFunction, "each", 
		   function_each, 0);


  /* now, we import the necessary symbols from Dvector */
  RB_IMPORT_SYMBOL(cDvector, Dvector_Data_for_Read);
  RB_IMPORT_SYMBOL(cDvector, Dvector_Data_for_Write);
  RB_IMPORT_SYMBOL(cDvector, Dvector_Data_Resize);
}

IMPLEMENT_SYMBOL(Dvector_Data_for_Read);
IMPLEMENT_SYMBOL(Dvector_Data_for_Write);
IMPLEMENT_SYMBOL(Dvector_Data_Resize);
