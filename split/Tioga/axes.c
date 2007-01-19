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
   VALUE stroke_color;
   double major_tick_width; // same units as line_width
   double minor_tick_width; // same units as line_width
   double major_tick_length; // in units of numeric label char heights
   double minor_tick_length; // in units of numeric label char heights
   bool log_values;
   bool ticks_inside; // inside frame or toward larger x or y value for specific location
   bool ticks_outside; // inside frame or toward smaller x or y value for specific location
   double tick_interval; // set to 0 to use default
   double min_between_major_ticks; // in units of numeric label char heights
   int number_of_minor_intervals; // set to 0 to use default
   VALUE locations_for_major_ticks; // set to nil to use defaults
   VALUE locations_for_minor_ticks; // set to nil to use defaults
   bool use_fixed_pt;
   int digits_max;
   VALUE tick_labels; // set to nil to use defaults
   double numeric_label_scale;
   double numeric_label_shift; // in char heights, positive for out from edge (or toward larger x or y value)
   double numeric_label_angle;
   int numeric_label_alignment;
   int numeric_label_justification;
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

static void Init_PlotAxis_struct(PlotAxis *s) {
   s->free_majors = s->free_strings_for_labels = false;
   s->stroke_color = s->locations_for_major_ticks = s->locations_for_minor_ticks = s->tick_labels = Qnil;
   s->majors = NULL;
   s->labels = NULL;
}

static void axis_stroke(FM *p);
void figure_moveto(FM *p, double x, double y) // figure coords
{
   c_moveto(p, convert_figure_to_output_x(p,x), convert_figure_to_output_y(p,y));
}

void figure_lineto(FM *p, double x, double y) // figure coords
{
   c_lineto(p, convert_figure_to_output_x(p,x), convert_figure_to_output_y(p,y));
}

void figure_join(FM *p, double x0, double y0, double x1, double y1) // figure coords
{
   figure_moveto(p, x0, y0);
   figure_lineto(p, x1, y1);
}

static void axis_stroke(FM *p)
{
   FM_stroke(p->fm);
}

void figure_join_and_stroke(FM *p, double x0, double y0, double x1, double y1) // figure coords
{
   figure_join(p, x0, y0, x1, y1);
   FM_stroke(p->fm);
}

static void Get_xaxis_Specs(FM *p, PlotAxis *s)
{
   s->type = p->xaxis_type;
   s->other_axis_type = p->yaxis_type;
   s->line_width = p->xaxis_line_width; // for axis line
   s->stroke_color = p->xaxis_stroke_color; // for axis line and tick marks
   s->major_tick_width = p->xaxis_major_tick_width; // same units as line_width
   s->minor_tick_width = p->xaxis_minor_tick_width; // same units as line_width
   s->major_tick_length = p->xaxis_major_tick_length; // in units of numeric label char heights
   s->minor_tick_length = p->xaxis_minor_tick_length; // in units of numeric label char heights
   s->log_values = p->xaxis_log_values;
   s->ticks_inside = p->xaxis_ticks_inside; // inside frame or toward larger x or y value for specific location
   s->ticks_outside = p->xaxis_ticks_outside; // inside frame or toward smaller x or y value for specific location
   s->tick_interval = p->xaxis_tick_interval; // set to 0 to use default
   s->min_between_major_ticks = p->xaxis_min_between_major_ticks; // in units of numeric label char heights
   s->number_of_minor_intervals = p->xaxis_number_of_minor_intervals; // set to 0 to use default
   s->locations_for_major_ticks = p->xaxis_locations_for_major_ticks; // set to nil to use defaults
   s->locations_for_minor_ticks = p->xaxis_locations_for_minor_ticks; // set to nil to use defaults
   s->use_fixed_pt = p->xaxis_use_fixed_pt;
   s->digits_max = p->xaxis_digits_max;
   s->tick_labels = p->xaxis_tick_labels; // set to nil to use defaults. else must have a label for each major tick
   s->numeric_label_decimal_digits = p->xaxis_numeric_label_decimal_digits; // set to negative to use default
   s->numeric_label_scale = p->xaxis_numeric_label_scale;
   s->numeric_label_shift = p->xaxis_numeric_label_shift; // in char heights, positive for out from edge (or toward larger x or y value)
   s->numeric_label_angle = p->xaxis_numeric_label_angle;
   s->numeric_label_alignment = p->xaxis_numeric_label_alignment;
   s->numeric_label_justification = p->xaxis_numeric_label_justification;
}

static void Get_yaxis_Specs(FM *p, PlotAxis *s)
{
   s->type = p->yaxis_type;
   s->other_axis_type = p->xaxis_type;
   s->line_width = p->yaxis_line_width; // for axis line
   s->stroke_color = p->yaxis_stroke_color; // for axis line and tick marks
   s->major_tick_width = p->yaxis_major_tick_width; // same units as line_width
   s->minor_tick_width = p->yaxis_minor_tick_width; // same units as line_width
   s->major_tick_length = p->yaxis_major_tick_length; // in units of numeric label char heights
   s->minor_tick_length = p->yaxis_minor_tick_length; // in units of numeric label char heights
   s->log_values = p->yaxis_log_values;
   s->ticks_inside = p->yaxis_ticks_inside; // inside frame or toward larger x or y value for specific location
   s->ticks_outside = p->yaxis_ticks_outside; // inside frame or toward smaller x or y value for specific location
   s->tick_interval = p->yaxis_tick_interval; // set to 0 to use default
   s->min_between_major_ticks = p->yaxis_min_between_major_ticks; // in units of numeric label char heights
   s->number_of_minor_intervals = p->yaxis_number_of_minor_intervals; // set to 0 to use default
   s->locations_for_major_ticks = p->yaxis_locations_for_major_ticks; // set to nil to use defaults
   s->locations_for_minor_ticks = p->yaxis_locations_for_minor_ticks; // set to nil to use defaults
   s->use_fixed_pt = p->yaxis_use_fixed_pt;
   s->digits_max = p->yaxis_digits_max;
   s->tick_labels = p->yaxis_tick_labels; // set to nil to use defaults. else must have a label for each major tick
   s->numeric_label_decimal_digits = p->yaxis_numeric_label_decimal_digits; // set to negative to use default
   s->numeric_label_scale = p->yaxis_numeric_label_scale;
   s->numeric_label_shift = p->yaxis_numeric_label_shift; // in char heights, positive for out from edge (or toward larger x or y value)
   s->numeric_label_angle = p->yaxis_numeric_label_angle;
   s->numeric_label_alignment = p->yaxis_numeric_label_alignment;
   s->numeric_label_justification = p->yaxis_numeric_label_justification;
}

/*======================================================================*/

static void draw_axis_line(FM *p, int location, PlotAxis *s)
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
   c_line_width_set(p, s->line_width);
   figure_join_and_stroke(p, s->x0, s->y0, s->x1, s->y1);
}

static char *Create_Label(double value, int scale, int prec, bool log_values, bool use_fixed_pt, char *postfix, PlotAxis *s)
{ // value in figure coords
   char buff[100], *string;
   int exponent = ROUND(value);
   buff[0] = 0;
   if (log_values && use_fixed_pt) {   /* Logarithmic */
     double sav_val = value, pow_val;
     value = pow(10.0, exponent);
     pow_val = pow(10.0, sav_val);
     if (exponent < 0) {
        char form[60];
        int numdig = ABS(exponent)+1; 
        sprintf(form, (s->vertical)? "\\tiogayaxisnumericlabel{%%.%df}" : "\\tiogaxaxisnumericlabel{%%.%df}", numdig);
        sprintf(buff, form, pow_val);
     } else if (abs(value - pow_val) > 0.1) {
        sprintf(buff, (s->vertical)? "\\tiogayaxisnumericlabel{%0.2f}" : "\\tiogaxaxisnumericlabel{%0.2f}", pow_val);
     } else {
        sprintf(buff,  (s->vertical)? "\\tiogayaxisnumericlabel{%d}" : "\\tiogaxaxisnumericlabel{%d}", (int) value);
     }
   } else if (log_values) {
     /* Exponential, i.e. 10^-1, 1, 10, 10^2, etc */
     double abs_diff = fabs(value - exponent);
     if (abs_diff > 0.1) sprintf(buff,  (s->vertical)? "\\tiogayaxisnumericlabel{10^{%0.1f}}" : "\\tiogaxaxisnumericlabel{10^{%0.1f}}", value);
     else if (exponent == 0) strcpy(buff, "1");
     else if (exponent == 1) strcpy(buff, "10");
     else sprintf(buff,  (s->vertical)? "\\tiogayaxisnumericlabel{10^{%d}}" : "\\tiogaxaxisnumericlabel{10^{%d}}", exponent);
   } else {   /* Linear */
      char form[10];
      double scale2;
      int precis = s->numeric_label_decimal_digits; //  use this precision if it is >= 0
      if (precis >= 0) prec = precis;
      if (scale) value /= pow(10.,(double)scale);
      /* This is necessary to prevent labels like "-0.0" on some systems */
      scale2 = pow(10., prec);
      value = floor((value * scale2) + .5) / scale2;
      sprintf(form, (s->vertical)?  "\\tiogayaxisnumericlabel{%%.%df}" : "\\tiogaxaxisnumericlabel{%%.%df}", (int) prec);
      sprintf(buff, form, value);
   }
   int len = strlen(buff);
   if (postfix != NULL) { strcpy(buff+len, postfix); len = strlen(buff); }
   string = ALLOC_N(char, len+1);
   strcpy(string, buff);
   return string;
}

char *Get_String(VALUE ary, int index) {
   VALUE string = rb_ary_entry(ary, index);
   return StringValuePtr(string);
}

#define MIN_FLTDIG   3 
#define MAX_FIXDIG_POS  6
#define MAX_FIXDIG_NEG  4
#define DIGMAX_DEF   5

static void Pick_Label_Precision(double vmin, double vmax, double tick, bool use_fixed_pt,
   int *mode, int *prec, int digmax, int *scale) {
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

static char **Get_Labels(FM *p, PlotAxis *s)
{
   char **labels = (char **)ALLOC_N(char *, s->nmajors);
   char postfix[50], *ps;
   int i;
   if (s->tick_labels==Qnil) { // create label strings
      int mode, prec, scale;
      Pick_Label_Precision(s->axis_min, s->axis_max, s->interval, s->use_fixed_pt, &mode, &prec, s->digits_max, &scale);
      int i, upper_right = (s->reversed)? 0 : s->nmajors-1, lower_left = (s->reversed)? s->nmajors-1 : 0;
      for (i = 0; i < s->nmajors; i++) {
         ps = NULL;
         if (i == upper_right && !s->log_values && mode && scale)
            sprintf(ps = postfix, 
                (s->vertical)? " (x\\tiogayaxisnumericlabel{10^{%d}})" : " (x\\tiogaxaxisnumericlabel{10^{%d}})", 
                scale);
         if (i == lower_left && s->nmajors >= 2 && s->vertical && 
                (s->majors[i] == ((s->reversed)? s->axis_max : s->axis_min)) &&
                (s->other_axis_type == AXIS_WITH_MAJOR_TICKS_AND_NUMERIC_LABELS ||
                 s->other_axis_type == AXIS_WITH_TICKS_AND_NUMERIC_LABELS)) {
            labels[i] = NULL;
         } else {
            labels[i] = Create_Label(s->majors[i], scale, prec, s->log_values, s->use_fixed_pt, ps, s);
        }
      }
      s->free_strings_for_labels = true;
   } else { // use the given label strings
      VALUE ary = rb_Array(s->tick_labels);
      if (RARRAY(ary)->len != s->nmajors)
         rb_raise(rb_eArgError, "Sorry: must have same number of labels as major ticks");
      for (i = 0; i < s->nmajors; i++) {
         labels[i] = Get_String(ary, i);
      }
      s->free_strings_for_labels = false;
   }
   return labels;
}
         
static double *Pick_Locations_for_Major_Ticks(double interval, double axis_min, double axis_max, int *num_locations)
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
   majors = ALLOC_N(double, nmajors);
   prev_tick = starting_tick;
   for (i=0; i < nmajors; i++) {
      majors[i] = prev_tick += interval;
   }
   *num_locations = nmajors;
   return majors;
}

static int Pick_Number_of_Minor_Intervals(double length)
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

static void Pick_Major_Tick_Interval(FM *p, double tick_min, double tick_gap, double length, bool log_values, double *tick)
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
   if (log_values) {
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
         rb_raise(rb_eArgError, "Sorry: magnitude of specified tick interval (%g) is too small", *tick);
      }
   }
}

static void draw_major_ticks(FM *p, PlotAxis *s)
{
   s->num_minors = s->number_of_minor_intervals; 
   if (s->locations_for_major_ticks != Qnil) {
      long len;
      s->majors = Dvector_Data_for_Read(s->locations_for_major_ticks, &len);
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
      Pick_Major_Tick_Interval(p, tick_min, tick_gap, s->length, s->log_values, &s->interval);
      s->majors = Pick_Locations_for_Major_Ticks(s->interval, s->axis_min, s->axis_max, &s->nmajors);
      s->free_majors = true;
   }
   int i;
   double inside=0.0, outside=0.0, length;
   bool did_line = false;
   length = s->major_tick_length * ((s->vertical)? p->default_text_height_dx : p->default_text_height_dy);
   if (s->ticks_inside) inside = length;
   if (s->ticks_outside) outside = length;
   if (s->top_or_right) { inside = -inside; outside = -outside; }
   if (s->line_width != s->major_tick_width)
      c_line_width_set(p, s->line_width = s->major_tick_width);
   for (i=0; i < s->nmajors; i++) {
      if (s->vertical)
         figure_join(p, s->x0+inside, s->majors[i], s->x0+outside, s->majors[i]);
      else
         figure_join(p, s->majors[i], s->y0+inside, s->majors[i], s->y0+outside);
      did_line = true;
   }
   if (did_line) axis_stroke(p);
}

static double log_subintervals[8] =
{
    0.301030, 0.477121, 0.602060, 0.698970,
    0.778151, 0.845098, 0.903090, 0.954243
};

static void draw_minor_ticks(FM *p, PlotAxis *s)
{
   if (s->number_of_minor_intervals <= 0) {
      if (s->log_values) {
         double interval = s->majors[1] - s->majors[0];
         s->number_of_minor_intervals = (abs(interval) != 1.0 || s->nmajors > 4)? 1 : 9; 
      }  else s->number_of_minor_intervals = Pick_Number_of_Minor_Intervals(s->interval);
   }
   int i, j, nsub = s->number_of_minor_intervals;
   double inside=0.0, outside=0.0, length;
   bool did_line = false;
   if (s->log_values && nsub > 9) nsub = 9;
   length = s->minor_tick_length * ((s->vertical)? p->default_text_height_dx : p->default_text_height_dy);
   if (s->ticks_inside) inside = length;
   if (s->ticks_outside) outside = length;
   if (s->top_or_right) { inside = -inside; outside = -outside; }
   if (s->line_width != s->minor_tick_width)
      c_line_width_set(p, s->line_width = s->minor_tick_width);
   if (s->locations_for_minor_ticks != Qnil) {
      long cnt;
      double *locs = Dvector_Data_for_Read(s->locations_for_minor_ticks, &cnt);
      for (i=0; i < cnt; i++) {
         if (s->vertical)
            figure_join(p, s->x0+inside, locs[i], s->x0+outside, locs[i]);
         else
            figure_join(p, locs[i], s->y0+inside, locs[i], s->y0+outside);
         did_line = true;
      }
   } else {
      for (i=0; i <= s->nmajors; i++) {
         double loc = (i==0)? s->majors[0] - s->interval : s->majors[i-1];
         double next_loc = (i==s->nmajors)? loc + s->interval : s->majors[i];
         double subinterval = (next_loc - loc) / nsub;
         if (subinterval <= 0.0) continue;
         for (j = 1; j < nsub; j++) {
            double subloc = loc + ((!s->log_values) ? (j * subinterval) : log_subintervals[j-1]);
            if (subloc >= next_loc) break;
            if (subloc <= s->axis_min || subloc >= s->axis_max) continue;
            if (s->vertical)
               figure_join(p, s->x0+inside, subloc, s->x0+outside, subloc);
            else
               figure_join(p, subloc, s->y0+inside, subloc, s->y0+outside);
            did_line = true;
         }
      }
   }
   if (did_line) axis_stroke(p);
}

static void show_numeric_label(FM *p , VALUE fmkr, PlotAxis *s, char *text, int location, double position, double shift)
{ // position is in figure coords and must be converted to frame coords
   double pos = ((!s->reversed)? (position - s->axis_min) : (s->axis_max - position)) / s->length;
   c_show_rotated_text(p, text, location, shift, pos, 
      s->numeric_label_scale, s->numeric_label_angle, s->numeric_label_justification, s->numeric_label_alignment);
}

static void draw_numeric_labels(FM *p , VALUE fmkr, int location, PlotAxis *s)
{
   int i;
   double shift = (s->ticks_outside) ? s->major_tick_length : 0.5; // default shift
   shift += s->numeric_label_shift;
   s->labels = Get_Labels(p, s);
   for (i=0; i < s->nmajors; i++) {
      if (s->labels[i] != NULL) show_numeric_label(p, fmkr, s, s->labels[i], location, s->majors[i], shift);
   }
}

static void c_show_side(VALUE fmkr, FM *p, PlotAxis *s) {
   int i;
   if (s->type == AXIS_HIDDEN) return;
   Start_Axis_Standard_State(p, s->stroke_color, s->line_width * p->default_line_scale);
      // gsave, set line type and stroke color
   draw_axis_line(p, s->location, s);
   if (s->type == AXIS_LINE_ONLY) goto done;
   draw_major_ticks(p, s);
   if (s->type == AXIS_WITH_MAJOR_TICKS_ONLY) goto done;
   if (s->type == AXIS_WITH_MAJOR_TICKS_AND_NUMERIC_LABELS) {
      draw_numeric_labels(p, fmkr, s->location, s);
      goto done;
   }
   draw_minor_ticks(p, s);
   if (s->type == AXIS_WITH_TICKS_ONLY) goto done;
   draw_numeric_labels(p, fmkr, s->location, s);
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

VALUE FM_show_axis(VALUE fmkr, VALUE loc)
{
   FM *p = Get_FM(fmkr);
   PlotAxis axis;
   int location;
   Init_PlotAxis_struct(&axis);
   loc = rb_Integer(loc);
   location = NUM2INT(loc);
   if (location == LEFT || location == RIGHT || location == AT_X_ORIGIN) {
      if (!p->yaxis_visible) goto done;
      Get_yaxis_Specs(p, &axis);
   } else if (location == TOP || location == BOTTOM || location == AT_Y_ORIGIN) {
      if (!p->xaxis_visible) goto done;
      Get_xaxis_Specs(p, &axis);
   } else rb_raise(rb_eArgError,
         "Sorry: invalid 'loc' for axis: must be one of LEFT, RIGHT, TOP, BOTTOM, AT_X_ORIGIN, or AT_Y_ORIGIN: is (%i)", location);
   axis.location = location;
   c_show_side(fmkr, p, &axis);
 done:
   return fmkr;
}
      
VALUE FM_show_edge(VALUE fmkr, VALUE loc) {
   FM *p = Get_FM(fmkr);
   PlotAxis axis;
   int location;
   Init_PlotAxis_struct(&axis);
   loc = rb_Integer(loc);
   location = NUM2INT(loc);
   switch (location) {
      case LEFT:
         if (!p->left_edge_visible) goto done;
         Get_yaxis_Specs(p, &axis); axis.type = p->left_edge_type;
         break;
      case RIGHT:
         if (!p->right_edge_visible) goto done;
         Get_yaxis_Specs(p, &axis); axis.type = p->right_edge_type;
         break;
      case BOTTOM:
         if (!p->bottom_edge_visible) goto done;
         Get_xaxis_Specs(p, &axis); axis.type = p->bottom_edge_type;
         break;
      case TOP:
         if (!p->top_edge_visible) goto done;
         Get_xaxis_Specs(p, &axis); axis.type = p->top_edge_type;
         break;
      default: rb_raise(rb_eArgError,
         "Sorry: invalid 'loc' for edge: must be one of LEFT, RIGHT, TOP, or BOTTOM: is (%i)", location);
   }
   axis.location = location;
   c_show_side(fmkr, p, &axis);
 done:
   return fmkr;
}


VALUE FM_no_title(VALUE fmkr)
{
   FM *p = Get_FM(fmkr);
   p->title_visible = false;
   return fmkr;
}

VALUE FM_no_xlabel(VALUE fmkr)
{
   FM *p = Get_FM(fmkr);
   p->xlabel_visible = false;
   return fmkr;
}


VALUE FM_no_ylabel(VALUE fmkr)
{
   FM *p = Get_FM(fmkr);
   p->ylabel_visible = false;
   return fmkr;
}


VALUE FM_no_xaxis(VALUE fmkr)
{
   FM *p = Get_FM(fmkr);
   p->xaxis_visible = false;
   return fmkr;
}


VALUE FM_no_yaxis(VALUE fmkr)
{
   FM *p = Get_FM(fmkr);
   p->yaxis_visible = false;
   return fmkr;
}


VALUE FM_no_left_edge(VALUE fmkr)
{
   FM *p = Get_FM(fmkr);
   p->left_edge_visible = false;
   return fmkr;
}


VALUE FM_no_right_edge(VALUE fmkr)
{
   FM *p = Get_FM(fmkr);
   p->right_edge_visible = false;
   return fmkr;
}


VALUE FM_no_top_edge(VALUE fmkr)
{
   FM *p = Get_FM(fmkr);
   p->top_edge_visible = false;
   return fmkr;
}

VALUE FM_no_bottom_edge(VALUE fmkr)
{
   FM *p = Get_FM(fmkr);
   p->bottom_edge_visible = false;
   return fmkr;
}

