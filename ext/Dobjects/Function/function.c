/**********************************************************************

   Function.c
 
   An object embedding two Dvectors for the ease of manipulation as
   a function. 
   
   Copyright (C) 2006,2010  Vincent Fourmond

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

#include <namespace.h>
#include <ruby.h>


#include <math.h>

/* Private include files */
#include "dvector.h"
#include "symbols.c"

/* compiler-dependent defintions, such as is_okay_number */
#include <defs.h>
/* End of private files */

/* the class we're defining */
static VALUE cFunction;
static VALUE cDvector;

/* ID used by different functions */
static ID idSize;
static ID idSetDirty;
static ID idDirty;
static ID idSort;
static ID idNew;

/* a few macros to work with Dvectors */
#define IS_A_DVECTOR(x) RTEST(rb_obj_is_kind_of(x, cDvector))

/* returns the size of a Dvector object */
#define DVECTOR_SIZE(x) (NUM2LONG(rb_funcall(x, idSize,0)))

#define DVECTOR_IS_DIRTY(x) (RTEST(rb_funcall(x, idDirty,0)))
#define DVECTOR_CLEAR(x) (rb_funcall(x, idSetDirty,1, Qfalse))
#define NUMERIC(x) (rb_type(x) == T_FIXNUM || \
rb_type(x) == T_BIGNUM)

#define X_VAL "@x_val"
#define Y_VAL "@y_val"
#define SPLINE_CACHE "@spline_cache"



/* basic functions for accessing the objects */


inline 
/* 
   The X vector.
*/
static VALUE get_x_vector(VALUE self) 
{
  return rb_iv_get(self, X_VAL);
}

inline 
static void set_x_vector(VALUE self, VALUE vector) 
{
  rb_iv_set(self, X_VAL, vector);
}

inline 
/* 
   The Y vector.
*/
static VALUE get_y_vector(VALUE self) 
{
  return rb_iv_get(self, Y_VAL);
}

inline 
static void set_y_vector(VALUE self, VALUE vector) 
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
  Checks that self is a Function, that it has X and Y Dvectors and that
  they both have the same size. In that case, the size is returned.
*/
static long function_sanity_check(VALUE self)
{
  if(RTEST(rb_obj_is_kind_of(self, cFunction)))
  {
    VALUE x = get_x_vector(self);
    VALUE y = get_y_vector(self);
    if(IS_A_DVECTOR(x)
       && IS_A_DVECTOR(y))
      {
	long size = DVECTOR_SIZE(x);
	if( size== DVECTOR_SIZE(y))
	  return size;
	else
	  {
	    rb_raise(rb_eRuntimeError, "X and Y vectors must have the"
		     " same size");
	    return -1;
	  }
      }
    else 
      {
	rb_raise(rb_eRuntimeError, "X and Y must be vectors");
	return -1;
      }
  }
  else 
    {
      rb_raise(rb_eRuntimeError, "self is no Function");
      return -1;
    }
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
	set_spline_vector(self,Qnil);
	/* We initialize the @spline_cache var */
      }
      else
	rb_raise(rb_eArgError,"both vectors must have the same size");
    }
  else 
    rb_raise(rb_eArgError,"both arguments must be Dvector");
  return self;
}

static VALUE Function_Create(VALUE x, VALUE y)
{
  return rb_funcall(cFunction, idNew, 2, x, y);
}

static int dvector_is_sorted(VALUE dvector)
{
  long size;
  const double * x_data;
  double prev;
  if(! IS_A_DVECTOR(dvector))
    rb_raise(rb_eArgError, "should take a Dvector as argument");
  else 
    {
      x_data = Dvector_Data_for_Read(dvector, &size);
      prev = x_data[0];
      while((--size) && prev <= *(++x_data))
	prev = *x_data;
      return (size == 0);
    }
  return 0;
}
  
/*
  Checks if the X values of the Function are sorted.
*/
static VALUE function_is_sorted(VALUE self)
{
  if(dvector_is_sorted(get_x_vector(self)))
    return Qtrue;
  else
    return Qfalse;
}

static VALUE function_sort(VALUE self);

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

/* 
   Computes spline data and caches it inside the object. Both X and Y vectors
   are cleared (see Dvector#clear) to make sure the cache is kept up-to-date.
   If the function is not sorted, sorts it.
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
      cache = rb_funcall(cDvector, idNew,
			 1, LONG2NUM(size));
  if(DVECTOR_SIZE(cache) != size) /* switch to the required size for cache */
    Dvector_Data_Resize(cache, size);

  /* we make sure that the X values are sorted */
  if(! RTEST(function_is_sorted(self)))
     function_sort(self);
  
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
      while(x_dat[low + 1] < x[hi] && low < dat_size - 1)
	low++; /* seek forward - shouldn't be too long ? */
      if(hi && x[hi] < x[hi - 1])
	rb_raise(rb_eArgError, 
		 "X values should be sorted");
      h = x_dat[low + 1] - x_dat[low];
      /* should hopefully not be zero */
      if(h <= 0.0)
	rb_raise(rb_eRuntimeError, 
		 "x_dat must be strictly growing");
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

/* makes sure that the spline data is present and up-to-date, refreshing
   it if necessary 
*/
static void function_ensure_spline_data_present(VALUE self)
{
  VALUE x_vec = get_x_vector(self);
  VALUE y_vec = get_y_vector(self);
  VALUE cache = get_spline_vector(self);
  long dat_size = function_sanity_check(self);

  if(! IS_A_DVECTOR(cache) || 
     DVECTOR_IS_DIRTY(x_vec) || 
     DVECTOR_IS_DIRTY(y_vec) || 
     DVECTOR_SIZE(cache) == dat_size
     )
    function_compute_spline_data(self);
}

/* Interpolates the value of the function at the points given.
   Returns a brand new Dvector. The X values must be sorted ! 
 */
static VALUE function_compute_spline(VALUE self, VALUE x_values)
{
  VALUE x_vec = get_x_vector(self);
  VALUE y_vec = get_y_vector(self);
  VALUE cache;
  VALUE ret_val;
  long dat_size = function_sanity_check(self);
  long size = DVECTOR_SIZE(x_values);
  
  function_ensure_spline_data_present(self);

  cache = get_spline_vector(self);

  ret_val = rb_funcall(cDvector, rb_intern("new"),
		       1, LONG2NUM(size));
  double * x_dat = Dvector_Data_for_Read(x_vec,NULL);
  double * y_dat = Dvector_Data_for_Read(y_vec,NULL);
  double * spline = Dvector_Data_for_Read(cache,NULL);
  double * x = Dvector_Data_for_Read(x_values,NULL);
  double * y = Dvector_Data_for_Write(ret_val,NULL);
  
  function_compute_spline_interpolation(dat_size, x_dat,
					y_dat, spline,
					size, x, y);
  return ret_val;
}

/*
  Returns an interpolant that can be fed to 
  Special_Paths#append_interpolant_to_path
  to make nice splines. 

  Can be used this way:

   f = Function.new(x,y)
   t.append_interpolant_to_path(f.make_interpolant)
   t.stroke
*/
static VALUE function_make_interpolant(VALUE self)
{
  VALUE x_vec = get_x_vector(self);
  VALUE y_vec = get_y_vector(self);
  VALUE cache;
  VALUE a_vec,b_vec,c_vec;
  VALUE ret_val;
  double *x, *y, *a, *b, *c, *y2;
  double delta_x;
  long size = function_sanity_check(self);
  long i;
  
  function_ensure_spline_data_present(self);

  cache = get_spline_vector(self);
  x = Dvector_Data_for_Read(x_vec,NULL);
  y = Dvector_Data_for_Read(y_vec,NULL);
  y2 = Dvector_Data_for_Read(cache,NULL);

  a_vec  = rb_funcall(cDvector, idNew, 1, LONG2NUM(size));
  a = Dvector_Data_for_Write(a_vec, NULL);
  b_vec  = rb_funcall(cDvector, idNew, 1, LONG2NUM(size));
  b = Dvector_Data_for_Write(b_vec, NULL);
  c_vec  = rb_funcall(cDvector, idNew, 1, LONG2NUM(size));
  c = Dvector_Data_for_Write(c_vec, NULL);

  /* from my computations, the formula is the following:
     A = (y_2n+1 - y_2n)/(6 * delta_x)
     B = 0.5 * y_2n
     C = (y_n+1 - y_n)/delta_x - (2 * y_2n + y_2n+1) * delta_x/6
  */

  for(i = 0; i < size - 1; i++)
    {
      delta_x = x[i+1] - x[i];
      a[i] = (y2[i+1] - y2[i]) / (6.0 * delta_x);
      b[i] = 0.5 * y2[i];
      c[i] = (y[i+1] - y[i])/delta_x - 
	(2 * y2[i] + y2[i+1]) * (delta_x / 6.0);
    }
  a[i] = b[i] = c[i] = 0.0;
  ret_val = rb_ary_new();
  rb_ary_push(ret_val, x_vec);
  rb_ary_push(ret_val, y_vec);
  rb_ary_push(ret_val, a_vec);
  rb_ary_push(ret_val, b_vec);
  rb_ary_push(ret_val, c_vec);

  return ret_val;
}
						     

/* the function fort joint sorting...*/
INTERN void joint_quicksort(double *const x_values, double * const y_values,
			    size_t total_elems);

/* Dvector's lock */
#define DVEC_TMPLOCK  FL_USER1

/* call-seq:
     Function.joint_sort(x,y)

   Sorts +x+, while ensuring that the corresponding +y+ values
   keep matching. Should be pretty fast, as it is derived from 
   glibc's quicksort.

    a = Dvector[3,2,1]
    b = a * 2                 -> [6,4,2]
    Function.joint_sort(a,b)  -> [[1,2,3], [2,4,6]]
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
  /* we return the array of both Dvectors */
  return rb_ary_new3(2,x,y); 
}


/* call-seq:
    f.each do |x,y| _code_ end
    
   Iterates over all the points in the Function, yielding X and Y for
   each point.
*/
static VALUE function_each(VALUE self) /* :yields: x,y */
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
   Makes sure the function is sorted.
*/
static VALUE function_ensure_sorted(VALUE self)
{
  if(!RTEST(function_is_sorted(self)))
    function_sort(self);
  return self;
}



/* 
   call-seq:
     interpolate(x_values)
     interpolate(a_number)

   Computes interpolated values of the data contained in +f+ and 
   returns a Function object holding both +x_values+ and the computed
   Y values. +x_values+ will be sorted if necessary.

   With the second form, specify only the number of points, and
   the function will construct the appropriate vector with equally spaced
   points within the function range.
*/
static VALUE function_interpolate(VALUE self, VALUE x_values)
{
  if(NUMERIC(x_values))
    {
      /* we're in the second case, although I sincerely doubt it would
	 come useful 
      */
      long size,i;
      /* we make sure the function is sorted */
      function_ensure_sorted(self);
      double * data;
      double x_min;
      double x_max;
      data = Dvector_Data_for_Read(get_x_vector(self), &size);
      x_min = *data;
      x_max = *(data + size -1);
      x_values = rb_funcall(cDvector, idNew, 1, x_values);
      data = Dvector_Data_for_Write(x_values, &size);
      for(i = 0;i < size; i++)
	data[i] = x_min + ((x_max - x_min)/((double) (size-1))) * i;
    }
  if(! IS_A_DVECTOR(x_values))
    rb_raise(rb_eArgError, "x_values should be a Dvector or a number");
  else 
    {
      /* sort x_values */
      if(! dvector_is_sorted(x_values))
	rb_funcall(x_values, idSort,0);
      VALUE y_values = function_compute_spline(self, x_values);
      return rb_funcall(cFunction, idNew, 2, x_values, y_values);
    }
  return Qnil;
}



/*
  Strips all the points containing NaN values from the function, and
  returns the number of points stripped.
*/
static VALUE function_strip_nan(VALUE self)
{
  long size = function_sanity_check(self);
  long nb_stripped = 0;
  long i;

  double *x = Dvector_Data_for_Write(get_x_vector(self),NULL);
  double *y = Dvector_Data_for_Write(get_y_vector(self),NULL);
  for( i = 0; i < size; i++)
    {
      if(isnan(x[i]) || isnan(y[i]))
	nb_stripped ++;
      else
	{
	  x[i - nb_stripped] = x[i];
	  y[i - nb_stripped] = y[i];
	}
    }
  if(nb_stripped)
    {
      Dvector_Data_Resize(get_x_vector(self), size - nb_stripped);
      Dvector_Data_Resize(get_y_vector(self), size - nb_stripped);
    }
  return INT2NUM(nb_stripped);
}

/*
  Splits the function into strictly monotonic sub-functions.
  Returns the array of the subfunctions. The returned values are
  necessarily new values.
*/
static VALUE function_split_monotonic(VALUE self)
{
  VALUE ret = rb_ary_new();
  VALUE cur_x = Dvector_Create();
  VALUE cur_y = Dvector_Create();

  long size = function_sanity_check(self);
  long i;
  if(size < 2)
    rb_raise(rb_eRuntimeError, "Function needs to have at least 2 points");

  double *x = Dvector_Data_for_Read(get_x_vector(self),NULL);
  double *y = Dvector_Data_for_Read(get_y_vector(self),NULL);

  double last_x;
  double direction; /* -1 if down, +1 if up, so that the product of 
		       (x - last_x) with direction should always be positive
		    */
  VALUE f;
		     
		       
  /* bootstrap */
  if(x[1] > x[0])
    direction = 1;
  else
    direction = -1;
  last_x = x[1];
  for(i = 0; i < 2; i++)
    {
      Dvector_Push_Double(cur_x, x[i]);
      Dvector_Push_Double(cur_y, y[i]);
    }

  for(i = 2; i < size; i++) 
    {
      if(direction * (x[i] - last_x) <= 0) 
	{
	  /* we need to add a new set of Dvectors */
	  f = Function_Create(cur_x, cur_y);
	  rb_ary_push(ret, f);
	  cur_x = Dvector_Create();
	  cur_y = Dvector_Create();
	  /* We don't store the previous point if 
	   the X value is the same*/
	  if(x[i] != last_x) 
	    {
	      Dvector_Push_Double(cur_x, x[i-1]);
	      Dvector_Push_Double(cur_y, y[i-1]);
	    }
	  direction *= -1;
	}
      /* store the current point */
      Dvector_Push_Double(cur_x, x[i]);
      Dvector_Push_Double(cur_y, y[i]);
      last_x = x[i];
    }
  f = Function_Create(cur_x, cur_y);
  rb_ary_push(ret, f);
  return ret;
}


/*
  Splits the function on NaN values for x, y or xy, depending on
  whether _sym_ is +:x+, +:y+ or +:xy+ (or, as a matter of fact,
  anything else than +:x+ or +:y+).

  This returns an array of new Function objects.

  This function will return empty Function objects between consecutive
  NaN values.
*/
static VALUE function_split_on_nan(VALUE self, VALUE sym)
{
  VALUE ret = rb_ary_new();
  VALUE cur_x = Dvector_Create();
  VALUE cur_y = Dvector_Create();
  int on_x = 1;
  int on_y = 1;
  long size = function_sanity_check(self);
  long cur_size = 0;
  long i;
  if(size < 2)
    rb_raise(rb_eRuntimeError, "Function needs to have at least 2 points");

  double *x = Dvector_Data_for_Read(get_x_vector(self),NULL);
  double *y = Dvector_Data_for_Read(get_y_vector(self),NULL);

  VALUE f;
  
  if(sym == ID2SYM(rb_intern("x")))
    on_y = 0;
  else if(sym == ID2SYM(rb_intern("y")))
    on_x = 0;


  for(i = 0; i < size; i++) {
    if((on_x && isnan(x[i])) ||
       (on_y && isnan(y[i]))) {
      /* We split */
      f = Function_Create(cur_x, cur_y);
      rb_ary_push(ret, f);
      cur_x = Dvector_Create();
      cur_y = Dvector_Create();
    }
    else {
      Dvector_Push_Double(cur_x, x[i]);
      Dvector_Push_Double(cur_y, y[i]);
    }
  }
  f = Function_Create(cur_x, cur_y);
  rb_ary_push(ret, f);
  return ret;
}


/*
  Sorts the X values while keeping the matching Y values. 
*/
static VALUE function_sort(VALUE self)
{
  return function_joint_sort(self,get_x_vector(self), get_y_vector(self));
}

/*
  Returns a Dvector with two elements: the X and Y values of the
  point at the given index.
*/
static VALUE function_point(VALUE self, VALUE index)
{
  if(! NUMERIC(index))
    rb_raise(rb_eArgError, "index has to be numeric");
  else
    {
      long i = NUM2LONG(index);
      long size = function_sanity_check(self);
      if(size > 0 && i < size)
	{
	  VALUE point = rb_funcall(cDvector, idNew, 1, INT2NUM(2));
	  double * dat = Dvector_Data_for_Write(point, NULL);
	  double *x = Dvector_Data_for_Read(get_x_vector(self),NULL);
	  double *y = Dvector_Data_for_Read(get_y_vector(self),NULL);
	  dat[0] = x[i];
	  dat[1] = y[i];
	  return point;
	}
      else
	return Qnil;
    }
  return Qnil;
}

static void init_IDs()
{
  idSize  = rb_intern("size");
  idSetDirty = rb_intern("dirty=");
  idDirty = rb_intern("dirty?");
  idSort = rb_intern("sort");
  idNew = rb_intern("new");
}


/* a smaller helper for the following function */
#define DISTANCE(x,y) (((x) - xpoint) * ((x) - xpoint) /xscale/xscale \
+ ((y) - ypoint) * ((y) - ypoint) /yscale/yscale)

/*
  Returns the distance of a point to the function, computed by the minimum
  of ((x - xpoint)/xscale)**2 + ((y - ypoint)/yscale)**2. If index
  is not NULL, it receives the index of the point of minimum distance.
*/
static double private_function_distance(VALUE self, 
					double xpoint, double ypoint,
					double xscale, double yscale,
					long * dest_index)
{
  long size = function_sanity_check(self);
  const double *x = Dvector_Data_for_Read(get_x_vector(self),NULL);
  const double *y = Dvector_Data_for_Read(get_y_vector(self),NULL);
  double min = DISTANCE(x[0],y[0]);
  double cur;
  long index = 0;
  long i;
  for(i = 1; i < size; i++)
    {
      cur = DISTANCE(x[i], y[i]);
      if(cur < min)
	{
	  index = i;
	  min = cur;
	}
    }
  if(dest_index)
    *dest_index = index;
  return sqrt(min);
}

/*
  call-seq:
    f.distance(x,y) -> a_number
    f.distance(x,y, xscale, yscale) -> a_number
  
  Returns the distance of the function to the given point. Optionnal
  xscale and yscale says by how much we should divide the x and y
  coordinates before computing the distance. Use it if the distance is not
  homogeneous.
*/

static VALUE function_distance(int argc, VALUE *argv, VALUE self)
{
  switch(argc)
    {
    case 2:
      return rb_float_new(private_function_distance(self, 
						    NUM2DBL(argv[0]),
						    NUM2DBL(argv[1]),
						    1.0,1.0,NULL));
    case 4:
      return rb_float_new(private_function_distance(self, 
						    NUM2DBL(argv[0]),
						    NUM2DBL(argv[1]),
						    NUM2DBL(argv[2]),
						    NUM2DBL(argv[3]),
						    NULL));
    default:
      rb_raise(rb_eArgError, "distance should have 2 or 4 parameters");
    }
  return Qnil;
}


/*
  Code for integration.
*/
static double private_function_integrate(VALUE self, long start, long end)
{
  long size = function_sanity_check(self);
  const double *x = Dvector_Data_for_Read(get_x_vector(self),NULL);
  const double *y = Dvector_Data_for_Read(get_y_vector(self),NULL);
  long i = start;
  double val = 0;
  if(end >= size)
    end = size - 1;
  if(start < 0)
    start = 0;
  while(i < (end))
    {
      val += (y[i] + y[i+1]) * (x[i+1] - x[i]) * 0.5;
      i++;
    }
  return val;
}

/*
  :call-seq:
    f.integrate()  -> value
    f.integrate(start_index, end_index) -> value

  Returns the value of the integral of the function between the
  two indexes given, or over the whole function if no indexes are
  specified.
*/
static VALUE function_integrate(int argc, VALUE *argv, VALUE self)
{
  long start,end;
  switch(argc) 
    {
    case 0:
      start = 0;
      end = function_sanity_check(self) - 1; 
      break;
    case 2:
      start = NUM2LONG(argv[0]);
      end = NUM2LONG(argv[1]);
      break;
    default:
      rb_raise(rb_eArgError, "integrate should have 0 or 2 parameters");
    }
  return rb_float_new(private_function_integrate(self,start,end));
}

/*
  Computes the primitive of the Function (whose value for the first point is 0)
  and returns it as a new Function.
  The newly created function shares the X vector with the previous one.
*/
static VALUE function_primitive(VALUE self)
{
  long size = function_sanity_check(self);
  const double *x = Dvector_Data_for_Read(get_x_vector(self),NULL);
  const double *y = Dvector_Data_for_Read(get_y_vector(self),NULL);
  VALUE primitive = Dvector_Create();
  long i = 0;
  double val = 0;
  while(i < (size - 1))
    {
      Dvector_Push_Double(primitive, val);
      val += (y[i] + y[i+1]) * (x[i+1] - x[i]) * 0.5;
      i++;
    }
  Dvector_Push_Double(primitive, val);
  return Function_Create(get_x_vector(self), primitive);
}

/*
  Computes the derivative of the Function and returns it as a new Function.
  The newly created function shares the X vector with the previous one.

  WARNING: this is a very naive 3-points algorithm; you should
  consider using diff_5p
*/
static VALUE function_derivative(VALUE self)
{
  long size = function_sanity_check(self);
  const double *x = Dvector_Data_for_Read(get_x_vector(self),NULL);
  const double *y = Dvector_Data_for_Read(get_y_vector(self),NULL);
  VALUE derivative = Dvector_Create();
  long i = 0;
  /* First value */
  Dvector_Push_Double(derivative, (y[i+1] - y[i]) /(x[i+1] - x[i]));
  i++;
  while(i < (size - 1))
    {
      Dvector_Push_Double(derivative, 
			  .5 * (
				(y[i+1] - y[i]) /(x[i+1] - x[i]) + 
				(y[i] - y[i-1]) /(x[i] - x[i-1])
				));
      i++;
    }
  Dvector_Push_Double(derivative, (y[i] - y[i-1]) /(x[i] - x[i-1]));
  return Function_Create(get_x_vector(self), derivative);
}

/*
  Computes a 4th order accurate derivative of the Function.

  This function *requires* that there are at the very least 5 data
  points !
*/
static VALUE function_diff_5p(VALUE self)
{
  long size = function_sanity_check(self);
  const double *x = Dvector_Data_for_Read(get_x_vector(self),NULL);
  const double *y = Dvector_Data_for_Read(get_y_vector(self),NULL);
  VALUE derivative = Dvector_Create();
  long i = 0;
  double delta_1, delta_2, delta_3, delta_4;
  double alpha_1, alpha_2, alpha_3, alpha_4;
  double v0,v1,v2,v3,v4;
  /* TODO: what happens when there are less than 5 points ? */

  for(i = 0; i < size; i++) {
    /* First initialize values, though this is very suboptimal */
    v0 = y[i];
    if(i == 0) {
      delta_1 = x[1] - x[0]; v1 = y[1];
      delta_2 = x[2] - x[0]; v2 = y[2];
      delta_3 = x[3] - x[0]; v3 = y[3];
      delta_4 = x[4] - x[0]; v4 = y[4];
    } else if(i == 1) {
      delta_1 = x[0] - x[1]; v1 = y[0];
      delta_2 = x[2] - x[1]; v2 = y[2];
      delta_3 = x[3] - x[1]; v3 = y[3];
      delta_4 = x[4] - x[1]; v4 = y[4];
    } else if(i == size - 2) {
      delta_1 = x[size-1] - x[size-2]; v1 = y[size-1];
      delta_2 = x[size-3] - x[size-2]; v2 = y[size-3];
      delta_3 = x[size-4] - x[size-2]; v3 = y[size-4];
      delta_4 = x[size-5] - x[size-2]; v4 = y[size-5];
    } else if(i == size - 1) {
      delta_1 = x[size-2] - x[size-1]; v1 = y[size-2];
      delta_2 = x[size-3] - x[size-1]; v2 = y[size-3];
      delta_3 = x[size-4] - x[size-1]; v3 = y[size-4];
      delta_4 = x[size-5] - x[size-1]; v4 = y[size-5];
    } else {
      delta_1 = x[i-2] - x[i]; v1 = y[i-2];
      delta_2 = x[i-1] - x[i]; v2 = y[i-1];
      delta_3 = x[i+2] - x[i]; v3 = y[i+2];
      delta_4 = x[i+1] - x[i]; v4 = y[i+1];
    }
    alpha_1 = delta_2*delta_3*delta_4/
      (delta_1 * (delta_2 - delta_1) * (delta_3 - delta_1) 
       * (delta_4 - delta_1));
    alpha_2 = delta_1*delta_3*delta_4/
      (delta_2 * (delta_1 - delta_2) * (delta_3 - delta_2) 
       * (delta_4 - delta_2));
    alpha_3 = delta_1*delta_2*delta_4/
      (delta_3 * (delta_1 - delta_3) * (delta_2 - delta_3) 
       * (delta_4 - delta_3));
    alpha_4 = delta_1*delta_2*delta_3/
      (delta_4 * (delta_1 - delta_4) * (delta_2 - delta_4) 
       * (delta_3 - delta_4));
    Dvector_Push_Double(derivative,
			-(alpha_1 + alpha_2 + alpha_3 + alpha_4) * v0 +
			alpha_1 * v1 + alpha_2 * v2 + 
			alpha_3 * v3 + alpha_4 * v4);
  }
  return Function_Create(get_x_vector(self), derivative);
}

/*
  Computes a 4th order accurate second derivative of the Function.

  This function *requires* that there are at the very least 5 data
  points!  
*/
static VALUE function_diff2_5p(VALUE self)
{
  long size = function_sanity_check(self);
  const double *x = Dvector_Data_for_Read(get_x_vector(self),NULL);
  const double *y = Dvector_Data_for_Read(get_y_vector(self),NULL);
  VALUE derivative = Dvector_Create();
  long i = 0;
  double delta_1, delta_2, delta_3, delta_4;
  double alpha_1, alpha_2, alpha_3, alpha_4;
  double v0,v1,v2,v3,v4;

  for(i = 0; i < size; i++) {
    /* First initialize values, though this is very suboptimal */
    v0 = y[i];
    if(i == 0) {
      delta_1 = x[1] - x[0]; v1 = y[1];
      delta_2 = x[2] - x[0]; v2 = y[2];
      delta_3 = x[3] - x[0]; v3 = y[3];
      delta_4 = x[4] - x[0]; v4 = y[4];
    } else if(i == 1) {
      delta_1 = x[0] - x[1]; v1 = y[0];
      delta_2 = x[2] - x[1]; v2 = y[2];
      delta_3 = x[3] - x[1]; v3 = y[3];
      delta_4 = x[4] - x[1]; v4 = y[4];
    } else if(i == size - 2) {
      delta_1 = x[size-1] - x[size-2]; v1 = y[size-1];
      delta_2 = x[size-3] - x[size-2]; v2 = y[size-3];
      delta_3 = x[size-4] - x[size-2]; v3 = y[size-4];
      delta_4 = x[size-5] - x[size-2]; v4 = y[size-5];
    } else if(i == size - 1) {
      delta_1 = x[size-2] - x[size-1]; v1 = y[size-2];
      delta_2 = x[size-3] - x[size-1]; v2 = y[size-3];
      delta_3 = x[size-4] - x[size-1]; v3 = y[size-4];
      delta_4 = x[size-5] - x[size-1]; v4 = y[size-5];
    } else {
      delta_1 = x[i-2] - x[i]; v1 = y[i-2];
      delta_2 = x[i-1] - x[i]; v2 = y[i-1];
      delta_3 = x[i+2] - x[i]; v3 = y[i+2];
      delta_4 = x[i+1] - x[i]; v4 = y[i+1];
    }
    alpha_1 = -2 * (delta_2*delta_3 + delta_2*delta_4 + delta_3*delta_4)/
      (delta_1 * (delta_2 - delta_1) * (delta_3 - delta_1) 
       * (delta_4 - delta_1));
    alpha_2 = -2 * (delta_1*delta_3 + delta_1*delta_4 + delta_3*delta_4)/
      (delta_2 * (delta_1 - delta_2) * (delta_3 - delta_2) 
       * (delta_4 - delta_2));
    alpha_3 = -2 * (delta_2*delta_1 + delta_2*delta_4 + delta_1*delta_4)/
      (delta_3 * (delta_1 - delta_3) * (delta_2 - delta_3) 
       * (delta_4 - delta_3));
    alpha_4 = -2 * (delta_2*delta_3 + delta_2*delta_1 + delta_3*delta_1)/
      (delta_4 * (delta_1 - delta_4) * (delta_2 - delta_4) 
       * (delta_3 - delta_4));
    Dvector_Push_Double(derivative,
			-(alpha_1 + alpha_2 + alpha_3 + alpha_4) * v0 +
			alpha_1 * v1 + alpha_2 * v2 + 
			alpha_3 * v3 + alpha_4 * v4);
  }
  return Function_Create(get_x_vector(self), derivative);
}

/*
  Returns the number of points inside the function.
*/
static VALUE function_size(VALUE self)
{
  long size = function_sanity_check(self);
  return LONG2NUM(size);
}

/* 
   Fuzzy substraction of two curves. Substracts the Y values of _op_ to
   the current Function, by making sure that the Y value substracted to
   a given point corresponds to the closest X_ value of the point in _op_.
   This function somehow assumes that the data is reasonably organised,
   and will never go backwards to find a matching X value in _op_.

   In any case, you really should consider using split_monotonic on it first.
 */

static VALUE function_fuzzy_substract(VALUE self, VALUE op)
{
  long ss = function_sanity_check(self);
  const double *xs = Dvector_Data_for_Read(get_x_vector(self),NULL);
  double *ys = Dvector_Data_for_Write(get_y_vector(self),NULL);
  long so = function_sanity_check(op);
  const double *xo = Dvector_Data_for_Read(get_x_vector(op),NULL);
  const double *yo = Dvector_Data_for_Read(get_y_vector(op),NULL);
  long i,j = 0;
  double diff;
  double fuzz = 0; 		/* The actual sum of the terms */
  
  for(i = 0; i < ss; i++) 
    {
      /* We first look for the closest point */
      diff = fabs(xs[i] - xo[j]);
      while((j < (so - 1)) && (fabs(xs[i] - xo[j+1]) <  diff))
	diff = fabs(xs[i] - xo[++j]);
      fuzz += diff;
      ys[i] -= yo[j];
    }
  return rb_float_new(fuzz);
}

/* 
  call-seq:
    f.bound_values(xmin, xmax, ymin, ymax)

   This function browses the points inside the Function and stores in
   the resulting new function only points which are within boundaries,
   and the points just next to them (so the general direction on the sides
   looks fine).

   Make sure _xmin_ < _xmax_ and _ymin_ < _ymax_, else you simply won't
   get any output.
 */
static VALUE function_bound_values(VALUE self, 
				   VALUE vxmin, VALUE vxmax,
				   VALUE vymin, VALUE vymax)
{
  long ss = function_sanity_check(self);
  const double *xs = Dvector_Data_for_Read(get_x_vector(self),NULL);
  const double *ys = Dvector_Data_for_Read(get_y_vector(self),NULL);
  double xmin = NUM2DBL(vxmin);
  double xmax = NUM2DBL(vxmax);
  double ymin = NUM2DBL(vymin);
  double ymax = NUM2DBL(vymax);

  /* Now, two dvectors for writing: */
  VALUE x_out = rb_funcall(cDvector, idNew, 0);
  VALUE y_out = rb_funcall(cDvector, idNew, 0);

  /* No forward computation of the size of the targets, meaning
     memory allocation penalty.
  */
  
  int last_point_in = 0; 	/* Whether the last point was in */
  long i;
  for(i = 0; i < ss; i++) {
    double x = xs[i];
    double y = ys[i];
    if( (xmin <= x) && (xmax >= x) && (ymin <= y) && (ymax >= y)) {
      if(! last_point_in) {
	last_point_in = 1;
	if(i) {			/* Not for the first element */
	  Dvector_Push_Double(x_out, xs[i-1]);
	  Dvector_Push_Double(y_out, ys[i-1]);
	}
      }
      Dvector_Push_Double(x_out, x);
      Dvector_Push_Double(y_out, y);
    }
    else {			/* Outside boundaries */
      if(last_point_in) {
	last_point_in = 0;
	Dvector_Push_Double(x_out, x);
	Dvector_Push_Double(y_out, y);
      }
    }
  }
  return Function_Create(x_out, y_out);
}

/* Reverses the function. Equivalent to doing 

   x.reverse!
   y.reverse!
  
  excepted that it is faster (though not *much* faster).
*/
static VALUE function_reverse(VALUE self)
{
  long len = function_sanity_check(self);
  double *xs = Dvector_Data_for_Write(get_x_vector(self),NULL);
  double *ys = Dvector_Data_for_Write(get_y_vector(self),NULL);
  
  double *xe = xs+len-1;
  double *ye = ys+len-1;
  double tmp;
  long i;
  for(i = 0; i < len/2; i++, xs++, ys++, xe--, ye--) {
    tmp = *xe; *xe = *xs; *xs = tmp;
    tmp = *ye; *ye = *ys; *ys = tmp;
  }
  return self;
}

/* Computes the linear regression of the dataset. */
static void reglin(const double *x, const double *y, long nb, 
		   double *a, double *b)
{
  double sx = 0;
  double sy = 0;
  double sxx = 0;
  double sxy = 0;
  long i = 0;
  double det;
  for(i = 0; i < nb; i++, x++, y++) {
    sx += *x;
    sy += *y;
    sxx += *x * *x;
    sxy += *x * *y;
  }
  det = nb*sxx - sx*sx;
  if(det == 0) {
    *a = 0;			/* Whichever, we only have one point */
    *b = sy/nb;
  }
  else {
    *a = (nb *sxy - sx*sy)/det; 
    *b = (sxx * sy - sx * sxy)/(det);
  }
}


/* 
   Performs a linear regression of the Function; returns the pair
    [ a, b]
   where f(x) = a*x + b

   if the optional arguments _first_ and _last_ are provided, they
   represent the indices of the first and last elements.
 */
static VALUE function_reglin(int argc, VALUE *argv, VALUE self)
{
  long len = function_sanity_check(self);
  const double *x = Dvector_Data_for_Read(get_x_vector(self),NULL);
  const double *y = Dvector_Data_for_Read(get_y_vector(self),NULL);
  VALUE ret = rb_funcall(cDvector, idNew, 1, INT2NUM(2));
  double * dat = Dvector_Data_for_Write(ret, NULL);
  long nb;
  if(argc == 2) {
    long f = NUM2LONG(argv[0]);
    long l = NUM2LONG(argv[1]);
    if(f < 0)
      f = len + f;
    if(l < 0)
      l = len + l;
    x += f;
    y += f;
    nb = l - f;
  }
  else if(argc == 0) {
    nb = len;
  }
  else {
    rb_raise(rb_eArgError, "reglin should have 0 or 2 parameters");
  }
  reglin(x,y,nb,dat,dat+1);
  return ret;
}


/* Simply returns the sign */
static int signof(double x)
{
  if(x > 0)
    return 1;
  else if(x < 0)
    return -1;
  else
    return 0;
}

/* 
   Returns a "smoothed" value, according to the algorithm implented
   for "smooth" markers in Soas. See DOI:
   10.1016/j.bioelechem.2009.02.010
   
   Basically, we start at a given range, and narrow the range until
   the number of consecutive residuals of the same sign is lower than
   a quarter of the interval.

   It works

*/
double smooth_pick(const double *x, const double *y, 
		   long nb, long idx, long range)
{
  long left, right,i,nb_same_sign;
  double a,b;
  int last_sign;
  do {
    left = idx - range/2;
    if(left < 0) 
      left = 0;
    right = idx + range/2;
    if(right > nb)
      right = nb;
    reglin(x+left, y+left, right-left,&a,&b);
    if(range == 6)
      break; 			/* We stop here */
    last_sign = 0;
    for(i = left; i < right; i++) {
      double residual = y[i] - a * x[i] - b;
      if(! last_sign)
	last_sign = signof(residual);
      else if(last_sign == signof(residual))
	nb_same_sign ++;
      else {
	nb_same_sign = 1;
	last_sign = signof(residual);
      }
    }
    if(nb_same_sign * 4 <= right - left)
      break;
    range -= (nb_same_sign * 4 -range)/2 + 2;
    if(range < 6)
      range = 6;
  } while(1);
  /* Now, we have a and b for the last range measured. */
  return a*x[idx] + b;
}

/* 
   Attempts to pick a smooth value for a point, according to the
   algorithm implented for "smooth" markers in Soas. See DOI:
   10.1016/j.bioelechem.2009.02.010

   Warning: be wary of this function as it will return a correct
   value only for rather noisy data !
 */
static VALUE function_smooth_pick(int argc, VALUE *argv, VALUE self)
{
  long len = function_sanity_check(self);
  const double *x = Dvector_Data_for_Read(get_x_vector(self),NULL);
  const double *y = Dvector_Data_for_Read(get_y_vector(self),NULL);
  long idx;
  long range;
  switch(argc) {
  case 2:
    range = NUM2LONG(argv[1]);
    break;
  case 1:
    range = len > 500 ? 50 : len/10;
    break;
  default:
    rb_raise(rb_eArgError, "smooth_a=t should have 1 or 2 parameters");
  }
  idx = NUM2LONG(argv[0]);
  if(idx < 0)
    idx = len + idx;
  return rb_float_new(smooth_pick(x,y,len,idx,range));
}

/* 
   Computes the convolution of the kernel with the dataset; the
   overall result is scaled
 */
static double norm_convolve(const double *y, long len, long idx,
			    const double * kernel, long klen, long kmid)
{
  double ret = 0;
  long ki,yi;
  double norm = 0;
  yi = idx - kmid;
  /* We ensure we don't go */
  if(yi < 0) {
    ki -= yi;
    yi = 0;
  }
  for(; ki < klen && yi < len; yi++, ki++) {
    norm += kernel[ki];
    ret += kernel[ki] * y[yi];
  }
  return ret/norm;
}

/*
  This functions tries to approximate the given data using a spline.

  The algorithm is the following:
  * one starts with 3 points: 2 on the sides and one at the middle
  * then, we pick an interval between the points where the sum of the
    square of the residuals is the greatest, and place a point there.
  * then, we repeat until we reach a maximum number of points (_nbmax_)

  Point positions are averaged over _nbavg_ using a gaussian-like
  filter.

  Interpolation is returned into the _xi_, _yi_ and _y2i_ vectors

  TODO: try to place the points more in the middle ? (provide a factor
  governing this)
  
  TODO: use moments to decide of the precise position of the
  points ?

*/
static void internal_spline_approximation(const double *x, const double *y,
					  long len, 
					  double *xi, double *yi, 
					  double *y2i,
					  long nbmax,
					  long nbavg, 
					  double * target) 
{
  double left_slope;		/* Derivative on the left */
  double right_slope;		/* Same on the right */
  
  /* The gaussian kernel for the average */
  double kernel[nbavg];
  
  /* The indices of the point where the residuals are maximal */
  long max_res_idx[nbmax-1];

  /* The indices of the corner points*/
  long indices[nbmax];

  long i;
  long cur_size = 3;
  double tmp,tmp2;
  /* Initialization of the kernel */
  long mid = nbavg/2;		/* Middle of the kernel */
  for(i = 0,tmp=0; i < nbavg; i++) {
    tmp = (3.2 * (i - nbavg/2))/nbavg; /* Gives about 7% left on the
					  side elements */
    tmp = exp(-tmp*tmp);
    kernel[i] = tmp;
  }
  
  /* Left side */
  xi[0] = x[0];
  reglin(x,y, mid+1, &left_slope, &tmp2);
  yi[0] = left_slope * x[0] + tmp2;
  indices[0] = 0;

  /* Middle */
  xi[1] = x[len/2];
  yi[1] = norm_convolve(y, len, len/2, kernel, nbavg, mid);
  indices[1] = len/2;

  /* Right */
  xi[2] = x[len-1];
  reglin(x+(len-(mid+2)),y + (len-(mid+2)), mid+1, &right_slope, &tmp2);
  yi[2] = right_slope * x[len-1] + tmp2;
  indices[2] = len - 1;

  do {
    long cur_seg;
    long max_res_seg = 0;	/* The segment where the residuals are
				   the greatest */
    double max_res = 0;

    long max_deviation_seg = 0; /* The segment where the deviation
				   (square of the average) is the
				   greatest */
    double max_deviation = 0;

    long chosen_seg;	      /* The segment in which we'll add a point */
    /* Compute interpolation */
    function_fill_second_derivatives(cur_size, xi, yi, y2i, 
				     left_slope, right_slope);


    /* We stop here if we have reached the max number and we're not
       interested in Y values */
    if(cur_size >= nbmax && !target)
      break;

    /* Now we compute the residuals  */
    for(cur_seg = 0; cur_seg < cur_size - 1; cur_seg++) {
      double residuals = 0;
      double a,b,int_y,delta,h = xi[cur_seg+1] - xi[cur_seg];
      double imr = 0;		/* Internal max residuals */
      double deviation = 0;
      /* printf("seg: %ld/%ld indices %ld -- %ld\n", cur_seg, cur_size-1,  */
      /* 	     indices[cur_seg], indices[cur_seg+1]); */
      for(i = indices[cur_seg] + 1; i < indices[cur_seg+1]; i++) {
	a = (xi[cur_seg+1] - x[i])/h;
	b = (x[i] - xi[cur_seg])/h;
	int_y = a * yi[cur_seg] + b * yi[cur_seg + 1] 
	  + ((a*a*a - a) * y2i[cur_seg] +
	     (b*b*b - b) * y2i[cur_seg + 1]) * 
	  (h * h)/6.0;
	if(target)		/* We set the value if applicable. */
	  target[i] = int_y;
	delta = int_y - y[i];
	deviation += delta;
	delta *= delta;
	residuals += delta;
	if(delta > imr) {
	  imr = delta;
	  max_res_idx[cur_seg] = i;
	}
      }
      if(max_res < residuals) {
	max_res = residuals;
	max_res_seg = cur_seg;
      }
      deviation *= deviation;
      if(deviation > max_deviation) {
	max_deviation_seg = cur_seg;
	max_deviation = deviation;
      }
      /* printf(" -> residuals %g\n", cur_seg, cur_size-1,  */
      /* 	     residuals); */
    }
    /* printf("-> max residuals at segment %d\n", max_res_seg); */

    if(cur_size >= nbmax)
      break;

    
    /* OK, so now we know in which segment the residuals are the
       greatest, and which point of this segment is holds the max
       residuals. So we just add a point there */
    chosen_seg = max_deviation_seg;

    /* We shift the positions */
    for(i = cur_size; i > chosen_seg + 1; i--) {
      xi[i] = xi[i-1];
      yi[i] = yi[i-1];
      y2i[i] = y2i[i-1];
      indices[i] = indices[i-1];
    }
    cur_size++;
    xi[chosen_seg + 1] = x[max_res_idx[chosen_seg]];
    yi[chosen_seg + 1] = norm_convolve(y, len, max_res_idx[chosen_seg], 
					kernel, nbavg, mid);
    indices[chosen_seg + 1] = max_res_idx[chosen_seg];
  } while(1);

  /* Now fill in the missing values of y, since we do not evaluate them */
  if(target) {
    for(i = 0; i < nbmax; i++)
      target[indices[i]] = yi[i];
  }
  
  
}

/* 
   Filters the Function through interpolation. _params_ holds a 
   hash with the following values:
   * ??

   It returns a hash.
*/
static VALUE function_spline_approximation(VALUE self, VALUE params)
{
  long len = function_sanity_check(self);
  const double *x = Dvector_Data_for_Read(get_x_vector(self),NULL);
  const double *y = Dvector_Data_for_Read(get_y_vector(self),NULL);
  VALUE xiret, yiret, y2iret, yintret,ret;
  double * xi, *yi, *y2i, *yint;
  long nbavg = 9;  
  long nbmax = 20;
  if(RTEST(rb_hash_aref(params, rb_str_new2("number"))))
    nbmax = NUM2LONG(rb_hash_aref(params, rb_str_new2("number")));
  if(RTEST(rb_hash_aref(params, rb_str_new2("average"))))
    nbavg = NUM2LONG(rb_hash_aref(params, rb_str_new2("average")));

  /* TODO: add checks that monotonic and growing. */
  
  xiret = rb_funcall(cDvector, idNew, 1, INT2NUM(nbmax)); 
  xi = Dvector_Data_for_Write(xiret, NULL);
  yiret = rb_funcall(cDvector, idNew, 1, INT2NUM(nbmax)); 
  yi = Dvector_Data_for_Write(yiret, NULL);
  y2iret = rb_funcall(cDvector, idNew, 1, INT2NUM(nbmax)); 
  y2i = Dvector_Data_for_Write(y2iret, NULL);
  yintret = rb_funcall(cDvector, idNew, 1, INT2NUM(len)); 
  yint = Dvector_Data_for_Write(yintret, NULL);

  internal_spline_approximation(x, y, len, xi, yi, y2i,
				nbmax, nbavg, yint);
  ret = rb_hash_new();
  rb_hash_aset(ret, rb_str_new2("xi"), xiret);
  rb_hash_aset(ret, rb_str_new2("yi"), yiret);
  rb_hash_aset(ret, rb_str_new2("y2i"), y2iret);
  rb_hash_aset(ret, rb_str_new2("y"), yintret);
  return ret;
}


/*
  Document-class: Dobjects::Function

  Function is a class that embeds two Dvectors, one for X data and one for Y 
  data. It provides 

  - facilities for sorting the X while keeping the Y matching, with #sort and
    Function.joint_sort;
  - to check if X data is sorted: #sorted?, #is_sorted;
  - interpolation, with #compute_spline, #compute_spline_data and #interpolate
  - some functions for data access : #x, #y, #point;
  - some utiliy functions: #split_monotonic, #strip_nan, #reverse!
  - data inspection: #min, #max;
  - some computational functions: #integrate, #primitive, #derivative,
    and now 4th-order accurate first and second derivatives: #diff_5p
    and #diff2_5p
  - utility for fuzzy operations, when the X values of two functions
    differ, but only slightly, of when points are missing: 
    #fuzzy_sub!
  - linear regression #reglin
  - a function to approximate data using a low-order spline:
    #spline_approximation

  And getting bigger (almost) everyday...
 */ 
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
  rb_define_method(cFunction, "reverse!", function_reverse, 0);
  rb_define_alias(cFunction,  "is_sorted", "sorted?");

  rb_define_singleton_method(cFunction, "joint_sort", function_joint_sort, 2);
  rb_define_method(cFunction, "sort", function_sort, 0);

  /* spline stuff :*/
  rb_define_method(cFunction, "compute_spline_data", 
		   function_compute_spline_data, 0);
  rb_define_method(cFunction, "compute_spline", 
		   function_compute_spline, 1);

  rb_define_method(cFunction, "interpolate", 
		   function_interpolate, 1);
  rb_define_method(cFunction, "make_interpolant", 
		   function_make_interpolant, 0);
  rb_define_method(cFunction, "spline_approximation", 
		   function_spline_approximation, 1);


  /* access to data */
  rb_define_method(cFunction, "point", function_point, 1);
  rb_define_method(cFunction, "[]", function_point, 1);
  rb_define_method(cFunction, "x", get_x_vector, 0);
  rb_define_method(cFunction, "y", get_y_vector, 0);


  rb_define_method(cFunction, "size", function_size, 0);
  rb_define_alias(cFunction,  "length", "size");

  /* Soas-like functions ;-) */
  rb_define_method(cFunction, "reglin", function_reglin, -1);
  rb_define_method(cFunction, "smooth_pick", function_smooth_pick, -1);

		   

  /* iterator */
  rb_define_method(cFunction, "each", 
		   function_each, 0);

  /* stripping of NaNs */
  rb_define_method(cFunction, "strip_nan", function_strip_nan, 0);

  /* split into subfunctions with given properties */
  rb_define_method(cFunction, "split_monotonic", function_split_monotonic, 0);
  rb_define_method(cFunction, "split_on_nan", function_split_on_nan, 1);

  /* integration between two integer boundaries */
  rb_define_method(cFunction, "integrate", function_integrate, -1);
  /* primitive */
  rb_define_method(cFunction, "primitive", function_primitive, 0);
  /* derivative */
  rb_define_method(cFunction, "derivative", function_derivative, 0);

  /* 5-points derivatives */
  rb_define_method(cFunction, "diff_5p", function_diff_5p, 0);
  rb_define_method(cFunction, "diff2_5p", function_diff2_5p, 0);

  /* distance to a point */
  rb_define_method(cFunction, "distance", function_distance, -1);

  /* Fuzzy operations */
  rb_define_method(cFunction, "fuzzy_sub!", 
		   function_fuzzy_substract, 1); /* Substraction */
  

  /* Boundary operations */
  rb_define_method(cFunction, "bound_values", 
		   function_bound_values, 4); /* Substraction */


  /* a few more methods better written in pure Ruby */
  rb_require("Dobjects/Function_extras.rb");

  /* now, we import the necessary symbols from Dvector */
  RB_IMPORT_SYMBOL(cDvector, Dvector_Data_for_Read);
  RB_IMPORT_SYMBOL(cDvector, Dvector_Data_for_Write);
  RB_IMPORT_SYMBOL(cDvector, Dvector_Data_Resize);
  RB_IMPORT_SYMBOL(cDvector, Dvector_Create);
  RB_IMPORT_SYMBOL(cDvector, Dvector_Push_Double);
}

IMPLEMENT_SYMBOL(Dvector_Data_for_Read);
IMPLEMENT_SYMBOL(Dvector_Data_for_Write);
IMPLEMENT_SYMBOL(Dvector_Data_Resize);
IMPLEMENT_SYMBOL(Dvector_Create);
IMPLEMENT_SYMBOL(Dvector_Push_Double);
