/* wrappers.c */
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

#include "figures.h"
#include "pdfs.h"
#include "generic.h"


// This file has the wrappers for ruby Tioga


// axes.c
OBJ_PTR FM_show_axis(OBJ_PTR fmkr, OBJ_PTR loc) {
   return c_show_axis(fmkr, Get_FM(fmkr), Number_to_int(loc)); } 
OBJ_PTR FM_show_edge(OBJ_PTR fmkr, OBJ_PTR loc) {
   return c_show_edge(fmkr, Get_FM(fmkr), Number_to_int(loc)); }
OBJ_PTR FM_no_title(OBJ_PTR fmkr) { return c_no_title(fmkr, Get_FM(fmkr)); }
OBJ_PTR FM_no_xlabel(OBJ_PTR fmkr) { return c_no_xlabel(fmkr, Get_FM(fmkr)); }
OBJ_PTR FM_no_ylabel(OBJ_PTR fmkr) { return c_no_ylabel(fmkr, Get_FM(fmkr)); }
OBJ_PTR FM_no_xaxis(OBJ_PTR fmkr) { return c_no_xaxis(fmkr, Get_FM(fmkr)); }
OBJ_PTR FM_no_yaxis(OBJ_PTR fmkr) { return c_no_yaxis(fmkr, Get_FM(fmkr)); }
OBJ_PTR FM_no_left_edge(OBJ_PTR fmkr) { return c_no_left_edge(fmkr, Get_FM(fmkr)); }
OBJ_PTR FM_no_right_edge(OBJ_PTR fmkr) { return c_no_right_edge(fmkr, Get_FM(fmkr)); }
OBJ_PTR FM_no_top_edge(OBJ_PTR fmkr) { return c_no_top_edge(fmkr, Get_FM(fmkr)); }
OBJ_PTR FM_no_bottom_edge(OBJ_PTR fmkr) { return c_no_bottom_edge(fmkr, Get_FM(fmkr)); }

// init.c
OBJ_PTR FM_private_init_fm_data(OBJ_PTR fmkr) { 
   return c_private_init_fm_data(fmkr, Get_FM(fmkr)); }
OBJ_PTR FM_set_frame_sides(OBJ_PTR fmkr, OBJ_PTR left, OBJ_PTR right, OBJ_PTR top, OBJ_PTR bottom) {
   return c_set_frame_sides(fmkr, Get_FM(fmkr),
      Number_to_double(left), Number_to_double(right), 
      Number_to_double(top), Number_to_double(bottom)); }
OBJ_PTR FM_set_device_pagesize(OBJ_PTR fmkr, OBJ_PTR width, OBJ_PTR height) {
   return c_set_device_pagesize(fmkr, Get_FM(fmkr), Number_to_double(width), Number_to_double(height)); }
OBJ_PTR FM_get_save_filename(OBJ_PTR fmkr, OBJ_PTR name) {
   return c_get_save_filename(fmkr, Get_FM(fmkr), name); }
OBJ_PTR FM_private_make(OBJ_PTR fmkr, OBJ_PTR name, OBJ_PTR cmd) {
   return c_private_make(fmkr, Get_FM(fmkr), name, cmd); }
OBJ_PTR FM_private_make_portfolio(OBJ_PTR fmkr, OBJ_PTR name, OBJ_PTR fignums, OBJ_PTR fignames) {
   return c_private_make_portfolio(fmkr, Get_FM(fmkr), name, fignums, fignames); }

// makers.c
OBJ_PTR FM_private_make_contour(OBJ_PTR fmkr, OBJ_PTR gaps,
  OBJ_PTR xs, OBJ_PTR ys, OBJ_PTR zs, OBJ_PTR z_level, OBJ_PTR legit, OBJ_PTR method) { 
     return c_private_make_contour(fmkr, Get_FM(fmkr), gaps, xs, ys, zs, Number_to_double(z_level),
        legit, Number_to_int(method)); }
OBJ_PTR FM_private_make_steps(OBJ_PTR fmkr, OBJ_PTR Xvec_data, OBJ_PTR Yvec_data,
     OBJ_PTR xfirst, OBJ_PTR yfirst, OBJ_PTR xlast, OBJ_PTR ylast) {
   return c_private_make_steps(fmkr, Get_FM(fmkr), Xvec_data, Yvec_data,
      Number_to_double(xfirst), Number_to_double(yfirst), 
      Number_to_double(xlast), Number_to_double(ylast)); }
OBJ_PTR FM_private_make_spline_interpolated_points(OBJ_PTR fmkr, 
      OBJ_PTR Xvec, OBJ_PTR Xvec_data, OBJ_PTR Yvec_data,
      OBJ_PTR start_slope, OBJ_PTR end_slope) {
   return c_private_make_spline_interpolated_points(fmkr, Get_FM(fmkr),
      Xvec, Xvec_data, Yvec_data, start_slope, end_slope); }

// pdfcolor.c
OBJ_PTR FM_stroke_opacity_set(OBJ_PTR fmkr, OBJ_PTR val) {
   c_stroke_opacity_set(fmkr, Get_FM(fmkr), Number_to_double(val)); return val; }
OBJ_PTR FM_fill_opacity_set(OBJ_PTR fmkr, OBJ_PTR val) {
   c_fill_opacity_set(fmkr, Get_FM(fmkr), Number_to_double(val)); return val; }
OBJ_PTR FM_private_axial_shading(
   OBJ_PTR fmkr, OBJ_PTR x0, OBJ_PTR y0, OBJ_PTR x1, OBJ_PTR y1,
   OBJ_PTR colormap, OBJ_PTR extend_start, OBJ_PTR extend_end) {
      return c_private_axial_shading(fmkr, Get_FM(fmkr), Number_to_double(x0), Number_to_double(y0),
            Number_to_double(x1), Number_to_double(y1), colormap, 
            extend_start == OBJ_TRUE, extend_end == OBJ_TRUE); }
OBJ_PTR FM_private_radial_shading(OBJ_PTR fmkr,
        OBJ_PTR x0, OBJ_PTR y0, OBJ_PTR r0,
        OBJ_PTR x1, OBJ_PTR y1, OBJ_PTR r1, OBJ_PTR colormap,
        OBJ_PTR a, OBJ_PTR b, OBJ_PTR c, OBJ_PTR d, OBJ_PTR extend_start, OBJ_PTR extend_end) {
   return c_private_radial_shading(fmkr, Get_FM(fmkr), 
      Number_to_double(x0), Number_to_double(y0), Number_to_double(r0),
      Number_to_double(x1), Number_to_double(y1), Number_to_double(r1), colormap,
      Number_to_double(a), Number_to_double(b), Number_to_double(c), Number_to_double(d),
      extend_start == OBJ_TRUE, extend_end == OBJ_TRUE); }
OBJ_PTR FM_private_create_colormap(OBJ_PTR fmkr, OBJ_PTR rgb_flag,
    OBJ_PTR length, OBJ_PTR Ps, OBJ_PTR C1s, OBJ_PTR C2s, OBJ_PTR C3s) {
       return c_private_create_colormap(fmkr, Get_FM(fmkr), rgb_flag != OBJ_FALSE, Number_to_int(length), Ps, C1s, C2s, C3s); }
OBJ_PTR FM_get_color_from_colormap(OBJ_PTR fmkr, OBJ_PTR color_map, OBJ_PTR color_position) {
   return c_get_color_from_colormap(fmkr, Get_FM(fmkr), color_map, Number_to_double(color_position)); }
OBJ_PTR FM_convert_to_colormap(OBJ_PTR fmkr, OBJ_PTR Rs, OBJ_PTR Gs, OBJ_PTR Bs) {
   return c_convert_to_colormap(fmkr, Get_FM(fmkr), Rs, Gs, Bs); }
OBJ_PTR FM_hls_to_rgb(OBJ_PTR fmkr, OBJ_PTR hls_vec) {
   return c_hls_to_rgb(fmkr, Get_FM(fmkr), hls_vec); }
OBJ_PTR FM_rgb_to_hls(OBJ_PTR fmkr, OBJ_PTR rgb_vec) {
   return c_rgb_to_hls(fmkr, Get_FM(fmkr), rgb_vec); }
OBJ_PTR FM_title_color_set(OBJ_PTR fmkr, OBJ_PTR val) {
   return c_title_color_set(fmkr, Get_FM(fmkr), val); }
OBJ_PTR FM_title_color_get(OBJ_PTR fmkr) {
   return c_title_color_get(fmkr, Get_FM(fmkr)); }
OBJ_PTR FM_xlabel_color_set(OBJ_PTR fmkr, OBJ_PTR val) { 
   return c_xlabel_color_set(fmkr, Get_FM(fmkr), val); }
OBJ_PTR FM_xlabel_color_get(OBJ_PTR fmkr) {
   return c_xlabel_color_get(fmkr, Get_FM(fmkr)); }
OBJ_PTR FM_ylabel_color_set(OBJ_PTR fmkr, OBJ_PTR val) {
   return c_ylabel_color_set(fmkr, Get_FM(fmkr), val); }
OBJ_PTR FM_ylabel_color_get(OBJ_PTR fmkr) {
   return c_ylabel_color_get(fmkr, Get_FM(fmkr)); }
OBJ_PTR FM_xaxis_stroke_color_set(OBJ_PTR fmkr, OBJ_PTR val) {
   return c_xaxis_stroke_color_set(fmkr, Get_FM(fmkr), val); }
OBJ_PTR FM_xaxis_stroke_color_get(OBJ_PTR fmkr) {
   return c_xaxis_stroke_color_get(fmkr, Get_FM(fmkr)); }
OBJ_PTR FM_yaxis_stroke_color_set(OBJ_PTR fmkr, OBJ_PTR val) {
   return c_yaxis_stroke_color_set(fmkr, Get_FM(fmkr), val); }
OBJ_PTR FM_yaxis_stroke_color_get(OBJ_PTR fmkr) {
   return c_yaxis_stroke_color_get(fmkr, Get_FM(fmkr)); }

// pdfcoords.c
OBJ_PTR FM_private_set_subframe(OBJ_PTR fmkr, OBJ_PTR left_margin, OBJ_PTR right_margin, 
      OBJ_PTR top_margin, OBJ_PTR bottom_margin) {
   return c_set_subframe(fmkr, Get_FM(fmkr), Number_to_double(left_margin), Number_to_double(right_margin), 
      Number_to_double(top_margin), Number_to_double(bottom_margin));}
OBJ_PTR FM_private_set_default_font_size(OBJ_PTR fmkr, OBJ_PTR size) {
   return c_private_set_default_font_size(fmkr, Get_FM(fmkr), Number_to_double(size)); }
OBJ_PTR FM_convert_to_degrees(OBJ_PTR fmkr, OBJ_PTR dx, OBJ_PTR dy) {
   return c_convert_to_degrees(fmkr, Get_FM(fmkr), Number_to_double(dx), Number_to_double(dy)); }
OBJ_PTR FM_convert_inches_to_output(OBJ_PTR fmkr, OBJ_PTR v) {
   return c_convert_inches_to_output(fmkr, Get_FM(fmkr), Number_to_double(v)); }
OBJ_PTR FM_convert_output_to_inches(OBJ_PTR fmkr, OBJ_PTR v) {
   return c_convert_output_to_inches(fmkr, Get_FM(fmkr), Number_to_double(v)); }
OBJ_PTR FM_convert_mm_to_output(OBJ_PTR fmkr, OBJ_PTR v) {
   return c_convert_mm_to_output(fmkr, Get_FM(fmkr), Number_to_double(v)); }
OBJ_PTR FM_convert_output_to_mm(OBJ_PTR fmkr, OBJ_PTR v) {
   return c_convert_output_to_mm(fmkr, Get_FM(fmkr), Number_to_double(v)); }
OBJ_PTR FM_convert_page_to_output_x(OBJ_PTR fmkr, OBJ_PTR v) {
   return c_convert_page_to_output_x(fmkr, Get_FM(fmkr), Number_to_double(v)); }
OBJ_PTR FM_convert_page_to_output_y(OBJ_PTR fmkr, OBJ_PTR v) {
   return c_convert_page_to_output_y(fmkr, Get_FM(fmkr), Number_to_double(v)); }
OBJ_PTR FM_convert_page_to_output_dx(OBJ_PTR fmkr, OBJ_PTR v) {
   return c_convert_page_to_output_dx(fmkr, Get_FM(fmkr), Number_to_double(v)); }
OBJ_PTR FM_convert_page_to_output_dy(OBJ_PTR fmkr, OBJ_PTR v) {
   return c_convert_page_to_output_dy(fmkr, Get_FM(fmkr), Number_to_double(v)); }
OBJ_PTR FM_convert_output_to_page_x(OBJ_PTR fmkr, OBJ_PTR v) {
   return c_convert_output_to_page_x(fmkr, Get_FM(fmkr), Number_to_double(v)); }
OBJ_PTR FM_convert_output_to_page_y(OBJ_PTR fmkr, OBJ_PTR v) {
   return c_convert_output_to_page_y(fmkr, Get_FM(fmkr), Number_to_double(v)); }
OBJ_PTR FM_convert_output_to_page_dx(OBJ_PTR fmkr, OBJ_PTR v) {
   return c_convert_output_to_page_dx(fmkr, Get_FM(fmkr), Number_to_double(v)); }
OBJ_PTR FM_convert_output_to_page_dy(OBJ_PTR fmkr, OBJ_PTR v) {
   return c_convert_output_to_page_dy(fmkr, Get_FM(fmkr), Number_to_double(v)); }
OBJ_PTR FM_convert_frame_to_page_x(OBJ_PTR fmkr, OBJ_PTR v) {
   return c_convert_frame_to_page_x(fmkr, Get_FM(fmkr), Number_to_double(v)); }
OBJ_PTR FM_convert_frame_to_page_y(OBJ_PTR fmkr, OBJ_PTR v) {
   return c_convert_frame_to_page_y(fmkr, Get_FM(fmkr), Number_to_double(v)); }
OBJ_PTR FM_convert_frame_to_page_dx(OBJ_PTR fmkr, OBJ_PTR v) {
   return c_convert_frame_to_page_dx(fmkr, Get_FM(fmkr), Number_to_double(v)); }
OBJ_PTR FM_convert_frame_to_page_dy(OBJ_PTR fmkr, OBJ_PTR v) {
   return c_convert_frame_to_page_dy(fmkr, Get_FM(fmkr), Number_to_double(v)); }
OBJ_PTR FM_convert_page_to_frame_x(OBJ_PTR fmkr, OBJ_PTR v) {
   return c_convert_page_to_frame_x(fmkr, Get_FM(fmkr), Number_to_double(v)); }
OBJ_PTR FM_convert_page_to_frame_y(OBJ_PTR fmkr, OBJ_PTR v) {
   return c_convert_page_to_frame_y(fmkr, Get_FM(fmkr), Number_to_double(v)); }
OBJ_PTR FM_convert_page_to_frame_dx(OBJ_PTR fmkr, OBJ_PTR v) {
   return c_convert_page_to_frame_dx(fmkr, Get_FM(fmkr), Number_to_double(v)); }
OBJ_PTR FM_convert_page_to_frame_dy(OBJ_PTR fmkr, OBJ_PTR v) {
   return c_convert_page_to_frame_dy(fmkr, Get_FM(fmkr), Number_to_double(v)); }
OBJ_PTR FM_convert_figure_to_frame_x(OBJ_PTR fmkr, OBJ_PTR v) {
   return c_convert_figure_to_frame_x(fmkr, Get_FM(fmkr), Number_to_double(v)); }
OBJ_PTR FM_convert_figure_to_frame_y(OBJ_PTR fmkr, OBJ_PTR v) {
   return c_convert_figure_to_frame_y(fmkr, Get_FM(fmkr), Number_to_double(v)); }
OBJ_PTR FM_convert_figure_to_frame_dx(OBJ_PTR fmkr, OBJ_PTR v) {
   return c_convert_figure_to_frame_dx(fmkr, Get_FM(fmkr), Number_to_double(v)); }
OBJ_PTR FM_convert_figure_to_frame_dy(OBJ_PTR fmkr, OBJ_PTR v) {
   return c_convert_figure_to_frame_dy(fmkr, Get_FM(fmkr), Number_to_double(v)); }
OBJ_PTR FM_convert_frame_to_figure_x(OBJ_PTR fmkr, OBJ_PTR v) {
   return c_convert_frame_to_figure_x(fmkr, Get_FM(fmkr), Number_to_double(v)); }
OBJ_PTR FM_convert_frame_to_figure_y(OBJ_PTR fmkr, OBJ_PTR v) {
   return c_convert_frame_to_figure_y(fmkr, Get_FM(fmkr), Number_to_double(v)); }
OBJ_PTR FM_convert_frame_to_figure_dx(OBJ_PTR fmkr, OBJ_PTR v) {
   return c_convert_frame_to_figure_dx(fmkr, Get_FM(fmkr), Number_to_double(v)); }
OBJ_PTR FM_convert_frame_to_figure_dy(OBJ_PTR fmkr, OBJ_PTR v) {
   return c_convert_frame_to_figure_dy(fmkr, Get_FM(fmkr), Number_to_double(v)); }
OBJ_PTR FM_convert_figure_to_output_x(OBJ_PTR fmkr, OBJ_PTR v) {
   return c_convert_figure_to_output_x(fmkr, Get_FM(fmkr), Number_to_double(v)); }
OBJ_PTR FM_convert_figure_to_output_y(OBJ_PTR fmkr, OBJ_PTR v) {
   return c_convert_figure_to_output_y(fmkr, Get_FM(fmkr), Number_to_double(v)); }
OBJ_PTR FM_convert_figure_to_output_dx(OBJ_PTR fmkr, OBJ_PTR v) {
   return c_convert_figure_to_output_dx(fmkr, Get_FM(fmkr), Number_to_double(v)); }
OBJ_PTR FM_convert_figure_to_output_dy(OBJ_PTR fmkr, OBJ_PTR v) {
   return c_convert_figure_to_output_dy(fmkr, Get_FM(fmkr), Number_to_double(v)); }
OBJ_PTR FM_convert_output_to_figure_x(OBJ_PTR fmkr, OBJ_PTR v) {
   return c_convert_output_to_figure_x(fmkr, Get_FM(fmkr), Number_to_double(v)); }
OBJ_PTR FM_convert_output_to_figure_y(OBJ_PTR fmkr, OBJ_PTR v) {
   return c_convert_output_to_figure_y(fmkr, Get_FM(fmkr), Number_to_double(v)); }
OBJ_PTR FM_convert_output_to_figure_dx(OBJ_PTR fmkr, OBJ_PTR v) {
   return c_convert_output_to_figure_dx(fmkr, Get_FM(fmkr), Number_to_double(v)); }
OBJ_PTR FM_convert_output_to_figure_dy(OBJ_PTR fmkr, OBJ_PTR v) {
   return c_convert_output_to_figure_dy(fmkr, Get_FM(fmkr), Number_to_double(v)); }
OBJ_PTR FM_doing_subplot(OBJ_PTR fmkr) { return c_doing_subplot(fmkr, Get_FM(fmkr)); }
OBJ_PTR FM_doing_subfigure(OBJ_PTR fmkr) { return c_doing_subfigure(fmkr, Get_FM(fmkr)); }
OBJ_PTR FM_private_set_bounds(OBJ_PTR fmkr, OBJ_PTR left, OBJ_PTR right, OBJ_PTR top, OBJ_PTR bottom) {
   return c_private_set_bounds(fmkr, Get_FM(fmkr), Number_to_double(left), Number_to_double(right),
      Number_to_double(top), Number_to_double(bottom)); }

// pdffile.c
OBJ_PTR FM_pdf_gsave(OBJ_PTR fmkr) { return c_pdf_gsave(fmkr, Get_FM(fmkr)); }
OBJ_PTR FM_pdf_grestore(OBJ_PTR fmkr) { return c_pdf_grestore(fmkr, Get_FM(fmkr)); }

// pdfimage.c
OBJ_PTR FM_private_create_image_data(OBJ_PTR fmkr, OBJ_PTR data,
            OBJ_PTR first_row, OBJ_PTR last_row, OBJ_PTR first_column, OBJ_PTR last_column,
            OBJ_PTR min_val, OBJ_PTR max_val, OBJ_PTR max_code, OBJ_PTR if_below_range, OBJ_PTR if_above_range)
{
   return c_private_create_image_data(fmkr, Get_FM(fmkr), data,
      Number_to_int(first_row), Number_to_int(last_row), 
      Number_to_int(first_column), Number_to_int(last_column),
      Number_to_double(min_val), Number_to_double(max_val), Number_to_int(max_code), 
      Number_to_int(if_below_range), Number_to_int(if_above_range));
}
OBJ_PTR FM_private_create_monochrome_image_data(OBJ_PTR fmkr, OBJ_PTR data,
            OBJ_PTR first_row, OBJ_PTR last_row, OBJ_PTR first_column, OBJ_PTR last_column,
            OBJ_PTR boundary, OBJ_PTR reverse)
{
   return c_private_create_monochrome_image_data(fmkr, Get_FM(fmkr), data,
      Number_to_int(first_row), Number_to_int(last_row), Number_to_int(first_column), Number_to_int(last_column),
      Number_to_double(boundary), reverse != OBJ_FALSE);
}
OBJ_PTR FM_private_show_jpg(OBJ_PTR fmkr, OBJ_PTR filename, 
   OBJ_PTR width, OBJ_PTR height, OBJ_PTR image_destination, OBJ_PTR mask_obj_num) {
   return c_private_show_jpg(fmkr, Get_FM(fmkr), String_Ptr(filename), 
      Number_to_int(width), Number_to_int(height), image_destination, Number_to_int(mask_obj_num)); }
OBJ_PTR FM_private_show_rgb_image(OBJ_PTR fmkr, OBJ_PTR llx, OBJ_PTR lly, OBJ_PTR lrx, OBJ_PTR lry,
    OBJ_PTR ulx, OBJ_PTR uly, OBJ_PTR interpolate, OBJ_PTR w, OBJ_PTR h, OBJ_PTR data, OBJ_PTR mask_obj_num)
{
   return c_private_show_image(fmkr, Get_FM(fmkr), RGB_IMAGE, Number_to_double(llx), Number_to_double(lly), Number_to_double(lrx), Number_to_double(lry), 
      Number_to_double(ulx), Number_to_double(uly), (interpolate != OBJ_FALSE), OBJ_FALSE, 
      Number_to_int(w), Number_to_int(h), (unsigned char *)String_Ptr(data), String_Len(data), OBJ_NIL, OBJ_NIL, OBJ_NIL, OBJ_NIL, Number_to_int(mask_obj_num));
}

OBJ_PTR FM_private_show_cmyk_image(OBJ_PTR fmkr, OBJ_PTR llx, OBJ_PTR lly, OBJ_PTR lrx, OBJ_PTR lry,
    OBJ_PTR ulx, OBJ_PTR uly, OBJ_PTR interpolate, OBJ_PTR w, OBJ_PTR h, OBJ_PTR data, OBJ_PTR mask_obj_num)
{
   return c_private_show_image(fmkr, Get_FM(fmkr), CMYK_IMAGE, Number_to_double(llx), Number_to_double(lly), Number_to_double(lrx), Number_to_double(lry), 
      Number_to_double(ulx), Number_to_double(uly), (interpolate != OBJ_FALSE), false, 
      Number_to_int(w), Number_to_int(h), (unsigned char *)String_Ptr(data), String_Len(data), OBJ_NIL, OBJ_NIL, OBJ_NIL, OBJ_NIL, Number_to_int(mask_obj_num));
}

OBJ_PTR FM_private_show_grayscale_image(OBJ_PTR fmkr, OBJ_PTR llx, OBJ_PTR lly, OBJ_PTR lrx, OBJ_PTR lry,
    OBJ_PTR ulx, OBJ_PTR uly, OBJ_PTR interpolate, OBJ_PTR w, OBJ_PTR h, OBJ_PTR data, OBJ_PTR mask_obj_num)
{
   return c_private_show_image(fmkr, Get_FM(fmkr), GRAY_IMAGE, Number_to_double(llx), Number_to_double(lly), Number_to_double(lrx), Number_to_double(lry), 
      Number_to_double(ulx), Number_to_double(uly), (interpolate != OBJ_FALSE), false, 
      Number_to_int(w), Number_to_int(h), (unsigned char *)String_Ptr(data), String_Len(data), OBJ_NIL, OBJ_NIL, OBJ_NIL, OBJ_NIL, Number_to_int(mask_obj_num));
}

OBJ_PTR FM_private_show_monochrome_image(OBJ_PTR fmkr, OBJ_PTR llx, OBJ_PTR lly, OBJ_PTR lrx, OBJ_PTR lry,
    OBJ_PTR ulx, OBJ_PTR uly, OBJ_PTR interpolate, OBJ_PTR reversed, OBJ_PTR w, OBJ_PTR h, OBJ_PTR data, OBJ_PTR mask_obj_num)
{
   return c_private_show_image(fmkr, Get_FM(fmkr), MONO_IMAGE, Number_to_double(llx), Number_to_double(lly), Number_to_double(lrx), Number_to_double(lry), 
      Number_to_double(ulx), Number_to_double(uly), (interpolate != OBJ_FALSE), (reversed == OBJ_TRUE), 
      Number_to_int(w), Number_to_int(h), (unsigned char *)String_Ptr(data), String_Len(data), OBJ_NIL, OBJ_NIL, OBJ_NIL, OBJ_NIL, Number_to_int(mask_obj_num));
}

OBJ_PTR FM_private_show_image(OBJ_PTR fmkr, OBJ_PTR llx, OBJ_PTR lly, OBJ_PTR lrx, OBJ_PTR lry,
    OBJ_PTR ulx, OBJ_PTR uly, OBJ_PTR interpolate, OBJ_PTR w, OBJ_PTR h, OBJ_PTR data,
    OBJ_PTR value_mask_min, OBJ_PTR value_mask_max, OBJ_PTR hival, OBJ_PTR lookup, OBJ_PTR mask_obj_num)
{
   return c_private_show_image(fmkr, Get_FM(fmkr), COLORMAP_IMAGE, Number_to_double(llx), Number_to_double(lly), Number_to_double(lrx), Number_to_double(lry), 
      Number_to_double(ulx), Number_to_double(uly), (interpolate != OBJ_FALSE), false, 
      Number_to_int(w), Number_to_int(h), (unsigned char *)String_Ptr(data), String_Len(data), value_mask_min, value_mask_max, hival, lookup, Number_to_int(mask_obj_num));
}


// pdfpath.c
OBJ_PTR FM_stroke_color_set(OBJ_PTR fmkr, OBJ_PTR value) { 
   return c_stroke_color_set(fmkr, Get_FM(fmkr), value); }
OBJ_PTR FM_stroke_color_get(OBJ_PTR fmkr) {
   return c_stroke_color_get(fmkr, Get_FM(fmkr)); }
OBJ_PTR FM_fill_color_set(OBJ_PTR fmkr, OBJ_PTR value) {
   return c_fill_color_set(fmkr, Get_FM(fmkr), value); }
OBJ_PTR FM_fill_color_get(OBJ_PTR fmkr) { 
   return c_fill_color_get(fmkr, Get_FM(fmkr)); }
OBJ_PTR FM_line_width_set(OBJ_PTR fmkr, OBJ_PTR val) { 
   c_line_width_set(fmkr, Get_FM(fmkr), Number_to_double(val)); return val; }
OBJ_PTR FM_rescale_lines(OBJ_PTR fmkr, OBJ_PTR scaling_factor) {
   return c_rescale_lines(fmkr, Get_FM(fmkr), Number_to_double(scaling_factor)); }
OBJ_PTR FM_line_cap_set(OBJ_PTR fmkr, OBJ_PTR val) {
   c_line_cap_set(fmkr, Get_FM(fmkr), Number_to_int(val)); return val; }
OBJ_PTR FM_line_join_set(OBJ_PTR fmkr, OBJ_PTR val) {
   c_line_join_set(fmkr, Get_FM(fmkr), Number_to_int(val)); return val; }
OBJ_PTR FM_miter_limit_set(OBJ_PTR fmkr, OBJ_PTR val) {
   c_miter_limit_set(fmkr, Get_FM(fmkr), Number_to_double(val)); return val; }
OBJ_PTR FM_line_type_set(OBJ_PTR fmkr, OBJ_PTR line_type) {
   return c_line_type_set(fmkr, Get_FM(fmkr), line_type); }
OBJ_PTR FM_update_bbox(OBJ_PTR fmkr, OBJ_PTR x, OBJ_PTR y) {
   return c_update_bbox(fmkr, Get_FM(fmkr), Number_to_double(x), Number_to_double(y)); }
OBJ_PTR FM_bbox_left(OBJ_PTR fmkr) { return c_bbox_left(fmkr, Get_FM(fmkr)); }
OBJ_PTR FM_bbox_right(OBJ_PTR fmkr) { return c_bbox_right(fmkr, Get_FM(fmkr)); }
OBJ_PTR FM_bbox_top(OBJ_PTR fmkr) { return c_bbox_top(fmkr, Get_FM(fmkr)); }
OBJ_PTR FM_bbox_bottom(OBJ_PTR fmkr) { return c_bbox_bottom(fmkr, Get_FM(fmkr)); }
OBJ_PTR FM_move_to_point(OBJ_PTR fmkr, OBJ_PTR x, OBJ_PTR y) {
   return c_move_to_point(fmkr, Get_FM(fmkr), Number_to_double(x), Number_to_double(y)); }   
OBJ_PTR FM_append_point_to_path(OBJ_PTR fmkr, OBJ_PTR x, OBJ_PTR y) {
   return c_append_point_to_path(fmkr, Get_FM(fmkr), Number_to_double(x), Number_to_double(y)); }
OBJ_PTR FM_bezier_control_points(OBJ_PTR fmkr, VALUE x0, VALUE y0, VALUE delta_x, VALUE a, VALUE b, VALUE c) {
   return c_bezier_control_points(fmkr, Get_FM(fmkr),
      Number_to_double(x0), Number_to_double(y0), Number_to_double(delta_x),
      Number_to_double(a), Number_to_double(b), Number_to_double(c));
}
OBJ_PTR FM_append_curve_to_path(OBJ_PTR fmkr, OBJ_PTR x1, OBJ_PTR y1, OBJ_PTR x2, OBJ_PTR y2, OBJ_PTR x3, OBJ_PTR y3) {
   return c_append_curve_to_path(fmkr, Get_FM(fmkr), Number_to_double(x1), Number_to_double(y1), 
      Number_to_double(x2), Number_to_double(y2), 
      Number_to_double(x3), Number_to_double(y3)); }
OBJ_PTR FM_close_path(OBJ_PTR fmkr) { return c_close_path(fmkr, Get_FM(fmkr)); }
OBJ_PTR FM_append_arc_to_path(OBJ_PTR fmkr, OBJ_PTR x_start, OBJ_PTR y_start, OBJ_PTR x_corner, OBJ_PTR y_corner,
   OBJ_PTR x_end, OBJ_PTR y_end, OBJ_PTR dx, OBJ_PTR dy) {
    // dx and dy are converted to output coords and smaller is used as radius
   return c_append_arc_to_path(fmkr, Get_FM(fmkr),
      Number_to_double(x_start), Number_to_double(y_start),
      Number_to_double(x_corner), Number_to_double(y_corner),
      Number_to_double(x_end), Number_to_double(y_end), Number_to_double(dx), Number_to_double(dy));
}
OBJ_PTR FM_append_rect_to_path(OBJ_PTR fmkr, OBJ_PTR x, OBJ_PTR y, OBJ_PTR width, OBJ_PTR height) {
   return c_append_rect_to_path(fmkr, Get_FM(fmkr), Number_to_double(x), Number_to_double(y), 
      Number_to_double(width), Number_to_double(height));
}
OBJ_PTR FM_append_rounded_rect_to_path(OBJ_PTR fmkr, OBJ_PTR x, OBJ_PTR y, 
   OBJ_PTR width, OBJ_PTR height, OBJ_PTR dx, OBJ_PTR dy) { // dx and dy are converted to output coords and smaller is used as radius
   return c_append_rounded_rect_to_path(fmkr, Get_FM(fmkr),
      Number_to_double(x), Number_to_double(y),
      Number_to_double(width), Number_to_double(height), 
      Number_to_double(dx), Number_to_double(dy));
}
OBJ_PTR FM_append_oval_to_path(OBJ_PTR fmkr, OBJ_PTR x, OBJ_PTR y, OBJ_PTR dx, OBJ_PTR dy, OBJ_PTR angle) {
   return c_append_oval_to_path(fmkr, Get_FM(fmkr),
      Number_to_double(x), Number_to_double(y), Number_to_double(dx), Number_to_double(dy), Number_to_double(angle)); }
OBJ_PTR FM_append_circle_to_path(OBJ_PTR fmkr, OBJ_PTR x, OBJ_PTR y, OBJ_PTR dx) {
   return c_append_circle_to_path(fmkr, Get_FM(fmkr), Number_to_double(x), Number_to_double(y), Number_to_double(dx)); } 
OBJ_PTR FM_append_points_to_path(OBJ_PTR fmkr, OBJ_PTR x_vec, OBJ_PTR y_vec) {
   return c_append_points_to_path(fmkr, Get_FM(fmkr), x_vec, y_vec); } 
OBJ_PTR FM_private_append_points_with_gaps_to_path(OBJ_PTR fmkr, OBJ_PTR x_vec, OBJ_PTR y_vec, OBJ_PTR gaps, OBJ_PTR close_gaps) {
   return c_private_append_points_with_gaps_to_path(fmkr, Get_FM(fmkr), x_vec, y_vec, gaps, (close_gaps == OBJ_TRUE)); }
OBJ_PTR FM_stroke(OBJ_PTR fmkr) { return c_stroke(fmkr, Get_FM(fmkr)); }
OBJ_PTR FM_close_and_stroke(OBJ_PTR fmkr) { return c_close_and_stroke(fmkr, Get_FM(fmkr)); }
OBJ_PTR FM_fill(OBJ_PTR fmkr) { return c_fill(fmkr, Get_FM(fmkr)); }
OBJ_PTR FM_discard_path(OBJ_PTR fmkr) { return c_discard_path(fmkr, Get_FM(fmkr)); }
OBJ_PTR FM_eofill(OBJ_PTR fmkr) { return c_eofill(fmkr, Get_FM(fmkr)); }
OBJ_PTR FM_fill_and_stroke(OBJ_PTR fmkr) { return c_fill_and_stroke(fmkr, Get_FM(fmkr)); }
OBJ_PTR FM_eofill_and_stroke(OBJ_PTR fmkr) { return c_eofill_and_stroke(fmkr, Get_FM(fmkr)); }
OBJ_PTR FM_close_fill_and_stroke(OBJ_PTR fmkr) { return c_close_fill_and_stroke(fmkr, Get_FM(fmkr)); }
OBJ_PTR FM_close_eofill_and_stroke(OBJ_PTR fmkr) { return c_close_eofill_and_stroke(fmkr, Get_FM(fmkr)); }
OBJ_PTR FM_clip(OBJ_PTR fmkr) { return c_clip(fmkr, Get_FM(fmkr)); }
OBJ_PTR FM_eoclip(OBJ_PTR fmkr) { return c_eoclip(fmkr, Get_FM(fmkr)); }
OBJ_PTR FM_fill_and_clip(OBJ_PTR fmkr) { return c_fill_and_clip(fmkr, Get_FM(fmkr)); }
OBJ_PTR FM_stroke_and_clip(OBJ_PTR fmkr) { return c_stroke_and_clip(fmkr, Get_FM(fmkr)); }
OBJ_PTR FM_fill_stroke_and_clip(OBJ_PTR fmkr) { return c_fill_stroke_and_clip(fmkr, Get_FM(fmkr)); }
OBJ_PTR FM_stroke_line(OBJ_PTR fmkr, OBJ_PTR x1, OBJ_PTR y1, OBJ_PTR x2, OBJ_PTR y2) {
   return c_stroke_line(fmkr, Get_FM(fmkr), 
      Number_to_double(x1), Number_to_double(y1), 
      Number_to_double(x2), Number_to_double(y2)); }
OBJ_PTR FM_fill_rect(OBJ_PTR fmkr, OBJ_PTR x, OBJ_PTR y, OBJ_PTR width, OBJ_PTR height) {
   return c_fill_rect(fmkr, Get_FM(fmkr), 
      Number_to_double(x), Number_to_double(y), 
      Number_to_double(width), Number_to_double(height)); }
OBJ_PTR FM_stroke_rect(OBJ_PTR fmkr, OBJ_PTR x, OBJ_PTR y, OBJ_PTR width, OBJ_PTR height) {
   return c_stroke_rect(fmkr, Get_FM(fmkr), 
      Number_to_double(x), Number_to_double(y), 
      Number_to_double(width), Number_to_double(height)); }
OBJ_PTR FM_fill_and_stroke_rect(OBJ_PTR fmkr, OBJ_PTR x, OBJ_PTR y, OBJ_PTR width, OBJ_PTR height) {
   return c_fill_and_stroke_rect(fmkr, Get_FM(fmkr), 
      Number_to_double(x), Number_to_double(y), 
      Number_to_double(width), Number_to_double(height)); }
OBJ_PTR FM_clip_rect(OBJ_PTR fmkr, OBJ_PTR x, OBJ_PTR y, OBJ_PTR width, OBJ_PTR height) {
   return c_clip_rect(fmkr, Get_FM(fmkr),
      Number_to_double(x), Number_to_double(y),
      Number_to_double(width), Number_to_double(height)); }
OBJ_PTR FM_clip_oval(OBJ_PTR fmkr, OBJ_PTR x, OBJ_PTR y, OBJ_PTR dx, OBJ_PTR dy, OBJ_PTR angle) {
   return c_clip_oval(fmkr, Get_FM(fmkr), Number_to_double(x), Number_to_double(y), 
      Number_to_double(dx), Number_to_double(dy), Number_to_double(angle)); }
OBJ_PTR FM_fill_oval(OBJ_PTR fmkr, OBJ_PTR x, OBJ_PTR y, OBJ_PTR dx, OBJ_PTR dy, OBJ_PTR angle) {
   return c_fill_oval(fmkr, Get_FM(fmkr), Number_to_double(x), Number_to_double(y), 
      Number_to_double(dx), Number_to_double(dy), Number_to_double(angle)); }
OBJ_PTR FM_stroke_oval(OBJ_PTR fmkr, OBJ_PTR x, OBJ_PTR y, OBJ_PTR dx, OBJ_PTR dy, OBJ_PTR angle) {
   return c_stroke_oval(fmkr, Get_FM(fmkr), Number_to_double(x), Number_to_double(y), 
      Number_to_double(dx), Number_to_double(dy), Number_to_double(angle)); }
OBJ_PTR FM_fill_and_stroke_oval(OBJ_PTR fmkr, OBJ_PTR x, OBJ_PTR y, OBJ_PTR dx, OBJ_PTR dy, OBJ_PTR angle) {
   return c_fill_and_stroke_oval(fmkr, Get_FM(fmkr), Number_to_double(x), Number_to_double(y), 
      Number_to_double(dx), Number_to_double(dy), Number_to_double(angle)); }
OBJ_PTR FM_clip_rounded_rect(OBJ_PTR fmkr, OBJ_PTR x, OBJ_PTR y, OBJ_PTR width, OBJ_PTR height, OBJ_PTR dx, OBJ_PTR dy) {
   return c_clip_rounded_rect(fmkr, Get_FM(fmkr), Number_to_double(x), Number_to_double(y), 
      Number_to_double(width), Number_to_double(height), Number_to_double(dx), Number_to_double(dy)); }
OBJ_PTR FM_fill_rounded_rect(OBJ_PTR fmkr, OBJ_PTR x, OBJ_PTR y, OBJ_PTR width, OBJ_PTR height, OBJ_PTR dx, OBJ_PTR dy) {
   return c_fill_rounded_rect(fmkr, Get_FM(fmkr), Number_to_double(x), Number_to_double(y), 
      Number_to_double(width), Number_to_double(height), Number_to_double(dx), Number_to_double(dy)); }
OBJ_PTR FM_stroke_rounded_rect(OBJ_PTR fmkr, OBJ_PTR x, OBJ_PTR y, OBJ_PTR width, OBJ_PTR height, OBJ_PTR dx, OBJ_PTR dy) {
   return c_stroke_rounded_rect(fmkr, Get_FM(fmkr), Number_to_double(x), Number_to_double(y), 
      Number_to_double(width), Number_to_double(height), Number_to_double(dx), Number_to_double(dy)); }
OBJ_PTR FM_fill_and_stroke_rounded_rect(OBJ_PTR fmkr, OBJ_PTR x, OBJ_PTR y, OBJ_PTR width, OBJ_PTR height, OBJ_PTR dx, OBJ_PTR dy) {
   return c_fill_and_stroke_rounded_rect(fmkr, Get_FM(fmkr), Number_to_double(x), Number_to_double(y), 
      Number_to_double(width), Number_to_double(height), Number_to_double(dx), Number_to_double(dy)); }
OBJ_PTR FM_clip_circle(OBJ_PTR fmkr, OBJ_PTR x, OBJ_PTR y, OBJ_PTR dx) {
   return c_clip_circle(fmkr, Get_FM(fmkr), 
      Number_to_double(x), Number_to_double(y), Number_to_double(dx)); } 
OBJ_PTR FM_fill_circle(OBJ_PTR fmkr, OBJ_PTR x, OBJ_PTR y, OBJ_PTR dx) {
   return c_fill_circle(fmkr, Get_FM(fmkr), 
      Number_to_double(x), Number_to_double(y), Number_to_double(dx)); } 
OBJ_PTR FM_stroke_circle(OBJ_PTR fmkr, OBJ_PTR x, OBJ_PTR y, OBJ_PTR dx) {
   return c_stroke_circle(fmkr, Get_FM(fmkr), 
      Number_to_double(x), Number_to_double(y), Number_to_double(dx)); } 
OBJ_PTR FM_fill_and_stroke_circle(OBJ_PTR fmkr, OBJ_PTR x, OBJ_PTR y, OBJ_PTR dx) {
   return c_fill_and_stroke_circle(fmkr, Get_FM(fmkr), 
      Number_to_double(x), Number_to_double(y), Number_to_double(dx)); } 
OBJ_PTR FM_append_frame_to_path(OBJ_PTR fmkr) { return c_append_frame_to_path(fmkr, Get_FM(fmkr)); }
OBJ_PTR FM_fill_frame(OBJ_PTR fmkr) { return c_fill_frame(fmkr, Get_FM(fmkr)); }
OBJ_PTR FM_stroke_frame(OBJ_PTR fmkr) { return c_stroke_frame(fmkr, Get_FM(fmkr)); }
OBJ_PTR FM_fill_and_stroke_frame(OBJ_PTR fmkr) { return c_fill_and_stroke_frame(fmkr, Get_FM(fmkr)); }
OBJ_PTR FM_clip_to_frame(OBJ_PTR fmkr) { return c_clip_to_frame(fmkr, Get_FM(fmkr)); }

// pdftext.c
OBJ_PTR FM_register_font(OBJ_PTR fmkr, OBJ_PTR font_name) {
   return c_register_font(fmkr, Get_FM(fmkr), String_Ptr(font_name)); }
OBJ_PTR FM_marker_string_info(OBJ_PTR fmkr, OBJ_PTR font_number, OBJ_PTR string, OBJ_PTR scale) {
   return c_marker_string_info(fmkr, Get_FM(fmkr), Number_to_int(font_number), 
      (unsigned char *)String_Ptr(string), Number_to_double(scale)); }
OBJ_PTR FM_private_show_marker(
   OBJ_PTR fmkr, OBJ_PTR integer_args, OBJ_PTR stroke_width, OBJ_PTR string,
   OBJ_PTR x, OBJ_PTR y, OBJ_PTR x_vec, OBJ_PTR y_vec,
   OBJ_PTR h_scale, OBJ_PTR v_scale, OBJ_PTR scale, OBJ_PTR it_angle, 
   OBJ_PTR ascent_angle, OBJ_PTR angle,
   OBJ_PTR fill_color, OBJ_PTR stroke_color) {
   return c_private_show_marker(fmkr, Get_FM(fmkr), Number_to_int(integer_args), stroke_width,
      string, x, y, x_vec, y_vec, Number_to_double(h_scale), Number_to_double(v_scale), 
      Number_to_double(scale), Number_to_double(it_angle), Number_to_double(ascent_angle), Number_to_double(angle),
      fill_color, stroke_color); }

// texout.c
OBJ_PTR FM_rescale_text(OBJ_PTR fmkr, OBJ_PTR scaling_factor) {
   return c_rescale_text(fmkr, Get_FM(fmkr), Number_to_double(scaling_factor)); }
OBJ_PTR FM_show_rotated_text(OBJ_PTR fmkr, OBJ_PTR text, OBJ_PTR frame_side, OBJ_PTR shift,
   OBJ_PTR fraction, OBJ_PTR scale, OBJ_PTR angle, OBJ_PTR justification, OBJ_PTR alignment) {
   return c_show_rotated_text(fmkr, Get_FM(fmkr), String_Ptr(text), Number_to_int(frame_side), Number_to_double(shift),
      Number_to_double(fraction), Number_to_double(scale), Number_to_double(angle), 
      Number_to_int(justification), Number_to_int(alignment)); }
OBJ_PTR FM_show_rotated_label(OBJ_PTR fmkr, OBJ_PTR text,
   OBJ_PTR xloc, OBJ_PTR yloc, OBJ_PTR scale, OBJ_PTR angle, OBJ_PTR justification, OBJ_PTR alignment) {
   return c_show_rotated_label(fmkr, Get_FM(fmkr), String_Ptr(text), Number_to_double(xloc), Number_to_double(yloc),
      Number_to_double(scale), Number_to_double(angle), Number_to_int(justification), Number_to_int(alignment)); }
OBJ_PTR FM_check_label_clip(OBJ_PTR fmkr, OBJ_PTR xloc, OBJ_PTR yloc) {
   return c_check_label_clip(fmkr, Get_FM(fmkr), Number_to_double(xloc), Number_to_double(yloc)); }
