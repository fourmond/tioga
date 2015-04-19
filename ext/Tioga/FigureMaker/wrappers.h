/* wrappers.h */
/*
   Copyright (C) 2007  Bill Paxton

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

#ifndef __wrappers_H__
#define __wrappers_H__

#include "figures.h"
#include "generic.h"


// These are the Ruby wrappers
 
// They provide the transition from the Ruby interpreter
// to the c routines.


/*======================================================================*/
// axes.c
extern OBJ_PTR FM_show_axis(OBJ_PTR fmkr, OBJ_PTR loc);
extern OBJ_PTR FM_show_edge(OBJ_PTR fmkr, OBJ_PTR loc);
extern OBJ_PTR FM_no_title(OBJ_PTR fmkr);
extern OBJ_PTR FM_no_xlabel(OBJ_PTR fmkr);
extern OBJ_PTR FM_no_ylabel(OBJ_PTR fmkr);
extern OBJ_PTR FM_no_xaxis(OBJ_PTR fmkr);
extern OBJ_PTR FM_no_yaxis(OBJ_PTR fmkr);
extern OBJ_PTR FM_no_left_edge(OBJ_PTR fmkr);
extern OBJ_PTR FM_no_right_edge(OBJ_PTR fmkr);
extern OBJ_PTR FM_no_top_edge(OBJ_PTR fmkr);
extern OBJ_PTR FM_no_bottom_edge(OBJ_PTR fmkr);
extern OBJ_PTR FM_axis_information(OBJ_PTR fmkr, OBJ_PTR loc);


/*======================================================================*/
// init.c
extern OBJ_PTR FM_private_init_fm_data(OBJ_PTR fmkr, OBJ_PTR scaling);
extern OBJ_PTR FM_set_frame_sides(OBJ_PTR fmkr, OBJ_PTR left, OBJ_PTR right, OBJ_PTR top, OBJ_PTR bottom); // in page coords [0..1]
extern OBJ_PTR FM_set_device_pagesize(OBJ_PTR fmkr, OBJ_PTR width, OBJ_PTR height); // size in output coords (decipoints)
extern OBJ_PTR FM_get_save_filename(OBJ_PTR fmkr, OBJ_PTR name);
extern OBJ_PTR FM_private_make(OBJ_PTR fmkr, OBJ_PTR name, OBJ_PTR cmd);
extern OBJ_PTR FM_private_make_portfolio(OBJ_PTR fmkr, OBJ_PTR name, OBJ_PTR fignums, OBJ_PTR fignames);

/*======================================================================*/
// makers.c
extern OBJ_PTR FM_private_make_contour(OBJ_PTR fmkr, OBJ_PTR gaps,
     OBJ_PTR xs, OBJ_PTR ys, OBJ_PTR zs, OBJ_PTR z_level, OBJ_PTR legit, OBJ_PTR method);
extern OBJ_PTR FM_private_make_steps(OBJ_PTR fmkr, OBJ_PTR Xdata, OBJ_PTR Ydata,
    OBJ_PTR xfirst, OBJ_PTR yfirst, OBJ_PTR xlast, OBJ_PTR ylast);
extern OBJ_PTR FM_private_make_spline_interpolated_points(OBJ_PTR fmkr, OBJ_PTR Xvec, 
   OBJ_PTR Xdata, OBJ_PTR Ydata, OBJ_PTR start_slope, OBJ_PTR end_slope);

/*======================================================================*/
// pdfcolor.c
extern OBJ_PTR FM_stroke_opacity_set(OBJ_PTR fmkr, OBJ_PTR val);
extern OBJ_PTR FM_fill_opacity_set(OBJ_PTR fmkr, OBJ_PTR val);
extern OBJ_PTR FM_private_axial_shading(OBJ_PTR fmkr, OBJ_PTR x0, OBJ_PTR y0,
        OBJ_PTR x1, OBJ_PTR y1, OBJ_PTR colormap, OBJ_PTR extend_start, OBJ_PTR extend_end);
extern OBJ_PTR FM_private_radial_shading(OBJ_PTR fmkr, OBJ_PTR x0, OBJ_PTR y0, OBJ_PTR r0,
        OBJ_PTR x1, OBJ_PTR y1, OBJ_PTR r1, OBJ_PTR colormap,
        OBJ_PTR a, OBJ_PTR b, OBJ_PTR c, OBJ_PTR d,
        OBJ_PTR extend_start, OBJ_PTR extend_end);
extern OBJ_PTR FM_private_create_colormap(OBJ_PTR fmkr, OBJ_PTR rgb_flag,
            OBJ_PTR length, OBJ_PTR Ps, OBJ_PTR C1s, OBJ_PTR C2s, OBJ_PTR C3s);
extern OBJ_PTR FM_get_color_from_colormap(OBJ_PTR fmkr, OBJ_PTR color_map, OBJ_PTR color_position);
extern OBJ_PTR FM_convert_to_colormap(OBJ_PTR fmkr, OBJ_PTR Rs, OBJ_PTR Gs, OBJ_PTR Bs);
extern OBJ_PTR FM_hls_to_rgb(OBJ_PTR fmkr, OBJ_PTR hls_vec);
extern OBJ_PTR FM_rgb_to_hls(OBJ_PTR fmkr, OBJ_PTR rgb_vec);
extern OBJ_PTR FM_title_color_set(OBJ_PTR fmkr, OBJ_PTR val);
extern OBJ_PTR FM_title_color_get(OBJ_PTR fmkr);
extern OBJ_PTR FM_xlabel_color_get(OBJ_PTR fmkr);
extern OBJ_PTR FM_xlabel_color_set(OBJ_PTR fmkr, OBJ_PTR val);
extern OBJ_PTR FM_ylabel_color_get(OBJ_PTR fmkr);
extern OBJ_PTR FM_ylabel_color_set(OBJ_PTR fmkr, OBJ_PTR val);
extern OBJ_PTR FM_xaxis_stroke_color_get(OBJ_PTR fmkr);
extern OBJ_PTR FM_xaxis_stroke_color_set(OBJ_PTR fmkr, OBJ_PTR val);
extern OBJ_PTR FM_yaxis_stroke_color_get(OBJ_PTR fmkr);
extern OBJ_PTR FM_yaxis_stroke_color_set(OBJ_PTR fmkr, OBJ_PTR val);
extern OBJ_PTR FM_xaxis_labels_color_get(OBJ_PTR fmkr);
extern OBJ_PTR FM_xaxis_labels_color_set(OBJ_PTR fmkr, OBJ_PTR val);
extern OBJ_PTR FM_yaxis_labels_color_get(OBJ_PTR fmkr);
extern OBJ_PTR FM_yaxis_labels_color_set(OBJ_PTR fmkr, OBJ_PTR val);

extern OBJ_PTR FM_string_hls_to_rgb_bang(OBJ_PTR fmkr, OBJ_PTR str);
extern OBJ_PTR FM_string_rgb_to_hls_bang(OBJ_PTR fmkr, OBJ_PTR str);

/*======================================================================*/
// pdfcoords.c
extern OBJ_PTR FM_private_set_subframe(OBJ_PTR fmkr, OBJ_PTR left_margin, OBJ_PTR right_margin, OBJ_PTR top_margin, OBJ_PTR bottom_margin);
extern OBJ_PTR FM_private_set_default_font_size(OBJ_PTR fmkr, OBJ_PTR size); // size in points
extern OBJ_PTR FM_doing_subplot(OBJ_PTR fmkr);
extern OBJ_PTR FM_doing_subfigure(OBJ_PTR fmkr);
extern OBJ_PTR FM_private_set_bounds(OBJ_PTR fmkr, OBJ_PTR left, OBJ_PTR right, OBJ_PTR top, OBJ_PTR bottom); /* in figure coords */
extern OBJ_PTR FM_convert_to_degrees(OBJ_PTR fmkr, OBJ_PTR dx, OBJ_PTR dy);
extern OBJ_PTR FM_convert_inches_to_output(OBJ_PTR fmkr, OBJ_PTR val);
extern OBJ_PTR FM_convert_output_to_inches(OBJ_PTR fmkr, OBJ_PTR val);
extern OBJ_PTR FM_convert_mm_to_output(OBJ_PTR fmkr, OBJ_PTR val);
extern OBJ_PTR FM_convert_output_to_mm(OBJ_PTR fmkr, OBJ_PTR val);
extern OBJ_PTR FM_convert_page_to_output_x(OBJ_PTR fmkr, OBJ_PTR val);
extern OBJ_PTR FM_convert_page_to_output_y(OBJ_PTR fmkr, OBJ_PTR val);
extern OBJ_PTR FM_convert_page_to_output_dx(OBJ_PTR fmkr, OBJ_PTR val);
extern OBJ_PTR FM_convert_page_to_output_dy(OBJ_PTR fmkr, OBJ_PTR val);
extern OBJ_PTR FM_convert_output_to_page_x(OBJ_PTR fmkr, OBJ_PTR val);
extern OBJ_PTR FM_convert_output_to_page_y(OBJ_PTR fmkr, OBJ_PTR val);
extern OBJ_PTR FM_convert_output_to_page_dx(OBJ_PTR fmkr, OBJ_PTR val);
extern OBJ_PTR FM_convert_output_to_page_dy(OBJ_PTR fmkr, OBJ_PTR val);
extern OBJ_PTR FM_convert_frame_to_page_x(OBJ_PTR fmkr, OBJ_PTR val);
extern OBJ_PTR FM_convert_frame_to_page_y(OBJ_PTR fmkr, OBJ_PTR val);
extern OBJ_PTR FM_convert_frame_to_page_dx(OBJ_PTR fmkr, OBJ_PTR val);
extern OBJ_PTR FM_convert_frame_to_page_dy(OBJ_PTR fmkr, OBJ_PTR val);
extern OBJ_PTR FM_convert_page_to_frame_x(OBJ_PTR fmkr, OBJ_PTR val);
extern OBJ_PTR FM_convert_page_to_frame_y(OBJ_PTR fmkr, OBJ_PTR val);
extern OBJ_PTR FM_convert_page_to_frame_dx(OBJ_PTR fmkr, OBJ_PTR val);
extern OBJ_PTR FM_convert_page_to_frame_dy(OBJ_PTR fmkr, OBJ_PTR val);
extern OBJ_PTR FM_convert_figure_to_frame_x(OBJ_PTR fmkr, OBJ_PTR val);
extern OBJ_PTR FM_convert_figure_to_frame_y(OBJ_PTR fmkr, OBJ_PTR val);
extern OBJ_PTR FM_convert_figure_to_frame_dx(OBJ_PTR fmkr, OBJ_PTR val);
extern OBJ_PTR FM_convert_figure_to_frame_dy(OBJ_PTR fmkr, OBJ_PTR val);
extern OBJ_PTR FM_convert_frame_to_figure_x(OBJ_PTR fmkr, OBJ_PTR val);
extern OBJ_PTR FM_convert_frame_to_figure_y(OBJ_PTR fmkr, OBJ_PTR val);
extern OBJ_PTR FM_convert_frame_to_figure_dx(OBJ_PTR fmkr, OBJ_PTR val);
extern OBJ_PTR FM_convert_frame_to_figure_dy(OBJ_PTR fmkr, OBJ_PTR val);
extern OBJ_PTR FM_convert_figure_to_output_x(OBJ_PTR fmkr, OBJ_PTR val);
extern OBJ_PTR FM_convert_figure_to_output_y(OBJ_PTR fmkr, OBJ_PTR val);
extern OBJ_PTR FM_convert_figure_to_output_dx(OBJ_PTR fmkr, OBJ_PTR val);
extern OBJ_PTR FM_convert_figure_to_output_dy(OBJ_PTR fmkr, OBJ_PTR val);
extern OBJ_PTR FM_convert_output_to_figure_x(OBJ_PTR fmkr, OBJ_PTR val);
extern OBJ_PTR FM_convert_output_to_figure_y(OBJ_PTR fmkr, OBJ_PTR val);
extern OBJ_PTR FM_convert_output_to_figure_dx(OBJ_PTR fmkr, OBJ_PTR val);
extern OBJ_PTR FM_convert_output_to_figure_dy(OBJ_PTR fmkr, OBJ_PTR val);

/*======================================================================*/
// pdffile.c
extern OBJ_PTR FM_pdf_gsave(OBJ_PTR fmkr);
extern OBJ_PTR FM_pdf_grestore(OBJ_PTR fmkr);

/*======================================================================*/
// pdfimage.c
extern OBJ_PTR FM_private_create_image_data(OBJ_PTR fmkr, OBJ_PTR data,
   OBJ_PTR first_row, OBJ_PTR last_row, OBJ_PTR first_column, OBJ_PTR last_column,
   OBJ_PTR min_OBJ_PTR, OBJ_PTR max_OBJ_PTR, OBJ_PTR max_code, OBJ_PTR if_below_range, OBJ_PTR if_above_range);
extern OBJ_PTR FM_private_create_monochrome_image_data(OBJ_PTR fmkr, OBJ_PTR data,
   OBJ_PTR first_row, OBJ_PTR last_row, OBJ_PTR first_column, OBJ_PTR last_column,
   OBJ_PTR boundary, OBJ_PTR reverse);

extern OBJ_PTR FM_private_show_image_from_ref(OBJ_PTR fmkr, OBJ_PTR ref, OBJ_PTR llx, OBJ_PTR lly, OBJ_PTR lrx, OBJ_PTR lry, OBJ_PTR ulx, OBJ_PTR uly);

extern OBJ_PTR FM_private_register_jpg(OBJ_PTR fmkr, OBJ_PTR filename, 
                                       OBJ_PTR width, OBJ_PTR height, 
                                       OBJ_PTR mask_xo_num);

extern OBJ_PTR FM_jpg_info(OBJ_PTR fmkr, OBJ_PTR filename);

extern OBJ_PTR FM_private_register_image(OBJ_PTR fmkr,
                                         OBJ_PTR interpolate, OBJ_PTR w, OBJ_PTR h, OBJ_PTR data, OBJ_PTR OBJ_PTR_mask_min, OBJ_PTR OBJ_PTR_mask_max,
   OBJ_PTR hival, OBJ_PTR lookup, OBJ_PTR mask_xo_num, OBJ_PTR components);

extern OBJ_PTR FM_private_register_hls_image(OBJ_PTR fmkr, 
   OBJ_PTR interpolate, OBJ_PTR w, OBJ_PTR h, OBJ_PTR data, OBJ_PTR mask_xo_num, OBJ_PTR components);
extern OBJ_PTR FM_private_register_rgb_image(OBJ_PTR fmkr, 
   OBJ_PTR interpolate, OBJ_PTR w, OBJ_PTR h, OBJ_PTR data, OBJ_PTR mask_xo_num, OBJ_PTR components);
extern OBJ_PTR FM_private_register_cmyk_image(OBJ_PTR fmkr, 
   OBJ_PTR interpolate, OBJ_PTR w, OBJ_PTR h, OBJ_PTR data, OBJ_PTR mask_xo_num, OBJ_PTR components);
extern OBJ_PTR FM_private_register_grayscale_image(OBJ_PTR fmkr, 
   OBJ_PTR interpolate, OBJ_PTR w, OBJ_PTR h, OBJ_PTR data, OBJ_PTR mask_xo_num, OBJ_PTR components);
extern OBJ_PTR FM_private_register_monochrome_image(OBJ_PTR fmkr, 
   OBJ_PTR interpolate, OBJ_PTR reversed, OBJ_PTR w, OBJ_PTR h, OBJ_PTR data, OBJ_PTR mask_xo_num);

/*======================================================================*/
// pdfpath.c
extern OBJ_PTR FM_stroke_color_set(OBJ_PTR fmkr, OBJ_PTR val);
extern OBJ_PTR FM_stroke_color_get(OBJ_PTR fmkr);
extern OBJ_PTR FM_fill_color_set(OBJ_PTR fmkr, OBJ_PTR val);
extern OBJ_PTR FM_fill_color_get(OBJ_PTR fmkr);
extern OBJ_PTR FM_line_width_set(OBJ_PTR fmkr, OBJ_PTR val);
extern OBJ_PTR FM_rescale_lines(OBJ_PTR fmkr, OBJ_PTR scaling);
extern OBJ_PTR FM_line_cap_set(OBJ_PTR fmkr, OBJ_PTR val);
extern OBJ_PTR FM_line_join_set(OBJ_PTR fmkr, OBJ_PTR val);
extern OBJ_PTR FM_miter_limit_set(OBJ_PTR fmkr, OBJ_PTR val);
extern OBJ_PTR FM_line_type_set(OBJ_PTR fmkr, OBJ_PTR line_type);
extern OBJ_PTR FM_update_bbox(OBJ_PTR fmkr, OBJ_PTR x, OBJ_PTR y);
extern OBJ_PTR FM_bbox_left(OBJ_PTR fmkr);
extern OBJ_PTR FM_bbox_right(OBJ_PTR fmkr);
extern OBJ_PTR FM_bbox_top(OBJ_PTR fmkr);
extern OBJ_PTR FM_bbox_bottom(OBJ_PTR fmkr);
extern OBJ_PTR FM_move_to_point(OBJ_PTR fmkr, OBJ_PTR x, OBJ_PTR y);  // x y m
extern OBJ_PTR FM_append_point_to_path(OBJ_PTR fmkr, OBJ_PTR x, OBJ_PTR y);  // x y l
extern OBJ_PTR FM_bezier_control_points(OBJ_PTR fmkr, OBJ_PTR x0, OBJ_PTR y0, OBJ_PTR delta_x, OBJ_PTR a, OBJ_PTR b, OBJ_PTR c);
extern OBJ_PTR FM_append_curve_to_path(OBJ_PTR fmkr, 
   OBJ_PTR x1, OBJ_PTR y1, OBJ_PTR x2, OBJ_PTR y2, OBJ_PTR x3, OBJ_PTR y3); 
extern OBJ_PTR FM_close_path(OBJ_PTR fmkr); // h
extern OBJ_PTR FM_append_arc_to_path(OBJ_PTR fmkr, OBJ_PTR x_start, OBJ_PTR y_start, OBJ_PTR x_corner, OBJ_PTR y_corner,
   OBJ_PTR x_end, OBJ_PTR y_end, OBJ_PTR dx, OBJ_PTR dy);
extern OBJ_PTR FM_append_rect_to_path(OBJ_PTR fmkr, OBJ_PTR x, OBJ_PTR y, OBJ_PTR width, OBJ_PTR height);  // x y w h re
extern OBJ_PTR FM_append_rounded_rect_to_path(OBJ_PTR fmkr, OBJ_PTR x, OBJ_PTR y, OBJ_PTR width, OBJ_PTR height, OBJ_PTR dx, OBJ_PTR dy);
extern OBJ_PTR FM_append_oval_to_path(OBJ_PTR fmkr, OBJ_PTR x, OBJ_PTR y, OBJ_PTR dx, OBJ_PTR dy, OBJ_PTR angle);
extern OBJ_PTR FM_append_circle_to_path(OBJ_PTR fmkr, OBJ_PTR x, OBJ_PTR y, OBJ_PTR dx);
extern OBJ_PTR FM_append_points_to_path(OBJ_PTR fmkr, OBJ_PTR x_vec, OBJ_PTR y_vec);
extern OBJ_PTR FM_private_append_points_with_gaps_to_path(OBJ_PTR fmkr, OBJ_PTR x_vec, OBJ_PTR y_vec, OBJ_PTR gaps, OBJ_PTR close_gaps);
extern OBJ_PTR FM_stroke(OBJ_PTR fmkr);  // S
extern OBJ_PTR FM_close_and_stroke(OBJ_PTR fmkr);  // s
extern OBJ_PTR FM_fill(OBJ_PTR fmkr); // f
extern OBJ_PTR FM_discard_path(OBJ_PTR fmkr); // n
extern OBJ_PTR FM_eofill(OBJ_PTR fmkr); // f*
extern OBJ_PTR FM_fill_and_stroke(OBJ_PTR fmkr); // B
extern OBJ_PTR FM_eofill_and_stroke(OBJ_PTR fmkr); // B*
extern OBJ_PTR FM_close_fill_and_stroke(OBJ_PTR fmkr); // b
extern OBJ_PTR FM_close_eofill_and_stroke(OBJ_PTR fmkr); // b*
extern OBJ_PTR FM_clip(OBJ_PTR fmkr);  // W n
extern OBJ_PTR FM_eoclip(OBJ_PTR fmkr);  // W* n
extern OBJ_PTR FM_stroke_line(OBJ_PTR fmkr, OBJ_PTR x1, OBJ_PTR y1, OBJ_PTR x2, OBJ_PTR y2);
extern OBJ_PTR FM_fill_rect(OBJ_PTR fmkr, OBJ_PTR x, OBJ_PTR y, OBJ_PTR width, OBJ_PTR height);
extern OBJ_PTR FM_stroke_rect(OBJ_PTR fmkr, OBJ_PTR x, OBJ_PTR y, OBJ_PTR width, OBJ_PTR height);
extern OBJ_PTR FM_fill_and_stroke_rect(OBJ_PTR fmkr, OBJ_PTR x, OBJ_PTR y, OBJ_PTR width, OBJ_PTR height);
extern OBJ_PTR FM_clip_rect(OBJ_PTR fmkr, OBJ_PTR x, OBJ_PTR y, OBJ_PTR width, OBJ_PTR height);
extern OBJ_PTR FM_fill_oval(OBJ_PTR fmkr, OBJ_PTR x, OBJ_PTR y, OBJ_PTR dx, OBJ_PTR dy, OBJ_PTR angle);
extern OBJ_PTR FM_stroke_oval(OBJ_PTR fmkr, OBJ_PTR x, OBJ_PTR y, OBJ_PTR dx, OBJ_PTR dy, OBJ_PTR angle);
extern OBJ_PTR FM_fill_and_stroke_oval(OBJ_PTR fmkr, OBJ_PTR x, OBJ_PTR y, OBJ_PTR dx, OBJ_PTR dy, OBJ_PTR angle);
extern OBJ_PTR FM_clip_oval(OBJ_PTR fmkr, OBJ_PTR x, OBJ_PTR y, OBJ_PTR dx, OBJ_PTR dy, OBJ_PTR angle);
extern OBJ_PTR FM_fill_rounded_rect(OBJ_PTR fmkr, OBJ_PTR x, OBJ_PTR y, OBJ_PTR width, OBJ_PTR height, OBJ_PTR dx, OBJ_PTR dy);
extern OBJ_PTR FM_stroke_rounded_rect(OBJ_PTR fmkr, OBJ_PTR x, OBJ_PTR y, OBJ_PTR width, OBJ_PTR height, OBJ_PTR dx, OBJ_PTR dy);
extern OBJ_PTR FM_fill_and_stroke_rounded_rect(OBJ_PTR fmkr, OBJ_PTR x, OBJ_PTR y, OBJ_PTR width, OBJ_PTR height, OBJ_PTR dx, OBJ_PTR dy);
extern OBJ_PTR FM_clip_rounded_rect(OBJ_PTR fmkr, OBJ_PTR x, OBJ_PTR y, OBJ_PTR width, OBJ_PTR height, OBJ_PTR dx, OBJ_PTR dy);
extern OBJ_PTR FM_fill_circle(OBJ_PTR fmkr, OBJ_PTR x, OBJ_PTR y, OBJ_PTR dx);
extern OBJ_PTR FM_stroke_circle(OBJ_PTR fmkr, OBJ_PTR x, OBJ_PTR y, OBJ_PTR dx);
extern OBJ_PTR FM_fill_and_stroke_circle(OBJ_PTR fmkr, OBJ_PTR x, OBJ_PTR y, OBJ_PTR dx);
extern OBJ_PTR FM_clip_circle(OBJ_PTR fmkr, OBJ_PTR x, OBJ_PTR y, OBJ_PTR dx);
extern OBJ_PTR FM_append_frame_to_path(OBJ_PTR fmkr);
extern OBJ_PTR FM_fill_frame(OBJ_PTR fmkr);
extern OBJ_PTR FM_stroke_frame(OBJ_PTR fmkr);
extern OBJ_PTR FM_fill_and_stroke_frame(OBJ_PTR fmkr);
extern OBJ_PTR FM_clip_to_frame(OBJ_PTR fmkr);

/*======================================================================*/
// pdftext.c
extern OBJ_PTR FM_register_font(OBJ_PTR fmkr, OBJ_PTR font_name);  // returns font number.
extern OBJ_PTR FM_marker_string_info(OBJ_PTR fmkr, OBJ_PTR font_number, OBJ_PTR string, OBJ_PTR scale);
extern OBJ_PTR FM_private_show_marker(OBJ_PTR fmkr, OBJ_PTR integer_args, OBJ_PTR stroke_width, OBJ_PTR string,
    OBJ_PTR x, OBJ_PTR y, OBJ_PTR x_vec, OBJ_PTR y_vec,
    OBJ_PTR h_scale, OBJ_PTR v_scale, OBJ_PTR scale, OBJ_PTR it_angle, OBJ_PTR ascent_angle, OBJ_PTR angle,
    OBJ_PTR fill_color, OBJ_PTR stroke_color);

/*======================================================================*/
// texout.c
extern OBJ_PTR FM_rescale_text(OBJ_PTR fmkr, OBJ_PTR scaling);
extern OBJ_PTR FM_show_rotated_text(OBJ_PTR fmkr, OBJ_PTR text, OBJ_PTR frame_side, OBJ_PTR shift,
				    OBJ_PTR fraction, OBJ_PTR scale, OBJ_PTR angle, OBJ_PTR justification, OBJ_PTR alignment, OBJ_PTR measure_name);
extern OBJ_PTR FM_show_rotated_label(OBJ_PTR fmkr, OBJ_PTR text, OBJ_PTR xloc, OBJ_PTR yloc, OBJ_PTR scale, OBJ_PTR angle, OBJ_PTR justification, OBJ_PTR alignment, OBJ_PTR measure_name);
extern OBJ_PTR FM_check_label_clip(OBJ_PTR fmkr, OBJ_PTR xloc, OBJ_PTR yloc);


/* For saving results of text measurements. */
extern OBJ_PTR FM_private_save_measure(OBJ_PTR fmkr, OBJ_PTR measure_name,
                                       OBJ_PTR width, OBJ_PTR height, 
                                       OBJ_PTR depth); 


#endif   /* __wrappers_H__ */

