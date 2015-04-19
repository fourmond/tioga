/* init.c */
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

#include "generic.h"
#include "figures.h"
#include "pdfs.h"

long trace_cnt; // counter for debugging traces
long trace_lvl; // set to 0 to turn tracing off. higher values turn on more tracing.
#define TRACE(fn) if (trace_lvl > 0) printf("%i %s\n",++trace_cnt,fn)

static ID_PTR fm_data_ID;
static ID_PTR save_dir_ID;
static ID_PTR quiet_mode_ID;
static ID_PTR tex_preview_documentclass_ID;
static ID_PTR tex_preamble_ID;
static ID_PTR xaxis_numeric_label_tex_ID;
static ID_PTR yaxis_numeric_label_tex_ID;
static ID_PTR tex_preview_pagestyle_ID;
static ID_PTR tex_preview_paper_width_ID;
static ID_PTR tex_preview_paper_height_ID;
static ID_PTR tex_preview_hoffset_ID;
static ID_PTR tex_preview_voffset_ID;
static ID_PTR tex_preview_figure_width_ID;
static ID_PTR tex_preview_figure_height_ID;
static ID_PTR tex_preview_tiogafigure_command_ID;
static ID_PTR tex_preview_fullpage_ID;
static ID_PTR tex_preview_minwhitespace_ID;
static ID_PTR do_cmd_ID;
static ID_PTR make_page_ID;
static ID_PTR initialized_ID;
static ID_PTR tex_xoffset_ID;
static ID_PTR tex_yoffset_ID;
static ID_PTR tex_fontsize_ID;
static ID_PTR tex_fontfamily_ID;
static ID_PTR tex_fontseries_ID;
static ID_PTR tex_fontshape_ID;
static ID_PTR line_type_ID;
static ID_PTR xaxis_locations_for_major_ticks_ID;
static ID_PTR xaxis_locations_for_minor_ticks_ID;
static ID_PTR xaxis_tick_labels_ID;
static ID_PTR yaxis_locations_for_major_ticks_ID;
static ID_PTR yaxis_locations_for_minor_ticks_ID;
static ID_PTR yaxis_tick_labels_ID;
ID_PTR measures_info_ID;


void Init_IDs(void)
{
   
   Init_generic();
   
	do_cmd_ID = ID_Get("do_cmd");
	make_page_ID = ID_Get("make_page");
   // class variables
	initialized_ID = ID_Get("@@initialized");
	// instance variables
   fm_data_ID = ID_Get("@fm_data");
	save_dir_ID = ID_Get("@save_dir");
	quiet_mode_ID = ID_Get("@quiet_mode");    
	tex_xoffset_ID = ID_Get("@tex_xoffset");
	tex_yoffset_ID = ID_Get("@tex_yoffset");
    tex_preview_documentclass_ID = ID_Get("@tex_preview_documentclass");
    tex_preamble_ID = ID_Get("@tex_preamble");
    xaxis_numeric_label_tex_ID = ID_Get("@xaxis_numeric_label_tex");
    yaxis_numeric_label_tex_ID = ID_Get("@yaxis_numeric_label_tex");
    tex_preview_pagestyle_ID = ID_Get("@tex_preview_pagestyle");
    
    tex_preview_paper_width_ID = ID_Get("@tex_preview_paper_width");
    tex_preview_paper_height_ID = ID_Get("@tex_preview_paper_height");
    tex_preview_hoffset_ID = ID_Get("@tex_preview_hoffset");
    tex_preview_voffset_ID = ID_Get("@tex_preview_voffset");
    tex_preview_figure_width_ID = ID_Get("@tex_preview_figure_width");
    tex_preview_figure_height_ID = ID_Get("@tex_preview_figure_height");

    tex_preview_fullpage_ID = ID_Get("@tex_preview_fullpage");
    tex_preview_minwhitespace_ID = ID_Get("@tex_preview_minwhitespace");

    tex_preview_tiogafigure_command_ID = ID_Get("@tex_preview_tiogafigure_command");
    
    tex_fontsize_ID = ID_Get("@tex_fontsize");
    tex_fontfamily_ID = ID_Get("@tex_fontfamily");
    tex_fontseries_ID = ID_Get("@tex_fontseries");
    tex_fontshape_ID = ID_Get("@tex_fontshape");

    line_type_ID = ID_Get("@line_type");
    xaxis_locations_for_major_ticks_ID = ID_Get("@xaxis_locations_for_major_ticks");
    xaxis_locations_for_minor_ticks_ID = ID_Get("@xaxis_locations_for_minor_ticks");
    xaxis_tick_labels_ID = ID_Get("@xaxis_tick_labels");
    yaxis_locations_for_major_ticks_ID = ID_Get("@yaxis_locations_for_major_ticks");
    yaxis_locations_for_minor_ticks_ID = ID_Get("@yaxis_locations_for_minor_ticks");
    yaxis_tick_labels_ID = ID_Get("@yaxis_tick_labels");
    measures_info_ID = ID_Get("@measures_info");
}

void do_cmd(OBJ_PTR fmkr, OBJ_PTR cmd, int *ierr) { 
   Call_Function(fmkr, do_cmd_ID, cmd, ierr); }

static void Type_Error(OBJ_PTR obj, ID_PTR name_ID, char *expected, int *ierr)
{
   char *name = ID_Name(name_ID, ierr);
   if (*ierr != 0) return;
   while (name[0] == '@') name++;
   RAISE_ERROR_ss("Require %s OBJ_PTR for '%s'", expected, name, ierr);
}

bool Get_bool(OBJ_PTR obj, ID_PTR name_ID, int *ierr) {
   OBJ_PTR v = Obj_Attr_Get(obj, name_ID, ierr);
   if (*ierr != 0) return false;
   if (v != OBJ_FALSE && v != OBJ_TRUE && v != OBJ_NIL) {
      Type_Error(v, name_ID, "true or false", ierr);
      return false;
   }
   return v == OBJ_TRUE;
}

int Get_int(OBJ_PTR obj, ID_PTR name_ID, int *ierr) {
   OBJ_PTR v = Obj_Attr_Get(obj, name_ID, ierr);
   if (*ierr != 0) return 0;
   if (!Is_Kind_of_Integer(v)) {
      Type_Error(v, name_ID, "Integer", ierr);
      return 0;
   }
   return Number_to_int(v, ierr);
}

double Get_double(OBJ_PTR obj, ID_PTR name_ID, int *ierr) {
   OBJ_PTR v = Obj_Attr_Get(obj, name_ID, ierr);
   if (*ierr != 0) return 0.0;
   if (!Is_Kind_of_Number(v)) {
      Type_Error(v, name_ID, "Numeric", ierr);
      return 0.0;
   }
   return Number_to_double(v, ierr);
}

char *Get_tex_preview_paper_width(OBJ_PTR fmkr, int *ierr) {
   OBJ_PTR v = Obj_Attr_Get(fmkr, tex_preview_paper_width_ID, ierr);
   if (*ierr != 0) return NULL;
   if (v == OBJ_NIL) return NULL;
   return String_Ptr(v, ierr);
}

char *Get_tex_preview_paper_height(OBJ_PTR fmkr, int *ierr) {
   OBJ_PTR v = Obj_Attr_Get(fmkr, tex_preview_paper_height_ID, ierr);
   if (*ierr != 0) return NULL;
   if (v == OBJ_NIL) return NULL;
   return String_Ptr(v, ierr);
}

char *Get_tex_preview_hoffset(OBJ_PTR fmkr, int *ierr) {
   OBJ_PTR v = Obj_Attr_Get(fmkr, tex_preview_hoffset_ID, ierr);
   if (*ierr != 0) return NULL;
   if (v == OBJ_NIL) return NULL;
   return String_Ptr(v, ierr);
}

char *Get_tex_preview_voffset(OBJ_PTR fmkr, int *ierr) {
   OBJ_PTR v = Obj_Attr_Get(fmkr, tex_preview_voffset_ID, ierr);
   if (*ierr != 0) return NULL;
   if (v == OBJ_NIL) return NULL;
   return String_Ptr(v, ierr);
}

char *Get_tex_preview_figure_width(OBJ_PTR fmkr, int *ierr) {
   OBJ_PTR v = Obj_Attr_Get(fmkr, tex_preview_figure_width_ID, ierr);
   if (*ierr != 0) return NULL;
   if (v == OBJ_NIL) return NULL;
   return String_Ptr(v, ierr);
}

char *Get_tex_preview_figure_height(OBJ_PTR fmkr, int *ierr) {
   OBJ_PTR v = Obj_Attr_Get(fmkr, tex_preview_figure_height_ID, ierr);
   if (*ierr != 0) return NULL;
   if (v == OBJ_NIL) return NULL;
   return String_Ptr(v, ierr);
}


char *Get_tex_fontsize(OBJ_PTR fmkr, int *ierr) {
   OBJ_PTR v = Obj_Attr_Get(fmkr, tex_fontsize_ID, ierr);
   if (*ierr != 0) return NULL;
   if (v == OBJ_NIL) return NULL;
   return String_Ptr(v, ierr);
}

char *Get_tex_fontfamily(OBJ_PTR fmkr, int *ierr) {
   OBJ_PTR v = Obj_Attr_Get(fmkr, tex_fontfamily_ID, ierr);
   if (*ierr != 0) return NULL;
   if (v == OBJ_NIL) return NULL;
   return String_Ptr(v, ierr);
}

char *Get_tex_fontseries(OBJ_PTR fmkr, int *ierr) {
   OBJ_PTR v = Obj_Attr_Get(fmkr, tex_fontseries_ID, ierr);
   if (*ierr != 0) return NULL;
   if (v == OBJ_NIL) return NULL;
   return String_Ptr(v, ierr);
}

char *Get_tex_fontshape(OBJ_PTR fmkr, int *ierr) {
   OBJ_PTR v = Obj_Attr_Get(fmkr, tex_fontshape_ID, ierr);
   if (*ierr != 0) return NULL;
   if (v == OBJ_NIL) return NULL;
   return String_Ptr(v, ierr);
}

char *Get_tex_preview_minwhitespace(OBJ_PTR fmkr, int *ierr) {
   OBJ_PTR v = Obj_Attr_Get(fmkr, tex_preview_minwhitespace_ID, ierr);
   if (*ierr != 0) return NULL;
   if (v == OBJ_NIL) return NULL;
   return String_Ptr(v, ierr);
}

bool Get_tex_preview_fullpage(OBJ_PTR fmkr, int *ierr) {
   OBJ_PTR v = Obj_Attr_Get(fmkr, tex_preview_fullpage_ID, ierr);
   if (*ierr != 0) return false;
   return v != OBJ_FALSE && v != OBJ_NIL;
}

/* gets the generated preamble */
char *Get_tex_preview_generated_preamble(OBJ_PTR fmkr, int *ierr) {
   /* it is a class constant... */
   OBJ_PTR v = TEX_PREAMBLE(fmkr, ierr);
   if (*ierr != 0) return NULL;
   if (v == OBJ_NIL) return NULL;
   return CString_Ptr(v, ierr);
}

double Get_tex_xoffset(OBJ_PTR fmkr, int *ierr) { return Get_double(fmkr, tex_xoffset_ID, ierr); }
double Get_tex_yoffset(OBJ_PTR fmkr, int *ierr) { return Get_double(fmkr, tex_yoffset_ID, ierr); }


OBJ_PTR Get_fm_data_attr(OBJ_PTR fmkr, int *ierr) { return Obj_Attr_Get(fmkr, fm_data_ID, ierr); }


static char *Get_save_dir(OBJ_PTR fmkr, int *ierr) {
   OBJ_PTR v = Obj_Attr_Get(fmkr, save_dir_ID, ierr);
   if (*ierr != 0) return NULL;
   if (v == OBJ_NIL) return NULL;
   return String_Ptr(v, ierr);
}

char *Get_tex_preview_documentclass(OBJ_PTR fmkr, int *ierr) {
   OBJ_PTR v = Obj_Attr_Get(fmkr, tex_preview_documentclass_ID, ierr);
   if (*ierr != 0) return NULL;
   if (v == OBJ_NIL) return NULL;
   return String_Ptr(v, ierr);
}

char *Get_tex_preamble(OBJ_PTR fmkr, int *ierr) {
   OBJ_PTR v = Obj_Attr_Get(fmkr, tex_preamble_ID, ierr);
   if (*ierr != 0) return NULL;
   if (v == OBJ_NIL) return NULL;
   return String_Ptr(v, ierr);
}

char *Get_xaxis_numeric_label_tex(OBJ_PTR fmkr, int *ierr) {
   OBJ_PTR v = Obj_Attr_Get(fmkr, xaxis_numeric_label_tex_ID, ierr);
   if (*ierr != 0) return NULL;
   if (v == OBJ_NIL) return NULL;
   return String_Ptr(v, ierr);
}

char *Get_yaxis_numeric_label_tex(OBJ_PTR fmkr, int *ierr) {
   OBJ_PTR v = Obj_Attr_Get(fmkr, yaxis_numeric_label_tex_ID, ierr);
   if (*ierr != 0) return NULL;
   if (v == OBJ_NIL) return NULL;
   return String_Ptr(v, ierr);
}

char *Get_tex_preview_pagestyle(OBJ_PTR fmkr, int *ierr) {
   OBJ_PTR v = Obj_Attr_Get(fmkr, tex_preview_pagestyle_ID, ierr);
   if (*ierr != 0) return NULL;
   if (v == OBJ_NIL) return NULL;
   return String_Ptr(v, ierr);
}

char *Get_tex_preview_tiogafigure_command(OBJ_PTR fmkr, int *ierr) {
   OBJ_PTR v = Obj_Attr_Get(fmkr, tex_preview_tiogafigure_command_ID, ierr);
   if (*ierr != 0) return NULL;
   if (v == OBJ_NIL) return NULL;
   return String_Ptr(v, ierr);
}

static bool Get_quiet_mode(OBJ_PTR fmkr, int *ierr) {
   OBJ_PTR v = Obj_Attr_Get(fmkr, quiet_mode_ID, ierr);
   if (*ierr != 0) return false;
   return v != OBJ_FALSE && v != OBJ_NIL;
}


static void Make_Save_Fname(OBJ_PTR fmkr, char *full_name, char *f_name,
   bool with_save_dir, bool with_pdf_extension, int *ierr) {
   int i, j, len;
   char c, *save=NULL;
   if (with_save_dir) { save = Get_save_dir(fmkr,ierr); if (*ierr != 0) return; }
   if (with_save_dir && save != NULL && strlen(save) > 0) { 
      sprintf(full_name, "%s/", save); j = strlen(full_name); }
   else j = 0;
   if (f_name == NULL) f_name = "plot";
   len = strlen(f_name);
   for (i=0; i < len; i++) {
      c = f_name[i];
      full_name[j++] = c;
      }
   full_name[j] = '\0';
   char *dot = strrchr(full_name,'.');
   if (dot == NULL || strcmp(dot+1,"pdf") != 0) { /* add pdf extension */
        full_name[j] = '\0';
        if (!with_pdf_extension) return;
        strcpy(full_name+j, ".pdf");
     }
}
   

OBJ_PTR c_get_save_filename(OBJ_PTR fmkr, FM *p, OBJ_PTR name, int *ierr) {
   char full_name[STRLEN];
   char *fname = (name == OBJ_NIL)? NULL : String_Ptr(name, ierr);
   if (*ierr != 0) RETURN_NIL;
   Make_Save_Fname(fmkr, full_name, fname, false, false, ierr);
   if (*ierr != 0) RETURN_NIL;
   return String_From_Cstring(full_name);
}
   
   
void c_private_make(OBJ_PTR fmkr, FM *p, OBJ_PTR name, OBJ_PTR cmd, int *ierr) {
   char full_name[STRLEN];
   bool quiet = Get_quiet_mode(fmkr, ierr);
   if (*ierr != 0) return;
   if (!Get_initialized()) {
      Init_pdf(ierr); if (*ierr != 0) return;
      Init_tex(ierr); if (*ierr != 0) return;
      Set_initialized();
   }
   char *fn = (name == OBJ_NIL)? NULL : String_Ptr(name,ierr);
   if (*ierr != 0) return;
   Make_Save_Fname(fmkr, full_name, fn, true, true, ierr);
   if (*ierr != 0) return;
   Open_pdf(fmkr, p, full_name, quiet, ierr);
   if (*ierr != 0) return;
   Open_tex(fmkr, full_name, quiet, ierr);
   if (*ierr != 0) return;
   Write_gsave();
   p->root_figure = true;
   p->in_subplot = false;
   Call_Function(fmkr, make_page_ID, cmd, ierr);
   if (*ierr != 0) return;
   Write_grestore();
   Close_pdf(fmkr, p, quiet, ierr);
   if (*ierr != 0) return;
   Close_tex(fmkr, quiet, ierr);
   if (*ierr != 0) return;
   Create_wrapper(fmkr, full_name, quiet, ierr);
}
   
      
OBJ_PTR c_private_make_portfolio(OBJ_PTR fmkr, FM *p, OBJ_PTR name, OBJ_PTR fignums, OBJ_PTR fignames, int *ierr) {
   char full_name[STRLEN];
   char *fn = (name == OBJ_NIL)? NULL : String_Ptr(name,ierr);
   if (*ierr != 0) RETURN_NIL;
   Make_Save_Fname(fmkr, full_name, fn, true, false, ierr);
   if (*ierr != 0) RETURN_NIL;
   private_make_portfolio(full_name, fignums, fignames, ierr);
   if (*ierr != 0) RETURN_NIL;
   return String_From_Cstring(full_name);
}


void c_set_device_pagesize(OBJ_PTR fmkr, FM *p, double width, double height, int *ierr) { 
   // sizes in units of 1/720 inch
   p->page_left = 0;
   p->page_right = width;
   p->page_bottom = 0;
   p->page_top = height;
   p->page_width = p->page_right - p->page_left;
   p->page_height = p->page_top - p->page_bottom;
   p->clip_left = p->page_left;
   p->clip_right = p->page_right;
   p->clip_top = p->page_top;
   p->clip_bottom = p->page_bottom;
}


void c_set_frame_sides(OBJ_PTR fmkr, FM *p, double left, double right, double top, double bottom, int *ierr) { // sizes in page coords [0..1]
   if (left > 1.0 || left < 0.0) RAISE_ERROR("Sorry: OBJ_PTR of left must be between 0 and 1 for set_frame_sides", ierr);
   if (right > 1.0 || right < 0.0) RAISE_ERROR("Sorry: OBJ_PTR of right must be between 0 and 1 for set_frame_sides", ierr);
   if (top > 1.0 || top < 0.0) RAISE_ERROR("Sorry: OBJ_PTR of top must be between 0 and 1 for set_frame_sides", ierr);
   if (bottom > 1.0 || bottom < 0.0) RAISE_ERROR("Sorry: OBJ_PTR of bottom must be between 0 and 1 for set_frame_sides", ierr);
   if (left >= right) RAISE_ERROR("Sorry: OBJ_PTR of left must be smaller than OBJ_PTR of right for set_frame_sides", ierr);
   if (bottom >= top) RAISE_ERROR("Sorry: OBJ_PTR of bottom must be smaller than OBJ_PTR of top for set_frame_sides", ierr);
   if (*ierr != 0) return;
   p->frame_left = left;
   p->frame_right = right;
   p->frame_bottom = bottom;
   p->frame_top = top;
   p->frame_width = right - left;
   p->frame_height = top - bottom;
}


void c_private_init_fm_data(OBJ_PTR fmkr, FM *p, double scale, int *ierr) {
   /* Page */
   p->scaling_factor = scale;
   p->root_figure = true;
   p->in_subplot = false;
   c_private_set_default_font_size(fmkr, p, 12.0, ierr);
   if (*ierr != 0) return;
   c_set_device_pagesize(fmkr, p, 5 * 72.0 * ENLARGE, 5 * 72.0 * ENLARGE, ierr);
   if (*ierr != 0) return;
   /* default frame */
   c_set_frame_sides(fmkr, p, 0.15, 0.85, 0.85, 0.15, ierr);
   if (*ierr != 0) return;
   /* default bounds */
   p->bounds_left = p->bounds_bottom = p->bounds_xmin = p->bounds_ymin = 0;
   p->bounds_right = p->bounds_top = p->bounds_xmax = p->bounds_ymax = 1;
   p->bounds_width = p->bounds_right - p->bounds_left;
   p->bounds_height = p->bounds_top - p->bounds_bottom;
   /* text attributes */
   p->justification = CENTERED;
   p->alignment = ALIGNED_AT_BASELINE;
   p->label_left_margin = 0; // as fraction of frame width
   p->label_right_margin = 0; // as fraction of frame width
   p->label_top_margin = 0; // as fraction of frame height
   p->label_bottom_margin = 0; // as fraction of frame height
   p->text_shift_on_left = 1.8;
   p->text_shift_on_right = 2.5;
   p->text_shift_on_top = 0.7;
   p->text_shift_on_bottom = 2.0;
   p->text_shift_from_x_origin = 1.8;
   p->text_shift_from_y_origin = 2.0;
   p->default_text_scale = 1.0;  Recalc_Font_Hts(p);
   /* graphics attributes */
   p->stroke_color_R = 0.0;
   p->stroke_color_G = 0.0;
   p->stroke_color_B = 0.0;
   p->fill_color_R = 0.0;
   p->fill_color_G = 0.0;
   p->fill_color_B = 0.0;
   p->default_line_scale = 1.0;
   p->line_width = 1.2;
   p->line_cap = LINE_CAP_ROUND;
   p->line_join = LINE_JOIN_ROUND;
   p->miter_limit = 2.0;
   
   p->stroke_opacity = 1.0;
   p->fill_opacity = 1.0;
   
   /* Title */
   p->title_visible = true;
   p->title_side = TOP;
   p->title_position = 0.5;
   p->title_scale = 1.1;
   p->title_shift = 0.7; // in char heights, positive for out from edge (or toward larger x or y OBJ_PTR)
   p->title_angle = 0.0;
   p->title_alignment = ALIGNED_AT_BASELINE;
   p->title_justification = CENTERED;
   p->title_color_R = 0.0;
   p->title_color_G = 0.0;
   p->title_color_B = 0.0;
   
   /* X label */
   p->xlabel_visible = true;
   p->xlabel_side = BOTTOM;
   p->xlabel_position = 0.5;
   p->xlabel_scale = 1.0;
   p->xlabel_shift = 2.0; // in char heights, positive for out from edge (or toward larger x or y OBJ_PTR)
   p->xlabel_angle = 0.0;
   p->xlabel_alignment = ALIGNED_AT_BASELINE;
   p->xlabel_justification = CENTERED;
   p->xlabel_color_R = 0.0;
   p->xlabel_color_G = 0.0;
   p->xlabel_color_B = 0.0;
   
   /* Y label */
   p->ylabel_visible = true;
   p->ylabel_side = LEFT;
   p->ylabel_position = 0.5;
   p->ylabel_scale = 1.0;
   p->ylabel_shift = 1.8; // in char heights, positive for out from edge (or toward larger x or y OBJ_PTR)
   p->ylabel_angle = 0.0;
   p->ylabel_alignment = ALIGNED_AT_BASELINE;
   p->ylabel_justification = CENTERED;
   p->ylabel_color_R = 0.0;
   p->ylabel_color_G = 0.0;
   p->ylabel_color_B = 0.0;
   
   /* X axis */
   p->xaxis_visible = true;
   p->xaxis_type = AXIS_WITH_TICKS_AND_NUMERIC_LABELS;
   p->xaxis_loc = BOTTOM;
   // line
   p->xaxis_line_width = 1.0; // for axis line

   p->xaxis_stroke_color_R = 0.0; // for axis line and tick marks
   p->xaxis_stroke_color_G = 0.0;
   p->xaxis_stroke_color_B = 0.0;

   p->xaxis_labels_color_R = 0.0; // for axis line and tick marks
   p->xaxis_labels_color_G = 0.0;
   p->xaxis_labels_color_B = 0.0;
   // tick marks
   p->xaxis_major_tick_width = 0.9; // same units as line_width
   p->xaxis_minor_tick_width = 0.7; // same units as line_width
   p->xaxis_major_tick_length = 0.6; // in units of numeric label char heights
   p->xaxis_minor_tick_length = 0.3; // in units of numeric label char heights
   p->xaxis_log_values = false;
   p->xaxis_ticks_inside = true; // inside frame or toward larger x or y OBJ_PTR for specific location
   p->xaxis_ticks_outside = false; // inside frame or toward smaller x or y OBJ_PTR for specific location
   p->xaxis_tick_interval = 0.0; // set to 0 to use default
   p->xaxis_min_between_major_ticks = 2; // in units of numeric label char heights
   p->xaxis_number_of_minor_intervals = 0; // set to 0 to use default
   // numeric labels on major ticks
   p->xaxis_use_fixed_pt = false;
   p->xaxis_digits_max = 0;
   p->xaxis_numeric_label_decimal_digits = -1; // set to negative to use default
   p->xaxis_numeric_label_scale = 0.7;
   p->xaxis_numeric_label_shift = 0.3; // in char heights, positive for out from edge (or toward larger x or y OBJ_PTR)
   p->xaxis_numeric_label_angle = 0.0;
   p->xaxis_numeric_label_alignment = ALIGNED_AT_MIDHEIGHT;
   p->xaxis_numeric_label_justification = CENTERED;
   p->xaxis_numeric_label_frequency = 1;
   p->xaxis_numeric_label_phase = 0;
   p->top_edge_type = EDGE_WITH_TICKS;
   p->top_edge_visible = true;
   p->bottom_edge_type = EDGE_WITH_TICKS;
   p->bottom_edge_visible = true;
   
   /* Y axis */
   p->yaxis_visible = true;
   p->yaxis_type = AXIS_WITH_TICKS_AND_NUMERIC_LABELS;
   p->yaxis_loc = LEFT;
   // line
   p->yaxis_line_width = 1.0; // for axis line
   p->yaxis_stroke_color_R = 0.0; // for axis line and tick marks
   p->yaxis_stroke_color_G = 0.0;
   p->yaxis_stroke_color_B = 0.0;

   p->yaxis_labels_color_R = 0.0; // for axis line and tick marks
   p->yaxis_labels_color_G = 0.0;
   p->yaxis_labels_color_B = 0.0;
   // tick marks
   p->yaxis_major_tick_width = 0.9; // same units as line_width
   p->yaxis_minor_tick_width = 0.7; // same units as line_width
   p->yaxis_major_tick_length = 0.6; // in units of numeric label char heights
   p->yaxis_minor_tick_length = 0.3; // in units of numeric label char heights
   p->yaxis_log_values = false;
   p->yaxis_ticks_inside = true; // inside frame or toward larger x or y OBJ_PTR for specific location
   p->yaxis_ticks_outside = false; // inside frame or toward smaller x or y OBJ_PTR for specific location
   p->yaxis_tick_interval = 0.0; // set to 0 to use default
   p->yaxis_min_between_major_ticks = 2; // in units of numeric label char heights
   p->yaxis_number_of_minor_intervals = 0; // set to 0 to use default
   // numeric labels on major ticks
   p->yaxis_use_fixed_pt = false;
   p->yaxis_digits_max = 0;
   p->yaxis_numeric_label_decimal_digits = -1; // set to negative to use default
   p->yaxis_numeric_label_scale = 0.7;
   p->yaxis_numeric_label_shift = 0.5; // in char heights, positive for out from edge (or toward larger x or y OBJ_PTR)
   p->yaxis_numeric_label_angle = 0.0;
   p->yaxis_numeric_label_alignment = ALIGNED_AT_MIDHEIGHT;
   p->yaxis_numeric_label_justification = CENTERED;
   p->yaxis_numeric_label_frequency = 1;
   p->yaxis_numeric_label_phase = 0;
   p->left_edge_type = EDGE_WITH_TICKS;
   p->left_edge_visible = true;
   p->right_edge_type = EDGE_WITH_TICKS;
   p->right_edge_visible = true;

   /* Legend */
   p->legend_line_x0 = 0.5;
   p->legend_line_x1 = 2.0;
   p->legend_line_dy = 0.25;
   p->legend_text_width = -1;
   p->legend_text_xstart = 2.8;
   p->legend_text_ystart = 2.0;
   p->legend_text_dy = 1.9;
   p->legend_line_width = -1;
   p->legend_scale = 0.6;
   p->legend_alignment = ALIGNED_AT_BASELINE;
   p->legend_justification = LEFT_JUSTIFIED;
   p->debug_verbosity_level = 0;
   /* emit a warning by default */
   p->croak_on_nonok_numbers = 1;
}

OBJ_PTR Get_line_type(OBJ_PTR fmkr, int *ierr) { 
   OBJ_PTR v = Obj_Attr_Get(fmkr, line_type_ID, ierr);
   if (*ierr != 0) RETURN_NIL;
   return v;
}

void Set_line_type(OBJ_PTR fmkr, OBJ_PTR v, int *ierr) { 
   Obj_Attr_Set(fmkr, line_type_ID, v, ierr);
}

OBJ_PTR Get_xaxis_locations_for_major_ticks(OBJ_PTR fmkr, int *ierr) { 
   OBJ_PTR v = Obj_Attr_Get(fmkr, xaxis_locations_for_major_ticks_ID, ierr);
   if (*ierr != 0) RETURN_NIL;
   return v;
}

OBJ_PTR Get_xaxis_locations_for_minor_ticks(OBJ_PTR fmkr, int *ierr) { 
   OBJ_PTR v = Obj_Attr_Get(fmkr, xaxis_locations_for_minor_ticks_ID, ierr);
   if (*ierr != 0) RETURN_NIL;
   return v;
}

OBJ_PTR Get_xaxis_tick_labels(OBJ_PTR fmkr, int *ierr) { 
   OBJ_PTR v = Obj_Attr_Get(fmkr, xaxis_tick_labels_ID, ierr);
   if (*ierr != 0) RETURN_NIL;
   return v;
}

OBJ_PTR Get_yaxis_locations_for_major_ticks(OBJ_PTR fmkr, int *ierr) { 
   OBJ_PTR v = Obj_Attr_Get(fmkr, yaxis_locations_for_major_ticks_ID, ierr);
   if (*ierr != 0) RETURN_NIL;
   return v;
}
OBJ_PTR Get_yaxis_locations_for_minor_ticks(OBJ_PTR fmkr, int *ierr) { 
   OBJ_PTR v = Obj_Attr_Get(fmkr, yaxis_locations_for_minor_ticks_ID, ierr);
   if (*ierr != 0) RETURN_NIL;
   return v;
}
OBJ_PTR Get_yaxis_tick_labels(OBJ_PTR fmkr, int *ierr) { 
   OBJ_PTR v = Obj_Attr_Get(fmkr, yaxis_tick_labels_ID, ierr);
   if (*ierr != 0) RETURN_NIL;
   return v;
}


