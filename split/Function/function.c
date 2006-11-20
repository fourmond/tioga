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

#include <namespace.h>
#include <ruby.h>

#include "dvector.h"

#include <math.h>
/* compiler-dependent definitions, such as is_okay_number */
#include <defs.h>

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
PRIVATE void joint_quicksort(double *const x_values, double * const y_values,
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
  Splits the function into monotonic sub-functions.
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
      if(direction * (x[i] - last_x) < 0) 
	{
	  /* we need to add a new set of Dvectors */
	  f = Function_Create(cur_x, cur_y);
	  rb_ary_push(ret, f);
	  cur_x = Dvector_Create();
	  cur_y = Dvector_Create();
	  Dvector_Push_Double(cur_x, x[i-1]);
	  Dvector_Push_Double(cur_y, y[i-1]);
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
   Computes the primitive of the Function and returns it as a new Function.
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
  Returns the number of points inside the function.
*/
static VALUE function_size(VALUE self)
{
  long size = function_sanity_check(self);
  return LONG2NUM(size);
}

/*
  Document-class: Dobjects::Function

  Function is a class that embeds two Dvectors, one for X data and one for Y 
  data. It provides 

  - facilities for sorting the X while keeping the Y matching, with #sort and
    Function.joint_sort;
  - to check if X data is sorted: #sorted?, #is_sorted;
  - interpolation, with #compute_spline, #compute_spline_data and #interpolate;
  - some functions for data access : #x, #y, #point;
  - some utiliy functions: #split_monotonic, #strip_nan;
  - data inspection: #min, #max;
  - some computationnal functions: #integrate, #primitive.

  And getting bigger everyday...
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

  /* access to data */
  rb_define_method(cFunction, "point", function_point, 1);
  rb_define_method(cFunction, "x", get_x_vector, 0);
  rb_define_method(cFunction, "y", get_y_vector, 0);


  rb_define_method(cFunction, "size", function_size, 0);
  rb_define_alias(cFunction,  "length", "size");
		   

  /* iterator */
  rb_define_method(cFunction, "each", 
		   function_each, 0);

  /* stripping of NaNs */
  rb_define_method(cFunction, "strip_nan", function_strip_nan, 0);

  /* split into monotonic subfunctions */
  rb_define_method(cFunction, "split_monotonic", function_split_monotonic, 0);

  /* integration between two integer boundaries */
  rb_define_method(cFunction, "integrate", function_integrate, -1);
  /* primitive */
  rb_define_method(cFunction, "primitive", function_primitive, 0);

  /* distance to a point */
  rb_define_method(cFunction, "distance", function_distance, -1);


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
