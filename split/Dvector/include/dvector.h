/* Dvector.h */
/*
   Copyright (C) 2005  Bill Paxton

   Dvector is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Library Public License as published
   by the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.

   Dvector is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with Dvector; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
*/

#ifndef __Dvector_H__
#define __Dvector_H__

/* this file has been heavily modified by Vincent Fourmond to take care
   of the 'RCR330' scheme of exporting symbols 
*/

#include <symbols.h>
#include <stdbool.h>

/*======================================================================*/

/* functions for handling Dvectors: */

DECLARE_SYMBOL(bool, isa_Dvector, (VALUE obj));
 /* returns true if the obj is a Dvector */
DECLARE_SYMBOL(long, len_Dvector, (VALUE dvector));
 /* returns length of the dvector */
DECLARE_SYMBOL(double, access_Dvector, (VALUE dvector, long idx));
 /* returns the value of entry idx in dvector */

DECLARE_SYMBOL(double *, Dvector_Data_for_Read, (VALUE dvector, long *len_ptr));
 /* returns pointer to the dvector's data (which may be shared) */
DECLARE_SYMBOL(double *, Dvector_Data_Copy, (VALUE dvector, long *len_ptr)); 
/* like Dvector_Data_for_Read, but returns pointer to a copy of the data */
DECLARE_SYMBOL(double *, Dvector_Data_for_Write, 
		(VALUE dvector, long *len_ptr));
DECLARE_SYMBOL(double *, Dvector_Data_Resize, (VALUE dvector, long new_len));
DECLARE_SYMBOL(double *, Dvector_Data_Replace,
	       (VALUE dvector, long len, double *data)); 
/* copies the data into the dvector */
DECLARE_SYMBOL(VALUE, Dvector_Create, (void));
DECLARE_SYMBOL(void, Dvector_Store_Double, (VALUE ary, long idx, double val));
/* pushes one element onto the vector */
DECLARE_SYMBOL(void, Dvector_Push_Double, (VALUE ary, double val));


/* functions for interpolation */
DECLARE_SYMBOL(void, c_dvector_create_spline_interpolant,
	       (int n_pts_data, double *Xs, double *Ys,
		bool start_clamped, double start_slope, 
		bool end_clamped, double end_slope,
		double *As, double *Bs, double *Cs));
DECLARE_SYMBOL(double, c_dvector_spline_interpolate,
	       (double x, int n_pts_data, double *Xs, double *Ys, 
		double *Bs, double *Cs, double *Ds));

DECLARE_SYMBOL(void, c_dvector_create_pm_cubic_interpolant,
	       (int n_pts_data, double *Xs, double *Ys,
		double *As, double *Bs, double *Cs));
DECLARE_SYMBOL(double, c_dvector_pm_cubic_interpolate,
	       (double x, int n_pts_data, double *Xs, double *Ys, 
		double *Bs, double *Cs, double *Ds));

DECLARE_SYMBOL(double, c_dvector_linear_interpolate,
	       (int num_pts, double *xs, double *ys, double x));
#endif   /* __Dvector_H__ */

