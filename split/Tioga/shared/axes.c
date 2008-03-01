/* axes.c */
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
#include "figures.h"
#include "pdfs.h"

typedef struct {
   int type;
   int other_axis_type;
   double line_width;
   double stroke_color_R;
   double stroke_color_G;
   double stroke_color_B;
   double major_tick_width; // same units as line_width
   double minor_tick_width; // same units as line_width
   double major_tick_length; // in units of numeric label char heights
   double minor_tick_length; // in units of numeric label char heights
   bool log_vals;
   bool ticks_inside; // inside frame or toward larger x or y value for specific location
   bool ticks_outside; // inside frame or toward smaller x or y value for specific location
   double tick_interval; // set to 0 to use default
   double min_between_major_ticks; // in units of numeric label char heights
   int number_of_minor_intervals; // set to 0 to use default
   OBJ_PTR locations_for_major_ticks; // set to nil to use defaults
   OBJ_PTR locations_for_minor_ticks; // set to nil to use defaults
   bool use_fixed_pt;
   int digits_max;
   OBJ_PTR tick_labels; // set to nil to use defaults
   double numeric_label_scale;
   double numeric_label_shift; // in char heights, positive for out from edge (or toward larger x or y value)
   double numeric_label_angle;
   int numeric_label_alignment;
   int numeric_label_justification;
   int numeric_label_frequency;
   int numeric_label_phase;
   int numeric_label_decimal_digits; // set to 0 to use default
   double *majors; /* if not NULL, then points to array of places where major ticks should appear (in figure coords) */
   int nmajors; /* if majors != NULL, then this tells how many entries it contains */
   bool free_majors;
   char **labels; /* if not null, then points to array of strings to use for labels at major ticks */
   /* labels[i] goes with majors[i], so this is only valid if majors is not null */
   bool free_strings_for_labels;
   double x0, y0, x1, y1, length, axis_min, axis_max; // in figure coords
   bool vertical;
   bool reversed;
   bool top_or_right;
   bool other_axis_reversed;
   int num_minors; // number of minor intervals
   double interval; // tick interval
   int location;
} PlotAxis;

static void figure_moveto(OBJ_PTR fmkr, FM *p, double x, double y, int *ierr) // figure coords
{
   c_moveto(fmkr, p, convert_figure_to_output_x(p,x), convert_figure_to_output_y(p,y), ierr);
}

static void figure_lineto(OBJ_PTR fmkr, FM *p, double x, double y, int *ierr) // figure coords
{
   c_lineto(fmkr, p, convert_figure_to_output_x(p,x), convert_figure_to_output_y(p,y), ierr);
}

static void figure_join(OBJ_PTR fmkr, FM *p, 
   double x0, double y0, double x1, double y1, int *ierr) // figure coords
{
   figure_moveto(fmkr, p, x0, y0, ierr);
   figure_lineto(fmkr, p, x1, y1, ierr);
}

static void axis_stroke(OBJ_PTR fmkr, FM *p, int *ierr)
{
   c_stroke(fmkr,p, ierr);
}

static void figure_join_and_stroke(OBJ_PTR fmkr, FM *p, 
   double x0, double y0, double x1, double y1, int *ierr) // figure coords
{
   figure_join(fmkr, p, x0, y0, x1, y1, ierr);
   c_stroke(fmkr,p, ierr);
}

static void Get_xaxis_Specs(OBJ_PTR fmkr, FM *p, PlotAxis *s, int *ierr)
{
   s->free_majors = s->free_strings_for_labels = false;
   s->stroke_color_R = 0.0; s->stroke_color_G = 0.0; s->stroke_color_B = 0.0; 
   s->majors = NULL;
   s->labels = NULL;
   s->type = p->xaxis_type;
   s->other_axis_type = p->yaxis_type;
   s->line_width = p->xaxis_line_width; // for axis line
   s->stroke_color_R = p->xaxis_stroke_color_R; // for axis line and tick marks
   s->stroke_color_G = p->xaxis_stroke_color_G;
   s->stroke_color_B = p->xaxis_stroke_color_B;
   s->major_tick_width = p->xaxis_major_tick_width; // same units as line_width
   s->minor_tick_width = p->xaxis_minor_tick_width; // same units as line_width
   s->major_tick_length = p->xaxis_major_tick_length; // in units of numeric label char heights
   s->minor_tick_length = p->xaxis_minor_tick_length; // in units of numeric label char heights
   s->log_vals = p->xaxis_log_values;
   s->ticks_inside = p->xaxis_ticks_inside; // inside frame or toward larger x or y value for specific location
   s->ticks_outside = p->xaxis_ticks_outside; // inside frame or toward smaller x or y value for specific location
   s->tick_interval = p->xaxis_tick_interval; // set to 0 to use default
   s->min_between_major_ticks = p->xaxis_min_between_major_ticks; // in units of numeric label char heights
   s->number_of_minor_intervals = p->xaxis_number_of_minor_intervals; // set to 0 to use default   
   s->locations_for_major_ticks = Get_xaxis_locations_for_major_ticks(fmkr, ierr);
   s->locations_for_minor_ticks = Get_xaxis_locations_for_minor_ticks(fmkr, ierr);
   s->use_fixed_pt = p->xaxis_use_fixed_pt;
   s->digits_max = p->xaxis_digits_max;
   s->tick_labels = Get_xaxis_tick_labels(fmkr, ierr);
   s->numeric_label_decimal_digits = p->xaxis_numeric_label_decimal_digits; // set to negative to use default
   s->numeric_label_scale = p->xaxis_numeric_label_scale;
   s->numeric_label_shift = p->xaxis_numeric_label_shift; // in char heights, positive for out from edge (or toward larger x or y value)
   s->numeric_label_angle = p->xaxis_numeric_label_angle;
   s->numeric_label_alignment = p->xaxis_numeric_label_alignment;
   s->numeric_label_justification = p->xaxis_numeric_label_justification;
   s->numeric_label_frequency = p->xaxis_numeric_label_frequency;
   s->numeric_label_phase = p->xaxis_numeric_label_phase;
}

static void Get_yaxis_Specs(OBJ_PTR fmkr, FM *p, PlotAxis *s, int *ierr)
{
   s->free_majors = s->free_strings_for_labels = false;
   s->stroke_color_R = 0.0; s->stroke_color_G = 0.0; s->stroke_color_B = 0.0; 
   s->majors = NULL;
   s->labels = NULL;
   s->type = p->yaxis_type;
   s->other_axis_type = p->xaxis_type;
   s->line_width = p->yaxis_line_width; // for axis line
   s->stroke_color_R = p->yaxis_stroke_color_R; // for axis line and tick marks
   s->stroke_color_G = p->yaxis_stroke_color_G;
   s->stroke_color_B = p->yaxis_stroke_color_B;
   s->major_tick_width = p->yaxis_major_tick_width; // same units as line_width
   s->minor_tick_width = p->yaxis_minor_tick_width; // same units as line_width
   s->major_tick_length = p->yaxis_major_tick_length; // in units of numeric label char heights
   s->minor_tick_length = p->yaxis_minor_tick_length; // in units of numeric label char heights
   s->log_vals = p->yaxis_log_values;
   s->ticks_inside = p->yaxis_ticks_inside; // inside frame or toward larger x or y value for specific location
   s->ticks_outside = p->yaxis_ticks_outside; // inside frame or toward smaller x or y value for specific location
   s->tick_interval = p->yaxis_tick_interval; // set to 0 to use default
   s->min_between_major_ticks = p->yaxis_min_between_major_ticks; // in units of numeric label char heights
   s->number_of_minor_intervals = p->yaxis_number_of_minor_intervals; // set to 0 to use default   
   s->locations_for_major_ticks = Get_yaxis_locations_for_major_ticks(fmkr, ierr);
   s->locations_for_minor_ticks = Get_yaxis_locations_for_minor_ticks(fmkr, ierr);
   s->tick_labels = Get_yaxis_tick_labels(fmkr, ierr);   
   s->use_fixed_pt = p->yaxis_use_fixed_pt;
   s->digits_max = p->yaxis_digits_max;
   s->numeric_label_decimal_digits = p->yaxis_numeric_label_decimal_digits; // set to negative to use default
   s->numeric_label_scale = p->yaxis_numeric_label_scale;
   s->numeric_label_shift = p->yaxis_numeric_label_shift; // in char heights, positive for out from edge (or toward larger x or y value)
   s->numeric_label_angle = p->yaxis_numeric_label_angle;
   s->numeric_label_alignment = p->yaxis_numeric_label_alignment;
   s->numeric_label_justification = p->yaxis_numeric_label_justification;
   s->numeric_label_frequency = p->yaxis_numeric_label_frequency;
   s->numeric_label_phase = p->yaxis_numeric_label_phase;
}

/*======================================================================*/

static void draw_axis_line(OBJ_PTR fmkr, FM *p, int location, PlotAxis *s, int *ierr)
{
   switch (location) {
      case LEFT:
         s->x0 = p->bounds_left;
         s->x1 = p->bounds_left;
         s->y0 = p->bounds_bottom;
         s->y1 = p->bounds_top;
         s->length = p->bounds_height;
         s->axis_min = p->bounds_ymin;
         s->axis_max = p->bounds_ymax;
         s->vertical = true;
         s->reversed = p->yaxis_reversed;
         s->other_axis_reversed = p->xaxis_reversed;
         s->top_or_right = false;
         break;
      case RIGHT:
         s->x0 = p->bounds_right;
         s->x1 = p->bounds_right;
         s->y0 = p->bounds_bottom;
         s->y1 = p->bounds_top;
         s->length = p->bounds_height;
         s->axis_min = p->bounds_ymin;
         s->axis_max = p->bounds_ymax;
         s->vertical = true;
         s->reversed = p->yaxis_reversed;
         s->other_axis_reversed = p->xaxis_reversed;
         s->top_or_right = true;
         break;
      case TOP:
         s->x0 = p->bounds_left;
         s->x1 = p->bounds_right;
         s->y0 = p->bounds_top;
         s->y1 = p->bounds_top;
         s->length = p->bounds_width;
         s->axis_min = p->bounds_xmin;
         s->axis_max = p->bounds_xmax;
         s->vertical = false;
         s->reversed = p->xaxis_reversed;
         s->other_axis_reversed = p->yaxis_reversed;
         s->top_or_right = true;
         break;
      case BOTTOM:
         s->x0 = p->bounds_left;
         s->x1 = p->bounds_right;
         s->y0 = p->bounds_bottom;
         s->y1 = p->bounds_bottom;
         s->length = p->bounds_width;
         s->axis_min = p->bounds_xmin;
         s->axis_max = p->bounds_xmax;
         s->vertical = false;
         s->reversed = p->xaxis_reversed;
         s->other_axis_reversed = p->yaxis_reversed;
         s->top_or_right = false;
         break;
      case AT_X_ORIGIN:
         s->x0 = 0.0;
         s->x1 = 0.0;
         s->y0 = p->bounds_bottom;
         s->y1 = p->bounds_top;
         s->length = p->bounds_height;
         s->axis_min = p->bounds_ymin;
         s->axis_max = p->bounds_ymax;
         s->vertical = true;
         s->reversed = p->yaxis_reversed;
         s->other_axis_reversed = p->xaxis_reversed;
         s->top_or_right = false;
         break;
      case AT_Y_ORIGIN:
         s->x0 = p->bounds_left;
         s->x1 = p->bounds_right;
         s->y0 = 0.0;
         s->y1 = 0.0;
         s->length = p->bounds_width;
         s->axis_min = p->bounds_xmin;
         s->axis_max = p->bounds_xmax;
         s->vertical = false;
         s->reversed = p->xaxis_reversed;
         s->other_axis_reversed = p->yaxis_reversed;
         s->top_or_right = false;
         break;
   }
   c_line_width_set(fmkr, p, s->line_width, ierr);
   figure_join_and_stroke(fmkr, p, s->x0, s->y0, s->x1, s->y1, ierr);
}

static char *Create_Label(double val, int scale, int prec, 
   bool log_vals, bool use_fixed_pt, char *postfix, PlotAxis *s, int *ierr)
{ // val in figure coords
   char buff[100], form[100], *string;
   int exponent = ROUND(val);
   buff[0] = 0;
   if (log_vals && use_fixed_pt) {   /* Logarithmic */
     double sav_val = val, pow_val;
     val = pow(10.0, exponent);
     pow_val = pow(10.0, sav_val);
     if (exponent < 0) {
        int numdig = ABS(exponent)+1; 
        snprintf(form, sizeof(form), (s->vertical)? "\\tiogayaxisnumericlabel{%%.%df}" : "\\tiogaxaxisnumericlabel{%%.%df}", numdig);
        snprintf(buff, sizeof(buff), form, pow_val);
     } else if (abs(val - pow_val) > 0.1) {
        snprintf(buff, sizeof(buff), (s->vertical)? "\\tiogayaxisnumericlabel{%0.2f}" : "\\tiogaxaxisnumericlabel{%0.2f}", pow_val);
     } else {
        snprintf(buff, sizeof(buff), (s->vertical)? "\\tiogayaxisnumericlabel{%d}" : "\\tiogaxaxisnumericlabel{%d}", (int) val);
     }
   } else if (log_vals) {
     /* Exponential, i.e. 10^-1, 1, 10, 10^2, etc */
     double abs_diff = fabs(val - exponent);
     if (abs_diff > 0.1) snprintf(buff, sizeof(buff), (s->vertical)? "\\tiogayaxisnumericlabel{10^{%0.1f}}" : "\\tiogaxaxisnumericlabel{10^{%0.1f}}", val);
     else if (exponent == 0) strcpy(buff, "1");
     else if (exponent == 1) strcpy(buff, "10");
     else snprintf(buff, sizeof(buff),  (s->vertical)? "\\tiogayaxisnumericlabel{10^{%d}}" : "\\tiogaxaxisnumericlabel{10^{%d}}", exponent);
   } else {   /* Linear */
      double scale2;
      int precis = s->numeric_label_decimal_digits; //  use this precision if it is >= 0
      if (precis >= 0) prec = precis;
      if (scale) val /= pow(10.,(double)scale);
      /* This is necessary to prevent labels like "-0.0" on some systems */
      scale2 = pow(10., prec);
      val = floor((val * scale2) + .5) / scale2;
      snprintf(form, sizeof(form), (s->vertical)?  "\\tiogayaxisnumericlabel{%%.%df}" : "\\tiogaxaxisnumericlabel{%%.%df}", (int) prec);
      snprintf(buff, sizeof(buff), form, val);
   }
   int len = strlen(buff);
   if (postfix != NULL) { strcpy(buff+len, postfix); len = strlen(buff); }
   string = ALLOC_N_char(len+1);
   strcpy(string, buff);
   return string;
}

char *Get_String(OBJ_PTR ary, int index, int *ierr) {
   OBJ_PTR s = Array_Entry(ary,index,ierr);
   if (*ierr != 0) return NULL;
   return String_Ptr(s,ierr);
}

#define MAX_FIXDIG_POS  6
#define MAX_FIXDIG_NEG  6
#define DIGMAX_DEF   6

static void Pick_Label_Precision(double vmin, double vmax, double tick, bool use_fixed_pt,
   int *mode, int *prec, int digmax, int *scale, int *ierr) {
   double chosen, notchosen, vmod, t0, shifted;
   int msd, notmsd, np, digmin, digfix;
   *mode = *scale = 0;
   if (digmax == 0) digmax = DIGMAX_DEF;
   /* Choose vmin or vmax depending on magnitudes of vmin and vmax. */
   if (ABS(vmax) >= ABS(vmin)) { chosen = vmax; notchosen = vmin; }
   else { chosen = vmin; notchosen = vmax; }
   /* Magnitute of chosen to get number of significant digits */
   
   if(ABS(chosen) > 0.) {
      vmod = ABS(chosen);
      t0 = (double) log10(vmod);
      msd = (int) floor(t0);
   }
   else {
      /* this branch occurs only when 0. --- 0. range put in */
      vmod = 1.;
      t0 = (double) log10(vmod);
      msd = (int) floor(t0);
   }
   if (ABS(notchosen) > 0.) notmsd = (int) floor( (double) log10(ABS(notchosen)));
   else notmsd = msd;
   /* Autoselect the mode flag */
   /* 'digmin' is the minimum number of places taken up by the label */
   
   if (msd >= 0) {
      /* n.b. no decimal point in the minimal case  */
      digmin = msd + 1;
      digfix = MAX_FIXDIG_POS;
      if (digmax > 0) digfix = MIN(digmax, MAX_FIXDIG_POS);
   }
   else {
      /* adjust digmin to account for leading 0 and decimal point */
      digmin = -msd + 2;
      digfix = MAX_FIXDIG_NEG;
      if (digmax > 0) digfix = MIN(digmax, MAX_FIXDIG_NEG);
   }
   /* adjust digmin to account for sign on the chosen end of axis or sign on the
   * nonchosen end of axis if notmsd = msd or (msd <= 0 and notmsd < 0)
   * For the latter case the notchosen label starts with "-0."
   * For checking for the latter case, the notmsd < 0 condition is redundant
   * since notmsd <= msd always and the equal part is selected by the first
   * condition.
   */
   if(chosen < 0.0 || (notchosen < 0. && (notmsd == msd || msd <= 0))) digmin++;
   
   if (digmin > digfix && !use_fixed_pt) { *mode = 1; *scale = msd; }
   
   /* Establish precision.  */
   /* It must be fine enough to resolve the tick spacing */
   
   shifted = tick; np = 0;
   while (ABS(shifted) < 1.0 && np > -6) { np--; shifted *= 10.0; }
   for (; np > -6; np--, shifted *= 10.0) {
      if (ABS(shifted - floor(shifted)) < 0.1) break;
   }

   if (*mode != 0) *prec = msd - np;
   else *prec = MAX(-np, 0);
   
   /* One last hack required: if exponent < 0, i.e. number has leading '0.',
   * it's better to change to floating point form if the number of digits
   * is insufficient to represent the tick spacing.
   */
   if (*mode == 0 && digmax > 0 && !use_fixed_pt) {
      if (t0 < 0.0) {
         if (digmax - 2 - *prec < 0) { *mode = 1; *scale = msd; }
      } else *prec = MAX(MIN(*prec, digmax - msd - 1), 0);
   }
   if (*mode != 0) {
      *prec = msd - np;
      *prec = *scale - floor(log10(tick));
      *prec = MIN(*prec,MAX(0,*prec));
   }
   
}

static char **Get_Labels(OBJ_PTR fmkr, FM *p, PlotAxis *s, int *ierr)
{
   char **labels = (char **)ALLOC_N_pointer(s->nmajors);
   char postfix[50], *ps;
   int i, k, j;
   k = s->numeric_label_frequency;
   j = s->numeric_label_phase;
   if (s->tick_labels==OBJ_NIL) { // create label strings
      int mode, prec, scale;
      Pick_Label_Precision(s->axis_min, s->axis_max, s->interval, 
         s->use_fixed_pt, &mode, &prec, s->digits_max, &scale, ierr);
      if (*ierr != 0) return NULL;
      int i;
      int upper_right = (s->reversed)? 0 : s->nmajors-1;
      int lower_left = (s->reversed)? s->nmajors-1 : 0;
      for (i = 0; i < s->nmajors; i++) {
         ps = NULL;
         if (i == upper_right && !s->log_vals && mode && scale)
	   snprintf(ps = postfix, sizeof(postfix), 
                (s->vertical)? "$\\times$\\tiogayaxisnumericlabel{10^{%d}}" : "$\\times$\\tiogaxaxisnumericlabel{10^{%d}}", 
                scale);
         if ((i+j) % k != 0) {
            labels[i] = NULL;
         } else if (i == lower_left && s->nmajors >= 2 && s->vertical && 
                (s->majors[i] == ((s->reversed)? s->axis_max : s->axis_min)) &&
                (s->other_axis_type == AXIS_WITH_MAJOR_TICKS_AND_NUMERIC_LABELS ||
                 s->other_axis_type == AXIS_WITH_TICKS_AND_NUMERIC_LABELS)) {
            labels[i] = NULL;
         } else {
            labels[i] = Create_Label(s->majors[i], scale, prec, s->log_vals, s->use_fixed_pt, ps, s, ierr);
            if (*ierr != 0) return NULL;
        }
      }
      s->free_strings_for_labels = true;
   } else { // use the given label strings
      int len = Array_Len(s->tick_labels,ierr);
      if (*ierr != 0) return NULL;
      if (len != s->nmajors) {
         RAISE_ERROR("Sorry: must have same number of labels as major ticks", ierr);
         return NULL;
      }
      for (i = 0; i < s->nmajors; i++) {
         labels[i] = Get_String(s->tick_labels, i, ierr);
         if (*ierr != 0) return NULL;
      }
      s->free_strings_for_labels = false;
   }
   return labels;
}
         
static double *Pick_Locations_for_Major_Ticks(double interval, 
   double axis_min, double axis_max, int *num_locations, int *ierr)
{
   double next_tick, prev_tick, starting_tick;
   double *majors;
   int nmajors, i;
   while (true) {
       nmajors = 0;
       starting_tick = interval * floor(axis_min / interval);
       if (starting_tick == axis_min) starting_tick -= interval;
       prev_tick = starting_tick;
       while (true) {
          next_tick = prev_tick + interval;
          if (next_tick > axis_max) break;
          nmajors++;
          prev_tick = next_tick;
       }
       if (nmajors > 1) break;
       interval *= 0.5;
   }
   majors = ALLOC_N_double(nmajors);
   prev_tick = starting_tick;
   for (i=0; i < nmajors; i++) {
      majors[i] = prev_tick += interval;
   }
   *num_locations = nmajors;
   return majors;
}

static int Pick_Number_of_Minor_Intervals(double length, int *ierr)
{
   double t1, fuzz, newlen;
   int np, num_subintervals, inewlen;
   t1 = (double) log10(length);
   np = (int) floor(t1);
   t1 -= np;
   newlen = pow(10.0, t1);
   inewlen = (int) floor(newlen + 0.5);
   fuzz = newlen - inewlen;
   fuzz = abs(fuzz);
   num_subintervals = 4;
   if (fuzz < 1e-2) {
      switch (inewlen) {
         case 0: case 1: case 2: num_subintervals = 4; break;
         case 4: break;
         case 8: case 10: num_subintervals = 2; break;
         case 3: case 6: case 9: num_subintervals = 3; break;
         default: num_subintervals = 5;
      }
   } else {
      if (newlen > 5.0) num_subintervals = 4;
      else if (newlen > 3.0) num_subintervals = 5;
      else if (newlen > 1.5) num_subintervals = 5;
   }
   return num_subintervals;
}

static void Pick_Major_Tick_Interval(OBJ_PTR fmkr, FM *p, 
      double tick_min, double tick_gap, double length, bool log_vals, double *tick, int *ierr)
{
   double t1, t2, tick_reasonable, base_interval;
   int np, i;
   /* Magnitude of min/max difference to get tick spacing */
   t1 = (double) log10(length);
   np = (int) floor(t1);
   t1 -= np;
   t1 = pow(10.0, t1);
   /* Get tick spacing. */
   if (t1 > 6.0) t2 = 2.0;
   else if (t1 > 3.0) t2 = 1.0;
   else if (t1 > 1.5) { t2 = 5.0; np--; }
   else { t2 = 2.0; np--; }
   /* Now compute reasonable tick spacing */
   if (log_vals) {
    tick_reasonable = 1.0; 
   } else {
       base_interval = pow(10.0, (double) np);
       if (t2 == 2.0 && t2 * base_interval < tick_gap) t2 = 1.0;
       i = 0;
       while (i < 100) {
          tick_reasonable = t2 * base_interval;
          if (tick_reasonable >= tick_min) break;
          t2++; i++;
       }
   }
   if (*tick == 0) *tick = tick_reasonable;
   else { // check the given interval compared to the default
      *tick = ABS(*tick);
      if(*tick < 1.e-4*tick_reasonable) {
         RAISE_ERROR_g("Sorry: magnitude of specified tick interval (%g) is too small", *tick, ierr);
         return;
      }
   }
}

static void draw_major_ticks(OBJ_PTR fmkr, FM *p, PlotAxis *s, int *ierr)
{
   s->num_minors = s->number_of_minor_intervals; 
   if (s->locations_for_major_ticks != OBJ_NIL) {
      long len;
      s->majors = Vector_Data_for_Read(s->locations_for_major_ticks, &len, ierr);
      if (*ierr != 0) return;
      s->nmajors = len;
      if (len > 1) {
        s->interval = s->majors[1] - s->majors[0];
        if (s->interval < 0.0) { s->interval = -s->interval; }
      }
   } else { // calculate major tick locations
      s->interval = s->tick_interval;
      double height = ((s->vertical)? p->default_text_height_dy : p->default_text_height_dx);
      double tick_min = s->min_between_major_ticks * height;
      double tick_gap = 10.0 * height;
      Pick_Major_Tick_Interval(fmkr, p, tick_min, tick_gap, s->length, s->log_vals, &s->interval, ierr);
      if (*ierr != 0) return;
      s->majors = Pick_Locations_for_Major_Ticks(s->interval, s->axis_min, s->axis_max, &s->nmajors, ierr);
      if (*ierr != 0) return;
      s->free_majors = true;
   }
   int i;
   double inside=0.0, outside=0.0, length;
   bool did_line = false;
   length = s->major_tick_length * ((s->vertical)? p->default_text_height_dx : p->default_text_height_dy);
   if (s->ticks_inside) inside = length;
   if (s->ticks_outside) outside = -length;
   if (s->top_or_right) { inside = -inside; outside = -outside; }
   if (s->line_width != s->major_tick_width) {
      c_line_width_set(fmkr, p, s->line_width = s->major_tick_width, ierr);
      if (*ierr != 0) return;
   }
   for (i=0; i < s->nmajors; i++) {
      if (s->vertical)
         figure_join(fmkr, p, s->x0+inside, s->majors[i], s->x0+outside, s->majors[i], ierr);
      else
         figure_join(fmkr, p, s->majors[i], s->y0+inside, s->majors[i], s->y0+outside, ierr);
      did_line = true;
      if (*ierr != 0) return;
   }
   if (did_line) axis_stroke(fmkr,p, ierr);
}

static void draw_minor_ticks(OBJ_PTR fmkr, FM *p, PlotAxis *s, int *ierr)
{
   if (s->number_of_minor_intervals <= 0) {
      if (s->log_vals) {
         double interval = s->majors[1] - s->majors[0];
         s->number_of_minor_intervals = (abs(interval) != 1.0 || s->nmajors > 10)? 1 : 9; 
      }  else {
         s->number_of_minor_intervals = Pick_Number_of_Minor_Intervals(s->interval, ierr);
         if (*ierr != 0) return;
      }
   }
   int i, j, nsub = s->number_of_minor_intervals;
   double inside=0.0, outside=0.0, length;
   bool did_line = false;
   double log_subintervals[8];
   log_subintervals[0] = 0.301030;
   log_subintervals[1] = 0.477121;
   log_subintervals[2] = 0.602060;
   log_subintervals[3] = 0.698970;
   log_subintervals[4] = 0.778151;
   log_subintervals[5] = 0.845098;
   log_subintervals[6] = 0.903090;
   log_subintervals[7] = 0.954243;
   if (s->log_vals && nsub > 9) nsub = 9;
   length = s->minor_tick_length * ((s->vertical)? p->default_text_height_dx : p->default_text_height_dy);
   if (s->ticks_inside) inside = length;
   if (s->ticks_outside) outside = -length;
   if (s->top_or_right) { inside = -inside; outside = -outside; }
   if (s->line_width != s->minor_tick_width) {
      c_line_width_set(fmkr, p, s->line_width = s->minor_tick_width, ierr);
      if (*ierr != 0) return;
   }
   if (s->locations_for_minor_ticks != OBJ_NIL) {
      long cnt;
      double *locs = Vector_Data_for_Read(s->locations_for_minor_ticks, &cnt, ierr);
      if (*ierr != 0) return;
      for (i=0; i < cnt; i++) {
         if (s->vertical)
            figure_join(fmkr, p, s->x0+inside, locs[i], s->x0+outside, locs[i], ierr);
         else
            figure_join(fmkr, p, locs[i], s->y0+inside, locs[i], s->y0+outside, ierr);
         did_line = true;
         if (*ierr != 0) return;
      }
   } else {
      for (i=0; i <= s->nmajors; i++) {
         double loc = (i==0)? s->majors[0] - s->interval : s->majors[i-1];
         double next_loc = (i==s->nmajors)? loc + s->interval : s->majors[i];
         double subinterval = (next_loc - loc) / nsub;
         if (subinterval <= 0.0) continue;
         for (j = 1; j < nsub; j++) {
            double subloc = loc + ((!s->log_vals) ? (j * subinterval) : log_subintervals[j-1]);
            if (subloc >= next_loc) break;
            if (subloc <= s->axis_min || subloc >= s->axis_max) continue;
            if (s->vertical) {
               figure_join(fmkr, p, s->x0+inside, subloc, s->x0+outside, subloc, ierr);
            }
            else {
               figure_join(fmkr, p, subloc, s->y0+inside, subloc, s->y0+outside, ierr);
            }
            did_line = true;
            if (*ierr != 0) return;
         }
      }
   }
   if (did_line) axis_stroke(fmkr,p, ierr);
}

static void show_numeric_label(OBJ_PTR fmkr, FM *p, PlotAxis *s, 
   char *text, int location, double position, double shift, int *ierr)
{ // position is in figure coords and must be converted to frame coords
   double pos = ((!s->reversed)? (position - s->axis_min) : (s->axis_max - position)) / s->length;
   c_show_rotated_text(fmkr, p, text, location, shift, pos, 
		       s->numeric_label_scale, s->numeric_label_angle, s->numeric_label_justification, s->numeric_label_alignment, OBJ_NIL, ierr);
}

static void draw_numeric_labels(OBJ_PTR fmkr, FM *p, int location, PlotAxis *s, int *ierr)
{
   int i;
   double shift = (s->ticks_outside) ? s->major_tick_length : 0.5; // default shift
   shift += s->numeric_label_shift;
   s->labels = Get_Labels(fmkr, p, s, ierr);
   for (i=0; i < s->nmajors; i++) {
      if (s->labels[i] != NULL) {
         show_numeric_label(fmkr, p, s, s->labels[i], location, s->majors[i], shift, ierr);
         if (*ierr != 0) return;
      }
   }
}

static void c_show_side(OBJ_PTR fmkr, FM *p, PlotAxis *s, int *ierr) {
   int i;
   if (s->type == AXIS_HIDDEN) return;
   Start_Axis_Standard_State(fmkr, p, 
      s->stroke_color_R, s->stroke_color_G, s->stroke_color_B, 
      s->line_width * p->default_line_scale, ierr);
   if (*ierr != 0) return;
   draw_axis_line(fmkr, p, s->location, s, ierr);
   if (*ierr != 0) return;
   if (s->type == AXIS_LINE_ONLY) goto done;
   draw_major_ticks(fmkr, p, s, ierr);
   if (*ierr != 0) return;
   if (s->type == AXIS_WITH_MAJOR_TICKS_ONLY) goto done;
   if (s->type == AXIS_WITH_MAJOR_TICKS_AND_NUMERIC_LABELS) {
      draw_numeric_labels(fmkr, p, s->location, s, ierr);
      if (*ierr != 0) return;
      goto done;
   }
   draw_minor_ticks(fmkr, p, s, ierr);
   if (*ierr != 0) return;
   if (s->type == AXIS_WITH_TICKS_ONLY) goto done;
   draw_numeric_labels(fmkr, p, s->location, s, ierr);
   if (*ierr != 0) return;
 done:
   End_Axis_Standard_State(); // grestore
   if (s->free_majors) free(s->majors);
   if (s->labels != NULL) {
      if (s->free_strings_for_labels) {
         for (i = 0; i < s->nmajors; i++)
            if (s->labels[i] != NULL) free(s->labels[i]);
      }
      free(s->labels);
   }
}


void c_show_axis(OBJ_PTR fmkr, FM *p, int location, int *ierr)
{
   PlotAxis axis;
   if (location == LEFT || location == RIGHT || location == AT_X_ORIGIN) {
      if (!p->yaxis_visible) return;
      Get_yaxis_Specs(fmkr, p, &axis, ierr);
   } else if (location == TOP || location == BOTTOM || location == AT_Y_ORIGIN) {
      if (!p->xaxis_visible) return;
      Get_xaxis_Specs(fmkr, p, &axis, ierr);
   } else RAISE_ERROR_i(
         "Sorry: invalid 'loc' for axis: must be one of LEFT, RIGHT, TOP, BOTTOM, AT_X_ORIGIN, or AT_Y_ORIGIN: is (%i)", location, ierr);
   if (*ierr != 0) return;
   axis.location = location;
   c_show_side(fmkr, p, &axis, ierr);
}
      
void c_show_edge(OBJ_PTR fmkr, FM *p, int location, int *ierr)
{
   PlotAxis axis;
   switch (location) {
      case LEFT:
         if (!p->left_edge_visible) return;
         Get_yaxis_Specs(fmkr, p, &axis, ierr); axis.type = p->left_edge_type;
         break;
      case RIGHT:
         if (!p->right_edge_visible) return;
         Get_yaxis_Specs(fmkr, p, &axis, ierr); axis.type = p->right_edge_type;
         break;
      case BOTTOM:
         if (!p->bottom_edge_visible) return;
         Get_xaxis_Specs(fmkr, p, &axis, ierr); axis.type = p->bottom_edge_type;
         break;
      case TOP:
         if (!p->top_edge_visible) return;
         Get_xaxis_Specs(fmkr, p, &axis, ierr); axis.type = p->top_edge_type;
         break;
      default: 
         RAISE_ERROR_i(
            "Sorry: invalid 'loc' for edge: must be one of LEFT, RIGHT, TOP, or BOTTOM: is (%i)", location, ierr);
   }
   if (*ierr != 0) return;
   axis.location = location;
   c_show_side(fmkr, p, &axis, ierr);
}


void c_no_title(OBJ_PTR fmkr, FM *p, int *ierr)
{
   p->title_visible = false;
}

void c_no_xlabel(OBJ_PTR fmkr, FM *p, int *ierr)
{
   p->xlabel_visible = false;
}

void c_no_ylabel(OBJ_PTR fmkr, FM *p, int *ierr)
{
   p->ylabel_visible = false;
}

void c_no_xaxis(OBJ_PTR fmkr, FM *p, int *ierr)
{
   p->xaxis_visible = false;
}

void c_no_yaxis(OBJ_PTR fmkr, FM *p, int *ierr)
{
   p->yaxis_visible = false;
}

void c_no_left_edge(OBJ_PTR fmkr, FM *p, int *ierr)
{
   p->left_edge_visible = false;
}

void c_no_right_edge(OBJ_PTR fmkr, FM *p, int *ierr)
{
   p->right_edge_visible = false;
}

void c_no_top_edge(OBJ_PTR fmkr, FM *p, int *ierr)
{
   p->top_edge_visible = false;
}

void c_no_bottom_edge(OBJ_PTR fmkr, FM *p, int *ierr)
{
   p->bottom_edge_visible = false;
}

