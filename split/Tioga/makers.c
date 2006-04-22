/* makers.c */
/*
   Copyright (C) 2005  Bill Paxton

   This file is part of Tioga.

   Tioga is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Library Public License as published
   by the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.

   Tioga is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with Tioga; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
*/

#include "figures.h"


/* Lines */

   void c_private_make_spline_interpolated_points(FM *p, VALUE Xvec, VALUE Yvec, VALUE Xvec_data, VALUE Yvec_data,
        int start_clamped, double start_slope, int end_clamped, double end_slope) {
      int i, n_pts_data;
      double *As, *Bs, *Cs, *Ds;
      long xlen, ylen, xdlen, ydlen;
      double *Xs = Dvector_Data_for_Write(Xvec, &xlen);
      double *Ys = Dvector_Data_for_Write(Yvec, &ylen);
      double *X_data = Dvector_Data_for_Read(Xvec_data, &xdlen);
      double *Y_data = Dvector_Data_for_Read(Yvec_data, &ydlen);
      if (Xs == NULL || Ys == NULL || X_data == NULL || Y_data == NULL || xdlen != ydlen) {
         rb_raise(rb_eArgError, "Sorry: bad args for make_curves");
      }
      if (xlen == 0) return;
      n_pts_data = xdlen;
      As = Y_data;
      Bs = (double *)ALLOC_N(double, n_pts_data);
      Cs = (double *)ALLOC_N(double, n_pts_data);
      Ds = (double *)ALLOC_N(double, n_pts_data);
      c_dvector_create_spline_interpolant(n_pts_data, X_data, Y_data,
         start_clamped, start_slope, end_clamped, end_slope, Bs, Cs, Ds);
      Ys = Dvector_Data_Resize(Yvec, xlen);
      for (i = 0; i < xlen; i++)
         Ys[i] = c_dvector_spline_interpolate(Xs[i], n_pts_data, X_data, As, Bs, Cs, Ds);
      free(Ds); free(Cs); free(Bs);
      USE_P
      }
      
   VALUE FM_private_make_spline_interpolated_points(VALUE fmkr, VALUE Xvec, VALUE Yvec, VALUE Xvec_data, VALUE Yvec_data,
        VALUE start_slope, VALUE end_slope) {
      FM *p = Get_FM(fmkr);
      bool start_clamped = (start_slope != Qnil), end_clamped = (end_slope != Qnil);
      double start=0, end=0;
      if (start_clamped) {
         start_slope = rb_Float(start_slope);
         start = NUM2DBL(start_slope);
      }
      if (end_clamped) {
         end_slope = rb_Float(end_slope);
         end = NUM2DBL(end_slope);
      }
      c_private_make_spline_interpolated_points(p, Xvec, Yvec, Xvec_data, Yvec_data,
         start_clamped, start, end_clamped, end);
      return fmkr;
   }

   void c_make_steps(FM *p, VALUE Xvec, VALUE Yvec, VALUE Xvec_data, VALUE Yvec_data,
        double xfirst, double yfirst, double xlast, double ylast){
      double xnext, xprev, x;
      int n_pts_to_add;
      int i, j, n, old_length, new_length;
      long xlen, ylen, xdlen, ydlen;
      double *Xs = Dvector_Data_for_Write(Xvec, &xlen);
      double *Ys = Dvector_Data_for_Write(Yvec, &ylen);
      double *X_data = Dvector_Data_for_Read(Xvec_data, &xdlen);
      double *Y_data = Dvector_Data_for_Read(Yvec_data, &ydlen);
      if (Xs == NULL || Ys == NULL || X_data == NULL || Y_data == NULL
            || xdlen != ydlen || xlen != ylen) {
         rb_raise(rb_eArgError, "Sorry: bad args for make_steps");
      }
      n = xdlen;
      n_pts_to_add = 2*(n+1);
      old_length = xlen;
      new_length = old_length + n_pts_to_add;
      Xs = Dvector_Data_Resize(Xvec, new_length);
      Ys = Dvector_Data_Resize(Yvec, new_length);
      for (i = 0, j = 0; i <= n; i++, j += 2) {
         xprev = (i==0)? xfirst : X_data[i-1];
         xnext = (i==n)? xlast : X_data[i];
         x = 0.5*(xprev + xnext);
         Xs[j+old_length] = Xs[j+1+old_length] = x;
         }
      Ys[0] = yfirst;
      for (i = 0, j = 1; i < n; i++, j += 2) {
         Ys[j+old_length] = Ys[j+1+old_length] = Y_data[i];
         }
      Ys[n_pts_to_add-1+old_length] = ylast;
      USE_P
      }
      
   VALUE FM_private_make_steps(VALUE fmkr, VALUE Xvec, VALUE Yvec, VALUE Xvec_data, VALUE Yvec_data,
        VALUE xfirst, VALUE yfirst, VALUE xlast, VALUE ylast) {
      FM *p = Get_FM(fmkr);
      xfirst = rb_Float(xfirst);
      yfirst = rb_Float(yfirst);
      xlast = rb_Float(xlast);
      ylast = rb_Float(ylast);
      c_make_steps(p, Xvec, Yvec, Xvec_data, Yvec_data,
         NUM2DBL(xfirst), NUM2DBL(yfirst), NUM2DBL(xlast), NUM2DBL(ylast));
      return fmkr;
   }

/*
CONREC: A Contouring Subroutine
written by Paul Bourke
see: http://astronomy.swin.edu.au/~pbourke/projection/conrec/

Copyright (c) 1996-1997 Nicholas Yue

This software is copyrighted by Nicholas Yue. This code is base on the work of
Paul D. Bourke CONREC.F routine

The authors hereby grant permission to use, copy, and distribute this
software and its documentation for any purpose, provided that existing
copyright notices are retained in all copies and that this notice is included
verbatim in any distributions. Additionally, the authors grant permission to
modify this software and its documentation for any purpose, provided that
such modifications are not distributed without the explicit consent of the
authors and that existing copyright notices are retained in all copies. Some
of the algorithms implemented by this software are patented, observe all
applicable patent law.

IN NO EVENT SHALL THE AUTHORS OR DISTRIBUTORS BE LIABLE TO ANY PARTY FOR
DIRECT, INDIRECT, SPECIAL, INCIDENTAL, OR CONSEQUENTIAL DAMAGES ARISING OUT
OF THE USE OF THIS SOFTWARE, ITS DOCUMENTATION, OR ANY DERIVATIVES THEREOF,
EVEN IF THE AUTHORS HAVE BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

THE AUTHORS AND DISTRIBUTORS SPECIFICALLY DISCLAIM ANY WARRANTIES, INCLUDING,
BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
PARTICULAR PURPOSE, AND NON-INFRINGEMENT.  THIS SOFTWARE IS PROVIDED ON AN
"AS IS" BASIS, AND THE AUTHORS AND DISTRIBUTORS HAVE NO OBLIGATION TO PROVIDE
MAINTENANCE, SUPPORT, UPDATES, ENHANCEMENTS, OR MODIFICATIONS.
*/

//=============================================================================
//
//     CONREC is a contouring subroutine for rectangularily spaced data.
//
//     It emits calls to a line drawing subroutine supplied by the user
//     which draws a contour map corresponding to real*4data on a randomly
//     spaced rectangular grid. The coordinates emitted are in the same
//     units given in the x() and y() arrays.
//
//     Any number of contour levels may be specified but they must be
//     in order of increasing value.
//
//     As this code is ported from FORTRAN-77, please be very careful of the
//     various indices like ilb,iub,jlb and jub, remeber that C/C++ indices
//     starts from zero (0)
//
//=============================================================================
#include <stdio.h>
#include <math.h>

#define xsect(p1,p2) (h[p2]*xh[p1]-h[p1]*xh[p2])/(h[p2]-h[p1])
#define ysect(p1,p2) (h[p2]*yh[p1]-h[p1]*yh[p2])/(h[p2]-h[p1])
#define min(x,y) (x<y?x:y)
#define max(x,y) (x>y?x:y)

#define PUSH_POINT(x,y) { \
   Dvector_Store_Double(dest_xs, num_pts, x); \
   Dvector_Store_Double(dest_ys, num_pts, y); \
   num_pts++; }

int conrec(double **d,
	   int ilb,
	   int iub,
	   int jlb,
	   int jub,
	   double *x,
	   double *y,
	   int nc,
	   double *z,
	   VALUE dest_xs,
	   VALUE dest_ys,
	   VALUE gaps,
	   double x_limit,
	   double y_limit)
// d               ! matrix of data to contour
// ilb,iub,jlb,jub ! index bounds of data matrix
// x               ! data matrix column coordinates
// y               ! data matrix row coordinates
// nc              ! number of contour levels
// z               ! contour levels in increasing order
{
int num_pts = 0;
double x_prev=0.0, y_prev=0.0;
  int m1,m2,m3,case_value;
  double dmin,dmax,x1=0.0,x2=0.0,y1=0.0,y2=0.0;
  register int i,j,k,m;
  double h[5];
  int sh[5];
  double xh[5],yh[5];
  //===========================================================================
  // The indexing of im and jm should be noted as it has to start from zero
  // unlike the fortran counter part
  //===========================================================================
  int im[4] = {0,1,1,0},jm[4]={0,0,1,1};
  //===========================================================================
  // Note that castab is arranged differently from the FORTRAN code because
  // Fortran and C/C++ arrays are transposes of each other, in this case
  // it is more tricky as castab is in 3 dimension
  //===========================================================================
  int castab[3][3][3] =
  {
    {
      {0,0,8},{0,2,5},{7,6,9}
    },
    {
      {0,3,4},{1,3,1},{4,3,0}
    },
    {
      {9,6,7},{5,2,0},{8,0,0}
    }
  };
  for (j=(jub-1);j>=jlb;j--) {
    for (i=ilb;i<=iub-1;i++) {
      double temp1,temp2;
      temp1 = min(d[i][j],d[i][j+1]);
      temp2 = min(d[i+1][j],d[i+1][j+1]);
      dmin = min(temp1,temp2);
      temp1 = max(d[i][j],d[i][j+1]);
      temp2 = max(d[i+1][j],d[i+1][j+1]);
      dmax = max(temp1,temp2);
      if (dmax>=z[0]&&dmin<=z[nc-1]) {
	for (k=0;k<nc;k++) {
	  if (z[k]>=dmin&&z[k]<=dmax) {
	    for (m=4;m>=0;m--) {
	      if (m>0) {
		//=============================================================
		// The indexing of im and jm should be noted as it has to
		// start from zero
		//=============================================================
		h[m] = d[i+im[m-1]][j+jm[m-1]]-z[k];
		xh[m] = x[i+im[m-1]];
		yh[m] = y[j+jm[m-1]];
	      } else {
		h[0] = 0.25*(h[1]+h[2]+h[3]+h[4]);
		xh[0]=0.5*(x[i]+x[i+1]);
		yh[0]=0.5*(y[j]+y[j+1]);
	      }
	      if (h[m]>0.0) {
		sh[m] = 1;
	      } else if (h[m]<0.0) {
		sh[m] = -1;
	      } else
		sh[m] = 0;
	    }
	    //=================================================================
	    //
	    // Note: at this stage the relative heights of the corners and the
	    // centre are in the h array, and the corresponding coordinates are
	    // in the xh and yh arrays. The centre of the box is indexed by 0
	    // and the 4 corners by 1 to 4 as shown below.
	    // Each triangle is then indexed by the parameter m, and the 3
	    // vertices of each triangle are indexed by parameters m1,m2,and
	    // m3.
	    // It is assumed that the centre of the box is always vertex 2
	    // though this isimportant only when all 3 vertices lie exactly on
	    // the same contour level, in which case only the side of the box
	    // is drawn.
	    //
	    //
	    //      vertex 4 +-------------------+ vertex 3
	    //               | \               / |
	    //               |   \    m-3    /   |
	    //               |     \       /     |
	    //               |       \   /       |
	    //               |  m=2    X   m=2   |       the centre is vertex 0
	    //               |       /   \       |
	    //               |     /       \     |
	    //               |   /    m=1    \   |
	    //               | /               \ |
	    //      vertex 1 +-------------------+ vertex 2
	    //
	    //
	    //
	    //               Scan each triangle in the box
	    //
	    //=================================================================
	    for (m=1;m<=4;m++) {
	      m1 = m;
	      m2 = 0;
	      if (m!=4)
		m3 = m+1;
	      else
		m3 = 1;
	      case_value = castab[sh[m1]+1][sh[m2]+1][sh[m3]+1];
	      if (case_value!=0) {
		switch (case_value) {
		  //===========================================================
		  //     Case 1 - Line between vertices 1 and 2
		  //===========================================================
		case 1:
		  x1=xh[m1];
		  y1=yh[m1];
		  x2=xh[m2];
		  y2=yh[m2];
		  break;
		  //===========================================================
		  //     Case 2 - Line between vertices 2 and 3
		  //===========================================================
		case 2:
		  x1=xh[m2];
		  y1=yh[m2];
		  x2=xh[m3];
		  y2=yh[m3];
		  break;
		  //===========================================================
		  //     Case 3 - Line between vertices 3 and 1
		  //===========================================================
		case 3:
		  x1=xh[m3];
		  y1=yh[m3];
		  x2=xh[m1];
		  y2=yh[m1];
		  break;
		  //===========================================================
		  //     Case 4 - Line between vertex 1 and side 2-3
		  //===========================================================
		case 4:
		  x1=xh[m1];
		  y1=yh[m1];
		  x2=xsect(m2,m3);
		  y2=ysect(m2,m3);
		  break;
		  //===========================================================
		  //     Case 5 - Line between vertex 2 and side 3-1
		  //===========================================================
		case 5:
		  x1=xh[m2];
		  y1=yh[m2];
		  x2=xsect(m3,m1);
		  y2=ysect(m3,m1);
		  break;
		  //===========================================================
		  //     Case 6 - Line between vertex 3 and side 1-2
		  //===========================================================
		case 6:
		  x1=xh[m3];
		  y1=yh[m3];
		  x2=xsect(m1,m2);
		  y2=ysect(m1,m2);
		  break;
		  //===========================================================
		  //     Case 7 - Line between sides 1-2 and 2-3
		  //===========================================================
		case 7:
		  x1=xsect(m1,m2);
		  y1=ysect(m1,m2);
		  x2=xsect(m2,m3);
		  y2=ysect(m2,m3);
		  break;
		  //===========================================================
		  //     Case 8 - Line between sides 2-3 and 3-1
		  //===========================================================
		case 8:
		  x1=xsect(m2,m3);
		  y1=ysect(m2,m3);
		  x2=xsect(m3,m1);
		  y2=ysect(m3,m1);
		  break;
		  //===========================================================
		  //     Case 9 - Line between sides 3-1 and 1-2
		  //===========================================================
		case 9:
		  x1=xsect(m3,m1);
		  y1=ysect(m3,m1);
		  x2=xsect(m1,m2);
		  y2=ysect(m1,m2);
		  break;
		default:
		  break;
		}
		double dx = x1 - x_prev, dy = y1 - y_prev;
		if (dx < 0) dx = -dx; if (dy < 0) dy = -dy;
		if (num_pts == 0 || dx > x_limit || dy > y_limit) {
         if (num_pts > 0) rb_ary_push(gaps, INT2FIX(num_pts));
         PUSH_POINT(x1,y1);
		}
		PUSH_POINT(x2,y2);
		x_prev = x2; y_prev = y2;
	      }
	    }
	  }
	}
      }
    }
  }
  return 0;
}

/* end of conrec */

   void c_make_contour(FM *p, VALUE dest_xs, VALUE dest_ys, VALUE gaps,
         VALUE xs, VALUE ys,  VALUE zs_data, double z_level) {
      long xlen, ylen, num_columns, num_rows;
      double *x_coords = Dvector_Data_for_Read(xs, &xlen);
      double *y_coords = Dvector_Data_for_Read(ys, &ylen);
      double **zs = Dtable_Ptr(zs_data, &num_columns, &num_rows);
      if (x_coords == NULL || gaps == Qnil || zs == NULL || y_coords == NULL) {
         rb_raise(rb_eArgError, "Sorry: bad args for make_contour.  Need to provide xs, ys, gaps, and zs.");
      }
      if (xlen != num_columns || ylen != num_rows) {
         rb_raise(rb_eArgError, "Sorry: bad args for make_contour.  Needs xs.size == num columns and ys.size == num rows.");
      }
      double x_limit, y_limit;
      x_limit = 0.001*(x_coords[xlen-1] - x_coords[0])/xlen;
      if (x_limit < 0) x_limit = -x_limit;
      y_limit = 0.001*(y_coords[ylen-1] - y_coords[0])/ylen;
      if (y_limit < 0) y_limit = -y_limit;
      // NOTE: conrec data is TRANSPOSE of our data, so we switch x's and y's in the call
      conrec(zs, 0, num_rows-1, 0, num_columns-1, y_coords, x_coords, 1, &z_level, dest_ys, dest_xs, gaps, y_limit, x_limit);
   }
   
   VALUE FM_private_make_contour(VALUE fmkr,
         VALUE dest_xs, VALUE dest_ys, VALUE gaps, // these Dvectors get the results
         VALUE xs, VALUE ys, // data x coordinates and y coordinates
         VALUE zs, VALUE z_level // the Dtable of values and the desired contour level
         ) {
      FM *p = Get_FM(fmkr);
      z_level = rb_Float(z_level);
      c_make_contour(p, dest_xs, dest_ys, gaps, xs, ys, zs, NUM2DBL(z_level));
      return fmkr;
   }


