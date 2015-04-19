/* -*- c-basic-offset: 3; -*- */
/* wrappers.c */
/*
   Copyright (C) 2007  Bill Paxton
             (C) 2008  Vincent Fourmond

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
OBJ_PTR FM_show_axis(OBJ_PTR fmkr, OBJ_PTR loc) { int ierr=0;
   /* Now choosing between c_show_axis_generic and c_show_axis */
   if(Is_Kind_of_Integer(loc))	/* A simple location */
      c_show_axis(fmkr, Get_FM(fmkr, &ierr), 
		  Number_to_int(loc, &ierr), &ierr);
   else				/* A hash */
      c_show_axis_generic(fmkr, Get_FM(fmkr, &ierr), loc, &ierr);
   RETURN_NIL; 
} 

OBJ_PTR FM_show_edge(OBJ_PTR fmkr, OBJ_PTR loc) { int ierr=0;
   c_show_edge(fmkr, Get_FM(fmkr, &ierr), Number_to_int(loc, &ierr), &ierr); RETURN_NIL; }
OBJ_PTR FM_no_title(OBJ_PTR fmkr) { int ierr=0; c_no_title(fmkr, Get_FM(fmkr, &ierr), &ierr); RETURN_NIL; }
OBJ_PTR FM_no_xlabel(OBJ_PTR fmkr) { int ierr=0; c_no_xlabel(fmkr, Get_FM(fmkr, &ierr), &ierr); RETURN_NIL; }
OBJ_PTR FM_no_ylabel(OBJ_PTR fmkr) { int ierr=0; c_no_ylabel(fmkr, Get_FM(fmkr, &ierr), &ierr); RETURN_NIL; }
OBJ_PTR FM_no_xaxis(OBJ_PTR fmkr) { int ierr=0; c_no_xaxis(fmkr, Get_FM(fmkr, &ierr), &ierr); RETURN_NIL; }
OBJ_PTR FM_no_yaxis(OBJ_PTR fmkr) { int ierr=0; c_no_yaxis(fmkr, Get_FM(fmkr, &ierr), &ierr); RETURN_NIL; }
OBJ_PTR FM_no_left_edge(OBJ_PTR fmkr) { int ierr=0; c_no_left_edge(fmkr, Get_FM(fmkr, &ierr), &ierr); RETURN_NIL; }
OBJ_PTR FM_no_right_edge(OBJ_PTR fmkr) { int ierr=0; c_no_right_edge(fmkr, Get_FM(fmkr, &ierr), &ierr); RETURN_NIL; }
OBJ_PTR FM_no_top_edge(OBJ_PTR fmkr) { int ierr=0; c_no_top_edge(fmkr, Get_FM(fmkr, &ierr), &ierr); RETURN_NIL; }
OBJ_PTR FM_no_bottom_edge(OBJ_PTR fmkr) { int ierr=0; c_no_bottom_edge(fmkr, Get_FM(fmkr, &ierr), &ierr); RETURN_NIL;}

OBJ_PTR FM_axis_information(OBJ_PTR fmkr, OBJ_PTR spec) 
{ 
   int ierr=0; 
   return c_axis_get_information(fmkr, Get_FM(fmkr, &ierr), spec, &ierr);
}

// init.c
OBJ_PTR FM_private_init_fm_data(OBJ_PTR fmkr, OBJ_PTR scale) { int ierr=0;
   double scl = Number_to_double(scale, &ierr);
   c_private_init_fm_data(fmkr, Get_FM(fmkr, &ierr), scl, &ierr); RETURN_NIL; }
OBJ_PTR FM_set_frame_sides(OBJ_PTR fmkr, OBJ_PTR left, OBJ_PTR right, OBJ_PTR top, OBJ_PTR bottom) { int ierr=0;
   c_set_frame_sides(fmkr, Get_FM(fmkr, &ierr),
      Number_to_double(left, &ierr), Number_to_double(right, &ierr), 
      Number_to_double(top, &ierr), Number_to_double(bottom, &ierr), &ierr); RETURN_NIL;}
OBJ_PTR FM_set_device_pagesize(OBJ_PTR fmkr, OBJ_PTR width, OBJ_PTR height) { int ierr=0;
   c_set_device_pagesize(fmkr, Get_FM(fmkr, &ierr), Number_to_double(width, &ierr), Number_to_double(height, &ierr), &ierr); RETURN_NIL;}
OBJ_PTR FM_get_save_filename(OBJ_PTR fmkr, OBJ_PTR name) { int ierr=0;
   return c_get_save_filename(fmkr, Get_FM(fmkr, &ierr), name, &ierr); }
OBJ_PTR FM_private_make(OBJ_PTR fmkr, OBJ_PTR name, OBJ_PTR cmd) { int ierr=0;
   c_private_make(fmkr, Get_FM(fmkr, &ierr), name, cmd, &ierr); RETURN_NIL; }
OBJ_PTR FM_private_make_portfolio(OBJ_PTR fmkr, OBJ_PTR name, OBJ_PTR fignums, OBJ_PTR fignames) { int ierr=0;
   c_private_make_portfolio(fmkr, Get_FM(fmkr, &ierr), name, fignums, fignames, &ierr); RETURN_NIL; }

// makers.c
OBJ_PTR FM_private_make_contour(OBJ_PTR fmkr, OBJ_PTR gaps,
  OBJ_PTR xs, OBJ_PTR ys, OBJ_PTR zs, OBJ_PTR z_level, OBJ_PTR legit, OBJ_PTR method) { int ierr=0; 
     return c_private_make_contour(Qnil, NULL, gaps, xs, ys, zs, Number_to_double(z_level, &ierr),
        legit, Number_to_int(method, &ierr), &ierr); }
OBJ_PTR FM_private_make_steps(OBJ_PTR fmkr, OBJ_PTR Xvec_data, OBJ_PTR Yvec_data,
     OBJ_PTR xfirst, OBJ_PTR yfirst, OBJ_PTR xlast, OBJ_PTR ylast) { int ierr=0;
   return c_private_make_steps(fmkr, Get_FM(fmkr, &ierr), Xvec_data, Yvec_data,
      Number_to_double(xfirst, &ierr), Number_to_double(yfirst, &ierr), 
      Number_to_double(xlast, &ierr), Number_to_double(ylast, &ierr), 
      CENTERED, /* by default, CENTERED */
      &ierr); }
OBJ_PTR FM_private_make_spline_interpolated_points(OBJ_PTR fmkr, 
      OBJ_PTR Xvec, OBJ_PTR Xvec_data, OBJ_PTR Yvec_data,
      OBJ_PTR start_slope, OBJ_PTR end_slope) { int ierr=0;
   return c_private_make_spline_interpolated_points(fmkr, Get_FM(fmkr, &ierr),
      Xvec, Xvec_data, Yvec_data, start_slope, end_slope, &ierr); }

// pdfcolor.c
OBJ_PTR FM_stroke_opacity_set(OBJ_PTR fmkr, OBJ_PTR val) { int ierr=0;
   c_stroke_opacity_set(fmkr, Get_FM(fmkr, &ierr), Number_to_double(val, &ierr), &ierr); RETURN_NIL; }
OBJ_PTR FM_fill_opacity_set(OBJ_PTR fmkr, OBJ_PTR val) { int ierr=0;
   c_fill_opacity_set(fmkr, Get_FM(fmkr, &ierr), Number_to_double(val, &ierr), &ierr); RETURN_NIL; }
OBJ_PTR FM_private_axial_shading(
   OBJ_PTR fmkr, OBJ_PTR x0, OBJ_PTR y0, OBJ_PTR x1, OBJ_PTR y1,
   OBJ_PTR colormap, OBJ_PTR extend_start, OBJ_PTR extend_end) { int ierr=0;
      c_private_axial_shading(fmkr, Get_FM(fmkr, &ierr), Number_to_double(x0, &ierr), Number_to_double(y0, &ierr),
            Number_to_double(x1, &ierr), Number_to_double(y1, &ierr), colormap, 
            extend_start == OBJ_TRUE, extend_end == OBJ_TRUE, &ierr); RETURN_NIL; }
OBJ_PTR FM_private_radial_shading(OBJ_PTR fmkr,
        OBJ_PTR x0, OBJ_PTR y0, OBJ_PTR r0,
        OBJ_PTR x1, OBJ_PTR y1, OBJ_PTR r1, OBJ_PTR colormap,
        OBJ_PTR a, OBJ_PTR b, OBJ_PTR c, OBJ_PTR d, OBJ_PTR extend_start, OBJ_PTR extend_end) { int ierr=0;
   c_private_radial_shading(fmkr, Get_FM(fmkr, &ierr), 
      Number_to_double(x0, &ierr), Number_to_double(y0, &ierr), Number_to_double(r0, &ierr),
      Number_to_double(x1, &ierr), Number_to_double(y1, &ierr), Number_to_double(r1, &ierr), colormap,
      Number_to_double(a, &ierr), Number_to_double(b, &ierr), Number_to_double(c, &ierr), Number_to_double(d, &ierr),
      extend_start == OBJ_TRUE, extend_end == OBJ_TRUE, &ierr); RETURN_NIL; }
OBJ_PTR FM_private_create_colormap(OBJ_PTR fmkr, OBJ_PTR rgb_flag,
    OBJ_PTR length, OBJ_PTR Ps, OBJ_PTR C1s, OBJ_PTR C2s, OBJ_PTR C3s) { int ierr=0;
       return c_private_create_colormap(fmkr, Get_FM(fmkr, &ierr), rgb_flag != OBJ_FALSE, Number_to_int(length, &ierr), Ps, C1s, C2s, C3s, &ierr); }
OBJ_PTR FM_get_color_from_colormap(OBJ_PTR fmkr, OBJ_PTR color_map, OBJ_PTR color_position) { int ierr=0;
   return c_get_color_from_colormap(fmkr, Get_FM(fmkr, &ierr), color_map, Number_to_double(color_position, &ierr), &ierr); }
OBJ_PTR FM_convert_to_colormap(OBJ_PTR fmkr, OBJ_PTR Rs, OBJ_PTR Gs, OBJ_PTR Bs) { int ierr=0;
   return c_convert_to_colormap(fmkr, Get_FM(fmkr, &ierr), Rs, Gs, Bs, &ierr); }
OBJ_PTR FM_hls_to_rgb(OBJ_PTR fmkr, OBJ_PTR hls_vec) { int ierr=0;
   return c_hls_to_rgb(Qnil, NULL, hls_vec, &ierr); }
OBJ_PTR FM_rgb_to_hls(OBJ_PTR fmkr, OBJ_PTR rgb_vec) { int ierr=0;
   return c_rgb_to_hls(Qnil, NULL, rgb_vec, &ierr); }
OBJ_PTR FM_title_color_set(OBJ_PTR fmkr, OBJ_PTR val) { int ierr=0;
   c_title_color_set(fmkr, Get_FM(fmkr, &ierr), val, &ierr); RETURN_NIL; }
OBJ_PTR FM_title_color_get(OBJ_PTR fmkr) { int ierr=0;
   return c_title_color_get(fmkr, Get_FM(fmkr, &ierr), &ierr); }
OBJ_PTR FM_xlabel_color_set(OBJ_PTR fmkr, OBJ_PTR val) { int ierr=0; 
   c_xlabel_color_set(fmkr, Get_FM(fmkr, &ierr), val, &ierr); RETURN_NIL; }
OBJ_PTR FM_xlabel_color_get(OBJ_PTR fmkr) { int ierr=0;
   return c_xlabel_color_get(fmkr, Get_FM(fmkr, &ierr), &ierr); }
OBJ_PTR FM_ylabel_color_set(OBJ_PTR fmkr, OBJ_PTR val) { int ierr=0;
   c_ylabel_color_set(fmkr, Get_FM(fmkr, &ierr), val, &ierr); RETURN_NIL; }
OBJ_PTR FM_ylabel_color_get(OBJ_PTR fmkr) { int ierr=0;
   return c_ylabel_color_get(fmkr, Get_FM(fmkr, &ierr), &ierr); }

OBJ_PTR FM_xaxis_stroke_color_set(OBJ_PTR fmkr, OBJ_PTR val) { int ierr=0;
   c_xaxis_stroke_color_set(fmkr, Get_FM(fmkr, &ierr), val, &ierr); RETURN_NIL; }
OBJ_PTR FM_xaxis_stroke_color_get(OBJ_PTR fmkr) { int ierr=0;
   return c_xaxis_stroke_color_get(fmkr, Get_FM(fmkr, &ierr), &ierr); }
OBJ_PTR FM_yaxis_stroke_color_set(OBJ_PTR fmkr, OBJ_PTR val) { int ierr=0;
   c_yaxis_stroke_color_set(fmkr, Get_FM(fmkr, &ierr), val, &ierr); RETURN_NIL; }
OBJ_PTR FM_yaxis_stroke_color_get(OBJ_PTR fmkr) { int ierr=0;
   return c_yaxis_stroke_color_get(fmkr, Get_FM(fmkr, &ierr), &ierr); }

OBJ_PTR FM_xaxis_labels_color_set(OBJ_PTR fmkr, OBJ_PTR val) { int ierr=0;
   c_xaxis_labels_color_set(fmkr, Get_FM(fmkr, &ierr), val, &ierr); RETURN_NIL; }
OBJ_PTR FM_xaxis_labels_color_get(OBJ_PTR fmkr) { int ierr=0;
   return c_xaxis_labels_color_get(fmkr, Get_FM(fmkr, &ierr), &ierr); }
OBJ_PTR FM_yaxis_labels_color_set(OBJ_PTR fmkr, OBJ_PTR val) { int ierr=0;
   c_yaxis_labels_color_set(fmkr, Get_FM(fmkr, &ierr), val, &ierr); RETURN_NIL; }
OBJ_PTR FM_yaxis_labels_color_get(OBJ_PTR fmkr) { int ierr=0;
   return c_yaxis_labels_color_get(fmkr, Get_FM(fmkr, &ierr), &ierr); }

OBJ_PTR FM_string_hls_to_rgb_bang(OBJ_PTR fmkr, OBJ_PTR str) { int ierr=0;
   c_string_hls_to_rgb_bang(Qnil, NULL,  
      (unsigned char *)String_Ptr(str, &ierr), String_Len(str, &ierr), &ierr);
   return str; }
OBJ_PTR FM_string_rgb_to_hls_bang(OBJ_PTR fmkr, OBJ_PTR str) { int ierr=0;
   c_string_rgb_to_hls_bang(Qnil, NULL,
      (unsigned char *)String_Ptr(str, &ierr), String_Len(str, &ierr), &ierr);
   return str; }

// pdfcoords.c
OBJ_PTR FM_private_set_subframe(OBJ_PTR fmkr, OBJ_PTR left_margin, OBJ_PTR right_margin, 
      OBJ_PTR top_margin, OBJ_PTR bottom_margin) { int ierr=0;
   c_set_subframe(fmkr, Get_FM(fmkr, &ierr), Number_to_double(left_margin, &ierr), Number_to_double(right_margin, &ierr), 
      Number_to_double(top_margin, &ierr), Number_to_double(bottom_margin, &ierr), &ierr); RETURN_NIL; }
OBJ_PTR FM_private_set_default_font_size(OBJ_PTR fmkr, OBJ_PTR size) { int ierr=0;
   c_private_set_default_font_size(fmkr, Get_FM(fmkr, &ierr), Number_to_double(size, &ierr), &ierr); RETURN_NIL; }
OBJ_PTR FM_convert_to_degrees(OBJ_PTR fmkr, OBJ_PTR dx, OBJ_PTR dy) { int ierr=0;
   return c_convert_to_degrees(fmkr, Get_FM(fmkr, &ierr), Number_to_double(dx, &ierr), Number_to_double(dy, &ierr), &ierr); }
OBJ_PTR FM_convert_inches_to_output(OBJ_PTR fmkr, OBJ_PTR v) { int ierr=0;
   return c_convert_inches_to_output(fmkr, Get_FM(fmkr, &ierr), Number_to_double(v, &ierr), &ierr); }
OBJ_PTR FM_convert_output_to_inches(OBJ_PTR fmkr, OBJ_PTR v) { int ierr=0;
   return c_convert_output_to_inches(fmkr, Get_FM(fmkr, &ierr), Number_to_double(v, &ierr), &ierr); }
OBJ_PTR FM_convert_mm_to_output(OBJ_PTR fmkr, OBJ_PTR v) { int ierr=0;
   return c_convert_mm_to_output(fmkr, Get_FM(fmkr, &ierr), Number_to_double(v, &ierr), &ierr); }
OBJ_PTR FM_convert_output_to_mm(OBJ_PTR fmkr, OBJ_PTR v) { int ierr=0;
   return c_convert_output_to_mm(fmkr, Get_FM(fmkr, &ierr), Number_to_double(v, &ierr), &ierr); }
OBJ_PTR FM_convert_page_to_output_x(OBJ_PTR fmkr, OBJ_PTR v) { int ierr=0;
   return c_convert_page_to_output_x(fmkr, Get_FM(fmkr, &ierr), Number_to_double(v, &ierr), &ierr); }
OBJ_PTR FM_convert_page_to_output_y(OBJ_PTR fmkr, OBJ_PTR v) { int ierr=0;
   return c_convert_page_to_output_y(fmkr, Get_FM(fmkr, &ierr), Number_to_double(v, &ierr), &ierr); }
OBJ_PTR FM_convert_page_to_output_dx(OBJ_PTR fmkr, OBJ_PTR v) { int ierr=0;
   return c_convert_page_to_output_dx(fmkr, Get_FM(fmkr, &ierr), Number_to_double(v, &ierr), &ierr); }
OBJ_PTR FM_convert_page_to_output_dy(OBJ_PTR fmkr, OBJ_PTR v) { int ierr=0;
   return c_convert_page_to_output_dy(fmkr, Get_FM(fmkr, &ierr), Number_to_double(v, &ierr), &ierr); }
OBJ_PTR FM_convert_output_to_page_x(OBJ_PTR fmkr, OBJ_PTR v) { int ierr=0;
   return c_convert_output_to_page_x(fmkr, Get_FM(fmkr, &ierr), Number_to_double(v, &ierr), &ierr); }
OBJ_PTR FM_convert_output_to_page_y(OBJ_PTR fmkr, OBJ_PTR v) { int ierr=0;
   return c_convert_output_to_page_y(fmkr, Get_FM(fmkr, &ierr), Number_to_double(v, &ierr), &ierr); }
OBJ_PTR FM_convert_output_to_page_dx(OBJ_PTR fmkr, OBJ_PTR v) { int ierr=0;
   return c_convert_output_to_page_dx(fmkr, Get_FM(fmkr, &ierr), Number_to_double(v, &ierr), &ierr); }
OBJ_PTR FM_convert_output_to_page_dy(OBJ_PTR fmkr, OBJ_PTR v) { int ierr=0;
   return c_convert_output_to_page_dy(fmkr, Get_FM(fmkr, &ierr), Number_to_double(v, &ierr), &ierr); }
OBJ_PTR FM_convert_frame_to_page_x(OBJ_PTR fmkr, OBJ_PTR v) { int ierr=0;
   return c_convert_frame_to_page_x(fmkr, Get_FM(fmkr, &ierr), Number_to_double(v, &ierr), &ierr); }
OBJ_PTR FM_convert_frame_to_page_y(OBJ_PTR fmkr, OBJ_PTR v) { int ierr=0;
   return c_convert_frame_to_page_y(fmkr, Get_FM(fmkr, &ierr), Number_to_double(v, &ierr), &ierr); }
OBJ_PTR FM_convert_frame_to_page_dx(OBJ_PTR fmkr, OBJ_PTR v) { int ierr=0;
   return c_convert_frame_to_page_dx(fmkr, Get_FM(fmkr, &ierr), Number_to_double(v, &ierr), &ierr); }
OBJ_PTR FM_convert_frame_to_page_dy(OBJ_PTR fmkr, OBJ_PTR v) { int ierr=0;
   return c_convert_frame_to_page_dy(fmkr, Get_FM(fmkr, &ierr), Number_to_double(v, &ierr), &ierr); }
OBJ_PTR FM_convert_page_to_frame_x(OBJ_PTR fmkr, OBJ_PTR v) { int ierr=0;
   return c_convert_page_to_frame_x(fmkr, Get_FM(fmkr, &ierr), Number_to_double(v, &ierr), &ierr); }
OBJ_PTR FM_convert_page_to_frame_y(OBJ_PTR fmkr, OBJ_PTR v) { int ierr=0;
   return c_convert_page_to_frame_y(fmkr, Get_FM(fmkr, &ierr), Number_to_double(v, &ierr), &ierr); }
OBJ_PTR FM_convert_page_to_frame_dx(OBJ_PTR fmkr, OBJ_PTR v) { int ierr=0;
   return c_convert_page_to_frame_dx(fmkr, Get_FM(fmkr, &ierr), Number_to_double(v, &ierr), &ierr); }
OBJ_PTR FM_convert_page_to_frame_dy(OBJ_PTR fmkr, OBJ_PTR v) { int ierr=0;
   return c_convert_page_to_frame_dy(fmkr, Get_FM(fmkr, &ierr), Number_to_double(v, &ierr), &ierr); }
OBJ_PTR FM_convert_figure_to_frame_x(OBJ_PTR fmkr, OBJ_PTR v) { int ierr=0;
   return c_convert_figure_to_frame_x(fmkr, Get_FM(fmkr, &ierr), Number_to_double(v, &ierr), &ierr); }
OBJ_PTR FM_convert_figure_to_frame_y(OBJ_PTR fmkr, OBJ_PTR v) { int ierr=0;
   return c_convert_figure_to_frame_y(fmkr, Get_FM(fmkr, &ierr), Number_to_double(v, &ierr), &ierr); }
OBJ_PTR FM_convert_figure_to_frame_dx(OBJ_PTR fmkr, OBJ_PTR v) { int ierr=0;
   return c_convert_figure_to_frame_dx(fmkr, Get_FM(fmkr, &ierr), Number_to_double(v, &ierr), &ierr); }
OBJ_PTR FM_convert_figure_to_frame_dy(OBJ_PTR fmkr, OBJ_PTR v) { int ierr=0;
   return c_convert_figure_to_frame_dy(fmkr, Get_FM(fmkr, &ierr), Number_to_double(v, &ierr), &ierr); }
OBJ_PTR FM_convert_frame_to_figure_x(OBJ_PTR fmkr, OBJ_PTR v) { int ierr=0;
   return c_convert_frame_to_figure_x(fmkr, Get_FM(fmkr, &ierr), Number_to_double(v, &ierr), &ierr); }
OBJ_PTR FM_convert_frame_to_figure_y(OBJ_PTR fmkr, OBJ_PTR v) { int ierr=0;
   return c_convert_frame_to_figure_y(fmkr, Get_FM(fmkr, &ierr), Number_to_double(v, &ierr), &ierr); }
OBJ_PTR FM_convert_frame_to_figure_dx(OBJ_PTR fmkr, OBJ_PTR v) { int ierr=0;
   return c_convert_frame_to_figure_dx(fmkr, Get_FM(fmkr, &ierr), Number_to_double(v, &ierr), &ierr); }
OBJ_PTR FM_convert_frame_to_figure_dy(OBJ_PTR fmkr, OBJ_PTR v) { int ierr=0;
   return c_convert_frame_to_figure_dy(fmkr, Get_FM(fmkr, &ierr), Number_to_double(v, &ierr), &ierr); }
OBJ_PTR FM_convert_figure_to_output_x(OBJ_PTR fmkr, OBJ_PTR v) { int ierr=0;
   return c_convert_figure_to_output_x(fmkr, Get_FM(fmkr, &ierr), Number_to_double(v, &ierr), &ierr); }
OBJ_PTR FM_convert_figure_to_output_y(OBJ_PTR fmkr, OBJ_PTR v) { int ierr=0;
   return c_convert_figure_to_output_y(fmkr, Get_FM(fmkr, &ierr), Number_to_double(v, &ierr), &ierr); }
OBJ_PTR FM_convert_figure_to_output_dx(OBJ_PTR fmkr, OBJ_PTR v) { int ierr=0;
   return c_convert_figure_to_output_dx(fmkr, Get_FM(fmkr, &ierr), Number_to_double(v, &ierr), &ierr); }
OBJ_PTR FM_convert_figure_to_output_dy(OBJ_PTR fmkr, OBJ_PTR v) { int ierr=0;
   return c_convert_figure_to_output_dy(fmkr, Get_FM(fmkr, &ierr), Number_to_double(v, &ierr), &ierr); }
OBJ_PTR FM_convert_output_to_figure_x(OBJ_PTR fmkr, OBJ_PTR v) { int ierr=0;
   return c_convert_output_to_figure_x(fmkr, Get_FM(fmkr, &ierr), Number_to_double(v, &ierr), &ierr); }
OBJ_PTR FM_convert_output_to_figure_y(OBJ_PTR fmkr, OBJ_PTR v) { int ierr=0;
   return c_convert_output_to_figure_y(fmkr, Get_FM(fmkr, &ierr), Number_to_double(v, &ierr), &ierr); }
OBJ_PTR FM_convert_output_to_figure_dx(OBJ_PTR fmkr, OBJ_PTR v) { int ierr=0;
   return c_convert_output_to_figure_dx(fmkr, Get_FM(fmkr, &ierr), Number_to_double(v, &ierr), &ierr); }
OBJ_PTR FM_convert_output_to_figure_dy(OBJ_PTR fmkr, OBJ_PTR v) { int ierr=0;
   return c_convert_output_to_figure_dy(fmkr, Get_FM(fmkr, &ierr), Number_to_double(v, &ierr), &ierr); RETURN_NIL; }
OBJ_PTR FM_doing_subplot(OBJ_PTR fmkr) { int ierr=0; c_doing_subplot(fmkr, Get_FM(fmkr, &ierr), &ierr); RETURN_NIL; }
OBJ_PTR FM_doing_subfigure(OBJ_PTR fmkr) { int ierr=0; c_doing_subfigure(fmkr, Get_FM(fmkr, &ierr), &ierr); RETURN_NIL; }
OBJ_PTR FM_private_set_bounds(OBJ_PTR fmkr, OBJ_PTR left, OBJ_PTR right, OBJ_PTR top, OBJ_PTR bottom) { int ierr=0;
   c_private_set_bounds(fmkr, Get_FM(fmkr, &ierr), Number_to_double(left, &ierr), Number_to_double(right, &ierr),
      Number_to_double(top, &ierr), Number_to_double(bottom, &ierr), &ierr); RETURN_NIL; }

// pdffile.c
OBJ_PTR FM_pdf_gsave(OBJ_PTR fmkr) { int ierr=0; c_pdf_gsave(fmkr, Get_FM(fmkr, &ierr), &ierr); RETURN_NIL; }
OBJ_PTR FM_pdf_grestore(OBJ_PTR fmkr) { int ierr=0; c_pdf_grestore(fmkr, Get_FM(fmkr, &ierr), &ierr); RETURN_NIL; }

// pdfimage.c
OBJ_PTR FM_private_create_image_data(OBJ_PTR fmkr, OBJ_PTR data,
            OBJ_PTR first_row, OBJ_PTR last_row, OBJ_PTR first_column, OBJ_PTR last_column,
            OBJ_PTR min_val, OBJ_PTR max_val, OBJ_PTR max_code, OBJ_PTR if_below_range, OBJ_PTR if_above_range)
{ int ierr=0;
   return c_private_create_image_data(fmkr, Get_FM(fmkr, &ierr), data,
      Number_to_int(first_row, &ierr), Number_to_int(last_row, &ierr), 
      Number_to_int(first_column, &ierr), Number_to_int(last_column, &ierr),
      Number_to_double(min_val, &ierr), Number_to_double(max_val, &ierr), Number_to_int(max_code, &ierr), 
      Number_to_int(if_below_range, &ierr), Number_to_int(if_above_range, &ierr), &ierr);
}
OBJ_PTR FM_private_create_monochrome_image_data(OBJ_PTR fmkr, OBJ_PTR data,
            OBJ_PTR first_row, OBJ_PTR last_row, OBJ_PTR first_column, OBJ_PTR last_column,
            OBJ_PTR boundary, OBJ_PTR reverse)
{ int ierr=0;
   return c_private_create_monochrome_image_data(fmkr, Get_FM(fmkr, &ierr), data,
      Number_to_int(first_row, &ierr), Number_to_int(last_row, &ierr), Number_to_int(first_column, &ierr), Number_to_int(last_column, &ierr),
      Number_to_double(boundary, &ierr), reverse != OBJ_FALSE, &ierr);
}
OBJ_PTR FM_private_register_jpg(OBJ_PTR fmkr, OBJ_PTR filename, 
   OBJ_PTR width, OBJ_PTR height, OBJ_PTR mask_obj_num) { int ierr=0;
   return Integer_New(c_private_register_jpg(fmkr, Get_FM(fmkr, &ierr), String_Ptr(filename, &ierr), 
                                             Number_to_int(width, &ierr), Number_to_int(height, &ierr), Number_to_int(mask_obj_num, &ierr), &ierr)); 
}

/* Get info on a JPEG file */
OBJ_PTR FM_jpg_info(OBJ_PTR fmkr, OBJ_PTR filename) {
   int ierr = 0;
   JPG_Info * info = Parse_JPG(String_Ptr(filename, &ierr));
   if(info) {
      OBJ_PTR hsh = Hash_New();
      Hash_Set_Obj(hsh, "width", Integer_New(info->width));
      Hash_Set_Obj(hsh, "height", Integer_New(info->height));
      Hash_Set_Obj(hsh, "jpg", filename);
      Free_JPG(info);
      return hsh;
   }
   else 
      return OBJ_NIL;
}

#define Str_Or_NULL(obj, ierr) ((obj == OBJ_NIL) ? NULL : CString_Ptr(obj, ierr))
      
OBJ_PTR FM_private_register_hls_image(OBJ_PTR fmkr, 
                                      OBJ_PTR interpolate, OBJ_PTR w, OBJ_PTR h, OBJ_PTR data, OBJ_PTR mask_obj_num, OBJ_PTR components, OBJ_PTR filters)
{ int ierr=0;
   return Integer_New(c_private_register_image(fmkr, Get_FM(fmkr, &ierr), HLS_IMAGE, 
                                   (interpolate != OBJ_FALSE), OBJ_FALSE, 
      Number_to_int(w, &ierr), Number_to_int(h, &ierr), (unsigned char *)String_Ptr(data, &ierr), String_Len(data, &ierr), 
      OBJ_NIL, OBJ_NIL, OBJ_NIL, OBJ_NIL, Number_to_int(mask_obj_num, &ierr),
                               Number_to_int(components, &ierr), 
                                               Str_Or_NULL(filters, &ierr), &ierr));
}
      
OBJ_PTR FM_private_register_rgb_image(OBJ_PTR fmkr, OBJ_PTR interpolate, OBJ_PTR w, OBJ_PTR h, OBJ_PTR data, OBJ_PTR mask_obj_num, OBJ_PTR components, OBJ_PTR filters)
{ int ierr=0;
   return Integer_New(c_private_register_image(fmkr, Get_FM(fmkr, &ierr), RGB_IMAGE, 
                                   (interpolate != OBJ_FALSE), OBJ_FALSE, 
      Number_to_int(w, &ierr), Number_to_int(h, &ierr), (unsigned char *)String_Ptr(data, &ierr), String_Len(data, &ierr), 
      OBJ_NIL, OBJ_NIL, OBJ_NIL, OBJ_NIL, Number_to_int(mask_obj_num, &ierr),
                               Number_to_int(components, &ierr), 
                                               Str_Or_NULL(filters, &ierr), &ierr));
}

OBJ_PTR FM_private_register_cmyk_image(OBJ_PTR fmkr, OBJ_PTR interpolate, OBJ_PTR w, OBJ_PTR h, OBJ_PTR data, OBJ_PTR mask_obj_num, OBJ_PTR components, OBJ_PTR filters)
{ int ierr=0;
   return Integer_New(c_private_register_image(fmkr, Get_FM(fmkr, &ierr), CMYK_IMAGE, (interpolate != OBJ_FALSE), false, 
      Number_to_int(w, &ierr), Number_to_int(h, &ierr), (unsigned char *)String_Ptr(data, &ierr), String_Len(data, &ierr), 
      OBJ_NIL, OBJ_NIL, OBJ_NIL, OBJ_NIL, Number_to_int(mask_obj_num, &ierr),
                               Number_to_int(components, &ierr), 
                                               Str_Or_NULL(filters, &ierr), &ierr));
}

OBJ_PTR FM_private_register_grayscale_image(OBJ_PTR fmkr, OBJ_PTR interpolate, OBJ_PTR w, OBJ_PTR h, OBJ_PTR data, OBJ_PTR mask_obj_num, OBJ_PTR components, OBJ_PTR filters)
{ int ierr=0;
   return Integer_New(c_private_register_image(fmkr, Get_FM(fmkr, &ierr), GRAY_IMAGE, (interpolate != OBJ_FALSE), false, 
      Number_to_int(w, &ierr), Number_to_int(h, &ierr), (unsigned char *)String_Ptr(data, &ierr), String_Len(data, &ierr), 
      OBJ_NIL, OBJ_NIL, OBJ_NIL, OBJ_NIL, Number_to_int(mask_obj_num, &ierr),
                               Number_to_int(components, &ierr), 
                                               Str_Or_NULL(filters, &ierr), &ierr));
}

OBJ_PTR FM_private_register_monochrome_image(OBJ_PTR fmkr, OBJ_PTR interpolate, OBJ_PTR reversed, OBJ_PTR w, OBJ_PTR h, OBJ_PTR data, OBJ_PTR mask_obj_num, OBJ_PTR filters)
{ int ierr=0;
   return Integer_New(c_private_register_image(fmkr, Get_FM(fmkr, &ierr), MONO_IMAGE, (interpolate != OBJ_FALSE), (reversed == OBJ_TRUE), 
      Number_to_int(w, &ierr), Number_to_int(h, &ierr), (unsigned char *)String_Ptr(data, &ierr), String_Len(data, &ierr), 
                               OBJ_NIL, OBJ_NIL, OBJ_NIL, OBJ_NIL, Number_to_int(mask_obj_num, &ierr), 1, 
                                               Str_Or_NULL(filters, &ierr), &ierr));
}

OBJ_PTR FM_private_register_image(OBJ_PTR fmkr,OBJ_PTR interpolate, OBJ_PTR w, OBJ_PTR h, OBJ_PTR data,
    OBJ_PTR value_mask_min, OBJ_PTR value_mask_max, OBJ_PTR hival, OBJ_PTR lookup, OBJ_PTR mask_obj_num, OBJ_PTR components, OBJ_PTR filters)
{ int ierr=0;
   return Integer_New(c_private_register_image(fmkr, Get_FM(fmkr, &ierr), COLORMAP_IMAGE, (interpolate != OBJ_FALSE), false, 
      Number_to_int(w, &ierr), Number_to_int(h, &ierr), (unsigned char *)String_Ptr(data, &ierr), String_Len(data, &ierr), 
      value_mask_min, value_mask_max, hival, lookup, Number_to_int(mask_obj_num, &ierr),
                               Number_to_int(components, &ierr), 
                                               Str_Or_NULL(filters, &ierr), &ierr));
}

OBJ_PTR FM_private_show_image_from_ref(OBJ_PTR fmkr, OBJ_PTR ref, OBJ_PTR llx, OBJ_PTR lly, OBJ_PTR lrx, OBJ_PTR lry, OBJ_PTR ulx, OBJ_PTR uly)
{
   int ierr=0;
   c_private_show_image_from_ref(fmkr, Get_FM(fmkr, &ierr), 
                                 Number_to_int(ref, &ierr),
                                 Number_to_double(llx, &ierr), Number_to_double(lly, &ierr), 
                                 Number_to_double(lrx, &ierr), Number_to_double(lry, &ierr), 
                                 Number_to_double(ulx, &ierr), Number_to_double(uly, &ierr), &ierr);
   RETURN_NIL;
}


// pdfpath.c
OBJ_PTR FM_stroke_color_set(OBJ_PTR fmkr, OBJ_PTR value) { int ierr=0; 
   c_stroke_color_set(fmkr, Get_FM(fmkr, &ierr), value, &ierr); RETURN_NIL; }
OBJ_PTR FM_stroke_color_get(OBJ_PTR fmkr) { int ierr=0;
   return c_stroke_color_get(fmkr, Get_FM(fmkr, &ierr), &ierr); }
OBJ_PTR FM_fill_color_set(OBJ_PTR fmkr, OBJ_PTR value) { int ierr=0;
   c_fill_color_set(fmkr, Get_FM(fmkr, &ierr), value, &ierr); RETURN_NIL; }
OBJ_PTR FM_fill_color_get(OBJ_PTR fmkr) { int ierr=0; 
   return c_fill_color_get(fmkr, Get_FM(fmkr, &ierr), &ierr); }
OBJ_PTR FM_line_width_set(OBJ_PTR fmkr, OBJ_PTR val) { int ierr=0; 
   c_line_width_set(fmkr, Get_FM(fmkr, &ierr), Number_to_double(val, &ierr), &ierr); RETURN_NIL; }
OBJ_PTR FM_rescale_lines(OBJ_PTR fmkr, OBJ_PTR scaling_factor) { int ierr=0;
   c_rescale_lines(fmkr, Get_FM(fmkr, &ierr), Number_to_double(scaling_factor, &ierr), &ierr); RETURN_NIL; }
OBJ_PTR FM_line_cap_set(OBJ_PTR fmkr, OBJ_PTR val) { int ierr=0;
   c_line_cap_set(fmkr, Get_FM(fmkr, &ierr), Number_to_int(val, &ierr), &ierr); RETURN_NIL; }
OBJ_PTR FM_line_join_set(OBJ_PTR fmkr, OBJ_PTR val) { int ierr=0;
   c_line_join_set(fmkr, Get_FM(fmkr, &ierr), Number_to_int(val, &ierr), &ierr); RETURN_NIL; }
OBJ_PTR FM_miter_limit_set(OBJ_PTR fmkr, OBJ_PTR val) { int ierr=0;
   c_miter_limit_set(fmkr, Get_FM(fmkr, &ierr), Number_to_double(val, &ierr), &ierr); RETURN_NIL; }
OBJ_PTR FM_line_type_set(OBJ_PTR fmkr, OBJ_PTR line_type) { int ierr=0;
   c_line_type_set(fmkr, Get_FM(fmkr, &ierr), line_type, &ierr); RETURN_NIL; }
OBJ_PTR FM_update_bbox(OBJ_PTR fmkr, OBJ_PTR x, OBJ_PTR y) { int ierr=0;
   c_update_bbox(fmkr, Get_FM(fmkr, &ierr), Number_to_double(x, &ierr), Number_to_double(y, &ierr), &ierr); RETURN_NIL; }
OBJ_PTR FM_bbox_left(OBJ_PTR fmkr) { int ierr=0; return c_bbox_left(fmkr, Get_FM(fmkr, &ierr), &ierr); RETURN_NIL; }
OBJ_PTR FM_bbox_right(OBJ_PTR fmkr) { int ierr=0; return c_bbox_right(fmkr, Get_FM(fmkr, &ierr), &ierr); RETURN_NIL; }
OBJ_PTR FM_bbox_top(OBJ_PTR fmkr) { int ierr=0; return c_bbox_top(fmkr, Get_FM(fmkr, &ierr), &ierr); RETURN_NIL; }
OBJ_PTR FM_bbox_bottom(OBJ_PTR fmkr) { int ierr=0; return c_bbox_bottom(fmkr, Get_FM(fmkr, &ierr), &ierr); RETURN_NIL; }
OBJ_PTR FM_move_to_point(OBJ_PTR fmkr, OBJ_PTR x, OBJ_PTR y) { int ierr=0;
   c_move_to_point(fmkr, Get_FM(fmkr, &ierr), Number_to_double(x, &ierr), Number_to_double(y, &ierr), &ierr); RETURN_NIL; }   
OBJ_PTR FM_append_point_to_path(OBJ_PTR fmkr, OBJ_PTR x, OBJ_PTR y) { int ierr=0;
   c_append_point_to_path(fmkr, Get_FM(fmkr, &ierr), Number_to_double(x, &ierr), Number_to_double(y, &ierr), &ierr); RETURN_NIL; }
OBJ_PTR FM_bezier_control_points(OBJ_PTR fmkr, OBJ_PTR x0, OBJ_PTR y0, OBJ_PTR delta_x, OBJ_PTR a, OBJ_PTR b, OBJ_PTR c) { int ierr=0;
   return c_bezier_control_points(fmkr, Get_FM(fmkr, &ierr),
      Number_to_double(x0, &ierr), Number_to_double(y0, &ierr), Number_to_double(delta_x, &ierr),
      Number_to_double(a, &ierr), Number_to_double(b, &ierr), Number_to_double(c, &ierr), &ierr);
}
OBJ_PTR FM_append_curve_to_path(OBJ_PTR fmkr, OBJ_PTR x1, OBJ_PTR y1, OBJ_PTR x2, OBJ_PTR y2, OBJ_PTR x3, OBJ_PTR y3) { int ierr=0;
   c_append_curve_to_path(fmkr, Get_FM(fmkr, &ierr), Number_to_double(x1, &ierr), Number_to_double(y1, &ierr), 
      Number_to_double(x2, &ierr), Number_to_double(y2, &ierr), 
      Number_to_double(x3, &ierr), Number_to_double(y3, &ierr), &ierr); RETURN_NIL; }
OBJ_PTR FM_close_path(OBJ_PTR fmkr) { int ierr=0; c_close_path(fmkr, Get_FM(fmkr, &ierr), &ierr); RETURN_NIL; }
OBJ_PTR FM_append_arc_to_path(OBJ_PTR fmkr, OBJ_PTR x_start, OBJ_PTR y_start, OBJ_PTR x_corner, OBJ_PTR y_corner,
   OBJ_PTR x_end, OBJ_PTR y_end, OBJ_PTR dx, OBJ_PTR dy) { int ierr=0;
    // dx and dy are converted to output coords and smaller is used as radius
   c_append_arc_to_path(fmkr, Get_FM(fmkr, &ierr),
      Number_to_double(x_start, &ierr), Number_to_double(y_start, &ierr),
      Number_to_double(x_corner, &ierr), Number_to_double(y_corner, &ierr),
      Number_to_double(x_end, &ierr), Number_to_double(y_end, &ierr), 
      Number_to_double(dx, &ierr), Number_to_double(dy, &ierr), &ierr);
   RETURN_NIL;
}
OBJ_PTR FM_append_rect_to_path(OBJ_PTR fmkr, OBJ_PTR x, OBJ_PTR y, OBJ_PTR width, OBJ_PTR height) { int ierr=0;
   c_append_rect_to_path(fmkr, Get_FM(fmkr, &ierr), Number_to_double(x, &ierr), Number_to_double(y, &ierr), 
      Number_to_double(width, &ierr), Number_to_double(height, &ierr), &ierr);
   RETURN_NIL;
}
OBJ_PTR FM_append_rounded_rect_to_path(OBJ_PTR fmkr, OBJ_PTR x, OBJ_PTR y, 
   OBJ_PTR width, OBJ_PTR height, OBJ_PTR dx, OBJ_PTR dy) { int ierr=0; // dx and dy are converted to output coords and smaller is used as radius
   c_append_rounded_rect_to_path(fmkr, Get_FM(fmkr, &ierr),
      Number_to_double(x, &ierr), Number_to_double(y, &ierr),
      Number_to_double(width, &ierr), Number_to_double(height, &ierr), 
      Number_to_double(dx, &ierr), Number_to_double(dy, &ierr), &ierr);
   RETURN_NIL;
}
OBJ_PTR FM_append_oval_to_path(OBJ_PTR fmkr, OBJ_PTR x, OBJ_PTR y, OBJ_PTR dx, OBJ_PTR dy, OBJ_PTR angle) { int ierr=0;
   c_append_oval_to_path(fmkr, Get_FM(fmkr, &ierr),
      Number_to_double(x, &ierr), Number_to_double(y, &ierr), Number_to_double(dx, &ierr), Number_to_double(dy, &ierr), Number_to_double(angle, &ierr), &ierr); RETURN_NIL; }
OBJ_PTR FM_append_circle_to_path(OBJ_PTR fmkr, OBJ_PTR x, OBJ_PTR y, OBJ_PTR dx) { int ierr=0;
   c_append_circle_to_path(fmkr, Get_FM(fmkr, &ierr), Number_to_double(x, &ierr), Number_to_double(y, &ierr), Number_to_double(dx, &ierr), &ierr); RETURN_NIL; } 
OBJ_PTR FM_append_points_to_path(OBJ_PTR fmkr, OBJ_PTR x_vec, OBJ_PTR y_vec) { int ierr=0;
   c_append_points_to_path(fmkr, Get_FM(fmkr, &ierr), x_vec, y_vec, &ierr); RETURN_NIL; } 
OBJ_PTR FM_private_append_points_with_gaps_to_path(OBJ_PTR fmkr, OBJ_PTR x_vec, OBJ_PTR y_vec, OBJ_PTR gaps, OBJ_PTR close_gaps) { int ierr=0;
   c_private_append_points_with_gaps_to_path(fmkr, Get_FM(fmkr, &ierr), x_vec, y_vec, gaps, (close_gaps == OBJ_TRUE), &ierr); RETURN_NIL; }
OBJ_PTR FM_stroke(OBJ_PTR fmkr) { int ierr=0; c_stroke(fmkr, Get_FM(fmkr, &ierr), &ierr); RETURN_NIL; }
OBJ_PTR FM_close_and_stroke(OBJ_PTR fmkr) { int ierr=0; c_close_and_stroke(fmkr, Get_FM(fmkr, &ierr), &ierr); RETURN_NIL; }
OBJ_PTR FM_fill(OBJ_PTR fmkr) { int ierr=0; c_fill(fmkr, Get_FM(fmkr, &ierr), &ierr); RETURN_NIL; }
OBJ_PTR FM_discard_path(OBJ_PTR fmkr) { int ierr=0; c_discard_path(fmkr, Get_FM(fmkr, &ierr), &ierr); RETURN_NIL; }
OBJ_PTR FM_eofill(OBJ_PTR fmkr) { int ierr=0; c_eofill(fmkr, Get_FM(fmkr, &ierr), &ierr); RETURN_NIL; }
OBJ_PTR FM_fill_and_stroke(OBJ_PTR fmkr) { int ierr=0; c_fill_and_stroke(fmkr, Get_FM(fmkr, &ierr), &ierr); RETURN_NIL; }
OBJ_PTR FM_eofill_and_stroke(OBJ_PTR fmkr) { int ierr=0; c_eofill_and_stroke(fmkr, Get_FM(fmkr, &ierr), &ierr); RETURN_NIL; }
OBJ_PTR FM_close_fill_and_stroke(OBJ_PTR fmkr) { int ierr=0; c_close_fill_and_stroke(fmkr, Get_FM(fmkr, &ierr), &ierr); RETURN_NIL; }
OBJ_PTR FM_close_eofill_and_stroke(OBJ_PTR fmkr) { int ierr=0; c_close_eofill_and_stroke(fmkr, Get_FM(fmkr, &ierr), &ierr); RETURN_NIL; }
OBJ_PTR FM_clip(OBJ_PTR fmkr) { int ierr=0; c_clip(fmkr, Get_FM(fmkr, &ierr), &ierr); RETURN_NIL; }
OBJ_PTR FM_eoclip(OBJ_PTR fmkr) { int ierr=0; c_eoclip(fmkr, Get_FM(fmkr, &ierr), &ierr); RETURN_NIL; }
OBJ_PTR FM_fill_and_clip(OBJ_PTR fmkr) { int ierr=0; c_fill_and_clip(fmkr, Get_FM(fmkr, &ierr), &ierr); RETURN_NIL; }
OBJ_PTR FM_stroke_and_clip(OBJ_PTR fmkr) { int ierr=0; c_stroke_and_clip(fmkr, Get_FM(fmkr, &ierr), &ierr); RETURN_NIL; }
OBJ_PTR FM_fill_stroke_and_clip(OBJ_PTR fmkr) { int ierr=0; c_fill_stroke_and_clip(fmkr, Get_FM(fmkr, &ierr), &ierr); RETURN_NIL; }
OBJ_PTR FM_stroke_line(OBJ_PTR fmkr, OBJ_PTR x1, OBJ_PTR y1, OBJ_PTR x2, OBJ_PTR y2) { int ierr=0;
   c_stroke_line(fmkr, Get_FM(fmkr, &ierr), 
      Number_to_double(x1, &ierr), Number_to_double(y1, &ierr), 
      Number_to_double(x2, &ierr), Number_to_double(y2, &ierr), &ierr); RETURN_NIL; }
OBJ_PTR FM_fill_rect(OBJ_PTR fmkr, OBJ_PTR x, OBJ_PTR y, OBJ_PTR width, OBJ_PTR height) { int ierr=0;
   c_fill_rect(fmkr, Get_FM(fmkr, &ierr), 
      Number_to_double(x, &ierr), Number_to_double(y, &ierr), 
      Number_to_double(width, &ierr), Number_to_double(height, &ierr), &ierr); RETURN_NIL; }
OBJ_PTR FM_stroke_rect(OBJ_PTR fmkr, OBJ_PTR x, OBJ_PTR y, OBJ_PTR width, OBJ_PTR height) { int ierr=0;
   c_stroke_rect(fmkr, Get_FM(fmkr, &ierr), 
      Number_to_double(x, &ierr), Number_to_double(y, &ierr), 
      Number_to_double(width, &ierr), Number_to_double(height, &ierr), &ierr); RETURN_NIL; }
OBJ_PTR FM_fill_and_stroke_rect(OBJ_PTR fmkr, OBJ_PTR x, OBJ_PTR y, OBJ_PTR width, OBJ_PTR height) { int ierr=0;
   c_fill_and_stroke_rect(fmkr, Get_FM(fmkr, &ierr), 
      Number_to_double(x, &ierr), Number_to_double(y, &ierr), 
      Number_to_double(width, &ierr), Number_to_double(height, &ierr), &ierr); RETURN_NIL; }
OBJ_PTR FM_clip_rect(OBJ_PTR fmkr, OBJ_PTR x, OBJ_PTR y, OBJ_PTR width, OBJ_PTR height) { int ierr=0;
   c_clip_rect(fmkr, Get_FM(fmkr, &ierr),
      Number_to_double(x, &ierr), Number_to_double(y, &ierr),
      Number_to_double(width, &ierr), Number_to_double(height, &ierr), &ierr); RETURN_NIL; }
OBJ_PTR FM_clip_oval(OBJ_PTR fmkr, OBJ_PTR x, OBJ_PTR y, OBJ_PTR dx, OBJ_PTR dy, OBJ_PTR angle) { int ierr=0;
   c_clip_oval(fmkr, Get_FM(fmkr, &ierr), Number_to_double(x, &ierr), Number_to_double(y, &ierr), 
      Number_to_double(dx, &ierr), Number_to_double(dy, &ierr), Number_to_double(angle, &ierr), &ierr); RETURN_NIL; }
OBJ_PTR FM_fill_oval(OBJ_PTR fmkr, OBJ_PTR x, OBJ_PTR y, OBJ_PTR dx, OBJ_PTR dy, OBJ_PTR angle) { int ierr=0;
   c_fill_oval(fmkr, Get_FM(fmkr, &ierr), Number_to_double(x, &ierr), Number_to_double(y, &ierr), 
      Number_to_double(dx, &ierr), Number_to_double(dy, &ierr), Number_to_double(angle, &ierr), &ierr); RETURN_NIL; }
OBJ_PTR FM_stroke_oval(OBJ_PTR fmkr, OBJ_PTR x, OBJ_PTR y, OBJ_PTR dx, OBJ_PTR dy, OBJ_PTR angle) { int ierr=0;
   c_stroke_oval(fmkr, Get_FM(fmkr, &ierr), Number_to_double(x, &ierr), Number_to_double(y, &ierr), 
      Number_to_double(dx, &ierr), Number_to_double(dy, &ierr), Number_to_double(angle, &ierr), &ierr); RETURN_NIL; }
OBJ_PTR FM_fill_and_stroke_oval(OBJ_PTR fmkr, OBJ_PTR x, OBJ_PTR y, OBJ_PTR dx, OBJ_PTR dy, OBJ_PTR angle) { int ierr=0;
   c_fill_and_stroke_oval(fmkr, Get_FM(fmkr, &ierr), Number_to_double(x, &ierr), Number_to_double(y, &ierr), 
      Number_to_double(dx, &ierr), Number_to_double(dy, &ierr), Number_to_double(angle, &ierr), &ierr); RETURN_NIL; }
OBJ_PTR FM_clip_rounded_rect(OBJ_PTR fmkr, OBJ_PTR x, OBJ_PTR y, OBJ_PTR width, OBJ_PTR height, OBJ_PTR dx, OBJ_PTR dy) { int ierr=0;
   c_clip_rounded_rect(fmkr, Get_FM(fmkr, &ierr), Number_to_double(x, &ierr), Number_to_double(y, &ierr), 
      Number_to_double(width, &ierr), Number_to_double(height, &ierr), Number_to_double(dx, &ierr), Number_to_double(dy, &ierr), &ierr); RETURN_NIL; }
OBJ_PTR FM_fill_rounded_rect(OBJ_PTR fmkr, OBJ_PTR x, OBJ_PTR y, OBJ_PTR width, OBJ_PTR height, OBJ_PTR dx, OBJ_PTR dy) { int ierr=0;
   c_fill_rounded_rect(fmkr, Get_FM(fmkr, &ierr), Number_to_double(x, &ierr), Number_to_double(y, &ierr), 
      Number_to_double(width, &ierr), Number_to_double(height, &ierr), Number_to_double(dx, &ierr), Number_to_double(dy, &ierr), &ierr); RETURN_NIL; }
OBJ_PTR FM_stroke_rounded_rect(OBJ_PTR fmkr, OBJ_PTR x, OBJ_PTR y, OBJ_PTR width, OBJ_PTR height, OBJ_PTR dx, OBJ_PTR dy) { int ierr=0;
   c_stroke_rounded_rect(fmkr, Get_FM(fmkr, &ierr), Number_to_double(x, &ierr), Number_to_double(y, &ierr), 
      Number_to_double(width, &ierr), Number_to_double(height, &ierr), Number_to_double(dx, &ierr), Number_to_double(dy, &ierr), &ierr); RETURN_NIL; }
OBJ_PTR FM_fill_and_stroke_rounded_rect(OBJ_PTR fmkr, OBJ_PTR x, OBJ_PTR y, OBJ_PTR width, OBJ_PTR height, OBJ_PTR dx, OBJ_PTR dy) { int ierr=0;
   c_fill_and_stroke_rounded_rect(fmkr, Get_FM(fmkr, &ierr), Number_to_double(x, &ierr), Number_to_double(y, &ierr), 
      Number_to_double(width, &ierr), Number_to_double(height, &ierr), Number_to_double(dx, &ierr), Number_to_double(dy, &ierr), &ierr); RETURN_NIL; }
OBJ_PTR FM_clip_circle(OBJ_PTR fmkr, OBJ_PTR x, OBJ_PTR y, OBJ_PTR dx) { int ierr=0;
   c_clip_circle(fmkr, Get_FM(fmkr, &ierr), 
      Number_to_double(x, &ierr), Number_to_double(y, &ierr), Number_to_double(dx, &ierr), &ierr); RETURN_NIL; } 
OBJ_PTR FM_fill_circle(OBJ_PTR fmkr, OBJ_PTR x, OBJ_PTR y, OBJ_PTR dx) { int ierr=0;
   c_fill_circle(fmkr, Get_FM(fmkr, &ierr), 
      Number_to_double(x, &ierr), Number_to_double(y, &ierr), Number_to_double(dx, &ierr), &ierr); RETURN_NIL; } 
OBJ_PTR FM_stroke_circle(OBJ_PTR fmkr, OBJ_PTR x, OBJ_PTR y, OBJ_PTR dx) { int ierr=0;
   c_stroke_circle(fmkr, Get_FM(fmkr, &ierr), 
      Number_to_double(x, &ierr), Number_to_double(y, &ierr), Number_to_double(dx, &ierr), &ierr); RETURN_NIL; } 
OBJ_PTR FM_fill_and_stroke_circle(OBJ_PTR fmkr, OBJ_PTR x, OBJ_PTR y, OBJ_PTR dx) { int ierr=0;
   c_fill_and_stroke_circle(fmkr, Get_FM(fmkr, &ierr), 
      Number_to_double(x, &ierr), Number_to_double(y, &ierr), Number_to_double(dx, &ierr), &ierr); RETURN_NIL; } 
OBJ_PTR FM_append_frame_to_path(OBJ_PTR fmkr) { int ierr=0; c_append_frame_to_path(fmkr, Get_FM(fmkr, &ierr), &ierr); RETURN_NIL; }
OBJ_PTR FM_fill_frame(OBJ_PTR fmkr) { int ierr=0; c_fill_frame(fmkr, Get_FM(fmkr, &ierr), &ierr); RETURN_NIL; }
OBJ_PTR FM_stroke_frame(OBJ_PTR fmkr) { int ierr=0; c_stroke_frame(fmkr, Get_FM(fmkr, &ierr), &ierr); RETURN_NIL; }
OBJ_PTR FM_fill_and_stroke_frame(OBJ_PTR fmkr) { int ierr=0; c_fill_and_stroke_frame(fmkr, Get_FM(fmkr, &ierr), &ierr); RETURN_NIL; }
OBJ_PTR FM_clip_to_frame(OBJ_PTR fmkr) { int ierr=0; c_clip_to_frame(fmkr, Get_FM(fmkr, &ierr), &ierr); RETURN_NIL; }

// pdftext.c
OBJ_PTR FM_register_font(OBJ_PTR fmkr, OBJ_PTR font_name) { int ierr=0;
   return c_register_font(fmkr, Get_FM(fmkr, &ierr), String_Ptr(font_name, &ierr), &ierr); }
OBJ_PTR FM_marker_string_info(OBJ_PTR fmkr, OBJ_PTR font_number, OBJ_PTR string, OBJ_PTR scale) { int ierr=0;
   return c_marker_string_info(fmkr, Get_FM(fmkr, &ierr), Number_to_int(font_number, &ierr), 
      (unsigned char *)String_Ptr(string, &ierr), Number_to_double(scale, &ierr), &ierr); }
OBJ_PTR FM_private_show_marker(OBJ_PTR fmkr, OBJ_PTR args) { int ierr=0; 
   c_private_show_marker(fmkr, Get_FM(fmkr, &ierr), args, &ierr); RETURN_NIL; }

// texout.c
OBJ_PTR FM_rescale_text(OBJ_PTR fmkr, OBJ_PTR scaling_factor) { int ierr=0;
   c_rescale_text(fmkr, Get_FM(fmkr, &ierr), Number_to_double(scaling_factor, &ierr), &ierr); RETURN_NIL; }
OBJ_PTR FM_show_rotated_text(OBJ_PTR fmkr, OBJ_PTR text, OBJ_PTR frame_side, OBJ_PTR shift,
			     OBJ_PTR fraction, OBJ_PTR scale, OBJ_PTR angle, OBJ_PTR justification, OBJ_PTR alignment, OBJ_PTR measure_name) { int ierr=0;
   c_show_rotated_text(fmkr, Get_FM(fmkr, &ierr), String_Ptr(text, &ierr), Number_to_int(frame_side, &ierr), Number_to_double(shift, &ierr),
      Number_to_double(fraction, &ierr), Number_to_double(scale, &ierr), Number_to_double(angle, &ierr), 
		       Number_to_int(justification, &ierr), Number_to_int(alignment, &ierr), measure_name, &ierr); RETURN_NIL; }
OBJ_PTR FM_show_rotated_label(OBJ_PTR fmkr, OBJ_PTR text,
			      OBJ_PTR xloc, OBJ_PTR yloc, OBJ_PTR scale, OBJ_PTR angle, OBJ_PTR justification, OBJ_PTR alignment, OBJ_PTR measure_name) { int ierr=0;
   c_show_rotated_label(fmkr, Get_FM(fmkr, &ierr), String_Ptr(text, &ierr), Number_to_double(xloc, &ierr), Number_to_double(yloc, &ierr),
      Number_to_double(scale, &ierr), Number_to_double(angle, &ierr), Number_to_int(justification, &ierr), Number_to_int(alignment, &ierr), measure_name, &ierr);
   RETURN_NIL; }
OBJ_PTR FM_check_label_clip(OBJ_PTR fmkr, OBJ_PTR xloc, OBJ_PTR yloc) { int ierr=0;
   return c_check_label_clip(fmkr, Get_FM(fmkr, &ierr), Number_to_double(xloc, &ierr), Number_to_double(yloc, &ierr), &ierr); }


OBJ_PTR FM_private_save_measure(OBJ_PTR fmkr, OBJ_PTR measure_name, 
                                OBJ_PTR width, OBJ_PTR height, OBJ_PTR depth)
{
  int ierr;
  c_private_save_measure(fmkr, measure_name, 
                         Number_to_double(width,&ierr), 
                         Number_to_double(height,&ierr),
                         Number_to_double(depth,&ierr));
  return OBJ_NIL;
}
