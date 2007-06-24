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




/*======================================================================*/
// axes.c
extern char *Get_String(OBJ_PTR ary, int index);
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

/*======================================================================*/
// figures.c
extern FM *Get_FM(OBJ_PTR fmkr);
extern bool Is_FM(OBJ_PTR fmkr);
void Init_FigureMaker(void);
extern bool Get_initialized();
extern void Set_initialized();

/*======================================================================*/
// init.c
extern void Init_IDs(void);
extern OBJ_PTR FM_set_device_pagesize(OBJ_PTR fmkr, OBJ_PTR width, OBJ_PTR height); // size in output coords (decipoints)
extern OBJ_PTR FM_set_frame_sides(OBJ_PTR fmkr, OBJ_PTR left, OBJ_PTR right, OBJ_PTR top, OBJ_PTR bottom); // in page coords [0..1]
extern void Initialize_Figure(OBJ_PTR fmkr);
extern OBJ_PTR do_cmd(OBJ_PTR fmkr, OBJ_PTR cmd);
extern bool Get_bool(OBJ_PTR obj, ID name_ID);
extern int Get_int(OBJ_PTR obj, ID name_ID);
extern double Get_double(OBJ_PTR obj, ID name_ID); // for instance variables of the obj
extern char *Get_tex_preview_paper_width(OBJ_PTR fmkr);
extern char *Get_tex_preview_paper_height(OBJ_PTR fmkr);
extern char *Get_tex_preview_hoffset(OBJ_PTR fmkr);
extern char *Get_tex_preview_voffset(OBJ_PTR fmkr);
extern char *Get_tex_preview_figure_width(OBJ_PTR fmkr);
extern char *Get_tex_preview_figure_height(OBJ_PTR fmkr);
extern char *Get_tex_fontsize(OBJ_PTR fmkr);
extern char *Get_tex_fontfamily(OBJ_PTR fmkr);
extern char *Get_tex_fontseries(OBJ_PTR fmkr);
extern char *Get_tex_fontshape(OBJ_PTR fmkr);
extern char *Get_tex_preview_minwhitespace(OBJ_PTR fmkr);
extern bool Get_tex_preview_fullpage(OBJ_PTR fmkr);
extern char *Get_tex_preview_tiogafigure_command(OBJ_PTR fmkr);
extern char *Get_tex_preview_generated_preamble(OBJ_PTR fmkr);
extern double Get_tex_xoffset(OBJ_PTR fmkr);
extern double Get_tex_yoffset(OBJ_PTR fmkr);
extern char *Get_tex_preview_documentclass(OBJ_PTR fmkr);
extern char *Get_tex_preamble(OBJ_PTR fmkr);
extern char *Get_tex_xaxis_numeric_label(OBJ_PTR fmkr);
extern char *Get_tex_yaxis_numeric_label(OBJ_PTR fmkr);
extern char *Get_tex_preview_pagestyle(OBJ_PTR fmkr);
extern char *Get_tex_preview_tiogafigure_command(OBJ_PTR fmkr);
extern OBJ_PTR Get_line_type(OBJ_PTR fmkr);
extern void Set_line_type(OBJ_PTR fmkr, OBJ_PTR v);
extern OBJ_PTR Get_xaxis_tick_labels(OBJ_PTR fmkr);
extern OBJ_PTR Get_xaxis_locations_for_major_ticks(OBJ_PTR fmkr);
extern OBJ_PTR Get_xaxis_locations_for_minor_ticks(OBJ_PTR fmkr);
extern OBJ_PTR Get_yaxis_tick_labels(OBJ_PTR fmkr);
extern OBJ_PTR Get_yaxis_locations_for_major_ticks(OBJ_PTR fmkr);
extern OBJ_PTR Get_yaxis_locations_for_minor_ticks(OBJ_PTR fmkr);
extern OBJ_PTR FM_get_save_filename(OBJ_PTR fmkr, OBJ_PTR name);
extern OBJ_PTR FM_private_make(OBJ_PTR fmkr, OBJ_PTR name, OBJ_PTR cmd);
extern OBJ_PTR FM_private_make_portfolio(OBJ_PTR fmkr, OBJ_PTR name, OBJ_PTR fignums, OBJ_PTR fignames);

/*======================================================================*/
// makers.c
extern OBJ_PTR FM_private_make_contour(OBJ_PTR fmkr, OBJ_PTR gaps,
     OBJ_PTR xs, OBJ_PTR ys, // data x coordinates and y coordinates
     OBJ_PTR zs, OBJ_PTR z_level, // the Dtable of OBJ_PTRs and the desired contour level
     OBJ_PTR legit, // the Dtable of flags (nonzero means okay)
     OBJ_PTR method // int == 1 means use CONREC
     );
extern OBJ_PTR FM_private_make_steps(OBJ_PTR fmkr, OBJ_PTR Xdata, OBJ_PTR Ydata,
    OBJ_PTR xfirst, OBJ_PTR yfirst, OBJ_PTR xlast, OBJ_PTR ylast);
extern OBJ_PTR FM_private_make_spline_interpolated_points(OBJ_PTR fmkr, OBJ_PTR Xvec, 
   OBJ_PTR Xdata, OBJ_PTR Ydata, OBJ_PTR start_slope, OBJ_PTR end_slope);

/*======================================================================*/
// pdfcolor.c
extern void Free_Functions();
extern void Write_Functions(void);
extern void Free_Stroke_Opacities(void);
extern OBJ_PTR FM_stroke_opacity_set(OBJ_PTR fmkr, OBJ_PTR OBJ_PTR);
extern void Free_Fill_Opacities(void);
extern OBJ_PTR FM_fill_opacity_set(OBJ_PTR fmkr, OBJ_PTR OBJ_PTR);
extern void Write_Stroke_Opacity_Objects(void);
extern void Write_Fill_Opacity_Objects(void);
extern void Free_Shadings();
extern void Write_Shadings(void);
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
extern OBJ_PTR FM_title_color_set(OBJ_PTR fmkr, OBJ_PTR OBJ_PTR);
extern OBJ_PTR FM_title_color_get(OBJ_PTR fmkr);
extern OBJ_PTR FM_xlabel_color_get(OBJ_PTR fmkr);
extern OBJ_PTR FM_xlabel_color_set(OBJ_PTR fmkr, OBJ_PTR OBJ_PTR);
extern OBJ_PTR FM_ylabel_color_get(OBJ_PTR fmkr);
extern OBJ_PTR FM_ylabel_color_set(OBJ_PTR fmkr, OBJ_PTR OBJ_PTR);
extern OBJ_PTR FM_xaxis_stroke_color_get(OBJ_PTR fmkr);
extern OBJ_PTR FM_xaxis_stroke_color_set(OBJ_PTR fmkr, OBJ_PTR OBJ_PTR);
extern OBJ_PTR FM_yaxis_stroke_color_get(OBJ_PTR fmkr);
extern OBJ_PTR FM_yaxis_stroke_color_set(OBJ_PTR fmkr, OBJ_PTR OBJ_PTR);

/*======================================================================*/
// pdfcoords.c
extern void Recalc_Font_Hts(FM *p);
extern OBJ_PTR FM_private_set_subframe(OBJ_PTR fmkr, OBJ_PTR left_margin, OBJ_PTR right_margin, OBJ_PTR top_margin, OBJ_PTR bottom_margin);
extern void c_private_set_default_font_size(FM *p, double size);
extern OBJ_PTR FM_private_set_default_font_size(OBJ_PTR fmkr, OBJ_PTR size); // size in points
extern OBJ_PTR FM_private_context(OBJ_PTR fmkr, OBJ_PTR cmd);
extern OBJ_PTR FM_doing_subplot(OBJ_PTR fmkr);
extern OBJ_PTR FM_doing_subfigure(OBJ_PTR fmkr);
extern OBJ_PTR FM_private_set_bounds(OBJ_PTR fmkr, OBJ_PTR left, OBJ_PTR right, OBJ_PTR top, OBJ_PTR bottom); /* in figure coords */
extern OBJ_PTR FM_convert_to_degrees(OBJ_PTR fmkr, OBJ_PTR dx, OBJ_PTR dy);
extern OBJ_PTR FM_convert_inches_to_output(OBJ_PTR fmkr, OBJ_PTR OBJ_PTR);
extern OBJ_PTR FM_convert_output_to_inches(OBJ_PTR fmkr, OBJ_PTR OBJ_PTR);
extern OBJ_PTR FM_convert_mm_to_output(OBJ_PTR fmkr, OBJ_PTR OBJ_PTR);
extern OBJ_PTR FM_convert_output_to_mm(OBJ_PTR fmkr, OBJ_PTR OBJ_PTR);
extern OBJ_PTR FM_convert_page_to_output_x(OBJ_PTR fmkr, OBJ_PTR OBJ_PTR);
extern OBJ_PTR FM_convert_page_to_output_y(OBJ_PTR fmkr, OBJ_PTR OBJ_PTR);
extern OBJ_PTR FM_convert_page_to_output_dx(OBJ_PTR fmkr, OBJ_PTR OBJ_PTR);
extern OBJ_PTR FM_convert_page_to_output_dy(OBJ_PTR fmkr, OBJ_PTR OBJ_PTR);
extern OBJ_PTR FM_convert_output_to_page_x(OBJ_PTR fmkr, OBJ_PTR OBJ_PTR);
extern OBJ_PTR FM_convert_output_to_page_y(OBJ_PTR fmkr, OBJ_PTR OBJ_PTR);
extern OBJ_PTR FM_convert_output_to_page_dx(OBJ_PTR fmkr, OBJ_PTR OBJ_PTR);
extern OBJ_PTR FM_convert_output_to_page_dy(OBJ_PTR fmkr, OBJ_PTR OBJ_PTR);
extern OBJ_PTR FM_convert_frame_to_page_x(OBJ_PTR fmkr, OBJ_PTR OBJ_PTR);
extern OBJ_PTR FM_convert_frame_to_page_y(OBJ_PTR fmkr, OBJ_PTR OBJ_PTR);
extern OBJ_PTR FM_convert_frame_to_page_dx(OBJ_PTR fmkr, OBJ_PTR OBJ_PTR);
extern OBJ_PTR FM_convert_frame_to_page_dy(OBJ_PTR fmkr, OBJ_PTR OBJ_PTR);
extern OBJ_PTR FM_convert_page_to_frame_x(OBJ_PTR fmkr, OBJ_PTR OBJ_PTR);
extern OBJ_PTR FM_convert_page_to_frame_y(OBJ_PTR fmkr, OBJ_PTR OBJ_PTR);
extern OBJ_PTR FM_convert_page_to_frame_dx(OBJ_PTR fmkr, OBJ_PTR OBJ_PTR);
extern OBJ_PTR FM_convert_page_to_frame_dy(OBJ_PTR fmkr, OBJ_PTR OBJ_PTR);
extern OBJ_PTR FM_convert_figure_to_frame_x(OBJ_PTR fmkr, OBJ_PTR OBJ_PTR);
extern OBJ_PTR FM_convert_figure_to_frame_y(OBJ_PTR fmkr, OBJ_PTR OBJ_PTR);
extern OBJ_PTR FM_convert_figure_to_frame_dx(OBJ_PTR fmkr, OBJ_PTR OBJ_PTR);
extern OBJ_PTR FM_convert_figure_to_frame_dy(OBJ_PTR fmkr, OBJ_PTR OBJ_PTR);
extern OBJ_PTR FM_convert_frame_to_figure_x(OBJ_PTR fmkr, OBJ_PTR OBJ_PTR);
extern OBJ_PTR FM_convert_frame_to_figure_y(OBJ_PTR fmkr, OBJ_PTR OBJ_PTR);
extern OBJ_PTR FM_convert_frame_to_figure_dx(OBJ_PTR fmkr, OBJ_PTR OBJ_PTR);
extern OBJ_PTR FM_convert_frame_to_figure_dy(OBJ_PTR fmkr, OBJ_PTR OBJ_PTR);
extern double convert_figure_to_output_x(FM *p, double x);
extern double convert_figure_to_output_y(FM *p, double y);
extern double convert_figure_to_output_dy(FM *p, double dy);
extern double convert_figure_to_output_dx(FM *p, double dx);
extern OBJ_PTR FM_convert_figure_to_output_x(OBJ_PTR fmkr, OBJ_PTR OBJ_PTR);
extern OBJ_PTR FM_convert_figure_to_output_y(OBJ_PTR fmkr, OBJ_PTR OBJ_PTR);
extern OBJ_PTR FM_convert_figure_to_output_dx(OBJ_PTR fmkr, OBJ_PTR OBJ_PTR);
extern OBJ_PTR FM_convert_figure_to_output_dy(OBJ_PTR fmkr, OBJ_PTR OBJ_PTR);
extern double convert_output_to_figure_x(FM *p, double x);
extern double convert_output_to_figure_y(FM *p, double y);
extern double convert_output_to_figure_dy(FM *p, double dy);
extern double convert_output_to_figure_dx(FM *p, double dx);
extern OBJ_PTR FM_convert_output_to_figure_x(OBJ_PTR fmkr, OBJ_PTR OBJ_PTR);
extern OBJ_PTR FM_convert_output_to_figure_y(OBJ_PTR fmkr, OBJ_PTR OBJ_PTR);
extern OBJ_PTR FM_convert_output_to_figure_dx(OBJ_PTR fmkr, OBJ_PTR OBJ_PTR);
extern OBJ_PTR FM_convert_output_to_figure_dy(OBJ_PTR fmkr, OBJ_PTR OBJ_PTR);

/*======================================================================*/
// pdffile.c
extern void Init_pdf(void);
extern void Record_Object_Offset(int obj_number);
extern void Open_pdf(OBJ_PTR fmkr, char *filename, bool quiet_mode);
extern void Start_Axis_Standard_State(FM *p, double r, double g, double b, double line_width);
extern void End_Axis_Standard_State(void);
extern void Write_gsave(void);
extern void Write_grestore(void);
extern void Close_pdf(OBJ_PTR fmkr, bool quiet_mode);
extern void Rename_pdf(char *oldname, char *newname);

/*======================================================================*/
// pdfimage.c
extern OBJ_PTR FM_private_show_jpg(OBJ_PTR fmkr, OBJ_PTR filename, 
   OBJ_PTR width, OBJ_PTR height, OBJ_PTR image_destination, OBJ_PTR mask_xo_num);
extern OBJ_PTR FM_private_create_image_data(OBJ_PTR fmkr, OBJ_PTR data,
   OBJ_PTR first_row, OBJ_PTR last_row, OBJ_PTR first_column, OBJ_PTR last_column,
   OBJ_PTR min_OBJ_PTR, OBJ_PTR max_OBJ_PTR, OBJ_PTR max_code, OBJ_PTR if_below_range, OBJ_PTR if_above_range);
extern OBJ_PTR FM_private_create_monochrome_image_data(OBJ_PTR fmkr, OBJ_PTR data,
   OBJ_PTR first_row, OBJ_PTR last_row, OBJ_PTR first_column, OBJ_PTR last_column,
   OBJ_PTR boundary, OBJ_PTR reverse);
extern OBJ_PTR FM_private_show_image(OBJ_PTR fmkr, OBJ_PTR llx, OBJ_PTR lly, OBJ_PTR lrx, OBJ_PTR lry, OBJ_PTR ulx, OBJ_PTR uly,
   OBJ_PTR interpolate, OBJ_PTR w, OBJ_PTR h, OBJ_PTR data, OBJ_PTR OBJ_PTR_mask_min, OBJ_PTR OBJ_PTR_mask_max,
   OBJ_PTR hival, OBJ_PTR lookup, OBJ_PTR mask_xo_num);
extern OBJ_PTR FM_private_show_rgb_image(OBJ_PTR fmkr, 
   OBJ_PTR llx, OBJ_PTR lly, OBJ_PTR lrx, OBJ_PTR lry, OBJ_PTR ulx, OBJ_PTR uly,
   OBJ_PTR interpolate, OBJ_PTR w, OBJ_PTR h, OBJ_PTR data, OBJ_PTR mask_xo_num);
extern OBJ_PTR FM_private_show_cmyk_image(OBJ_PTR fmkr, 
   OBJ_PTR llx, OBJ_PTR lly, OBJ_PTR lrx, OBJ_PTR lry, OBJ_PTR ulx, OBJ_PTR uly,
   OBJ_PTR interpolate, OBJ_PTR w, OBJ_PTR h, OBJ_PTR data, OBJ_PTR mask_xo_num);
extern OBJ_PTR FM_private_show_grayscale_image(OBJ_PTR fmkr, 
   OBJ_PTR llx, OBJ_PTR lly, OBJ_PTR lrx, OBJ_PTR lry, OBJ_PTR ulx, OBJ_PTR uly,
   OBJ_PTR interpolate, OBJ_PTR w, OBJ_PTR h, OBJ_PTR data, OBJ_PTR mask_xo_num);
extern OBJ_PTR FM_private_show_monochrome_image(OBJ_PTR fmkr, OBJ_PTR llx, OBJ_PTR lly, OBJ_PTR lrx, OBJ_PTR lry, OBJ_PTR ulx, OBJ_PTR uly,
   OBJ_PTR interpolate, OBJ_PTR reversed, OBJ_PTR w, OBJ_PTR h, OBJ_PTR data, OBJ_PTR mask_xo_num);

/*======================================================================*/
// pdfpath.c
extern void Unpack_RGB(OBJ_PTR rgb, double *rp, double *gp, double *bp);
void c_stroke_color_set(FM *p, double r, double g, double b);
extern OBJ_PTR FM_stroke_color_set(OBJ_PTR fmkr, OBJ_PTR OBJ_PTR);
extern OBJ_PTR FM_stroke_color_get(OBJ_PTR fmkr);
void c_fill_color_set(FM *p, double r, double g, double b);
extern OBJ_PTR FM_fill_color_set(OBJ_PTR fmkr, OBJ_PTR OBJ_PTR);
extern OBJ_PTR FM_fill_color_get(OBJ_PTR fmkr);
void c_line_width_set(FM *p, double line_width);
extern OBJ_PTR FM_line_width_set(OBJ_PTR fmkr, OBJ_PTR OBJ_PTR);
extern OBJ_PTR FM_rescale_lines(OBJ_PTR fmkr, OBJ_PTR scaling);
void c_line_cap_set(FM *p, int line_cap);
extern OBJ_PTR FM_line_cap_set(OBJ_PTR fmkr, OBJ_PTR OBJ_PTR);
void c_line_join_set(FM *p, int line_join);
extern OBJ_PTR FM_line_join_set(OBJ_PTR fmkr, OBJ_PTR OBJ_PTR);
void c_miter_limit_set(FM *p, double miter_limit);
extern OBJ_PTR FM_miter_limit_set(OBJ_PTR fmkr, OBJ_PTR OBJ_PTR);
extern OBJ_PTR FM_line_type_set(OBJ_PTR fmkr, OBJ_PTR line_type);
void update_bbox(FM *p, double x, double y);
extern OBJ_PTR FM_update_bbox(OBJ_PTR fmkr, OBJ_PTR x, OBJ_PTR y);
extern OBJ_PTR FM_bbox_left(OBJ_PTR fmkr);
extern OBJ_PTR FM_bbox_right(OBJ_PTR fmkr);
extern OBJ_PTR FM_bbox_top(OBJ_PTR fmkr);
extern OBJ_PTR FM_bbox_bottom(OBJ_PTR fmkr);
extern void c_moveto(FM *p, double x, double y);
extern OBJ_PTR FM_move_to_point(OBJ_PTR fmkr, OBJ_PTR x, OBJ_PTR y);  // x y m
extern void c_lineto(FM *p, double x, double y);
extern OBJ_PTR FM_append_point_to_path(OBJ_PTR fmkr, OBJ_PTR x, OBJ_PTR y);  // x y l
extern void c_bezier_control_points(double *data, double x0, double y0, double delta_x, double a, double b, double c);
extern OBJ_PTR FM_bezier_control_points(OBJ_PTR fmkr, OBJ_PTR x0, OBJ_PTR y0, OBJ_PTR delta_x, OBJ_PTR a, OBJ_PTR b, OBJ_PTR c);
extern void c_curveto(FM *p, double x1, double y1, double x2, double y2, double x3, double y3);
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
void Init_Font_Dictionary(void);
extern OBJ_PTR FM_register_font(OBJ_PTR fmkr, OBJ_PTR font_name);  // returns font number.
extern bool Used_Any_Fonts(void);
extern void Clear_Fonts_In_Use_Flags(void);
extern void Write_Font_Dictionaries(void);
extern void Write_Font_Descriptors(void);
extern void Write_Font_Widths(void);
extern OBJ_PTR FM_marker_string_info(OBJ_PTR fmkr, OBJ_PTR font_number, OBJ_PTR string, OBJ_PTR scale);
extern OBJ_PTR FM_private_show_marker(OBJ_PTR fmkr, OBJ_PTR integer_args, OBJ_PTR stroke_width, OBJ_PTR string,
    OBJ_PTR x, OBJ_PTR y, OBJ_PTR x_vec, OBJ_PTR y_vec,
    OBJ_PTR h_scale, OBJ_PTR v_scale, OBJ_PTR scale, OBJ_PTR it_angle, OBJ_PTR ascent_angle, OBJ_PTR angle,
    OBJ_PTR fill_color, OBJ_PTR stroke_color);

/*======================================================================*/
// texout.c
extern OBJ_PTR FM_rescale_text(OBJ_PTR fmkr, OBJ_PTR scaling);
extern void c_show_rotated_text(FM *p, char *text, int frame_side, double shift, double fraction,
   double scale, double angle, int justification, int alignment);
extern OBJ_PTR FM_show_rotated_text(OBJ_PTR fmkr, OBJ_PTR text, OBJ_PTR frame_side, OBJ_PTR shift,
    OBJ_PTR fraction, OBJ_PTR scale, OBJ_PTR angle, OBJ_PTR justification, OBJ_PTR alignment);
extern void c_show_rotated_label(FM *p, char *text, 
   double xloc, double yloc, double scale, double angle, int justification, int alignment);
extern OBJ_PTR FM_show_rotated_label(OBJ_PTR fmkr, OBJ_PTR text,
    OBJ_PTR xloc, OBJ_PTR yloc, OBJ_PTR scale, OBJ_PTR angle, OBJ_PTR justification, OBJ_PTR alignment);
extern OBJ_PTR FM_check_label_clip(OBJ_PTR fmkr, OBJ_PTR xloc, OBJ_PTR yloc);
extern void Open_tex(OBJ_PTR fmkr, char *filename, bool quiet_mode);
extern void Close_tex(OBJ_PTR fmkr, bool quiet_mode);
extern void Create_wrapper(OBJ_PTR fmkr, char *filename, bool quiet_mode);
extern void Init_tex(void);
extern void Rename_tex(char *oldname, char *newname);
extern void private_make_portfolio(char *filename, OBJ_PTR fignums, OBJ_PTR fignames);

/*======================================================================*/

// ENLARGE = the conversion factor from "big points" to output units
#define ENLARGE 10.0
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

