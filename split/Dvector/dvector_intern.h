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

/*======================================================================*/

extern void Init_Dvector();
/* dirty hack to prevent namespace conflicts */
#define extern static
extern bool Is_Dvector(VALUE obj);
extern double *Dvector_Data_for_Read(VALUE dvector, long *len_ptr); /* returns pointer to the dvector's data (which may be shared) */
extern double *Dvector_Data_Copy(VALUE dvector, long *len_ptr); /* like Dvector_Data_for_Read, but returns pointer to a copy of the data */
extern double *Dvector_Data_for_Write(VALUE dvector, long *len_ptr);
extern double *Dvector_Data_Resize(VALUE dvector, long new_len);
extern double *Dvector_Data_Replace(VALUE dvector, long len, double *data); /* copies the data into the dvector */
extern VALUE Dvector_Create(void);
extern void Dvector_Push_Double(VALUE ary, double val);
extern void Dvector_Store_Double(VALUE ary, long idx, double val);

extern VALUE Read_Dvectors(char *filename, VALUE destinations, int first_row_of_file, int number_of_rows);
extern VALUE Read_Row(char *filename, int row, VALUE row_ary);

/* implementations for Ruby methods */

extern VALUE dvector_apply_math_op(VALUE source, double (*op)(double));
extern VALUE dvector_apply_math_op2(VALUE ary1, VALUE ary2, double (*op)(double, double));
extern VALUE dvector_apply_math_op_bang(VALUE ary, double (*op)(double));
extern VALUE dvector_apply_math_op2_bang(VALUE ary1, VALUE ary2, double (*op)(double, double));

extern VALUE dvector_freeze(VALUE ary);
extern VALUE dvector_frozen_p(VALUE ary);
extern VALUE make_new_dvector(VALUE klass, long len, long capa);
extern VALUE dvector_new2(long len, long capa);
extern VALUE dvector_new();
extern VALUE dvector_new4(long len, VALUE *data);
extern VALUE dvector_new4_dbl(long len, double *data);
extern VALUE dvector_check_array_type(VALUE ary);
extern VALUE dvector_initialize(int argc, VALUE *argv, VALUE ary);
extern VALUE dvector_push(VALUE ary, VALUE item);
extern VALUE dvector_push_m(int argc, VALUE *argv, VALUE ary);
extern VALUE dvector_pop(VALUE ary);
extern VALUE dvector_shift(VALUE ary);
extern VALUE dvector_unshift_m(int argc, VALUE *argv, VALUE ary);
extern VALUE dvector_entry(VALUE ary, long offset);
extern VALUE dvector_subseq(VALUE ary, long beg, long len);
extern VALUE dvector_aref(int argc, VALUE *argv, VALUE ary);
extern VALUE dvector_at(VALUE ary, VALUE pos);
extern VALUE dvector_first(int argc, VALUE *argv, VALUE ary);
extern VALUE dvector_last(int argc, VALUE *argv, VALUE ary);
extern VALUE dvector_uniq_bang(VALUE ary);
extern VALUE dvector_uniq(VALUE ary);
extern VALUE dvector_fetch(int argc, VALUE *argv, VALUE ary);
extern VALUE dvector_index(VALUE ary, VALUE val);
extern VALUE dvector_rindex(VALUE ary, VALUE val);
extern VALUE dvector_aset(int argc, VALUE *argv, VALUE ary);
extern VALUE dvector_insert(int argc, VALUE *argv, VALUE ary);
extern VALUE dvector_each(VALUE ary);
extern VALUE dvector_each2(VALUE ary, VALUE ary2);
extern VALUE dvector_each_index(VALUE ary);
extern VALUE dvector_each_with_index(VALUE ary);
extern VALUE dvector_each2_with_index(VALUE ary, VALUE ary2);
extern VALUE dvector_reverse_each(VALUE ary);
extern VALUE dvector_reverse_each2(VALUE ary, VALUE ary2);
extern VALUE dvector_reverse_each_index(VALUE ary);
extern VALUE dvector_reverse_each_with_index(VALUE ary);
extern VALUE dvector_reverse_each2_with_index(VALUE ary, VALUE ary2);
extern VALUE dvector_length(VALUE ary);
extern VALUE dvector_empty_p(VALUE ary);
extern VALUE dvector_dup(VALUE ary);
extern VALUE dvector_join(VALUE ary, VALUE sep);
extern VALUE dvector_join_m(int argc, VALUE *argv, VALUE ary);
extern VALUE dvector_to_s(VALUE ary);
extern VALUE dvector_to_a(VALUE ary);
extern VALUE dvector_read(int argc, VALUE *argv, VALUE klass);
extern VALUE dvector_read_row(int argc, VALUE *argv, VALUE klass);
extern VALUE dvector_read_rows(int argc, VALUE *argv, VALUE klass);
extern VALUE dvector_reverse(VALUE ary);
extern VALUE dvector_reverse_bang(VALUE ary);
extern VALUE dvector_reverse_m(VALUE ary);
extern VALUE dvector_sort_bang(VALUE ary);
extern VALUE dvector_sort(VALUE ary);
extern VALUE dvector_collect(VALUE ary);
extern VALUE dvector_collect2(VALUE ary, VALUE ary2);
extern VALUE dvector_collect_bang(VALUE ary);
extern VALUE dvector_collect2_bang(VALUE ary, VALUE ary2);
extern VALUE dvector_values_at(int argc, VALUE *argv, VALUE ary);
extern VALUE dvector_select(VALUE ary);
extern VALUE dvector_delete(VALUE ary, VALUE item);
extern VALUE dvector_delete_at(VALUE ary, long pos);
extern VALUE dvector_slice_bang(int argc, VALUE *argv, VALUE ary);
extern VALUE dvector_reject_bang(VALUE ary);
extern VALUE dvector_reject(VALUE ary);
extern VALUE dvector_delete_if(VALUE ary);
extern VALUE dvector_replace(VALUE dest, VALUE orig);
extern VALUE dvector_clear(VALUE ary);
extern VALUE dvector_fill(int argc, VALUE *argv, VALUE ary);
extern VALUE dvector_eql(VALUE ary1, VALUE ary2);
extern VALUE dvector_includes(VALUE ary, VALUE item);
extern VALUE dvector_cmp(VALUE ary1, VALUE ary2);
extern VALUE dvector_mod(VALUE ary, VALUE arg);
extern VALUE dvector_min(int argc, VALUE *argv, VALUE ary);
extern VALUE dvector_max(int argc, VALUE *argv, VALUE ary);

extern void c_dvector_create_spline_interpolant(int n_pts_data, double *Xs, double *Ys,
    bool start_clamped, double start_slope, bool end_clamped, double end_slope,
    double *Bs, double *Cs, double *Ds);
extern VALUE dvector_create_spline_interpolant(int argc, VALUE *argv, VALUE klass);
    
extern double c_dvector_spline_interpolate(double x, int n_pts_data, 
    double *Xs, double *Ys, double *Bs, double *Cs, double *Ds);
extern VALUE dvector_spline_interpolate(int argc, VALUE *argv, VALUE klass);

extern double c_dvector_linear_interpolate(int num_pts, double *xs, double *ys, double x);
extern VALUE dvector_linear_interpolate(int argc, VALUE *argv, VALUE klass);

/* end of dirty hack */
#undef extern

#endif   /* __Dvector_H__ */

