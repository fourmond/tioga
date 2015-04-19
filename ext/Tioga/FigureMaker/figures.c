/* figures.c */
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

/*
:stopdoc:
*/

#include "figures.h"
#include "pdfs.h"
#include "wrappers.h"
#include "dvector.h"
#include "dtable.h"
#include "flate.h"

#include <symbols.h>
#include <symbols.c>

#include <stdio.h>


#define DBL_ATTR(attr) \
static VALUE FM_##attr##_get(VALUE fmkr) { int ierr=0; FM *p = Get_FM(fmkr,&ierr); return (ierr != 0)? OBJ_NIL : rb_float_new(p->attr); } \
static VALUE FM_##attr##_set(VALUE fmkr, VALUE val) { \
   int ierr=0; FM *p = Get_FM(fmkr,&ierr); if (ierr != 0) RETURN_NIL; VALUE v = rb_Float(val); p->attr = NUM2DBL(v); return val; }

#define INT_ATTR(attr) \
static VALUE FM_##attr##_get(VALUE fmkr) { int ierr=0; FM *p = Get_FM(fmkr,&ierr); return (ierr != 0)? OBJ_NIL : INT2FIX(p->attr); } \
static VALUE FM_##attr##_set(VALUE fmkr, VALUE val) { \
   int ierr=0; FM *p = Get_FM(fmkr,&ierr); if (ierr != 0) RETURN_NIL; VALUE v = rb_Integer(val); p->attr = NUM2INT(v); return val; }

#define VAL_ATTR(attr) \
static VALUE FM_##attr##_get(VALUE fmkr) { int ierr=0; FM *p = Get_FM(fmkr,&ierr); return (ierr != 0)? OBJ_NIL : p->attr; } \
static VALUE FM_##attr##_set(VALUE fmkr, VALUE val) { \
   int ierr=0; FM *p = Get_FM(fmkr,&ierr); if (ierr != 0) RETURN_NIL; p->attr = val; return val; }

#define BOOL_ATTR(attr) \
static VALUE FM_##attr##_get(VALUE fmkr) { int ierr=0; FM *p = Get_FM(fmkr,&ierr); return (ierr != 0)? OBJ_NIL : ((p->attr)? Qtrue : Qfalse); } \
static VALUE FM_##attr##_set(VALUE fmkr, VALUE val) { \
   int ierr=0; FM *p = Get_FM(fmkr,&ierr); if (ierr != 0) RETURN_NIL; p->attr = (val != Qfalse); return val; }

#define RO_DBL_ATTR(attr) \
static VALUE FM_##attr##_get(VALUE fmkr) { int ierr=0; FM *p = Get_FM(fmkr,&ierr); return (ierr != 0)? OBJ_NIL : rb_float_new(p->attr); }

#define RO_INT_ATTR(attr) \
static VALUE FM_##attr##_get(VALUE fmkr) { int ierr=0; FM *p = Get_FM(fmkr,&ierr); return (ierr != 0)? OBJ_NIL : INT2FIX(p->attr); }

#define RO_VAL_ATTR(attr) \
static VALUE FM_##attr##_get(VALUE fmkr) { int ierr=0; FM *p = Get_FM(fmkr,&ierr); return (ierr != 0)? OBJ_NIL : p->attr; }

#define RO_BOOL_ATTR(attr) \
static VALUE FM_##attr##_get(VALUE fmkr) { int ierr=0; FM *p = Get_FM(fmkr,&ierr); return (ierr != 0)? OBJ_NIL : ((p->attr)? Qtrue : Qfalse); }


char *data_dir = NULL;

OBJ_PTR cFM; /* the Tioga/FigureMaker class object */

FM *Get_FM(OBJ_PTR fmkr, int *ierr) {
   FM *p = (FM *)Dvector_Data_for_Write(Get_fm_data_attr(fmkr, ierr), NULL);
   if (*ierr != 0) RAISE_ERROR("FigMkr is missing @fm_data", ierr);
   return p;
}

/* page attribute accessors */
   RO_BOOL_ATTR(root_figure)
   RO_BOOL_ATTR(in_subplot)

   RO_DBL_ATTR(scaling_factor)

/* device page size accessors -- can set these using set_device_pagesize */
   RO_DBL_ATTR(page_left)
   RO_DBL_ATTR(page_bottom)
   RO_DBL_ATTR(page_top)
   RO_DBL_ATTR(page_right)
   RO_DBL_ATTR(page_width)
   RO_DBL_ATTR(page_height)
   
/* frame attribute accessors -- can set these using set_frame_sides */
   RO_DBL_ATTR(frame_left)
   RO_DBL_ATTR(frame_right)
   RO_DBL_ATTR(frame_top)
   RO_DBL_ATTR(frame_bottom)
   RO_DBL_ATTR(frame_width)
   RO_DBL_ATTR(frame_height)
   
/* bounds attribute accessors */
   RO_DBL_ATTR(bounds_left)
   RO_DBL_ATTR(bounds_right)
   RO_DBL_ATTR(bounds_top)
   RO_DBL_ATTR(bounds_bottom)
   RO_DBL_ATTR(bounds_xmin)
   RO_DBL_ATTR(bounds_xmax)
   RO_DBL_ATTR(bounds_ymin)
   RO_DBL_ATTR(bounds_ymax)
   RO_DBL_ATTR(bounds_width)
   RO_DBL_ATTR(bounds_height)
   RO_BOOL_ATTR(xaxis_reversed)
   RO_BOOL_ATTR(yaxis_reversed)
   
/* text attribute accessors */
   RO_DBL_ATTR(default_font_size)
   RO_DBL_ATTR(default_text_scale)
   INT_ATTR(justification)
   INT_ATTR(alignment)
   RO_DBL_ATTR(default_text_height_dx)        
   RO_DBL_ATTR(default_text_height_dy)
   DBL_ATTR(label_left_margin)
   DBL_ATTR(label_right_margin)
   DBL_ATTR(label_top_margin)
   DBL_ATTR(label_bottom_margin)
   DBL_ATTR(text_shift_on_left)
   DBL_ATTR(text_shift_on_right)
   DBL_ATTR(text_shift_on_top)
   DBL_ATTR(text_shift_on_bottom)
   DBL_ATTR(text_shift_from_x_origin)
   DBL_ATTR(text_shift_from_y_origin)
   
/* graphics attribute accessors */
   RO_DBL_ATTR(default_line_scale)        
   RO_DBL_ATTR(line_width)        
   RO_INT_ATTR(line_cap)
   RO_INT_ATTR(line_join)
   RO_DBL_ATTR(miter_limit)
   RO_DBL_ATTR(stroke_opacity)
   RO_DBL_ATTR(fill_opacity)
   
/* Title */
   RO_BOOL_ATTR(title_visible)
   INT_ATTR(title_side)
   DBL_ATTR(title_position)
   DBL_ATTR(title_scale)
   DBL_ATTR(title_shift)
   DBL_ATTR(title_angle)
   INT_ATTR(title_alignment)
   INT_ATTR(title_justification)
   //VAL_ATTR(title_color)
    
/* X label */
   RO_BOOL_ATTR(xlabel_visible)
   DBL_ATTR(xlabel_position)
   DBL_ATTR(xlabel_scale)
   DBL_ATTR(xlabel_shift)
   DBL_ATTR(xlabel_angle)
   INT_ATTR(xlabel_side)
   INT_ATTR(xlabel_alignment)
   INT_ATTR(xlabel_justification)
    
/* Y label */
   RO_BOOL_ATTR(ylabel_visible)
   DBL_ATTR(ylabel_position)
   DBL_ATTR(ylabel_scale)
   DBL_ATTR(ylabel_shift)
   DBL_ATTR(ylabel_angle)
   INT_ATTR(ylabel_side)
   INT_ATTR(ylabel_alignment)
   INT_ATTR(ylabel_justification)
    
/* X axis */
   RO_BOOL_ATTR(xaxis_visible)
   INT_ATTR(xaxis_loc)
   INT_ATTR(xaxis_type)
   DBL_ATTR(xaxis_line_width)
   DBL_ATTR(xaxis_major_tick_width)
   DBL_ATTR(xaxis_minor_tick_width)
   DBL_ATTR(xaxis_major_tick_length)
   DBL_ATTR(xaxis_minor_tick_length)
   BOOL_ATTR(xaxis_log_values)
   BOOL_ATTR(xaxis_ticks_inside)
   BOOL_ATTR(xaxis_ticks_outside)
   DBL_ATTR(xaxis_tick_interval)
   DBL_ATTR(xaxis_min_between_major_ticks)
   INT_ATTR(xaxis_number_of_minor_intervals)
   BOOL_ATTR(xaxis_use_fixed_pt)
   INT_ATTR(xaxis_digits_max)
   INT_ATTR(xaxis_numeric_label_decimal_digits)
   DBL_ATTR(xaxis_numeric_label_scale)
   DBL_ATTR(xaxis_numeric_label_shift)
   DBL_ATTR(xaxis_numeric_label_angle)
   INT_ATTR(xaxis_numeric_label_alignment)
   INT_ATTR(xaxis_numeric_label_justification)
   INT_ATTR(xaxis_numeric_label_frequency)
   INT_ATTR(xaxis_numeric_label_phase)
   INT_ATTR(top_edge_type)
   RO_BOOL_ATTR(top_edge_visible)
   INT_ATTR(bottom_edge_type)
   RO_BOOL_ATTR(bottom_edge_visible)
    
/* Y axis */
   RO_BOOL_ATTR(yaxis_visible)
   INT_ATTR(yaxis_loc)
   INT_ATTR(yaxis_type)
   DBL_ATTR(yaxis_line_width)
   DBL_ATTR(yaxis_major_tick_width)
   DBL_ATTR(yaxis_minor_tick_width)
   DBL_ATTR(yaxis_major_tick_length)
   DBL_ATTR(yaxis_minor_tick_length)
   BOOL_ATTR(yaxis_log_values)
   BOOL_ATTR(yaxis_ticks_inside)
   BOOL_ATTR(yaxis_ticks_outside)
   DBL_ATTR(yaxis_tick_interval)
   DBL_ATTR(yaxis_min_between_major_ticks)
   INT_ATTR(yaxis_number_of_minor_intervals)
   BOOL_ATTR(yaxis_use_fixed_pt)
   INT_ATTR(yaxis_digits_max)
   INT_ATTR(yaxis_numeric_label_decimal_digits)
   DBL_ATTR(yaxis_numeric_label_scale)
   DBL_ATTR(yaxis_numeric_label_shift)
   DBL_ATTR(yaxis_numeric_label_angle)
   INT_ATTR(yaxis_numeric_label_alignment)
   INT_ATTR(yaxis_numeric_label_justification)
   INT_ATTR(yaxis_numeric_label_frequency)
   INT_ATTR(yaxis_numeric_label_phase)
   INT_ATTR(left_edge_type)
   RO_BOOL_ATTR(left_edge_visible)
   INT_ATTR(right_edge_type)
   RO_BOOL_ATTR(right_edge_visible)

/* Major ticks picking algorithm */
   BOOL_ATTR(vincent_or_bill)
   INT_ATTR(log_minor_ticks_limit)

    
/* Legend */
   DBL_ATTR(legend_text_width)
   DBL_ATTR(legend_line_x0)
   DBL_ATTR(legend_line_x1)
   DBL_ATTR(legend_line_dy)
   DBL_ATTR(legend_text_xstart)
   DBL_ATTR(legend_text_ystart)
   DBL_ATTR(legend_text_dy)
   DBL_ATTR(legend_line_width)
   DBL_ATTR(legend_scale)
   INT_ATTR(legend_alignment)
   INT_ATTR(legend_justification)
   
/* Debugging */
   INT_ATTR(debug_verbosity_level)

/* Warning on non-ok numbers */
   BOOL_ATTR(croak_on_nonok_numbers)

bool Get_initialized() {
   OBJ_PTR v = rb_cv_get(cFM, "@@initialized");
   return v != OBJ_FALSE && v != OBJ_NIL;
}

void Set_initialized() {
   rb_cv_set(cFM, "@@initialized", OBJ_TRUE);
}

static void Set_fm_data_size() {
   rb_cv_set(cFM, "@@fm_data_size", Integer_New(1 + (sizeof(FM) / sizeof(double)))); 
   // size is number of doubles needed to hold FM data
}

#define attr_reader(attr) rb_define_method(cFM, #attr , FM_##attr##_get, 0);
#define attr_writer(attr) rb_define_method(cFM, #attr "=", FM_##attr##_set, 1);
#define attr_accessors(attr) attr_reader(attr) attr_writer(attr)

void Init_FigureMaker(void) { 
   /* called by Ruby when the extension is loaded */

   /* this function has been modified by Vincent Fourmond for the splitting
      out of libraries more general than Tioga */

   /*
   Init_Flate();
   Init_Dvector();
   Init_Dtable();
   */

   /* These functions don't belong here anymore, as they will be called
      by their respective modules. Instead, we require them */

   rb_require("Dobjects/Dvector");
   rb_require("Dobjects/Dtable");
   rb_require("Flate");

   
   OBJ_PTR mTioga = rb_define_module("Tioga");


   /* and now, we need to import Dobjects and Flate modules*/
   OBJ_PTR mDobjects = rb_define_module("Dobjects");
   OBJ_PTR mFlate = rb_define_module("Flate");
   rb_include_module(mTioga, mDobjects);
   rb_include_module(mTioga, mFlate);

   cFM = rb_define_class_under(mTioga, "FigureMaker", rb_cObject);

   /* This is necessary to get the tex preamble for the previews*/
   rb_require("Tioga/TexPreamble.rb");

   /* Inclusion of the external objects to get them easily */
   rb_include_module(cFM, mDobjects);
   rb_include_module(cFM, mFlate);

   Init_IDs();
   Init_Font_Dictionary();
   rb_define_method(cFM, "private_make", FM_private_make, 2);
   rb_define_method(cFM, "get_save_filename", FM_get_save_filename, 1);
   rb_define_method(cFM, "private_make_portfolio", FM_private_make_portfolio, 3);
   rb_define_method(cFM, "private_init_fm_data", FM_private_init_fm_data, 1);

/* page attribute accessors */
   attr_reader(root_figure)
   attr_reader(in_subplot)
   attr_reader(scaling_factor)
   attr_reader(page_left)
   attr_reader(page_right)
   attr_reader(page_bottom)
   attr_reader(page_top)
   attr_reader(page_width)
   attr_reader(page_height)
/* frame attribute accessors */
   attr_reader(frame_left)
   attr_reader(frame_right)
   attr_reader(frame_bottom)
   attr_reader(frame_top)
   attr_reader(frame_width)
   attr_reader(frame_height)
/* bounds attribute accessors */
   attr_reader(bounds_left)
   attr_reader(bounds_right)
   attr_reader(bounds_bottom)
   attr_reader(bounds_top)
   attr_reader(bounds_width)
   attr_reader(bounds_height)
   attr_reader(bounds_xmin)
   attr_reader(bounds_xmax)
   attr_reader(bounds_ymin)
   attr_reader(bounds_ymax)
   attr_reader(xaxis_reversed)
   attr_reader(yaxis_reversed)
/* text attribute accessors */
   attr_reader(default_font_size)
   attr_reader(default_text_scale)
   attr_accessors(justification)
   attr_accessors(alignment)
   attr_reader(default_text_height_dx)
   attr_reader(default_text_height_dy)
   attr_accessors(label_left_margin)
   attr_accessors(label_right_margin)
   attr_accessors(label_top_margin)
   attr_accessors(label_bottom_margin)
   attr_accessors(text_shift_on_left)
   attr_accessors(text_shift_on_right)
   attr_accessors(text_shift_on_top)
   attr_accessors(text_shift_on_bottom)
   attr_accessors(text_shift_from_x_origin)
   attr_accessors(text_shift_from_y_origin)
/* graphics attribute accessors */
   attr_reader(default_line_scale)
   attr_accessors(stroke_color)
   attr_accessors(fill_color)
   attr_accessors(line_width)
   attr_accessors(line_cap)
   attr_accessors(line_join)
   rb_define_method(cFM, "line_type_set", FM_line_type_set, 1);
   attr_accessors(miter_limit)
   attr_accessors(stroke_opacity)
   attr_accessors(fill_opacity)
/* croak on non ok */
   attr_accessors(croak_on_nonok_numbers)

/* methods */
   rb_define_method(cFM, "pdf_gsave", FM_pdf_gsave, 0);
   rb_define_method(cFM, "pdf_grestore", FM_pdf_grestore, 0);
   rb_define_method(cFM, "private_set_bounds", FM_private_set_bounds, 4);
   rb_define_method(cFM, "private_set_subframe", FM_private_set_subframe, 4);
   rb_define_method(cFM, "doing_subfigure", FM_doing_subfigure, 0);
/* colors */
   
   rb_define_method(cFM, "hls_to_rgb", FM_hls_to_rgb, 1);
   rb_define_method(cFM, "rgb_to_hls", FM_rgb_to_hls, 1);
   rb_define_method(cFM, "string_hls_to_rgb!", FM_string_hls_to_rgb_bang, 1);
   rb_define_method(cFM, "string_rgb_to_hls!", FM_string_rgb_to_hls_bang, 1);
   /* We define them as class methods too, as they do not depend on a
      FigureMaker instance*/

   rb_define_singleton_method(cFM, "hls_to_rgb", FM_hls_to_rgb, 1);
   rb_define_singleton_method(cFM, "rgb_to_hls", FM_rgb_to_hls, 1);
   rb_define_singleton_method(cFM, "string_hls_to_rgb!", FM_string_hls_to_rgb_bang, 1);
   rb_define_singleton_method(cFM, "string_rgb_to_hls!", FM_string_rgb_to_hls_bang, 1);


/* coordinate system conversions */
   rb_define_method(cFM, "convert_inches_to_output", FM_convert_inches_to_output, 1);
   rb_define_method(cFM, "convert_output_to_inches", FM_convert_output_to_inches, 1);
   rb_define_method(cFM, "convert_mm_to_output", FM_convert_mm_to_output, 1);
   rb_define_method(cFM, "convert_output_to_mm", FM_convert_output_to_mm, 1);
   rb_define_method(cFM, "convert_page_to_output_x", FM_convert_page_to_output_x, 1);
   rb_define_method(cFM, "convert_page_to_output_y", FM_convert_page_to_output_y, 1);
   rb_define_method(cFM, "convert_page_to_output_dx", FM_convert_page_to_output_dx, 1);
   rb_define_method(cFM, "convert_page_to_output_dy", FM_convert_page_to_output_dy, 1);
   rb_define_method(cFM, "convert_output_to_page_x", FM_convert_output_to_page_x, 1);
   rb_define_method(cFM, "convert_output_to_page_y", FM_convert_output_to_page_y, 1);
   rb_define_method(cFM, "convert_output_to_page_dx", FM_convert_output_to_page_dx, 1);
   rb_define_method(cFM, "convert_output_to_page_dy", FM_convert_output_to_page_dy, 1);
   rb_define_method(cFM, "convert_page_to_frame_x", FM_convert_page_to_frame_x, 1);
   rb_define_method(cFM, "convert_page_to_frame_y", FM_convert_page_to_frame_y, 1);
   rb_define_method(cFM, "convert_page_to_frame_dx", FM_convert_page_to_frame_dx, 1);
   rb_define_method(cFM, "convert_page_to_frame_dy", FM_convert_page_to_frame_dy, 1);
   rb_define_method(cFM, "convert_frame_to_page_x", FM_convert_frame_to_page_x, 1);
   rb_define_method(cFM, "convert_frame_to_page_y", FM_convert_frame_to_page_y, 1);
   rb_define_method(cFM, "convert_frame_to_page_dx", FM_convert_frame_to_page_dx, 1);
   rb_define_method(cFM, "convert_frame_to_page_dy", FM_convert_frame_to_page_dy, 1);
   rb_define_method(cFM, "convert_figure_to_frame_x", FM_convert_figure_to_frame_x, 1);
   rb_define_method(cFM, "convert_figure_to_frame_y", FM_convert_figure_to_frame_y, 1);
   rb_define_method(cFM, "convert_figure_to_frame_dx", FM_convert_figure_to_frame_dx, 1);
   rb_define_method(cFM, "convert_figure_to_frame_dy", FM_convert_figure_to_frame_dy, 1);
   rb_define_method(cFM, "convert_frame_to_figure_x", FM_convert_frame_to_figure_x, 1);
   rb_define_method(cFM, "convert_frame_to_figure_y", FM_convert_frame_to_figure_y, 1);
   rb_define_method(cFM, "convert_frame_to_figure_dx", FM_convert_frame_to_figure_dx, 1);
   rb_define_method(cFM, "convert_frame_to_figure_dy", FM_convert_frame_to_figure_dy, 1);
   rb_define_method(cFM, "convert_figure_to_output_x", FM_convert_figure_to_output_x, 1);
   rb_define_method(cFM, "convert_figure_to_output_y", FM_convert_figure_to_output_y, 1);
   rb_define_method(cFM, "convert_figure_to_output_dx", FM_convert_figure_to_output_dx, 1);
   rb_define_method(cFM, "convert_figure_to_output_dy", FM_convert_figure_to_output_dy, 1);
   rb_define_method(cFM, "convert_output_to_figure_x", FM_convert_output_to_figure_x, 1);
   rb_define_method(cFM, "convert_output_to_figure_y", FM_convert_output_to_figure_y, 1);
   rb_define_method(cFM, "convert_output_to_figure_dx", FM_convert_output_to_figure_dx, 1);
   rb_define_method(cFM, "convert_output_to_figure_dy", FM_convert_output_to_figure_dy, 1);
   rb_define_method(cFM, "convert_to_degrees", FM_convert_to_degrees, 2);
/* text */
   rb_define_method(cFM, "private_set_default_font_size", FM_private_set_default_font_size, 1);
   rb_define_method(cFM, "rescale_text", FM_rescale_text, 1);
   rb_define_method(cFM, "show_rotated_text", FM_show_rotated_text, 9);
   rb_define_method(cFM, "show_rotated_label", FM_show_rotated_label, 8);
   rb_define_method(cFM, "check_label_clip", FM_check_label_clip, 2);
/* text measurements */
   rb_define_method(cFM, "private_save_measure", FM_private_save_measure, 4);
/* path construction */
   rb_define_method(cFM, "move_to_point", FM_move_to_point, 2);
   rb_define_method(cFM, "append_point_to_path", FM_append_point_to_path, 2);
   rb_define_method(cFM, "bezier_control_points", FM_bezier_control_points, 6);
   rb_define_method(cFM, "append_curve_to_path", FM_append_curve_to_path, 6);
   rb_define_method(cFM, "close_path", FM_close_path, 0);
   rb_define_method(cFM, "append_points_to_path", FM_append_points_to_path, 2);
   rb_define_method(cFM, "private_append_points_with_gaps_to_path", FM_private_append_points_with_gaps_to_path, 4);
   rb_define_method(cFM, "append_arc_to_path", FM_append_arc_to_path, 8);
   rb_define_method(cFM, "append_rect_to_path", FM_append_rect_to_path, 4);
   rb_define_method(cFM, "append_rounded_rect_to_path", FM_append_rounded_rect_to_path, 6);
   rb_define_method(cFM, "append_circle_to_path", FM_append_circle_to_path, 3);
   rb_define_method(cFM, "append_oval_to_path", FM_append_oval_to_path, 5);
   rb_define_method(cFM, "append_frame_to_path", FM_append_frame_to_path, 0);
   rb_define_method(cFM, "update_bbox", FM_update_bbox, 2);
   rb_define_method(cFM, "bbox_left", FM_bbox_left, 0);
   rb_define_method(cFM, "bbox_right", FM_bbox_right, 0);
   rb_define_method(cFM, "bbox_top", FM_bbox_top, 0);
   rb_define_method(cFM, "bbox_bottom", FM_bbox_bottom, 0);
/* path painting */
   rb_define_method(cFM, "rescale_lines", FM_rescale_lines, 1);
   rb_define_method(cFM, "discard_path", FM_discard_path, 0);
   rb_define_method(cFM, "stroke", FM_stroke, 0);
   rb_define_method(cFM, "close_and_stroke", FM_close_and_stroke, 0);
   rb_define_method(cFM, "fill", FM_fill, 0);
   rb_define_method(cFM, "eofill", FM_eofill, 0);
   rb_define_method(cFM, "fill_and_stroke", FM_fill_and_stroke, 0);
   rb_define_method(cFM, "eofill_and_stroke", FM_eofill_and_stroke, 0);
   rb_define_method(cFM, "close_fill_and_stroke", FM_close_fill_and_stroke, 0);
   rb_define_method(cFM, "close_eofill_and_stroke", FM_close_eofill_and_stroke, 0);
   rb_define_method(cFM, "clip", FM_clip, 0);
   rb_define_method(cFM, "eoclip", FM_eoclip, 0);

   rb_define_method(cFM, "stroke_line", FM_stroke_line, 4);
   rb_define_method(cFM, "fill_rect", FM_fill_rect, 4);
   rb_define_method(cFM, "stroke_rect", FM_stroke_rect, 4);
   rb_define_method(cFM, "fill_and_stroke_rect", FM_fill_and_stroke_rect, 4);
   rb_define_method(cFM, "clip_rect", FM_clip_rect, 4);
   rb_define_method(cFM, "stroke_frame", FM_stroke_frame, 0);
   rb_define_method(cFM, "fill_frame", FM_fill_frame, 0);
   rb_define_method(cFM, "fill_and_stroke_frame", FM_fill_and_stroke_frame, 0);
   rb_define_method(cFM, "clip_circle", FM_clip_circle, 3);
   rb_define_method(cFM, "clip_to_frame", FM_clip_to_frame, 0);
   rb_define_method(cFM, "fill_circle", FM_fill_circle, 3);
   rb_define_method(cFM, "stroke_circle", FM_stroke_circle, 3);
   rb_define_method(cFM, "fill_and_stroke_circle", FM_fill_and_stroke_circle, 3);
   rb_define_method(cFM, "clip_oval", FM_clip_oval, 5);
   rb_define_method(cFM, "fill_oval", FM_fill_oval, 5);
   rb_define_method(cFM, "stroke_oval", FM_stroke_oval, 5);
   rb_define_method(cFM, "fill_and_stroke_oval", FM_fill_and_stroke_oval, 5);
   rb_define_method(cFM, "rounded_rect_oval", FM_clip_rounded_rect, 6);
   rb_define_method(cFM, "fill_rounded_rect", FM_fill_rounded_rect, 6);
   rb_define_method(cFM, "stroke_rounded_rect", FM_stroke_rounded_rect, 6);
   rb_define_method(cFM, "fill_and_stroke_rounded_rect", FM_fill_and_stroke_rounded_rect, 6);
/* shading */
   rb_define_method(cFM, "private_axial_shading", FM_private_axial_shading, 7);
   rb_define_method(cFM, "private_radial_shading", FM_private_radial_shading, 13);
/* markers */
   rb_define_method(cFM, "register_font", FM_register_font, 1);
   rb_define_method(cFM, "private_show_marker", FM_private_show_marker, 1);
   rb_define_method(cFM, "marker_string_info", FM_marker_string_info, 3);
/* images */

   rb_define_method(cFM, "private_register_jpg", FM_private_register_jpg, 4);
   rb_define_method(cFM, "jpg_info", FM_jpg_info, 1);
   rb_define_method(cFM, "private_register_rgb_image", FM_private_register_rgb_image, 7);
   rb_define_method(cFM, "private_register_hls_image", FM_private_register_hls_image, 7);
   rb_define_method(cFM, "private_register_cmyk_image", FM_private_register_cmyk_image, 7);
   rb_define_method(cFM, "private_register_grayscale_image", FM_private_register_grayscale_image, 7);
   rb_define_method(cFM, "private_register_monochrome_image", FM_private_register_monochrome_image, 7);
   rb_define_method(cFM, "private_register_image", FM_private_register_image, 11);
   rb_define_method(cFM, "private_show_image_from_ref", FM_private_show_image_from_ref, 7);


   rb_define_method(cFM, "private_create_image_data", FM_private_create_image_data, 10);
   rb_define_method(cFM, "private_create_monochrome_image_data", FM_private_create_monochrome_image_data, 7);
/* colormaps */
   rb_define_method(cFM, "private_create_colormap", FM_private_create_colormap, 6);
   rb_define_method(cFM, "convert_to_colormap", FM_convert_to_colormap, 3);
   rb_define_method(cFM, "get_color_from_colormap", FM_get_color_from_colormap, 2);
/* plots */
   rb_define_method(cFM, "set_frame_sides", FM_set_frame_sides, 4);
   rb_define_method(cFM, "set_device_pagesize", FM_set_device_pagesize, 2);
   rb_define_method(cFM, "doing_subplot", FM_doing_subplot, 0);
   rb_define_method(cFM, "show_axis", FM_show_axis, 1);
   rb_define_method(cFM, "show_edge", FM_show_edge, 1);
   rb_define_method(cFM, "axis_information", FM_axis_information, 1);
   rb_define_method(cFM, "no_title", FM_no_title, 0);
   rb_define_method(cFM, "no_xlabel", FM_no_xlabel, 0);
   rb_define_method(cFM, "no_ylabel", FM_no_ylabel, 0);
   rb_define_method(cFM, "no_xaxis", FM_no_xaxis, 0);
   rb_define_method(cFM, "no_yaxis", FM_no_yaxis, 0);
   rb_define_method(cFM, "no_left_edge", FM_no_left_edge, 0);
   rb_define_method(cFM, "no_right_edge", FM_no_right_edge, 0);
   rb_define_method(cFM, "no_top_edge", FM_no_top_edge, 0);
   rb_define_method(cFM, "no_bottom_edge", FM_no_bottom_edge, 0);
/* makers */
   rb_define_singleton_method(cFM, "private_make_contour", 
			      FM_private_make_contour, 7);

   rb_define_method(cFM, "private_make_spline_interpolated_points", FM_private_make_spline_interpolated_points, 5);
   rb_define_method(cFM, "private_make_steps", FM_private_make_steps, 6);
   
/* Title */
   attr_reader(title_visible)
   attr_accessors(title_side)
   attr_accessors(title_position)
   attr_accessors(title_scale)
   attr_accessors(title_shift)
   attr_accessors(title_angle)
   attr_accessors(title_alignment)
   attr_accessors(title_justification)
   attr_accessors(title_color)
    
/* X label */
   attr_reader(xlabel_visible)
   attr_accessors(xlabel_position)
   attr_accessors(xlabel_scale)
   attr_accessors(xlabel_shift)
   attr_accessors(xlabel_angle)
   attr_accessors(xlabel_side)
   attr_accessors(xlabel_alignment)
   attr_accessors(xlabel_justification)
   attr_accessors(xlabel_color)
    
/* Y label */
   attr_reader(ylabel_visible)
   attr_accessors(ylabel_position)
   attr_accessors(ylabel_scale)
   attr_accessors(ylabel_shift)
   attr_accessors(ylabel_angle)
   attr_accessors(ylabel_side)
   attr_accessors(ylabel_alignment)
   attr_accessors(ylabel_justification)
   attr_accessors(ylabel_color)
    
/* X axis */
   attr_reader(xaxis_visible)
   attr_accessors(xaxis_loc)
   attr_accessors(xaxis_type)
   attr_accessors(xaxis_line_width)
   attr_accessors(xaxis_stroke_color)
   attr_accessors(xaxis_labels_color)
   attr_accessors(xaxis_major_tick_width)
   attr_accessors(xaxis_minor_tick_width)
   attr_accessors(xaxis_major_tick_length)
   attr_accessors(xaxis_minor_tick_length)
   attr_accessors(xaxis_log_values)
   attr_accessors(xaxis_ticks_inside)
   attr_accessors(xaxis_ticks_outside)
   attr_accessors(xaxis_tick_interval)
   attr_accessors(xaxis_min_between_major_ticks)
   attr_accessors(xaxis_number_of_minor_intervals)
   attr_accessors(xaxis_use_fixed_pt)
   attr_accessors(xaxis_digits_max)
   attr_accessors(xaxis_numeric_label_decimal_digits)
   attr_accessors(xaxis_numeric_label_scale)
   attr_accessors(xaxis_numeric_label_shift)
   attr_accessors(xaxis_numeric_label_angle)
   attr_accessors(xaxis_numeric_label_alignment)
   attr_accessors(xaxis_numeric_label_justification)
   attr_accessors(xaxis_numeric_label_frequency)
   attr_accessors(xaxis_numeric_label_phase)
   attr_accessors(top_edge_type)
   attr_reader(top_edge_visible)
   attr_accessors(bottom_edge_type)
   attr_reader(bottom_edge_visible)
    
/* Y axis */
   attr_reader(yaxis_visible)
   attr_accessors(yaxis_loc)
   attr_accessors(yaxis_type)
   attr_accessors(yaxis_line_width)
   attr_accessors(yaxis_stroke_color)
   attr_accessors(yaxis_labels_color)
   attr_accessors(yaxis_major_tick_width)
   attr_accessors(yaxis_minor_tick_width)
   attr_accessors(yaxis_major_tick_length)
   attr_accessors(yaxis_minor_tick_length)
   attr_accessors(yaxis_log_values)
   attr_accessors(yaxis_ticks_inside)
   attr_accessors(yaxis_ticks_outside)
   attr_accessors(yaxis_tick_interval)
   attr_accessors(yaxis_min_between_major_ticks)
   attr_accessors(yaxis_number_of_minor_intervals)
   attr_accessors(yaxis_use_fixed_pt)
   attr_accessors(yaxis_digits_max)
   attr_accessors(yaxis_numeric_label_decimal_digits)
   attr_accessors(yaxis_numeric_label_scale)
   attr_accessors(yaxis_numeric_label_shift)
   attr_accessors(yaxis_numeric_label_angle)
   attr_accessors(yaxis_numeric_label_alignment)
   attr_accessors(yaxis_numeric_label_justification)
   attr_accessors(yaxis_numeric_label_frequency)
   attr_accessors(yaxis_numeric_label_phase)
   attr_accessors(left_edge_type)
   attr_reader(left_edge_visible)
   attr_accessors(right_edge_type)
   attr_reader(right_edge_visible)


/* Major ticks algorithm */
   attr_accessors(vincent_or_bill)
   attr_accessors(log_minor_ticks_limit)
    
/* Legend */
   attr_accessors(legend_text_width)
   attr_accessors(legend_line_x0)
   attr_accessors(legend_line_x1)
   attr_accessors(legend_line_dy)
   attr_accessors(legend_text_xstart)
   attr_accessors(legend_text_ystart)
   attr_accessors(legend_text_dy)
   attr_accessors(legend_line_width)
   attr_accessors(legend_scale)
   attr_accessors(legend_alignment)
   attr_accessors(legend_justification)
   
/* Debugging */
   attr_accessors(debug_verbosity_level)
      
      
   Set_fm_data_size(); // must set this before create a FigureMaker instance
   rb_require("Tioga/FigMkr.rb");
   /* We now need to import the symbols */

   /* imports from Dvector */
   OBJ_PTR cDvector = rb_define_class_under(mDobjects, "Dvector", rb_cObject);
   RB_IMPORT_SYMBOL(cDvector, Dvector_Create);
   RB_IMPORT_SYMBOL(cDvector, Dvector_Data_Resize);
   RB_IMPORT_SYMBOL(cDvector, Dvector_Data_Replace);
   RB_IMPORT_SYMBOL(cDvector, isa_Dvector);
   RB_IMPORT_SYMBOL(cDvector, len_Dvector);
   RB_IMPORT_SYMBOL(cDvector, access_Dvector);
   RB_IMPORT_SYMBOL(cDvector, Dvector_Data_for_Read);
   RB_IMPORT_SYMBOL(cDvector, Dvector_Data_for_Write);
   RB_IMPORT_SYMBOL(cDvector, Dvector_Store_Double);
   RB_IMPORT_SYMBOL(cDvector, c_dvector_spline_interpolate);
   RB_IMPORT_SYMBOL(cDvector, c_dvector_linear_interpolate);
   RB_IMPORT_SYMBOL(cDvector, c_dvector_create_spline_interpolant);

   /* imports from Flate */
   RB_IMPORT_SYMBOL(mFlate, flate_compress);
   RB_IMPORT_SYMBOL(mFlate, flate_expand);

   /* imports from Dtable */
   OBJ_PTR cDtable = rb_define_class_under(mDobjects, "Dtable", rb_cObject);
   RB_IMPORT_SYMBOL(cDtable, Dtable_Ptr);
   RB_IMPORT_SYMBOL(cDtable, Read_Dtable);
}

/* implementation of the various functions */
IMPLEMENT_SYMBOL(Dvector_Create);
IMPLEMENT_SYMBOL(Dvector_Data_Resize);
IMPLEMENT_SYMBOL(Dvector_Data_Replace);
IMPLEMENT_SYMBOL(isa_Dvector);
IMPLEMENT_SYMBOL(len_Dvector);
IMPLEMENT_SYMBOL(access_Dvector);
IMPLEMENT_SYMBOL(Dvector_Data_for_Read);
IMPLEMENT_SYMBOL(Dvector_Data_for_Write);
IMPLEMENT_SYMBOL(Dvector_Store_Double);
IMPLEMENT_SYMBOL(c_dvector_spline_interpolate);
IMPLEMENT_SYMBOL(c_dvector_linear_interpolate);
IMPLEMENT_SYMBOL(c_dvector_create_spline_interpolant);

IMPLEMENT_SYMBOL(flate_compress);
IMPLEMENT_SYMBOL(flate_expand);

IMPLEMENT_SYMBOL(Dtable_Ptr);
IMPLEMENT_SYMBOL(Read_Dtable);


