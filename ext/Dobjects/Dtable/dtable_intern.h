/* dtable.h */
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

#ifndef __dtable_H__
#define __dtable_H__

#include <namespace.h>   
#include <ruby.h>
#include "dvector.h"

/*======================================================================*/


PUBLIC void Init_Dtable();
PRIVATE VALUE Read_Dtable(VALUE dest, char *filename, int skip_lines);
PRIVATE double **Dtable_Ptr(VALUE dtable, long *num_cols, long *num_rows);

PRIVATE bool Is_Dtable(VALUE obj);

PRIVATE VALUE dtable_apply_math_op(VALUE source, double (*op)(double));
PRIVATE VALUE dtable_apply_math_op2(VALUE ary1, VALUE ary2, double (*op)(double, double));
PRIVATE VALUE dtable_apply_math_op_bang(VALUE ary, double (*op)(double));
PRIVATE VALUE dtable_apply_math_op2_bang(VALUE ary1, VALUE ary2, double (*op)(double, double));

PRIVATE VALUE dtable_dup(VALUE ary);
PRIVATE VALUE dtable_transpose(VALUE ary);
PRIVATE VALUE dtable_reverse_rows(VALUE ary);
PRIVATE VALUE dtable_reverse_cols(VALUE ary);
PRIVATE VALUE dtable_rotate_cw90(VALUE ary);
PRIVATE VALUE dtable_rotate_ccw90(VALUE ary);
PRIVATE VALUE dtable_num_cols(VALUE ary);
PRIVATE VALUE dtable_num_rows(VALUE ary);
PRIVATE VALUE dtable_min(VALUE ary);
PRIVATE VALUE dtable_max(VALUE ary);
PRIVATE VALUE dtable_minmax(VALUE ary);
PRIVATE VALUE dtable_row(VALUE ary, VALUE row_num);
PRIVATE VALUE dtable_column(VALUE ary, VALUE column_num);
PRIVATE VALUE dtable_set_row(VALUE ary, VALUE row_num, VALUE dvec);
PRIVATE VALUE dtable_set_column(VALUE ary, VALUE col_num, VALUE dvec);
PRIVATE VALUE dtable_clear(VALUE ary, VALUE val);
PRIVATE VALUE dtable_set(VALUE ary, VALUE val);
PRIVATE VALUE dtable_neg(VALUE ary);
PRIVATE VALUE dtable_abs(VALUE ary);
PRIVATE VALUE dtable_sin(VALUE ary);
PRIVATE VALUE dtable_cos(VALUE ary);
PRIVATE VALUE dtable_tan(VALUE ary);
PRIVATE VALUE dtable_asin(VALUE ary);
PRIVATE VALUE dtable_acos(VALUE ary);
PRIVATE VALUE dtable_atan(VALUE ary);
PRIVATE VALUE dtable_sinh(VALUE ary);
PRIVATE VALUE dtable_cosh(VALUE ary);
PRIVATE VALUE dtable_tanh(VALUE ary);
PRIVATE VALUE dtable_asinh(VALUE ary);
PRIVATE VALUE dtable_acosh(VALUE ary);
PRIVATE VALUE dtable_atanh(VALUE ary);
PRIVATE VALUE dtable_ceil(VALUE ary);
PRIVATE VALUE dtable_floor(VALUE ary);
PRIVATE VALUE dtable_round(VALUE ary);
PRIVATE VALUE dtable_exp(VALUE ary);
PRIVATE VALUE dtable_exp10(VALUE ary);
PRIVATE VALUE dtable_log(VALUE ary);
PRIVATE VALUE dtable_log10(VALUE ary);
PRIVATE VALUE dtable_inv(VALUE ary);
PRIVATE VALUE dtable_sqrt(VALUE ary);
PRIVATE VALUE dtable_neg_bang(VALUE ary);
PRIVATE VALUE dtable_abs_bang(VALUE ary);
PRIVATE VALUE dtable_sin_bang(VALUE ary);
PRIVATE VALUE dtable_cos_bang(VALUE ary);
PRIVATE VALUE dtable_tan_bang(VALUE ary);
PRIVATE VALUE dtable_asin_bang(VALUE ary);
PRIVATE VALUE dtable_acos_bang(VALUE ary);
PRIVATE VALUE dtable_atan_bang(VALUE ary);
PRIVATE VALUE dtable_sinh_bang(VALUE ary);
PRIVATE VALUE dtable_cosh_bang(VALUE ary);
PRIVATE VALUE dtable_tanh_bang(VALUE ary);
PRIVATE VALUE dtable_asinh_bang(VALUE ary);
PRIVATE VALUE dtable_acosh_bang(VALUE ary);
PRIVATE VALUE dtable_atanh_bang(VALUE ary);
PRIVATE VALUE dtable_ceil_bang(VALUE ary);
PRIVATE VALUE dtable_floor_bang(VALUE ary);
PRIVATE VALUE dtable_round_bang(VALUE ary);
PRIVATE VALUE dtable_exp_bang(VALUE ary);
PRIVATE VALUE dtable_exp10_bang(VALUE ary);
PRIVATE VALUE dtable_log_bang(VALUE ary);
PRIVATE VALUE dtable_log10_bang(VALUE ary);
PRIVATE VALUE dtable_inv_bang(VALUE ary);
PRIVATE VALUE dtable_sqrt_bang(VALUE ary);
PRIVATE VALUE dtable_trim(int argc, VALUE *argv, VALUE self);
PRIVATE VALUE dtable_safe_log(int argc, VALUE *argv, VALUE self);
PRIVATE VALUE dtable_safe_log10(int argc, VALUE *argv, VALUE self);
PRIVATE VALUE dtable_safe_inv(int argc, VALUE *argv, VALUE self);
PRIVATE VALUE dtable_safe_asin(VALUE ary);
PRIVATE VALUE dtable_safe_acos(VALUE ary);
PRIVATE VALUE dtable_safe_sqrt(VALUE ary);
PRIVATE VALUE dtable_atan2_bang(VALUE ary, VALUE arg);
PRIVATE VALUE dtable_modulo_bang(VALUE ary, VALUE arg);
PRIVATE VALUE dtable_remainder_bang(VALUE ary, VALUE arg);
PRIVATE VALUE dtable_trim_bang(int argc, VALUE *argv, VALUE self);
PRIVATE VALUE dtable_pow_bang(VALUE ary, VALUE arg);
PRIVATE VALUE dtable_as_exponent_of_bang(VALUE ary, VALUE arg);
PRIVATE VALUE dtable_safe_log_bang(int argc, VALUE *argv, VALUE self);
PRIVATE VALUE dtable_safe_log10_bang(int argc, VALUE *argv, VALUE self);
PRIVATE VALUE dtable_safe_inv_bang(int argc, VALUE *argv, VALUE self);
PRIVATE VALUE dtable_safe_sqrt_bang(VALUE ary);
PRIVATE VALUE dtable_safe_asin_bang(VALUE ary);
PRIVATE VALUE dtable_safe_acos_bang(VALUE ary);
PRIVATE VALUE dtable_add(VALUE ary, VALUE arg);
PRIVATE VALUE dtable_sub(VALUE ary, VALUE arg);
PRIVATE VALUE dtable_mul(VALUE ary, VALUE arg);
PRIVATE VALUE dtable_div(VALUE ary, VALUE arg);
PRIVATE VALUE dtable_mod(VALUE ary, VALUE arg);
PRIVATE VALUE dtable_remainder(VALUE ary, VALUE arg);
PRIVATE VALUE dtable_pow(VALUE ary, VALUE arg);
PRIVATE VALUE dtable_as_exponent_of(VALUE ary, VALUE arg);
PRIVATE VALUE dtable_atan2(VALUE ary, VALUE arg);
PRIVATE VALUE dtable_add_bang(VALUE ary, VALUE arg);
PRIVATE VALUE dtable_sub_bang(VALUE ary, VALUE arg);
PRIVATE VALUE dtable_mul_bang(VALUE ary, VALUE arg);
PRIVATE VALUE dtable_div_bang(VALUE ary, VALUE arg);
PRIVATE VALUE dtable_read(int argc, VALUE *argv, VALUE self);
PRIVATE VALUE dtable_at(VALUE ary, VALUE xloc, VALUE yloc);
PRIVATE VALUE dtable_aset(VALUE ary, VALUE xloc, VALUE yloc, VALUE val);
PRIVATE void dtable_store(VALUE ary, long i, long j, double v);

#endif   /* __dtable_H__ */

