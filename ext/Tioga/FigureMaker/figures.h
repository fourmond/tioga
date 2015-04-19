/* figures.h */
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

#ifndef __figures_H__
#define __figures_H__

#include <namespace.h>
#include <math.h>
#include <stdbool.h>
#include <ctype.h>

#include "generic.h"

#include <defs.h>

// Axes

#define NO_TICKS 0
#define MAJOR_TICKS_ONLY 1
#define MAJOR_AND_MINOR_TICKS 2

// Y Label Orientation

#define VERTICAL_BOTTOM_TO_TOP 0
#define HORIZONTAL 1
#define VERTICAL_TOP_TO_BOTTOM 2

// Frame Sides

#define LEFT -1
	// LEFT == LEFT_JUSTIFIED
#define RIGHT 1
	// RIGHT == RIGHT_JUSTIFIED
#define TOP 2
#define BOTTOM 3

// PlotAxis locations = Frame Sides plus the following

#define AT_X_ORIGIN 4
#define AT_Y_ORIGIN 5
    
// axis types

#define AXIS_HIDDEN 0
#define AXIS_LINE_ONLY 1
#define AXIS_WITH_MAJOR_TICKS_ONLY 2
#define AXIS_WITH_TICKS_ONLY 3
#define AXIS_WITH_MAJOR_TICKS_AND_NUMERIC_LABELS 4
#define AXIS_WITH_TICKS_AND_NUMERIC_LABELS 5

// edge types

#define EDGE_HIDDEN 0
#define EDGE_LINE_ONLY 1
#define EDGE_WITH_MAJOR_TICKS_ONLY 2
#define EDGE_WITH_TICKS 3

// Lines

#define LINE_CAP_BUTT 0
#define LINE_CAP_ROUND 1
#define LINE_CAP_SQUARE 2

#define LINE_JOIN_MITER 0
#define LINE_JOIN_ROUND 1
#define LINE_JOIN_BEVEL 2
    
// Text and Markers

#define LEFT_JUSTIFIED LEFT
#define CENTERED 0
#define RIGHT_JUSTIFIED RIGHT

#define ALIGNED_AT_TOP 0
#define ALIGNED_AT_MIDHEIGHT 1
#define ALIGNED_AT_BASELINE 2
#define ALIGNED_AT_BOTTOM 3

//  Rendering Modes

#define FILL 0
#define STROKE 1
#define FILL_AND_STROKE 2
#define DISCARD 3 
#define FILL_AND_CLIP 4
#define STROKE_AND_CLIP 5
#define FILL_STROKE_AND_CLIP 6
#define CLIP 7

typedef struct {

/* page attributes */
    bool root_figure;
    bool in_subplot;

    /* 
       the scaling factor from a postscript point to the smallest unit
       representable in the PDF file. Higher numbers give you more
       precision at the cost of a larger PDF file
    */
    double scaling_factor;
  
    double page_left, page_bottom, page_top, page_right, page_width, page_height;
        // in output units.    READ ONLY.
    
/* frame and bounds attributes.  READ ONLY.  */
    // frame location as fractions of page width & height.
    double frame_left, frame_right, frame_top, frame_bottom, frame_width, frame_height;
    // bounds in figure coords.
    double bounds_left, bounds_right, bounds_top, bounds_bottom;
    double bounds_xmin, bounds_xmax, bounds_ymin, bounds_ymax;
    double bounds_width, bounds_height;
    bool xaxis_reversed, yaxis_reversed;
    
/* text attributes */
    int justification;
    int alignment;
    
    double default_font_size; // in points
    double default_text_scale;
    double default_text_height_dx; // in figure coords.  READ ONLY.  set when change default_text_scale.
    double default_text_height_dy; // in figure coords.  READ ONLY.  set when change default_text_scale.
    
    double label_left_margin; // as fraction of frame width
    double label_right_margin; // as fraction of frame width
    double label_top_margin; // as fraction of frame height
    double label_bottom_margin; // as fraction of frame height
    
    double text_shift_on_left; // default shift for text on LEFT side of frame
    double text_shift_on_right; // default shift for text on RIGHT side of frame
    double text_shift_on_top; // default shift for text on TOP side of frame
    double text_shift_on_bottom; // default shift for text on BOTTOM side of frame
    double text_shift_from_x_origin; // default shift for text away from y axis at x origin
    double text_shift_from_y_origin; // default shift for text away from x axis at y origin
    
/* graphics attributes */

    /* color attributes */
    double stroke_color_R;
    double stroke_color_G;
    double stroke_color_B;
    double fill_color_R;
    double fill_color_G;
    double fill_color_B;
    
    /* stroke attributes */
    double default_line_scale; // multipler for line_width
    double line_width;  // in points (prior to multiplication by default_line_scale)
    int line_cap;
    int line_join;
    double miter_limit;
    
    /* transparency */
    double stroke_opacity;
    double fill_opacity;
    
    /* Title */
    bool title_visible;
    int title_side;
    double title_position;
    
    double title_scale;
    double title_shift; // in char heights, positive for out from edge (or toward larger x or y OBJ_PTR)
    double title_angle;
    int title_alignment;
    int title_justification;
    double title_color_R;
    double title_color_G;
    double title_color_B;
    
    /* X label */
    bool xlabel_visible;
    double xlabel_position; // xlabel side is given by xaxis_loc
    
    double xlabel_scale;
    double xlabel_shift; // in char heights, positive for out from edge (or toward larger x or y OBJ_PTR)
    double xlabel_angle;
    int xlabel_side;
    int xlabel_alignment;
    int xlabel_justification;
    double xlabel_color_R;
    double xlabel_color_G;
    double xlabel_color_B;
    
    /* Y label */
    bool ylabel_visible;
    double ylabel_position; // ylabel side is given by yaxis_loc
    
    double ylabel_scale;
    double ylabel_shift; // in char heights, positive for out from edge (or toward larger x or y OBJ_PTR)
    double ylabel_angle;
    int ylabel_side;
    int ylabel_alignment;
    int ylabel_justification;
    double ylabel_color_R;
    double ylabel_color_G;
    double ylabel_color_B;
    
    /* X axis */
    bool xaxis_visible;
    int xaxis_type;
    int xaxis_loc;
    // line
    double xaxis_line_width; // for axis line
    double xaxis_stroke_color_R; // for axis line and tick marks
    double xaxis_stroke_color_G;
    double xaxis_stroke_color_B;

    /* For tick labels */
    double xaxis_labels_color_R;
    double xaxis_labels_color_G;
    double xaxis_labels_color_B;
    // tick marks
    double xaxis_major_tick_width; // same units as line_width
    double xaxis_minor_tick_width; // same units as line_width
    double xaxis_major_tick_length; // in units of numeric label char heights
    double xaxis_minor_tick_length; // in units of numeric label char heights
    bool xaxis_log_values;
    bool xaxis_ticks_inside; // inside frame or toward larger x or y OBJ_PTR for specific location
    bool xaxis_ticks_outside; // inside frame or toward smaller x or y OBJ_PTR for specific location
    double xaxis_tick_interval; // set to 0 to use default
    double xaxis_min_between_major_ticks; // in units of numeric label char heights
    int xaxis_number_of_minor_intervals; // set to 0 to use default
    // numeric labels on major ticks
    bool xaxis_use_fixed_pt;
    int xaxis_digits_max;
    int xaxis_numeric_label_decimal_digits; // set to negative to use default
    double xaxis_numeric_label_scale;
    double xaxis_numeric_label_shift; // in char heights, positive for out from edge (or toward larger x or y OBJ_PTR)
    double xaxis_numeric_label_angle;
    int xaxis_numeric_label_alignment;
    int xaxis_numeric_label_justification;
    int xaxis_numeric_label_frequency;
    int xaxis_numeric_label_phase;
    // note that the top and bottom edges and drawn in addition to the x axis.
    // if the edge type specifies ticks, the ticks match those of the x axis.
    bool top_edge_visible;
    int top_edge_type;
    bool bottom_edge_visible;
    int bottom_edge_type;
    
    /* Y axis */
    bool yaxis_visible;
    int yaxis_type;
    int yaxis_loc;
    // line
    double yaxis_line_width; // for axis line
    double yaxis_stroke_color_R; // for axis line and tick marks
    double yaxis_stroke_color_G;
    double yaxis_stroke_color_B;

    /* For tick labels */
    double yaxis_labels_color_R;
    double yaxis_labels_color_G;
    double yaxis_labels_color_B;
    // tick marks
    double yaxis_major_tick_width; // same units as line_width
    double yaxis_minor_tick_width; // same units as line_width
    double yaxis_major_tick_length; // in units of numeric label char heights
    double yaxis_minor_tick_length; // in units of numeric label char heights
    bool yaxis_log_values;
    bool yaxis_ticks_inside; // inside frame or toward larger x or y OBJ_PTR for specific location
    bool yaxis_ticks_outside; // inside frame or toward smaller x or y OBJ_PTR for specific location
    double yaxis_tick_interval; // set to 0 to use default
    double yaxis_min_between_major_ticks; // in units of numeric label char heights
    int yaxis_number_of_minor_intervals; // set to 0 to use default
    // numeric labels on major ticks
    bool yaxis_use_fixed_pt;
    int yaxis_digits_max;
    int yaxis_numeric_label_decimal_digits; // set to negative to use default
    double yaxis_numeric_label_scale;
    double yaxis_numeric_label_shift; // in char heights, positive for out from edge (or toward larger x or y OBJ_PTR)
    double yaxis_numeric_label_angle;
    int yaxis_numeric_label_alignment;
    int yaxis_numeric_label_justification;
    int yaxis_numeric_label_frequency;
    int yaxis_numeric_label_phase;
    // note that the left and right edges and drawn in addition to the y axis.
    // if the edge type specifies ticks, the ticks match those of the y axis.
    int left_edge_type;
    bool left_edge_visible;
    int right_edge_type;
    bool right_edge_visible;

    /* General axis */
    bool vincent_or_bill; /* whether to use Bill's or Vincent's algorithms for minor ticks */
    int log_minor_ticks_limit; /* the number after which we disable the display of minor ticks on log scale */

    /* Legend */
    // units are text heights for x and y for locations and sizes
    double legend_line_x0; // x location of start of line
    double legend_line_x1; // x location of end of line
    double legend_line_dy; // move the line up by this amount from the text baseline
    double legend_text_xstart; // x location of start of text
    double legend_text_width; // in text heights measured from legend_text_xstart
    double legend_text_ystart; // y location of first entry text
    double legend_text_dy; // move down by this amount for subsequent legend entries
    double legend_line_width; // for legend lines (or negative to use the weight from the plot)
    double legend_scale; // scaling factor for size of legend text
    int legend_alignment;
    int legend_justification;
    
    /* Debugging */
    int debug_verbosity_level; // 0 turns off the trace info

    /* Whether to complain about out non ok numbers in paths */
    int croak_on_nonok_numbers;

/* PRIVATE -- not to be included in the user interface */
    double clip_left, clip_right, clip_top, clip_bottom; // in output coords
} FM;

typedef FM Figure_Maker;

extern double bbox_llx, bbox_lly, bbox_urx, bbox_ury;

extern char *data_dir;


// all routines set *ierr nonzero in case of error.
// if call a routine that gets ierr as arg, check ierr when it returns.
// cleanup and return if find ierr != 0.
// *ierr is set to 0 by wrapper routines before 1st call to "c_" routines.


/*======================================================================*/
// axes.c
extern char *Get_String(OBJ_PTR ary, int index, int *ierr);

extern void c_show_axis(OBJ_PTR fmkr, FM *p, int location, int *ierr);
extern void c_show_edge(OBJ_PTR fmkr, FM *p, int location, int *ierr);
extern void c_no_title(OBJ_PTR fmkr, FM *p, int *ierr);
extern void c_no_xlabel(OBJ_PTR fmkr, FM *p, int *ierr);
extern void c_no_ylabel(OBJ_PTR fmkr, FM *p, int *ierr);
extern void c_no_xaxis(OBJ_PTR fmkr, FM *p, int *ierr);
extern void c_no_yaxis(OBJ_PTR fmkr, FM *p, int *ierr);
extern void c_no_left_edge(OBJ_PTR fmkr, FM *p, int *ierr);
extern void c_no_right_edge(OBJ_PTR fmkr, FM *p, int *ierr);
extern void c_no_top_edge(OBJ_PTR fmkr, FM *p, int *ierr);
extern void c_no_bottom_edge(OBJ_PTR fmkr, FM *p, int *ierr);
extern void c_show_axis_generic(OBJ_PTR fmkr, FM *p, OBJ_PTR dict, int *ierr);
extern OBJ_PTR c_axis_get_information(OBJ_PTR fmkr, FM *p, 
				      OBJ_PTR axis_spec, int *ierr);


/*======================================================================*/
// figures.c
extern FM *Get_FM(OBJ_PTR fmkr, int *ierr);
void Init_FigureMaker(void);
extern bool Get_initialized(void);
extern void Set_initialized(void);

/*======================================================================*/
// init.c
extern void Init_IDs(void);
extern OBJ_PTR Get_fm_data_attr(OBJ_PTR fmkr, int *ierr);
extern void Initialize_Figure(OBJ_PTR fmkr);
extern void do_cmd(OBJ_PTR fmkr, OBJ_PTR cmd, int *ierr);
extern bool Get_bool(OBJ_PTR obj, ID_PTR name_ID, int *ierr);
extern int Get_int(OBJ_PTR obj, ID_PTR name_ID, int *ierr);
extern double Get_double(OBJ_PTR obj, ID_PTR name_ID, int *ierr);
extern char *Get_tex_preview_paper_width(OBJ_PTR fmkr, int *ierr);
extern char *Get_tex_preview_paper_height(OBJ_PTR fmkr, int *ierr);
extern char *Get_tex_preview_hoffset(OBJ_PTR fmkr, int *ierr);
extern char *Get_tex_preview_voffset(OBJ_PTR fmkr, int *ierr);
extern char *Get_tex_preview_figure_width(OBJ_PTR fmkr, int *ierr);
extern char *Get_tex_preview_figure_height(OBJ_PTR fmkr, int *ierr);
extern char *Get_tex_fontsize(OBJ_PTR fmkr, int *ierr);
extern char *Get_tex_fontfamily(OBJ_PTR fmkr, int *ierr);
extern char *Get_tex_fontseries(OBJ_PTR fmkr, int *ierr);
extern char *Get_tex_fontshape(OBJ_PTR fmkr, int *ierr);
extern char *Get_tex_preview_minwhitespace(OBJ_PTR fmkr, int *ierr);
extern bool Get_tex_preview_fullpage(OBJ_PTR fmkr, int *ierr);
extern char *Get_tex_preview_tiogafigure_command(OBJ_PTR fmkr, int *ierr);
extern char *Get_tex_preview_generated_preamble(OBJ_PTR fmkr, int *ierr);
extern double Get_tex_xoffset(OBJ_PTR fmkr, int *ierr);
extern double Get_tex_yoffset(OBJ_PTR fmkr, int *ierr);
extern char *Get_tex_preview_documentclass(OBJ_PTR fmkr, int *ierr);
extern char *Get_tex_preamble(OBJ_PTR fmkr, int *ierr);
extern char *Get_xaxis_numeric_label_tex(OBJ_PTR fmkr, int *ierr);
extern char *Get_yaxis_numeric_label_tex(OBJ_PTR fmkr, int *ierr);
extern char *Get_tex_preview_pagestyle(OBJ_PTR fmkr, int *ierr);
extern char *Get_tex_preview_tiogafigure_command(OBJ_PTR fmkr, int *ierr);
extern OBJ_PTR Get_line_type(OBJ_PTR fmkr, int *ierr);
extern void Set_line_type(OBJ_PTR fmkr, OBJ_PTR v, int *ierr);
extern OBJ_PTR Get_xaxis_tick_labels(OBJ_PTR fmkr, int *ierr);
extern OBJ_PTR Get_xaxis_locations_for_major_ticks(OBJ_PTR fmkr, int *ierr);
extern OBJ_PTR Get_xaxis_locations_for_minor_ticks(OBJ_PTR fmkr, int *ierr);
extern OBJ_PTR Get_yaxis_tick_labels(OBJ_PTR fmkr, int *ierr);
extern OBJ_PTR Get_yaxis_locations_for_major_ticks(OBJ_PTR fmkr, int *ierr);
extern OBJ_PTR Get_yaxis_locations_for_minor_ticks(OBJ_PTR fmkr, int *ierr);

extern void c_private_init_fm_data(OBJ_PTR fmkr, FM *p, double scale, int *ierr);
extern void c_set_frame_sides(OBJ_PTR fmkr, FM *p, 
   double left, double right, double top, double bottom, int *ierr);
extern void c_set_device_pagesize(OBJ_PTR fmkr, FM *p, double width, double height, int *ierr);
extern OBJ_PTR c_get_save_filename(OBJ_PTR fmkr, FM *p, OBJ_PTR name, int *ierr);
extern void c_private_make(OBJ_PTR fmkr, FM *p, OBJ_PTR name, OBJ_PTR cmd, int *ierr);
extern OBJ_PTR c_private_make_portfolio(OBJ_PTR fmkr, FM *p, OBJ_PTR name, OBJ_PTR fignums, OBJ_PTR fignames, int *ierr);

/* The ID of the measures_info instance variable */
extern ID_PTR measures_info_ID;

/*======================================================================*/
// makers.c
extern OBJ_PTR c_private_make_contour(OBJ_PTR fmkr, FM *p,
         OBJ_PTR gaps, // these vectors get the results
         OBJ_PTR xs, OBJ_PTR ys, // data x coordinates and y coordinates
         OBJ_PTR zs, double z_level, // the table of values and the desired contour level
         OBJ_PTR legit, // the table of flags (nonzero means okay)
         int method, // method == 1 means CONREC
         int *ierr);
extern OBJ_PTR c_private_make_steps(OBJ_PTR fmkr, FM *p, OBJ_PTR Xvec_data, OBJ_PTR Yvec_data,
				    double xfirst, double yfirst, double xlast, double ylast, int justification, int *ierr);
        /* adds n_pts_to_add points to Xs and Ys for steps with the given parameters.
            X_data and Y_data are arrays of n values where n_pts_to_add = 2*(n+1)
            (xfirst,yfirst) and (xlast,ylast) are extra data points to fix the first and last steps.
        The X_data plus xfirst and xlast determine the widths of the steps.
        The Y_data plus yfirst and ylast determine the height of the steps.
        The steps occur at locations midway between the given x locations. */
extern OBJ_PTR c_private_make_spline_interpolated_points(OBJ_PTR fmkr, FM *p,
         OBJ_PTR Xvec, OBJ_PTR Xvec_data, OBJ_PTR Yvec_data,
         OBJ_PTR start_slope, OBJ_PTR end_slope, int *ierr);

/*======================================================================*/
// pdfcolor.c
extern void Free_Functions(void);
extern void Write_Functions(int *ierr);
extern void Free_Stroke_Opacities(void);
extern void Free_Fill_Opacities(void);
extern void Write_Stroke_Opacity_Objects(void);
extern void Write_Fill_Opacity_Objects(void);
extern void Free_Shadings(void);
extern void Write_Shadings(void);

extern void c_stroke_opacity_set(OBJ_PTR fmkr, FM *p, double stroke_opacity, int *ierr);
extern void c_fill_opacity_set(OBJ_PTR fmkr, FM *p, double fill_opacity, int *ierr);
extern void c_private_axial_shading(OBJ_PTR fmkr, FM *p, 
      double x0, double y0, double x1, double y1,
      OBJ_PTR colormap, bool extend_start, bool extend_end, int *ierr);
extern void c_private_radial_shading(OBJ_PTR fmkr, FM *p, 
      double x0, double y0, double r0,
      double x1, double y1, double r1, OBJ_PTR colormap,
      double a, double b, double c, double d, bool extend_start, bool extend_end, int *ierr);
extern OBJ_PTR c_private_create_colormap(OBJ_PTR fmkr, FM *p, 
      bool rgb, int length, OBJ_PTR Ps, OBJ_PTR C1s, OBJ_PTR C2s, OBJ_PTR C3s, int *ierr);
extern OBJ_PTR c_get_color_from_colormap(OBJ_PTR fmkr, FM *p, OBJ_PTR color_map, double x, int *ierr);
extern OBJ_PTR c_convert_to_colormap(OBJ_PTR fmkr, FM* p, OBJ_PTR Rs, OBJ_PTR Gs, OBJ_PTR Bs, int *ierr);
extern OBJ_PTR c_hls_to_rgb(OBJ_PTR fmkr, FM *p, OBJ_PTR hls_vec, int *ierr);
extern OBJ_PTR c_rgb_to_hls(OBJ_PTR fmkr, FM *p, OBJ_PTR rgb_vecc_hls_to_rgb, int *ierr);
extern void c_title_color_set(OBJ_PTR fmkr, FM *p, OBJ_PTR valc_hls_to_rgb, int *ierr);
extern OBJ_PTR c_title_color_get(OBJ_PTR fmkr, FM *pc_hls_to_rgb, int *ierr); // value is array of [r, g, b] intensities from 0 to 1
extern void c_xlabel_color_set(OBJ_PTR fmkr, FM *p, OBJ_PTR valc_hls_to_rgb, int *ierr);
extern OBJ_PTR c_xlabel_color_get(OBJ_PTR fmkr, FM *pc_hls_to_rgb, int *ierr); // value is array of [r, g, b] intensities from 0 to 1
extern void c_ylabel_color_set(OBJ_PTR fmkr, FM *p, OBJ_PTR valc_hls_to_rgb, int *ierr);
extern OBJ_PTR c_ylabel_color_get(OBJ_PTR fmkr, FM *pc_hls_to_rgb, int *ierr); // value is array of [r, g, b] intensities from 0 to 1
extern void c_xaxis_stroke_color_set(OBJ_PTR fmkr, FM *p, OBJ_PTR valc_hls_to_rgb, int *ierr);
extern OBJ_PTR c_xaxis_stroke_color_get(OBJ_PTR fmkr, FM *pc_hls_to_rgb, int *ierr); // value is array of [r, g, b] intensities from 0 to 1
extern void c_yaxis_stroke_color_set(OBJ_PTR fmkr, FM *p, OBJ_PTR valc_hls_to_rgb, int *ierr);
extern OBJ_PTR c_yaxis_stroke_color_get(OBJ_PTR fmkr, FM *pc_hls_to_rgb, int *ierr); // value is array of [r, g, b] intensities from 0 to 1

/* Same but for label colors: */
extern void c_xaxis_labels_color_set(OBJ_PTR fmkr, FM *p, OBJ_PTR valc_hls_to_rgb, int *ierr);
extern OBJ_PTR c_xaxis_labels_color_get(OBJ_PTR fmkr, FM *pc_hls_to_rgb, int *ierr); // value is array of [r, g, b] intensities from 0 to 1
extern void c_yaxis_labels_color_set(OBJ_PTR fmkr, FM *p, OBJ_PTR valc_hls_to_rgb, int *ierr);
extern OBJ_PTR c_yaxis_labels_color_get(OBJ_PTR fmkr, FM *pc_hls_to_rgb, int *ierr); // value is array of [r, g, b] intensities from 0 to 1


extern void c_string_hls_to_rgb_bang(OBJ_PTR fmkr, FM *p, unsigned char* hls_str, long len, int *ierr);
extern void c_string_rgb_to_hls_bang(OBJ_PTR fmkr, FM *p, unsigned char* rgb_str, long len, int *ierr);

/*======================================================================*/
// pdfcoords.c
extern void Recalc_Font_Hts(FM *p);
extern double convert_figure_to_output_x(FM *p, double x);
extern double convert_figure_to_output_y(FM *p, double y);
extern double convert_figure_to_output_dy(FM *p, double dy);
extern double convert_figure_to_output_dx(FM *p, double dx);
extern double convert_output_to_figure_x(FM *p, double x);
extern double convert_output_to_figure_y(FM *p, double y);
extern double convert_output_to_figure_dy(FM *p, double dy);
extern double convert_output_to_figure_dx(FM *p, double dx);

extern void c_set_subframe(OBJ_PTR fmkr, FM *p, 
      double left_margin, double right_margin, double top_margin, double bottom_margin, int *ierr);
extern void c_private_set_default_font_size(OBJ_PTR fmkr, FM *p, double size, int *ierr);
extern OBJ_PTR c_convert_to_degrees(OBJ_PTR fmkr, FM *p, double dx, double dy, int *ierr); // dx and dy in figure coords
extern OBJ_PTR c_convert_inches_to_output(OBJ_PTR fmkr, FM *p, double val, int *ierr);
extern OBJ_PTR c_convert_output_to_inches(OBJ_PTR fmkr, FM *p, double val, int *ierr);
extern OBJ_PTR c_convert_mm_to_output(OBJ_PTR fmkr, FM *p, double val, int *ierr);
extern OBJ_PTR c_convert_output_to_mm(OBJ_PTR fmkr, FM *p, double val, int *ierr);
extern OBJ_PTR c_convert_page_to_output_x(OBJ_PTR fmkr, FM *p, double val, int *ierr);
extern OBJ_PTR c_convert_page_to_output_y(OBJ_PTR fmkr, FM *p, double val, int *ierr);
extern OBJ_PTR c_convert_page_to_output_dx(OBJ_PTR fmkr, FM *p, double val, int *ierr);
extern OBJ_PTR c_convert_page_to_output_dy(OBJ_PTR fmkr, FM *p, double val, int *ierr);
extern OBJ_PTR c_convert_output_to_page_x(OBJ_PTR fmkr, FM *p, double val, int *ierr);
extern OBJ_PTR c_convert_output_to_page_y(OBJ_PTR fmkr, FM *p, double val, int *ierr);
extern OBJ_PTR c_convert_output_to_page_dx(OBJ_PTR fmkr, FM *p, double val, int *ierr);
extern OBJ_PTR c_convert_output_to_page_dy(OBJ_PTR fmkr, FM *p, double val, int *ierr);
extern OBJ_PTR c_convert_frame_to_page_x(OBJ_PTR fmkr, FM *p, double val, int *ierr);
extern OBJ_PTR c_convert_frame_to_page_y(OBJ_PTR fmkr, FM *p, double val, int *ierr);
extern OBJ_PTR c_convert_frame_to_page_dx(OBJ_PTR fmkr, FM *p, double val, int *ierr);
extern OBJ_PTR c_convert_frame_to_page_dy(OBJ_PTR fmkr, FM *p, double val, int *ierr);
extern OBJ_PTR c_convert_page_to_frame_x(OBJ_PTR fmkr, FM *p, double val, int *ierr);
extern OBJ_PTR c_convert_page_to_frame_y(OBJ_PTR fmkr, FM *p, double val, int *ierr);
extern OBJ_PTR c_convert_page_to_frame_dx(OBJ_PTR fmkr, FM *p, double val, int *ierr);
extern OBJ_PTR c_convert_page_to_frame_dy(OBJ_PTR fmkr, FM *p, double val, int *ierr);
extern OBJ_PTR c_convert_figure_to_frame_x(OBJ_PTR fmkr, FM *p, double val, int *ierr);
extern OBJ_PTR c_convert_figure_to_frame_y(OBJ_PTR fmkr, FM *p, double val, int *ierr);
extern OBJ_PTR c_convert_figure_to_frame_dx(OBJ_PTR fmkr, FM *p, double val, int *ierr);
extern OBJ_PTR c_convert_figure_to_frame_dy(OBJ_PTR fmkr, FM *p, double val, int *ierr);
extern OBJ_PTR c_convert_frame_to_figure_x(OBJ_PTR fmkr, FM *p, double val, int *ierr);
extern OBJ_PTR c_convert_frame_to_figure_y(OBJ_PTR fmkr, FM *p, double val, int *ierr);
extern OBJ_PTR c_convert_frame_to_figure_dx(OBJ_PTR fmkr, FM *p, double val, int *ierr);
extern OBJ_PTR c_convert_frame_to_figure_dy(OBJ_PTR fmkr, FM *p, double val, int *ierr);
extern OBJ_PTR c_convert_figure_to_output_x(OBJ_PTR fmkr, FM *p, double val, int *ierr);
extern OBJ_PTR c_convert_figure_to_output_y(OBJ_PTR fmkr, FM *p, double val, int *ierr);
extern OBJ_PTR c_convert_figure_to_output_dx(OBJ_PTR fmkr, FM *p, double val, int *ierr);
extern OBJ_PTR c_convert_figure_to_output_dy(OBJ_PTR fmkr, FM *p, double val, int *ierr);
extern OBJ_PTR c_convert_output_to_figure_x(OBJ_PTR fmkr, FM *p, double val, int *ierr);
extern OBJ_PTR c_convert_output_to_figure_y(OBJ_PTR fmkr, FM *p, double val, int *ierr);
extern OBJ_PTR c_convert_output_to_figure_dx(OBJ_PTR fmkr, FM *p, double val, int *ierr);
extern OBJ_PTR c_convert_output_to_figure_dy(OBJ_PTR fmkr, FM *p, double val, int *ierr);
extern void c_doing_subplot(OBJ_PTR fmkr, FM *p, int *ierr);
extern void c_doing_subfigure(OBJ_PTR fmkr, FM *p, int *ierr);
extern void c_private_set_bounds(OBJ_PTR fmkr, FM *p, 
   double left_boundary, double right_boundary, double top_boundary, double bottom_boundary, int *ierr);

/*======================================================================*/
// pdffile.c
extern void Init_pdf(int *ierr);
extern void Open_pdf(OBJ_PTR fmkr, FM *p, char *filename, bool quiet_mode, int *ierr);
extern void Start_Axis_Standard_State(OBJ_PTR fmkr, FM *p, double r, double g, double b, double line_width, int *ierr);
extern void End_Axis_Standard_State(void);
extern void Write_gsave(void);
extern void Write_grestore(void);
extern void Close_pdf(OBJ_PTR fmkr, FM *p, bool quiet_mode, int *ierr);
extern void Rename_pdf(char *oldname, char *newname);

extern void c_pdf_gsave(OBJ_PTR fmkr, FM *p, int *ierr);
extern void c_pdf_grestore(OBJ_PTR fmkr, FM *p, int *ierr);

/*======================================================================*/
// pdfimage.c
extern OBJ_PTR c_private_create_image_data(OBJ_PTR fmkr, FM *p, OBJ_PTR table,
            int first_row, int last_row, int first_column, int last_column,
            double min_val, double max_val, int max_code, int if_below_range, int if_above_range, int *ierr);
extern OBJ_PTR c_private_create_monochrome_image_data(OBJ_PTR fmkr, FM *p, OBJ_PTR table,
            int first_row, int last_row, int first_column, int last_column,
            double boundary, bool reversed, int *ierr);
extern void c_private_show_jpg(OBJ_PTR fmkr, FM *p, char *filename, 
   int width, int height, OBJ_PTR image_destination, int mask_obj_num, int *ierr);

extern OBJ_PTR c_private_show_image(OBJ_PTR fmkr, FM *p, int image_type, double llx, double lly, double lrx, double lry,
    double ulx, double uly, bool interpolate, bool reversed, int w, int h, unsigned char* data, long len, 
                                    OBJ_PTR mask_min, OBJ_PTR mask_max, OBJ_PTR hival, OBJ_PTR lookup_data, int mask_obj_num, int components, const char * filters, int *ierr);

extern int c_private_register_image(OBJ_PTR fmkr, FM *p, int image_type,
                                    bool interpolate, bool reversed, 
                                    int w, int h, unsigned char* data, long len, 
                                    OBJ_PTR mask_min, OBJ_PTR mask_max, OBJ_PTR hival, OBJ_PTR lookup_data, int mask_obj_num, int components, const char * filters, int *ierr);

extern int c_private_register_jpg(OBJ_PTR fmkr, FM *p, char *filename, 
                                  int width, int height, int mask_obj_num, 
                                  int *ierr);

extern void c_private_show_image_from_ref(OBJ_PTR fmkr, FM *p, 
                                          int ref, double llx, double lly, 
                                          double lrx, double lry,
                                          double ulx, double uly, int *ierr);



/*======================================================================*/
// pdfpath.c
extern void Unpack_RGB(OBJ_PTR rgb, double *rp, double *gp, double *bp, int *ierr);
extern void update_bbox(FM *p, double x, double y);

extern void c_stroke_color_set_RGB(OBJ_PTR fmkr, FM *p, double r, double g, double b, int *ierr);
extern void c_stroke_color_set(OBJ_PTR fmkr, FM *p, OBJ_PTR value, int *ierr);
extern OBJ_PTR c_stroke_color_get(OBJ_PTR fmkr, FM *p, int *ierr);
extern void c_fill_color_set_RGB(OBJ_PTR fmkr, FM *p, double r, double g, double b, int *ierr);
extern void c_fill_color_set(OBJ_PTR fmkr, FM *p, OBJ_PTR value, int *ierr);
extern OBJ_PTR c_fill_color_get(OBJ_PTR fmkr, FM *p, int *ierr);
extern void c_line_width_set(OBJ_PTR fmkr, FM *p, double line_width, int *ierr);
extern void c_rescale_lines(OBJ_PTR fmkr, FM *p, double scaling_factor, int *ierr);
extern void c_line_cap_set(OBJ_PTR fmkr, FM *p, int line_cap, int *ierr);
extern void c_line_join_set(OBJ_PTR fmkr, FM *p, int line_join, int *ierr);
extern void c_miter_limit_set(OBJ_PTR fmkr, FM *p, double miter_limit, int *ierr);
extern void c_line_type_set(OBJ_PTR fmkr, FM *p, OBJ_PTR line_type, int *ierr);
extern void c_update_bbox(OBJ_PTR fmkr, FM *p, double x, double y, int *ierr);
extern OBJ_PTR c_bbox_left(OBJ_PTR fmkr, FM *p, int *ierr);
extern OBJ_PTR c_bbox_right(OBJ_PTR fmkr, FM *p, int *ierr);
extern OBJ_PTR c_bbox_top(OBJ_PTR fmkr, FM *p, int *ierr);
extern OBJ_PTR c_bbox_bottom(OBJ_PTR fmkr, FM *p, int *ierr);
extern void c_move_to_point(OBJ_PTR fmkr, FM *p, double x, double y, int *ierr);
extern void c_moveto(OBJ_PTR fmkr, FM *p, double x, double y, int *ierr);
extern void c_append_point_to_path(OBJ_PTR fmkr, FM *p, double x, double y, int *ierr);
extern void c_lineto(OBJ_PTR fmkr, FM *p, double x, double y, int *ierr);
extern OBJ_PTR c_bezier_control_points(OBJ_PTR fmkr, FM *p, 
   double x0, double y0, double delta_x, double a, double b, double c, int *ierr);
extern void c_append_curve_to_path(OBJ_PTR fmkr, FM *p, double x1, double y1, 
   double x2, double y2, double x3, double y3, int *ierr);
extern void c_curveto(OBJ_PTR fmkr, FM *p, 
   double x1, double y1, double x2, double y2, double x3, double y3, int *ierr);
extern void c_close_path(OBJ_PTR fmkr, FM *p, int *ierr);
extern void c_append_arc_to_path(OBJ_PTR fmkr, FM *p, 
   double x_start, double y_start, double x_corner, double y_corner,
   double x_end, double y_end, double dx, double dy, int *ierr);
extern void c_append_arc(OBJ_PTR fmkr, FM *p, 
   double x_start, double y_start, double x_corner, double y_corner,
   double x_end, double y_end, double radius, int *ierr);
extern void c_append_rect_to_path(OBJ_PTR fmkr, FM *p, double x, double y, double width, double height, int *ierr);
extern void c_append_rect(OBJ_PTR fmkr, FM *p, double x, double y, double width, double height, int *ierr);
extern void c_append_rounded_rect_to_path(OBJ_PTR fmkr, FM *p, 
   double x, double y, double width, double height, double dx, double dy, int *ierr);
extern void c_append_rounded_rect(OBJ_PTR fmkr, FM *p, 
   double x, double y, double width, double height, double radius, int *ierr);
extern void c_append_oval_to_path(OBJ_PTR fmkr, FM *p, double x, double y, double dx, double dy, double angle, int *ierr);
extern void c_append_oval(OBJ_PTR fmkr, FM *p, double x, double y, double dx, double dy, double angle, int *ierr);
extern void c_append_circle_to_path(OBJ_PTR fmkr, FM *p, double x, double y, double dx, int *ierr);
extern void c_append_points_to_path(OBJ_PTR fmkr, FM *p, OBJ_PTR x_vec, OBJ_PTR y_vec, int *ierr);
extern void c_private_append_points_with_gaps_to_path(OBJ_PTR fmkr, FM *p,
   OBJ_PTR x_vec, OBJ_PTR y_vec, OBJ_PTR gaps, bool do_close, int *ierr);
extern void c_stroke(OBJ_PTR fmkr, FM *p, int *ierr);
extern void c_close_and_stroke(OBJ_PTR fmkr, FM *p, int *ierr);
extern void c_fill(OBJ_PTR fmkr, FM *p, int *ierr);
extern void c_discard_path(OBJ_PTR fmkr, FM *p, int *ierr);
extern void c_eofill(OBJ_PTR fmkr, FM *p, int *ierr);
extern void c_fill_and_stroke(OBJ_PTR fmkr, FM *p, int *ierr);
extern void c_eofill_and_stroke(OBJ_PTR fmkr, FM *p, int *ierr);
extern void c_close_fill_and_stroke(OBJ_PTR fmkr, FM *p, int *ierr);
extern void c_close_eofill_and_stroke(OBJ_PTR fmkr, FM *p, int *ierr);
extern void c_clip(OBJ_PTR fmkr, FM *p, int *ierr);
extern void c_eoclip(OBJ_PTR fmkr, FM *p, int *ierr);
extern void c_fill_and_clip(OBJ_PTR fmkr, FM *p, int *ierr);
extern void c_stroke_and_clip(OBJ_PTR fmkr, FM *p, int *ierr);
extern void c_fill_stroke_and_clip(OBJ_PTR fmkr, FM *p, int *ierr);
extern void c_stroke_line(OBJ_PTR fmkr, FM *p, double x1, double y1, double x2, double y2, int *ierr);
extern void c_fill_rect(OBJ_PTR fmkr, FM *p, double x, double y, double width, double height, int *ierr);
extern void c_stroke_rect(OBJ_PTR fmkr, FM *p, double x, double y, double width, double height, int *ierr);
extern void c_fill_and_stroke_rect(OBJ_PTR fmkr, FM *p, double x, double y, double width, double height, int *ierr);
extern void c_clip_rect(OBJ_PTR fmkr, FM *p, double x, double y, double width, double height, int *ierr);
extern void c_clip_dev_rect(OBJ_PTR fmkr, FM *p, double x, double y, double width, double height, int *ierr);
extern void c_clip_oval(OBJ_PTR fmkr, FM *p, double x, double y, double dx, double dy, double angle, int *ierr);
extern void c_fill_oval(OBJ_PTR fmkr, FM *p, double x, double y, double dx, double dy, double angle, int *ierr);
extern void c_stroke_oval(OBJ_PTR fmkr, FM *p, double x, double y, double dx, double dy, double angle, int *ierr);
extern void c_fill_and_stroke_oval(OBJ_PTR fmkr, FM *p, double x, double y, double dx, double dy, double angle, int *ierr);
extern void c_clip_rounded_rect(OBJ_PTR fmkr, FM *p, double x, double y, double width, double height, double dx, double dy, int *ierr);
extern void c_fill_rounded_rect(OBJ_PTR fmkr, FM *p, double x, double y, double width, double height, double dx, double dy, int *ierr);
extern void c_stroke_rounded_rect(OBJ_PTR fmkr, FM *p, 
   double x, double y, double width, double height, double dx, double dy, int *ierr);
extern void c_fill_and_stroke_rounded_rect(OBJ_PTR fmkr, FM *p, 
   double x, double y, double width, double height, double dx, double dy, int *ierr);
extern void c_clip_circle(OBJ_PTR fmkr, FM *p, double x, double y, double dx, int *ierr);
extern void c_fill_circle(OBJ_PTR fmkr, FM *p, double x, double y, double dx, int *ierr);
extern void c_stroke_circle(OBJ_PTR fmkr, FM *p, double x, double y, double dx, int *ierr);
extern void c_fill_and_stroke_circle(OBJ_PTR fmkr, FM *p, double x, double y, double dx, int *ierr);
extern void c_append_frame(OBJ_PTR fmkr, FM *p, bool clip, int *ierr);
extern void c_append_frame_to_path(OBJ_PTR fmkr, FM *p, int *ierr);
extern void c_fill_frame(OBJ_PTR fmkr, FM *p, int *ierr);
extern void c_stroke_frame(OBJ_PTR fmkr, FM *p, int *ierr);
extern void c_fill_and_stroke_frame(OBJ_PTR fmkr, FM *p, int *ierr);
extern void c_clip_to_frame(OBJ_PTR fmkr, FM *p, int *ierr);

/*======================================================================*/
// pdftext.c
extern void Init_Font_Dictionary(void);
extern bool Used_Any_Fonts(void);
extern void Clear_Fonts_In_Use_Flags(void);
extern void Write_Font_Dictionaries(void);
extern void Write_Font_Descriptors(void);
extern void Write_Font_Widths(void);

extern OBJ_PTR c_register_font(OBJ_PTR fmkr, FM *p, char *font_name, int *ierr);
extern OBJ_PTR c_marker_string_info(OBJ_PTR fmkr, FM *p, int fnt, unsigned char *text, double scale, int *ierr);
extern void c_private_show_marker(OBJ_PTR fmkr, FM *p, OBJ_PTR args, int *ierr);

/*======================================================================*/
// texout.c
extern void Open_tex(OBJ_PTR fmkr, char *filename, bool quiet_mode, int *ierr);
extern void Close_tex(OBJ_PTR fmkr, bool quiet_mode, int *ierr);
extern void Create_wrapper(OBJ_PTR fmkr, char *filename, bool quiet_mode, int *ierr);
extern void Init_tex(int *ierr);
extern void Rename_tex(char *oldname, char *newname, int *ierr);
extern void private_make_portfolio(char *filename, OBJ_PTR fignums, OBJ_PTR fignames, int *ierr);

extern void c_rescale_text(OBJ_PTR fmkr, FM *p, double scaling_factor, int *ierr);
extern void c_show_rotated_text(OBJ_PTR fmkr, FM *p, char *text, int frame_side, double shift, double fraction,
				double scale, double angle, int justification, int alignment, OBJ_PTR measure_name, int *ierr);
extern void c_show_rotated_label(OBJ_PTR fmkr, FM *p, char *text, 
   double xloc, double yloc, double scale, double angle, int justification, int alignment, OBJ_PTR measure_name, int *ierr);
extern OBJ_PTR c_check_label_clip(OBJ_PTR fmkr, FM *p, double x, double y, int *ierr);
extern void c_private_save_measure(OBJ_PTR fmkr, OBJ_PTR measure_name, 
                                   double width, double height, double depth);


/*======================================================================*/

// ENLARGE = the conversion factor from "big points" to output units
#define ENLARGE (p->scaling_factor)
#define BIG_POINTS_PER_INCH 72.0
#define INCHES_PER_MM 0.0393700787

// 10 miles should be enough!
#define iMAX_DEV_COORD_ALLOWED (BIG_POINTS_PER_INCH*12*5280*10)
#define MAX_DEV_COORD_ALLOWED ((double)iMAX_DEV_COORD_ALLOWED)

#define convert_inches_to_output(inches) ((ENLARGE*BIG_POINTS_PER_INCH)*(inches))
#define convert_output_to_inches(output) ((output)/(ENLARGE*BIG_POINTS_PER_INCH))

#define convert_mm_to_output(mm) ((ENLARGE*BIG_POINTS_PER_INCH*INCHES_PER_MM)*(mm))
#define convert_output_to_mm(output) ((output)/(ENLARGE*BIG_POINTS_PER_INCH*INCHES_PER_MM))

#define convert_page_to_output_x(p,x) ((p)->page_left + (x)*(p)->page_width)
#define convert_page_to_output_y(p,y) ((p)->page_bottom + (y)*(p)->page_height)
#define convert_page_to_output_dx(p,dx) ((dx)*(p)->page_width)
#define convert_page_to_output_dy(p,dy) ((dy)*(p)->page_height)

#define convert_output_to_page_x(p,x) (((x) - (p)->page_left)/(p)->page_width)
#define convert_output_to_page_y(p,y) (((y) - (p)->page_bottom)/(p)->page_height)
#define convert_output_to_page_dx(p,dx) ((dx)/(p)->page_width)
#define convert_output_to_page_dy(p,dy) ((dy)/(p)->page_height)

#define convert_frame_to_page_x(p,x) ((p)->frame_left + (x)*(p)->frame_width)
#define convert_frame_to_page_y(p,y) ((p)->frame_bottom + (y)*(p)->frame_height)
#define convert_frame_to_page_dx(p,dx) ((dx)*(p)->frame_width)
#define convert_frame_to_page_dy(p,dy) ((dy)*(p)->frame_height)

#define convert_page_to_frame_x(p,x) (((x) - (p)->frame_left)/(p)->frame_width)
#define convert_page_to_frame_y(p,y) (((y) - (p)->frame_bottom)/(p)->frame_height)
#define convert_page_to_frame_dx(p,dx) ((dx)/(p)->frame_width)
#define convert_page_to_frame_dy(p,dy) ((dy)/(p)->frame_height)

#define convert_figure_to_frame_x(p,x) (\
   ((p)->bounds_left > (p)->bounds_right)? (1.0 - ((x)-(p)->bounds_xmin)/(p)->bounds_width) : ((x)-(p)->bounds_xmin)/(p)->bounds_width)
#define convert_figure_to_frame_y(p,y) (\
   ((p)->bounds_bottom > (p)->bounds_top)? (1.0 - ((y)-(p)->bounds_ymin)/(p)->bounds_height) : ((y)-(p)->bounds_ymin)/(p)->bounds_height)
#define convert_figure_to_frame_dx(p,dx) (\
   ((p)->bounds_left > (p)->bounds_right)? -(dx)/(p)->bounds_width : (dx)/(p)->bounds_width)
#define convert_figure_to_frame_dy(p,dy) (\
   ((p)->bounds_bottom > (p)->bounds_top)? -(dy)/(p)->bounds_height : (dy)/(p)->bounds_height)

#define convert_frame_to_figure_x(p,x) (\
   ((p)->bounds_left > (p)->bounds_right)? ((p)->bounds_left - (x)*(p)->bounds_width): ((p)->bounds_left + (x)*(p)->bounds_width))
#define convert_frame_to_figure_y(p,y) (\
   ((p)->bounds_bottom > (p)->bounds_top)? ((p)->bounds_bottom - (y)*(p)->bounds_height): ((p)->bounds_bottom + (y)*(p)->bounds_height))
#define convert_frame_to_figure_dx(p,dx) (\
   ((p)->bounds_left > (p)->bounds_right)? (-(dx)*(p)->bounds_width): ((dx)*(p)->bounds_width))
#define convert_frame_to_figure_dy(p,dy) (\
   ((p)->bounds_bottom > (p)->bounds_top)? (-(dy)*(p)->bounds_height): ((dy)*(p)->bounds_height))

#ifndef USE_P
#define USE_P p = NULL;
#endif
#ifndef STRLEN
#define STRLEN 256
#endif
#ifndef PI
#define PI 3.1415926535897932384
#endif
#ifndef MAX
#define MAX(a,b)    (((a) > (b)) ? (a) : (b))
#endif
#ifndef MIN
#define MIN(a,b)    (((a) < (b)) ? (a) : (b))
#endif
#ifndef ABS
#define ABS(a)      ((a)<0 ? -(a) : (a))
#endif
#ifndef ROUND
#define ROUND(a)    (int)((a)<0.0 ? ((a)-0.5) : ((a)+0.5))
#endif
#ifndef SIGN
#define SIGN(a)         ((a)<0 ? -1 : 1)
#endif

#endif   /* __figures_H__ */

