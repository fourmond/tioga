/* makers.c */
/*
   Copyright (C) 2005  Bill Paxton
   Copyright (C) 2007  Taro Sato

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
#include "generic.h"


/* Lines */

/*
 * this is copied from dvector so that makers.c won't depend on dvectors.
 */
static void
create_spline_interpolant(int n_pts_data, double *Xs, double *Ys,
                          bool start_clamped, double start_slope,
                          bool end_clamped, double end_slope,
                          double *As, double *Bs, double *Cs)

{
  double *Hs = ALLOC_N_double(n_pts_data);
  double *alphas = ALLOC_N_double(n_pts_data);
  double *Ls = ALLOC_N_double(n_pts_data);
  double *mu_s = ALLOC_N_double(n_pts_data);
  double *Zs = ALLOC_N_double(n_pts_data);
  int n = n_pts_data-1, i, j;
  for (i = 0; i < n; i++)
    Hs[i] = Xs[i+1] - Xs[i];
  if (start_clamped) alphas[0] = 3.0*(Ys[1]-Ys[0])/Hs[0] - 3.0*start_slope;
  if (end_clamped) alphas[n] = 3.0*end_slope - 3.0*(Ys[n]-Ys[n-1])/Hs[n-1];
  for (i = 1; i < n; i++)
    alphas[i] = (3.0 * (Ys[i+1]*Hs[i-1] - Ys[i]*(Xs[i+1]-Xs[i-1])
                        + Ys[i-1]*Hs[i]) / (Hs[i-1]*Hs[i]));
  if (start_clamped) {
    Ls[0] = 2.0*Hs[0]; mu_s[0] = 0.5; Zs[0] = alphas[0]/Ls[0];
  }
  else {
    Ls[0] = 1.0; mu_s[0] = 0.0; Zs[0] = 0.0;
  }
  for (i = 1; i < n; i++) {
    Ls[i] = 2.0*(Xs[i+1]-Xs[i-1]) - Hs[i-1]*mu_s[i-1];
    mu_s[i] = Hs[i]/Ls[i];
    Zs[i] = (alphas[i] - Hs[i-1]*Zs[i-1])/Ls[i];
  }
  if (end_clamped) { 
    Ls[n] = Hs[n-1]*(2.0-mu_s[n-1]);
    Bs[n] = Zs[n] = (alphas[n]-Hs[n-1]*Zs[n-1])/Ls[n];
  }
  else {
    Ls[n] = 1.0; Zs[n] = 0.0; Bs[n] = 0.0;
  }
  for (j = n-1; j >= 0; j--) {
    Bs[j] = Zs[j] - mu_s[j]*Bs[j+1];
    Cs[j] = (Ys[j+1]-Ys[j])/Hs[j] - Hs[j]*(Bs[j+1]+2.0*Bs[j])/3.0;
    As[j] = (Bs[j+1]-Bs[j])/(3.0*Hs[j]);
  }
  free(Zs); free(mu_s); free(Ls); free(alphas); free(Hs);
}


/*
 * this is copied from dvector so that makers.c won't depend on dvectors.
 */
static double
spline_interpolate(double x, int n_pts_data, 
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


OBJ_PTR
c_private_make_spline_interpolated_points(OBJ_PTR fmkr, FM *p, OBJ_PTR Xvec,
                                          OBJ_PTR Xvec_data, OBJ_PTR Yvec_data,
                                          OBJ_PTR start_slope,
                                          OBJ_PTR end_slope, int *ierr)
{
  bool start_clamped = (start_slope != OBJ_NIL);
  bool end_clamped = (end_slope != OBJ_NIL);
  long xlen;
  double start=0, end=0, *Ys;
  double *Xs = Vector_Data_for_Read(Xvec, &xlen, ierr);
  if (*ierr != 0) RETURN_NIL;
  OBJ_PTR Yvec;

  if (start_clamped) start = Number_to_double(start_slope, ierr);
  if (end_clamped) end = Number_to_double(end_slope, ierr);
  if (*ierr != 0) RETURN_NIL;

  Ys = ALLOC_N_double(xlen); // Ys are same length as Xs

  int i, n_pts_data;
  double *As, *Bs, *Cs, *Ds;
  long xdlen, ydlen;
  double *X_data = Vector_Data_for_Read(Xvec_data, &xdlen, ierr);
  if (*ierr != 0) RETURN_NIL;
  double *Y_data = Vector_Data_for_Read(Yvec_data, &ydlen, ierr);
  if (*ierr != 0) RETURN_NIL;
  if (Xs == NULL || Ys == NULL || X_data == NULL || Y_data == NULL
      || xdlen != ydlen) {
    RAISE_ERROR("Sorry: bad args",ierr);
    RETURN_NIL;
  }
  if (xlen == 0) RETURN_NIL;
  n_pts_data = xdlen;
  As = Y_data;
  Bs = ALLOC_N_double(n_pts_data);
  Cs = ALLOC_N_double(n_pts_data);
  Ds = ALLOC_N_double(n_pts_data);
  create_spline_interpolant(n_pts_data, X_data, Y_data,
                            start_clamped, start, end_clamped, end,
                            Bs, Cs, Ds);
  for (i = 0; i < xlen; i++)
    Ys[i] = spline_interpolate(Xs[i], n_pts_data, X_data, As, Bs, Cs, Ds);
  free(Ds); free(Cs); free(Bs);
  Yvec = Vector_New(xlen, Ys);
  free(Ys);
  return Yvec;
}


/*
 * Make points xs and ys to define a step function.  x_data and y_data
 * are arrays from which the step functions are generated.  (xfirst,
 * yfirst) and (xlast, ylast) are extra data points to fix the first
 * and last steps.  The x_data plus xfirst and xlast determine the
 * widths of the steps.  The y_data plus yfirst and ylast determine
 * the height of the steps.  For CENTERED justification, the steps
 * occur at locations midway between the given x locations.  For
 * LEFT_JUSTIFIED, (x_data[i], y_data[i]) and (x_data[i], y_data[i+1])
 * specifies where steps occurs.  For RIGHT_JUSTIFIED, (x_data[i],
 * y_data[i]) and (x_data[i], y_data[i-1]) specifies where steps
 * occurs.
 *
 * Aug 24, 2007:
 *
 *   TS added 'justification' to control the justification of steps.
 *   The use of [xy]first and [xy]last might need improvement.
 */
static void
c_make_steps(FM *p,
             long *xsteps_len_ptr, double **xs_ptr, 
             long *ysteps_len_ptr, double **ys_ptr, 
             OBJ_PTR xvec_data, OBJ_PTR yvec_data,
             double xfirst, double yfirst, double xlast, double ylast,
             int justification, int *ierr)
{
  double xnext, xprev, x;
  long i, j, length, xdlen, ydlen;
  double *xs = NULL, *ys = NULL;
  double *x_data = Vector_Data_for_Read(xvec_data, &xdlen, ierr);
  if (*ierr != 0) return;
  double *y_data = Vector_Data_for_Read(yvec_data, &ydlen, ierr);
  if (*ierr != 0) return;
  if (x_data == NULL || y_data == NULL || xdlen != ydlen) {
    RAISE_ERROR("Sorry: bad args for make_steps", ierr);
    return;
  }

  // allocate memory for arrays to be returned
  length = 2 * (xdlen + 1) + ((justification != CENTERED) ? 1 : 0);

  *xsteps_len_ptr = length;
  xs = ALLOC_N_double(length);
  *xs_ptr = xs;

  *ysteps_len_ptr = length;
  ys = ALLOC_N_double(length);
  *ys_ptr = ys;

  // fill the arrays
  switch (justification) {
  case CENTERED:
    for (i = 0, j = 0; i <= xdlen; ++i, j += 2) {
      xprev = (i == 0) ? xfirst : x_data[i - 1];
      xnext = (i == xdlen) ? xlast : x_data[i];
      x = 0.5 * (xprev + xnext);
      xs[j] = xs[j + 1] = x;
    }
    ys[0] = yfirst;
    for (i = 0, j = 1; i < xdlen; ++i, j += 2) {
      ys[j] = ys[j + 1] = y_data[i];
    }
    ys[length - 1] = ylast;
    break;
  case LEFT_JUSTIFIED:
    xs[0] = xfirst;
    for (i = 0, j = 1; i <= xdlen; ++i, j += 2) {
      xs[j] = xs[j + 1] = (i == xdlen) ? xlast : x_data[i];
    }
    ys[0] = ys[1] = yfirst;
    for (i = 0, j = 2; i < xdlen; ++i, j += 2) {
      ys[j] = ys[j + 1] = y_data[i];
    }
    ys[length - 1] = ylast;
    break;
  case RIGHT_JUSTIFIED:
    for (i = 0, j = 0; i <= xdlen; ++i, j += 2) {
      xs[j] = xs[j + 1] = (i == 0) ? xfirst : x_data[i - 1];
    }
    xs[length - 1] = xlast;
    ys[0] = yfirst;
    for (i = 0, j = 1; i <= xdlen; ++i, j += 2) {
      ys[j] = ys[j + 1] = (i == xdlen) ? ylast : y_data[i];
    }
    break;
  default:
    RAISE_ERROR_i("Sorry: unsupported justification specified (%d)",
                  justification, ierr);
    return;
  }

  /* TS: I don't understand the use of the macro USE_P here, which
     translates to p = NULL; For now, I'll comment out. */
  //USE_P
}


OBJ_PTR
c_private_make_steps(OBJ_PTR fmkr, FM *p, OBJ_PTR xvec_data, OBJ_PTR yvec_data,
                     double xfirst, double yfirst, double xlast, double ylast,
		     int justification, int *ierr)
{
  OBJ_PTR xvec;
  OBJ_PTR yvec;
  OBJ_PTR pts_array;
  long xsteps_len = 0, ysteps_len = 0;
  double *xsteps_data = NULL, *ysteps_data = NULL;

  c_make_steps(p, &xsteps_len, &xsteps_data, &ysteps_len, &ysteps_data,
               xvec_data, yvec_data, xfirst, yfirst, xlast, ylast,
               justification, ierr);
  if (*ierr != 0) RETURN_NIL;

  xvec = Vector_New(xsteps_len, xsteps_data);
  yvec = Vector_New(ysteps_len, ysteps_data);
  free(xsteps_data);
  free(ysteps_data);

  pts_array = Array_New(2);
  Array_Store(pts_array, 0, xvec, ierr);
  if (*ierr != 0) RETURN_NIL;
  Array_Store(pts_array, 1, yvec, ierr);
  if (*ierr != 0) RETURN_NIL;
  return pts_array;
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
   if (*dest_len_ptr >= *dest_sz_ptr) { \
      *dest_sz_ptr += *dest_sz_ptr + 100; \
      REALLOC_double(dest_xs_ptr,*dest_sz_ptr); \
      REALLOC_double(dest_ys_ptr,*dest_sz_ptr); \
   } \
   (*dest_xs_ptr)[*dest_len_ptr] = x; \
   (*dest_ys_ptr)[*dest_len_ptr] = y; \
   (*dest_len_ptr)++; \
}

static int conrec(double **d,
                  int ilb,
                  int iub,
                  int jlb,
                  int jub,
                  double *x,
                  double *y,
                  int nc,
                  double *z,
                  long *dest_len_ptr,
                  double **dest_xs_ptr,
                  double **dest_ys_ptr,
                  long *dest_sz_ptr,
                  OBJ_PTR gaps,
                  double x_limit,
                  double y_limit,
                  int *ierr)
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
  //==========================================================================
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
                  if (num_pts > 0) {
                    Array_Push(gaps, Integer_New(num_pts), ierr);
                    if (*ierr != 0) return 0;
                  }
                  PUSH_POINT(x1,y1); num_pts++;
                }
                PUSH_POINT(x2,y2); num_pts++;
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


/*
 * the following code is from Gri
 */

#include <math.h>
#include <stdio.h>
#include <string.h>

// globals to this file
static int nx_1, ny_1, iGT, jGT, iLE, jLE;

static void free_space_for_curve(void);
static void get_space_for_curve(int *ierr);
static void draw_the_contour(long *dest_len_ptr,
                             double **dest_xs_ptr,
                             double **dest_ys_ptr,
                             long *dest_sz_ptr,
                             OBJ_PTR gaps,
                             int *ierr);
static bool trace_contour(double z0,
                          double *x,
                          double *y,
                          double **z,
                          double **legit,
                          long *dest_len_ptr,
                          double **dest_xs_ptr,
                          double **dest_ys_ptr,
                          long *dest_sz_ptr,
                          OBJ_PTR gaps,
                          int *iterr);
static int FLAG(int ni, int nj, int ind, int *ierr);
static int append_segment(double xr, double yr, double zr, double OKr,
                          double xs, double ys, double zs, double OKs,
                          double z0, int *ierr);

// Space for curve, shared by several routines
static double *xcurve, *ycurve;
static bool *legitcurve;
#define INITIAL_CURVE_SIZE 100
static int num_in_curve, max_in_curve, num_in_path;
static bool curve_storage_exists = false;


static void
free_space_for_curve(void)
{
  if (curve_storage_exists) {
    free(xcurve);
    free(ycurve);
    free(legitcurve);
    curve_storage_exists = false;
  }
  num_in_curve = 0;
  num_in_path = 0;
}


static void
get_space_for_curve(int *ierr)
{
  max_in_curve = INITIAL_CURVE_SIZE;
  if(curve_storage_exists) {
    RAISE_ERROR("storage is messed up (internal error)", ierr);
    return;
  }
  xcurve = ALLOC_N_double(max_in_curve);
  ycurve = ALLOC_N_double(max_in_curve);
  legitcurve = ALLOC_N_bool(max_in_curve);
  curve_storage_exists = true;
  num_in_curve = 0;
  num_in_path = 0;
}


/*
 *  gr_contour() -- draw contour line for gridded data
 * 
 * DESCRIPTION: Draws a contour for the value z0, through data z[i][j]
 * defined on the rectangular grid x[i] and y[j] (where 0<=i<nx and
 * 0<=j<ny).  That the grid is rectangular but needn't be square or
 * regular. Contours are drawn only in triangular regions surrounded
 * by 3 good points (ie, 3 points with legit[i][j] != 0.0).
 *
 * The contour is labelled, with the string lab, at intervals of
 * contour_space_later centimeters, starting with a space of
 * contour_space_first from the beginning of the trace.
 */
static void
gr_contour(double *x,
           double *y,
           double **z,
           double **legit,
           int nx,
           int ny, 
           double z0,
           long *dest_len_ptr,
           double **dest_xs_ptr,
           double **dest_ys_ptr,
           long *dest_sz_ptr,
           OBJ_PTR gaps,
           int *ierr)
{
  register int    i, j;
  // Test for errors
  if (nx <= 0) { RAISE_ERROR("nx<=0 (internal error)", ierr); return; }
  if (ny <= 0) { RAISE_ERROR("ny<=0 (internal error)", ierr); return; }
  // Save some globals
  nx_1 = nx - 1;
  ny_1 = ny - 1;
  // Clear  all switches.
  FLAG(nx, ny, -1, ierr);
  // Get space for the curve.
  get_space_for_curve(ierr);
  if (*ierr != 0) return;
    
  // Search for a contour intersecting various places on the grid. Whenever
  // a contour is found to be between two grid points, call trace_contour()
  // after defining the global variables iLE,jLE,iGT,jGT so that
  // z[iLE]jLE] <= z0 < z[iGT][jGT], where legit[iLE][jLE] != 0
  // and legit[iGT][jGT] != 0.
  //
  // NOTE: always start a contour running upwards (to greater j), between
  // two sideways neighboring points (same j).  Later, in trace_contour(),
  // test 'locate' for value 5.  If it's 5, it means that the same geometry
  // obtains, so set a flag and check whether already set.  If already
  // set, it means we've traced this contour before, so trace_contour()
  // knows to stop then.

  // Search bottom
  for (i = 1; i < nx; i++) {
    j = 0;
    while (j < ny_1) {
      // move north to first legit point
      while (j < ny_1 
             && (legit == NULL || !(legit[i][j] != 0.0
                                    && legit[i - 1][j] != 0.0))
             ) {
        j++;
      }
      // trace a contour if it hits here
      if (j < ny_1 && z[i][j] > z0 && z[i - 1][j] <= z0) {
        iLE = i - 1;
        jLE = j;
        iGT = i;
        jGT = j;
        trace_contour(z0, x, y, z, legit, dest_len_ptr, dest_xs_ptr,
                      dest_ys_ptr, dest_sz_ptr, gaps, ierr);
        if (*ierr != 0) return;
      }
      // Space through legit points, that is, skipping through good
      // data looking for another island of bad data which will
      // thus be a new 'bottom edge'.
      while (j < ny_1 && (legit == NULL || (legit[i][j] != 0.0
                                            && legit[i - 1][j] != 0.0)))
        j++;
    }
  }
  
  // search right edge
  for (j = 1; j < ny; j++) {
    i = nx_1;
    while (i > 0) {
      // move west to first legit point
      while (i > 0 && (legit == NULL || !(legit[i][j] != 0.0
                                          && legit[i][ j - 1] != 0.0)))
        i--;
      // trace a contour if it hits here
      if (i > 0 && z[i][j] > z0 && z[i][j - 1] <= z0) {
        iLE = i;
        jLE = j - 1;
        iGT = i;
        jGT = j;
        trace_contour(z0, x, y, z, legit, dest_len_ptr, dest_xs_ptr,
                      dest_ys_ptr, dest_sz_ptr, gaps, ierr);
        if (*ierr != 0) return;
      }
      // space through legit points
      while (i > 0 && (legit == NULL || (legit[i][j] != 0.0
                                         && legit[i][ j - 1] != 0.0)))
        i--;
    }
  }
  
  // search top edge
  for (i = nx_1 - 1; i > -1; i--) {
    j = ny_1;
    while (j > 0) {
      while (j > 0 && (legit == NULL || !(legit[i][j] != 0.0
                                          && legit[i + 1][ j] != 0.0)))
        j--;
      // trace a contour if it hits here
      if (j > 0 && z[i][j] > z0 && z[i + 1][ j] <= z0) {
        iLE = i + 1;
        jLE = j;
        iGT = i;
        jGT = j;
        trace_contour(z0, x, y, z, legit, dest_len_ptr, dest_xs_ptr,
                      dest_ys_ptr, dest_sz_ptr, gaps, ierr);
        if (*ierr != 0) return;
      }
      // space through legit points
      while (j > 0 && (legit == NULL || (legit[i][j] != 0.0
                                         && legit[i + 1][ j] != 0.0)))
        j--;
    }
  }
  
  // search left edge
  for (j = ny_1 - 1; j > -1; j--) {
    i = 0;
    while (i < nx_1) {
      while (i < nx_1 && (legit == NULL || !(legit[i][j] != 0.0
                                             && legit[i][ j + 1] != 0.0)))
        i++;
      // trace a contour if it hits here
      if (i < nx_1 && z[i][j] > z0 && z[i][j + 1] <= z0) {
        iLE = i;
        jLE = j + 1;
        iGT = i;
        jGT = j;
        trace_contour(z0, x, y, z, legit, dest_len_ptr, dest_xs_ptr,
                      dest_ys_ptr, dest_sz_ptr, gaps, ierr);
        if (*ierr != 0) return;
      }
      // space through legit points
      while (i < nx_1 && (legit == NULL || (legit[i][j] != 0.0
                                            && legit[i][ j + 1] != 0.0)))
        i++;
    }
  }
  
  // Search interior. Pass up from bottom (starting at left), through all
  // interior points. Look for contours which enter, with high to right,
  // between iLE on left and iGT on right.
  for (j = 1; j < ny_1; j++) {
    int             flag_is_set;
    for (i = 1; i < nx; i++) {
      // trace a contour if it hits here
      flag_is_set = FLAG(i, j, 0, ierr);
      if (*ierr != 0) return;
      if (flag_is_set < 0) {
        RAISE_ERROR("ran out of storage (internal error)", ierr);
        return;
      }
      if (!flag_is_set
          && (legit == NULL || legit[i][j] != 0.0)
          && z[i][j] > z0
          && (legit == NULL || legit[i - 1][j] != 0.0)
          && z[i - 1][j] <= z0) {
        iLE = i - 1;
        jLE = j;
        iGT = i;
        jGT = j;
        trace_contour(z0, x, y, z, legit, dest_len_ptr, dest_xs_ptr,
                      dest_ys_ptr, dest_sz_ptr, gaps, ierr);
        if (*ierr != 0) return;
      }
    }
  }
  // Free up space.
  free_space_for_curve();
  FLAG(nx, ny, 2, ierr);
}

/*
 * trace_contour() -- trace_contour a contour line with high values of
 * z to it's right.  Stores points in (*xcurve, *ycurve) and the legit
 * flag is stored in *legitcurve; initially these must be empty; you
 * must also free them after this call, so that the next call will
 * work OK.
 */
static bool
trace_contour(double z0,
              double *x,
              double *y,
              double **z,
              double **legit,
              long *dest_len_ptr,
              double **dest_xs_ptr,
              double **dest_ys_ptr,
              long *dest_sz_ptr,
              OBJ_PTR gaps,
              int *ierr)
{
  int i, ii, j, jj;
  double zp, vx, vy, zcentre;
  int locate;
  // locate tells where delta-grid point is.  It codes as follows to
  // i_test[] and j_test[] 6 7 8 3 4 5 0 1 2
  static int i_test[9] =
    {
      0, 1, 1,          // 6 7 8
      0, 9, 0,          // 3 4 5
      -1, -1, 0         // 0 1 2
  };
  static int j_test[9] =
    {
      -1, 0, 0,         // 6 7 8
      -1, 9, 1,         // 3 4 5
      0, 0, 1           // 0 1 2
    };
  static int dtest[9] =
    {
      0, 1, 0,          // 6 7 8
      1, 0, 1,          // 3 4 5
      0, 1, 0           // 0 1 2
    };

  // Trace the curve, storing results with append_segment() into *xcurve,
  // *ycurve, *legitcurve.  When done, call draw_the_contour(), which draws
  // the contour stored in these arrays.
  while (true) {
    append_segment(x[iLE], y[jLE], z[iLE][jLE],
                   (legit == NULL)? 1.0: legit[iLE][jLE],
                   x[iGT], y[jGT], z[iGT][jGT],
                   (legit == NULL)? 1.0: legit[iGT][jGT],
                   z0, ierr);
    if (*ierr != 0) return false;
    // Find the next point to check through a table lookup.
    locate = 3 * (jGT - jLE) + (iGT - iLE) + 4;
    i = iLE + i_test[locate];
    j = jLE + j_test[locate];
    
    // Did it hit an edge?
    if (i > nx_1 || i < 0 || j > ny_1 || j < 0) {
      draw_the_contour(dest_len_ptr, dest_xs_ptr, dest_ys_ptr, dest_sz_ptr,
                       gaps, ierr);
      if (*ierr != 0) return false;
      return true; // all done
    }
    
    // Test if retracing an existing contour.  See explanation
    // above, in grcntour(), just before search starts. 
    if (locate == 5) {
      int already_set = FLAG(iGT, jGT, 1, ierr);
      if (*ierr != 0) return false;
      if (already_set < 0) {
        RAISE_ERROR("ran out of storage (internal error)", ierr);
        return false;
      }
      if (already_set) {
        draw_the_contour(dest_len_ptr, dest_xs_ptr, dest_ys_ptr, dest_sz_ptr,
                         gaps, ierr);
        if (*ierr != 0) return false;
        return true; // all done
      }
    }
    
    // Following new for 2.1.13
    if (legit != NULL && legit[i][j] == 0.0) {
      draw_the_contour(dest_len_ptr, dest_xs_ptr, dest_ys_ptr, dest_sz_ptr,
                       gaps, ierr);
      if (*ierr != 0) return false;
      return true; // all done
    }
    
    if (!dtest[locate]) {
      zp = z[i][j];
      if (zp > z0)
        iGT = i, jGT = j;
      else
        iLE = i, jLE = j;
      continue;
    }
    vx = (x[iGT] + x[i]) * 0.5;
    vy = (y[jGT] + y[j]) * 0.5;
    locate = 3 * (jGT - j) + iGT - i + 4;
    // Fourth point in rectangular boundary
    ii = i + i_test[locate];
    jj = j + j_test[locate];
    bool legit_diag = 
      (legit == NULL || (legit[iLE][jLE] != 0.0
			 && legit[iGT][jGT] != 0.0 
			 && legit[i][j] != 0.0
			 && legit[ii][jj] != 0.0)) ? true : false;
    zcentre = 0.25 * (z[iLE][jLE] + z[iGT][jGT] + z[i][j] + z[ii][jj]);
    
    if (zcentre <= z0) {
      append_segment(x[iGT], y[jGT], z[iGT][jGT],
                     (legit == NULL)? 1.0: legit[iGT][jGT],
                     vx, vy, zcentre, legit_diag,
                     z0, ierr);
      if (*ierr != 0) return false;
      if (z[ii][jj] <= z0) {
        iLE = ii, jLE = jj;
        continue;
      }
      append_segment(x[ii], y[jj], z[ii][jj],
                     (legit == NULL)? 1.0: legit[ii][jj],
                     vx, vy, zcentre, legit_diag,
                     z0, ierr);
      if (*ierr != 0) return false;
      if (z[i][j] <= z0) {
        iGT = ii, jGT = jj;
        iLE = i, jLE = j;
        continue;
      }
      append_segment(x[i], y[j], z[i][j], (legit == NULL)? 1.0: legit[i][j],
                     vx, vy, zcentre, legit_diag,
                     z0, ierr);
      if (*ierr != 0) return false;
      iGT = i, jGT = j;
      continue;
    }
    append_segment(vx, vy, zcentre, legit_diag,
                   x[iLE], y[jLE], z[iLE][jLE],
                   (legit == NULL)? 1.0: legit[iLE][jLE],
                   z0, ierr);
    if (*ierr != 0) return false;
    if (z[i][j] > z0) {
      iGT = i, jGT = j;
      continue;
    }
    append_segment(vx, vy, zcentre, legit_diag,
                   x[i], y[j], z[i][j], (legit == NULL)? 1.0: legit[i][j],
                   z0, ierr);
    if (*ierr != 0) return false;
    if (z[ii][jj] <= z0) {
      append_segment(vx, vy, zcentre, legit_diag,
                     x[ii], y[jj], z[ii][jj],
                     (legit == NULL)? 1.0: legit[ii][jj],
                     z0, ierr);
      if (*ierr != 0) return false;
      iLE = ii;
      jLE = jj;
      continue;
    }
    iLE = i;
    jLE = j;
    iGT = ii;
    jGT = jj;
  }
}


/*
 * append_segment() -- append a line segment on the contour
 */
static double xplot_last, yplot_last;
static int
append_segment(double xr, double yr, double zr, double OKr,
               double xs, double ys, double zs, double OKs,
               double z0, int *ierr)
{
  if (zr == zs) {
    RAISE_ERROR("Contouring problem: zr = zs, which is illegal", ierr);
    return 0;
  }
  double frac = (zr - z0) / (zr - zs);
  if (frac < 0.0) {
    RAISE_ERROR("Contouring problem: frac < 0", ierr);
    return 0;
  }
  if (frac > 1.0) {
    RAISE_ERROR("Contouring problem: frac > 1", ierr);
    return 0;
  }
  double xplot = xr - frac * (xr - xs);
  double yplot = yr - frac * (yr - ys);
  // Avoid replot, which I suppose must be possible, given this code
  if (num_in_curve > 0 && xplot == xplot_last && yplot == yplot_last)
    return 1;
  if (num_in_curve > max_in_curve - 1) {
    // Get new storage if running on empty.  Better to
    // do this with an STL vector class
    max_in_curve *= 2;
    int i;
    double *tmp = ALLOC_N_double(num_in_curve);
    for (i = 0; i < num_in_curve; i++) tmp[i] = xcurve[i];
    free(xcurve); xcurve = ALLOC_N_double(max_in_curve);
    for (i = 0; i < num_in_curve; i++) xcurve[i] = tmp[i];
    for (i = 0; i < num_in_curve; i++) tmp[i] = ycurve[i];
    free(ycurve); ycurve = ALLOC_N_double(max_in_curve);
    for (i = 0; i < num_in_curve; i++) ycurve[i] = tmp[i];
    free(tmp);
    bool *tmpl = ALLOC_N_bool(num_in_curve);
    for (i = 0; i < num_in_curve; i++)	tmpl[i] = legitcurve[i];
    free(legitcurve); legitcurve = ALLOC_N_bool(max_in_curve);
    for (i = 0; i < num_in_curve; i++)	legitcurve[i] = tmpl[i];
    free(tmpl);
  }
  // A segment is appended only if both the present point and the last
  // point came by interpolating between OK points.
  xcurve[num_in_curve] = xplot;
  ycurve[num_in_curve] = yplot;
  if (OKr != 0.0 && OKs != 0.0)
    legitcurve[num_in_curve] = true;
  else
    legitcurve[num_in_curve] = false;
  num_in_curve++;
  xplot_last = xplot;
  yplot_last = yplot;
  return 1;
}


/*
 * Draw contour stored in (xcurve[],ycurve[],legitcurve[]), possibly
 * with labels (depending on global Label_contours).
 */ 
#define FACTOR 3.0 // contour must be FACTOR*len long to be labelled
static void
draw_the_contour(long *dest_len_ptr,
                 double **dest_xs_ptr,
                 double **dest_ys_ptr,
                 long *dest_sz_ptr,
                 OBJ_PTR gaps,
                 int *ierr)
{
  if (num_in_curve == 1) {
    num_in_curve = 0;
    return;
  }
  int i, k;
  for (i = 0, k = 0; i < num_in_curve; i++) {
    if (legitcurve[i] == true) {
      // PUSH_POINT does num_in_path++
      PUSH_POINT(xcurve[i],ycurve[i]); num_in_path++;
    }
    else {
      if (num_in_path > 0 && num_in_path != k) {
        Array_Push(gaps, Integer_New(num_in_path), ierr);
        if (*ierr != 0) return;
      }
      k = num_in_path;
    }
  }
  Array_Push(gaps, Integer_New(num_in_path), ierr);
  num_in_curve = 0;
}


/*
 * FLAG() -- check flag for gr_contour() and trace_contour()
 * ni = row (or, if ind==-1, number of rows)
 * nj = col (or, if ind==-1, number of cols)
 * if (ind == -1), get flag storage space; initialize flags to 0
 * if (ind == 1), check flag and then set it
 * if (ind == 2), clear the flag storage space
 * if (ind == 0), check flag, return value
 * RETURN value: Normally, the flag value (0 or 1).  If the storage is
 * exhausted, return a number <0.
 */
#define	NBITS 32
static int
FLAG(int ni, int nj, int ind, int *ierr)
{
  static bool flag_storage_exists = false;
  static unsigned long *flag, mask[NBITS];
  static int size;
  static int ni_max;	// x-dimension is saved
  int i, ipos, iword, ibit, return_value;
  switch (ind) {
  case -1:
    // Allocate storage for flag array
    if (flag_storage_exists) {
      RAISE_ERROR("storage is messed up (internal error)", ierr);
      return 0;
    }
    size = 1 + ni * nj / NBITS;	// total storage array length
    flag = ALLOC_N_unsigned_long(size);
    // Create mask
    mask[0] = 1;
    for (i = 1; i < NBITS; i++)
      mask[i] = 2 * mask[i - 1];
    for (i = 0; i < size; i++)	// Zero out flag
      flag[i] = 0;
    ni_max = ni;		// Save for later
    flag_storage_exists = true;
    return 0;
  case 2:
    if (!flag_storage_exists) {
      RAISE_ERROR("No flag storage exists", ierr);
      return 0;
    }
    free(flag);
    flag_storage_exists = false;
    return 0;
  default:
    if (!flag_storage_exists) {
      RAISE_ERROR("No flag storage exists", ierr);
      return 0;
    }
    break;
  }
  // ind was not -1 or 2
  // Find location of bit.
  ipos = nj * ni_max + ni;
  iword = ipos / NBITS;
  ibit = ipos - iword * NBITS;
  // Check for something being broken here, causing to run out of space.
  // This should never happen, but may as well check.
  if (iword >= size)
    return (-99);		// no space
  // Get flag.
  return_value = (0 != (*(flag + iword) & mask[ibit]));
  // If ind=1 and flag wasn't set, set the flag
  if (ind == 1 && !return_value)
    flag[iword] |= mask[ibit];
  // Return the flag value
  return return_value;
}
#undef NBITS


/*
 * end of contour code from Gri
 */



static void
c_make_contour(FM *p, 
               long *dest_len_ptr,
               double **dest_xs_ptr,
               double **dest_ys_ptr,
               long *dest_sz_ptr,
               OBJ_PTR gaps,
               OBJ_PTR xs, OBJ_PTR ys,  
               OBJ_PTR zs_data, double z_level,
               OBJ_PTR legit_data, int use_conrec, int *ierr)
{
  long xlen, ylen, num_zcolumns, num_zrows, num_columns, num_rows;
  double *x_coords = Vector_Data_for_Read(xs, &xlen, ierr);
  if (*ierr != 0) return;
  double *y_coords = Vector_Data_for_Read(ys, &ylen, ierr);
  if (*ierr != 0) return;
  double **zs = Table_Data_for_Read(zs_data, &num_zcolumns, &num_zrows, ierr);
  if (*ierr != 0) return;
  double **legit = Table_Data_for_Read(legit_data, &num_columns, &num_rows,
                                       ierr);
  if (*ierr != 0) return;
  double x_limit, y_limit;
  
  if (x_coords == NULL || gaps == OBJ_NIL || zs == NULL || y_coords == NULL) {
    RAISE_ERROR("Sorry: bad args for make_contour.  Need to provide xs, ys, "
                "gaps, and zs.", ierr);
    return;
  }
  if (xlen != num_columns || ylen != num_rows) {
    RAISE_ERROR("Sorry: bad args for make_contour.  Needs xs.size == "
                "num columns and ys.size == num rows.", ierr);
    return;
  }
  if (num_zcolumns != num_columns || num_zrows != num_rows) {
    RAISE_ERROR("Sorry: bad args for make_contour.  Needs same dimension zs "
                "and legit flags.", ierr);
    return;
  }
  
  // NOTE: contour data is TRANSPOSE of tioga data, so we switch x's
  // and y's in the call
  
  if (use_conrec == 1) {
    x_limit = 0.001*(x_coords[xlen-1] - x_coords[0])/xlen;
    if (x_limit < 0) x_limit = -x_limit;
    y_limit = 0.001*(y_coords[ylen-1] - y_coords[0])/ylen;
    if (y_limit < 0) y_limit = -y_limit;
    conrec(zs, 0, num_rows-1, 0, num_columns-1, y_coords, x_coords, 1,
           &z_level, dest_len_ptr, dest_ys_ptr, dest_xs_ptr, dest_sz_ptr,
           gaps, y_limit, x_limit, ierr);
  }
  else {
    gr_contour(y_coords, x_coords, zs, legit, num_rows, num_columns, z_level, 
               dest_len_ptr, dest_ys_ptr, dest_xs_ptr, dest_sz_ptr, gaps,
               ierr);
  }
}


/*
 * uses Xvec_data and Yvec_data to create a cubic spline interpolant.
 *
 * once the spline interpolant is created, it is sampled at the
 * n_pts_to_add in Xs.
 *
 * Xvec entry i is set to the value of the spline at Yvec entry i.
 * Both the X_data and the Xs should be stored in ascending order.
 * There is a boundary condition choice to be made for each end concerning the slope.
 * If clamped is true, the corresponding slope argument value sets the slope.
 * If clamped is false (known as a "free" or "natural" spline), 
 * the 2nd derivative is set to 0 and the slope is determined by the fit.
 * In this case, the corresponding slope argument is ignored.
 */
OBJ_PTR c_private_make_contour(OBJ_PTR fmkr, FM * p,
                               OBJ_PTR gaps,
                               // these vectors get the results
                               OBJ_PTR xs, OBJ_PTR ys,
                               // data x coordinates and y coordinates
                               OBJ_PTR zs, double z_level,
                               // the table of values and the desired
                               // contour level
                               OBJ_PTR legit,
                               // the table of flags (nonzero means
                               // okay)
                               int method,
                               // int == 1 means CONREC
                               int *ierr)
{
  long dest_len, dest_sz;
  double *dest_xs_data;
  double *dest_ys_data;
  OBJ_PTR Xvec;
  OBJ_PTR Yvec;
  OBJ_PTR pts_array;
  
  dest_len = 0; dest_sz = 3000;
  dest_xs_data = ALLOC_N_double(dest_sz);
  dest_ys_data = ALLOC_N_double(dest_sz);
  
  c_make_contour(NULL, &dest_len, &dest_xs_data, &dest_ys_data, &dest_sz, 
                 gaps, xs, ys, zs, z_level, legit, method, ierr);
  if (*ierr != 0) RETURN_NIL;
  
  Xvec = Vector_New(dest_len, dest_xs_data);
  Yvec = Vector_New(dest_len, dest_ys_data);
  free(dest_xs_data);
  free(dest_ys_data);
  
  pts_array = Array_New(2);
  Array_Store(pts_array,0,Xvec,ierr);
  if (*ierr != 0) RETURN_NIL;
  Array_Store(pts_array,1,Yvec,ierr);
  if (*ierr != 0) RETURN_NIL;
  return pts_array;
}
