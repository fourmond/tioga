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

#include "ruby.h"
#include <stdbool.h>
#include <namespace.h>



/*======================================================================*/

PUBLIC void Init_Dvector();
/* dirty hack to prevent namespace conflicts */
PRIVATE bool Is_Dvector(VALUE obj);
PRIVATE double *Dvector_Data_for_Read(VALUE dvector, long *len_ptr); /* returns pointer to the dvector's data (which may be shared) */
PRIVATE double *Dvector_Data_Copy(VALUE dvector, long *len_ptr); /* like Dvector_Data_for_Read, but returns pointer to a copy of the data */
PRIVATE double *Dvector_Data_for_Write(VALUE dvector, long *len_ptr);
PRIVATE double *Dvector_Data_Resize(VALUE dvector, long new_len);
PRIVATE double *Dvector_Data_Replace(VALUE dvector, long len, double *data); /* copies the data into the dvector */
PRIVATE VALUE Dvector_Create(void);
PRIVATE void Dvector_Push_Double(VALUE ary, double val);
PRIVATE void Dvector_Store_Double(VALUE ary, long idx, double val);

PRIVATE VALUE Read_Dvectors(char *filename, VALUE destinations, int first_row_of_file, int number_of_rows);
PRIVATE VALUE Read_Row(char *filename, int row, VALUE row_ary);

/* implementations for Ruby methods */

PRIVATE VALUE dvector_apply_math_op(VALUE source, double (*op)(double));
PRIVATE VALUE dvector_apply_math_op2(VALUE ary1, VALUE ary2, double (*op)(double, double));
PRIVATE VALUE dvector_apply_math_op_bang(VALUE ary, double (*op)(double));
PRIVATE VALUE dvector_apply_math_op2_bang(VALUE ary1, VALUE ary2, double (*op)(double, double));

PRIVATE VALUE dvector_freeze(VALUE ary);
PRIVATE VALUE dvector_frozen_p(VALUE ary);
PRIVATE VALUE make_new_dvector(VALUE klass, long len, long capa);
PRIVATE VALUE dvector_new2(long len, long capa);
PRIVATE VALUE dvector_new();
PRIVATE VALUE dvector_new4(long len, VALUE *data);
PRIVATE VALUE dvector_new4_dbl(long len, double *data);
PRIVATE VALUE dvector_check_array_type(VALUE ary);
PRIVATE VALUE dvector_initialize(int argc, VALUE *argv, VALUE ary);
PRIVATE VALUE dvector_push(VALUE ary, VALUE item);
PRIVATE VALUE dvector_push_m(int argc, VALUE *argv, VALUE ary);
PRIVATE VALUE dvector_pop(VALUE ary);
PRIVATE VALUE dvector_shift(VALUE ary);
PRIVATE VALUE dvector_unshift_m(int argc, VALUE *argv, VALUE ary);
PRIVATE VALUE dvector_entry(VALUE ary, long offset);
PRIVATE VALUE dvector_subseq(VALUE ary, long beg, long len);
PRIVATE VALUE dvector_aref(int argc, VALUE *argv, VALUE ary);
PRIVATE VALUE dvector_at(VALUE ary, VALUE pos);
PRIVATE VALUE dvector_first(int argc, VALUE *argv, VALUE ary);
PRIVATE VALUE dvector_last(int argc, VALUE *argv, VALUE ary);
PRIVATE VALUE dvector_uniq_bang(VALUE ary);
PRIVATE VALUE dvector_uniq(VALUE ary);
PRIVATE VALUE dvector_fetch(int argc, VALUE *argv, VALUE ary);
PRIVATE VALUE dvector_index(VALUE ary, VALUE val);
PRIVATE VALUE dvector_rindex(VALUE ary, VALUE val);
PRIVATE VALUE dvector_aset(int argc, VALUE *argv, VALUE ary);
PRIVATE VALUE dvector_insert(int argc, VALUE *argv, VALUE ary);
PRIVATE VALUE dvector_each(VALUE ary);
PRIVATE VALUE dvector_each2(VALUE ary, VALUE ary2);
PRIVATE VALUE dvector_each_index(VALUE ary);
PRIVATE VALUE dvector_each_with_index(VALUE ary);
PRIVATE VALUE dvector_each2_with_index(VALUE ary, VALUE ary2);
PRIVATE VALUE dvector_reverse_each(VALUE ary);
PRIVATE VALUE dvector_reverse_each2(VALUE ary, VALUE ary2);
PRIVATE VALUE dvector_reverse_each_index(VALUE ary);
PRIVATE VALUE dvector_reverse_each_with_index(VALUE ary);
PRIVATE VALUE dvector_reverse_each2_with_index(VALUE ary, VALUE ary2);
PRIVATE VALUE dvector_each3(VALUE ary, VALUE ary2, VALUE ary3);
PRIVATE VALUE dvector_each3_with_index(VALUE ary, VALUE ary2, VALUE ary3);
PRIVATE VALUE dvector_reverse_each3(VALUE ary, VALUE ary2, VALUE ary3);
PRIVATE VALUE dvector_reverse_each3_with_index(VALUE ary, VALUE ary2, VALUE ary3);
PRIVATE VALUE dvector_length(VALUE ary);
PRIVATE VALUE dvector_empty_p(VALUE ary);
PRIVATE VALUE dvector_dup(VALUE ary);
PRIVATE VALUE dvector_join(VALUE ary, VALUE sep);
PRIVATE VALUE dvector_join_m(int argc, VALUE *argv, VALUE ary);
PRIVATE VALUE dvector_to_s(VALUE ary);
PRIVATE VALUE dvector_to_a(VALUE ary);
PRIVATE VALUE dvector_read(int argc, VALUE *argv, VALUE klass);
PRIVATE VALUE dvector_read_row(int argc, VALUE *argv, VALUE klass);
PRIVATE VALUE dvector_read_rows(int argc, VALUE *argv, VALUE klass);
PRIVATE VALUE dvector_reverse(VALUE ary);
PRIVATE VALUE dvector_reverse_bang(VALUE ary);
PRIVATE VALUE dvector_reverse_m(VALUE ary);
PRIVATE VALUE dvector_sort_bang(VALUE ary);
PRIVATE VALUE dvector_sort(VALUE ary);
PRIVATE VALUE dvector_collect(VALUE ary);
PRIVATE VALUE dvector_collect2(VALUE ary, VALUE ary2);
PRIVATE VALUE dvector_collect_bang(VALUE ary);
PRIVATE VALUE dvector_collect2_bang(VALUE ary, VALUE ary2);
PRIVATE VALUE dvector_values_at(int argc, VALUE *argv, VALUE ary);
PRIVATE VALUE dvector_select(VALUE ary);
PRIVATE VALUE dvector_delete(VALUE ary, VALUE item);
PRIVATE VALUE dvector_delete_at(VALUE ary, long pos);
PRIVATE VALUE dvector_prune(VALUE ary, VALUE pos);
PRIVATE VALUE dvector_prune_bang(VALUE ary, VALUE pos);
PRIVATE VALUE dvector_slice_bang(int argc, VALUE *argv, VALUE ary);
PRIVATE VALUE dvector_reject_bang(VALUE ary);
PRIVATE VALUE dvector_reject(VALUE ary);
PRIVATE VALUE dvector_delete_if(VALUE ary);
PRIVATE VALUE dvector_replace(VALUE dest, VALUE orig);
PRIVATE VALUE dvector_clear(VALUE ary);
PRIVATE VALUE dvector_fill(int argc, VALUE *argv, VALUE ary);
PRIVATE VALUE dvector_eql(VALUE ary1, VALUE ary2);
PRIVATE VALUE dvector_includes(VALUE ary, VALUE item);
PRIVATE VALUE dvector_cmp(VALUE ary1, VALUE ary2);
PRIVATE VALUE dvector_mod(VALUE ary, VALUE arg);
PRIVATE VALUE dvector_min(int argc, VALUE *argv, VALUE ary);
PRIVATE VALUE dvector_max(int argc, VALUE *argv, VALUE ary);

PRIVATE void c_dvector_create_spline_interpolant(int n_pts_data, double *Xs, double *Ys,
    bool start_clamped, double start_slope, bool end_clamped, double end_slope,
    double *Bs, double *Cs, double *Ds);
PRIVATE VALUE dvector_create_spline_interpolant(int argc, VALUE *argv, VALUE klass);
    
PRIVATE double c_dvector_spline_interpolate(double x, int n_pts_data, 
    double *Xs, double *Ys, double *Bs, double *Cs, double *Ds);
PRIVATE VALUE dvector_spline_interpolate(int argc, VALUE *argv, VALUE klass);



PRIVATE void c_dvector_create_pm_cubic_interpolant(int n_pts_data, double *Xs, double *Ys,
    double *Bs, double *Cs, double *Ds);
PRIVATE VALUE dvector_create_pm_cubic_interpolant(int argc, VALUE *argv, VALUE klass);
    
PRIVATE double c_dvector_pm_cubic_interpolate(double x, int n_pts_data, 
    double *Xs, double *Ys, double *Bs, double *Cs, double *Ds);
PRIVATE VALUE dvector_pm_cubic_interpolate(int argc, VALUE *argv, VALUE klass);



PRIVATE double c_dvector_linear_interpolate(int num_pts, double *xs, double *ys, double x);
PRIVATE VALUE dvector_linear_interpolate(int argc, VALUE *argv, VALUE klass);

/* end of dirty hack */

#endif   /* __Dvector_H__ */

