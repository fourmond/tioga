/* pdfpath.c */
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

bool have_current_point, constructing_path, writing_file;
double bbox_llx, bbox_lly, bbox_urx, bbox_ury;

/* emits a warning on nonok numbers if croak_on_nonok_numbers is true */
static void croak_on_nonok(FM *p, const char * function)
{
  if(p->croak_on_nonok_numbers)
    rb_warn("Illegal coordinates in function %s, element suppressed", 
	    function);
}

/* small macro to check if a number is ok to be output */

#define CROAK_ON_NONOK(p) croak_on_nonok(p, __FUNCTION__)
#define ARE_OK_NUMBERS(x,y) if(! is_okay_number(x) || ! is_okay_number(y)) {\
CROAK_ON_NONOK(p); return;}


/* PDF graphics */
   
/* graphics attributes */

void Unpack_RGB(VALUE rgb, double *rp, double *gp, double *bp)
{
   if (rgb == Qnil) { *rp = *gp = *bp = 0.0; return; }
   rgb = rb_Array(rgb);
   if (RARRAY(rgb)->len != 3) rb_raise(rb_eArgError, "Sorry: invalid rgb array for setting color: must have 3 entries");
   VALUE entry = rb_ary_entry(rgb, 0);
   entry = rb_Float(entry);
   double r = NUM2DBL(entry);
   entry = rb_ary_entry(rgb, 1);
   entry = rb_Float(entry);
   double g = NUM2DBL(entry);
   entry = rb_ary_entry(rgb, 2);
   entry = rb_Float(entry);
   double b = NUM2DBL(entry);
   if (r < 0.0 || r > 1.0) rb_raise(rb_eArgError, "Sorry: invalid red (%g) for color: must be between 0 and 1", r);
   if (g < 0.0 || g > 1.0) rb_raise(rb_eArgError, "Sorry: invalid green (%g) for color: must be between 0 and 1", g);
   if (b < 0.0 || b > 1.0) rb_raise(rb_eArgError, "Sorry: invalid blue (%g) for color: must be between 0 and 1", b);
   *rp = r; *gp = g; *bp = b;
}

void c_stroke_color_set(FM *p, double r, double g, double b)
{
   if (writing_file) fprintf(TF, "%0.3f %0.3f %0.3f RG\n", r, g, b);
}

VALUE FM_stroke_color_set(VALUE fmkr, VALUE value) // value is array of [r, g, b] intensities from 0 to 1
{  // r g b RG
   FM *p = Get_FM(fmkr);
   double r, g, b;
   Unpack_RGB(value, &r, &g, &b);
   c_stroke_color_set(p, r, g, b);
   p->stroke_color = value;
   return value;
}

VALUE FM_fill_color_set(VALUE fmkr, VALUE value) // value is array of [r, g, b] intensities from 0 to 1
{ // r g b rg
   FM *p = Get_FM(fmkr);
   double r, g, b;
   Unpack_RGB(value, &r, &g, &b);
   if (writing_file) fprintf(TF, "%0.3f %0.3f %0.3f rg\n", r, g, b);
   p->fill_color = value;
   return value;
}

void c_line_width_set(FM *p, double line_width)
{
   if (line_width < 0.0) rb_raise(rb_eArgError, "Sorry: invalid line width (%g points): must be positive", line_width);
   if (line_width > 1e3) rb_raise(rb_eArgError, "Sorry: too large line width (%g points)", line_width);
   if (writing_file) fprintf(TF, "%0.3f w\n", line_width * ENLARGE * p->default_line_scale);
   p->line_width = line_width;
}

VALUE FM_line_width_set(VALUE fmkr, VALUE value)  // value is thickness in points
{ // w
   FM *p = Get_FM(fmkr);
   value = rb_Float(value);
   c_line_width_set(p, NUM2DBL(value));
   return value;
}

void c_line_scale_set(FM *p, double new_scale)
{
   if (new_scale <= 0) rb_raise(rb_eArgError, "Sorry: line scale must be positive");
   p->default_line_scale = new_scale;
   c_line_width_set(p, p->line_width);
}

VALUE FM_rescale_lines(VALUE fmkr, VALUE scaling_factor)
{
   FM *p = Get_FM(fmkr);
   scaling_factor = rb_Float(scaling_factor);
   c_line_scale_set(p, NUM2DBL(scaling_factor) * p->default_line_scale);
   return fmkr;
}

void c_line_cap_set(FM *p, int line_cap)
{
   if (line_cap < 0 || line_cap > 3) rb_raise(rb_eArgError, "Sorry: invalid arg for setting line_cap (%i)", line_cap);
   if (writing_file) fprintf(TF, "%d J\n", line_cap);
   p->line_cap = line_cap;
}

VALUE FM_line_cap_set(VALUE fmkr, VALUE value)
{ // J
   FM *p = Get_FM(fmkr);
   value = rb_Integer(value);
   c_line_cap_set(p, NUM2INT(value));
   return value;
}

void c_line_join_set(FM *p, int line_join)
{
   if (line_join < 0 || line_join > 3) rb_raise(rb_eArgError, "Sorry: invalid arg for setting line_join (%i)", line_join);
   if (writing_file) fprintf(TF, "%d j\n", line_join);
   p->line_join = line_join;
}

VALUE FM_line_join_set(VALUE fmkr, VALUE value)
{  // j
   FM *p = Get_FM(fmkr);
   value = rb_Integer(value);
   c_line_join_set(p, NUM2INT(value));
   return value;
}

void c_miter_limit_set(FM *p, double miter_limit)
{
   if (miter_limit < 0.0)
      rb_raise(rb_eArgError,
         "Sorry: invalid miter limit (%g): must be positive ratio for max miter length to line width", miter_limit);
   if (writing_file) fprintf(TF, "%0.3f M\n", miter_limit);
   p->miter_limit = miter_limit;
}

VALUE FM_miter_limit_set(VALUE fmkr, VALUE value) // value is max ratio of miter length to line width
{ // M
   FM *p = Get_FM(fmkr);
   if (constructing_path) rb_raise(rb_eArgError, "Sorry: must not be constructing a path when change miter limit");
   value = rb_Float(value);
   c_miter_limit_set(p, NUM2DBL(value));
   return value;
}

VALUE FM_line_type_set(VALUE fmkr, VALUE line_type)
{ // array phase d  (distances given in points)
   FM *p = Get_FM(fmkr);
   double sz;
   if (constructing_path) rb_raise(rb_eArgError, "Sorry: must not be constructing a path when change line_type");
   if (line_type == Qnil) {
      fprintf(TF, "[] 0 d\n");
   } else {
      line_type = rb_Array(line_type);
      if (writing_file) {
         if (RARRAY(line_type)->len != 2)
            rb_raise(rb_eArgError, "Sorry: invalid line_type.  Must be [ [dash pattern] dash phase ]");
         VALUE dashArray = rb_ary_entry(line_type, 0), dashPhase = rb_ary_entry(line_type, 1);
         fprintf(TF, "[ ");
         if (dashArray != Qnil) {
            dashArray = rb_Array(dashArray);
            long i, len = RARRAY(dashArray)->len;
            for (i=0; i < len; i++) {
               VALUE entry = rb_ary_entry(dashArray, i);
               entry = rb_Float(entry);
               sz = NUM2DBL(entry);
               if (sz < 0.0) rb_raise(rb_eArgError, "Sorry: invalid dash array entry (%g): must be positive", sz);
               fprintf(TF, "%0.3f ", sz * ENLARGE);
            }
         }
         dashPhase = rb_Float(dashPhase);
         sz = NUM2DBL(dashPhase);
         if (sz < 0.0) rb_raise(rb_eArgError, "Sorry: invalid dash phase (%g): must be positive", sz);
         fprintf(TF, "] %0.3f d\n", sz * ENLARGE);
      }
   }
   p->line_type = line_type;
   return fmkr;
}


/* Path construction operators */

    /* all locations and vectors are in figure coordinates */

long c_round_dev(FM *p, double v) { // make sure that we don't get too far out or can overflow!
    if (v > MAX_DEV_COORD_ALLOWED) {
        //if (p->debug_verbosity_level > 0) printf("c_round_dev clipping %g\n", v);
        return iMAX_DEV_COORD_ALLOWED;
    }
    if (v < -MAX_DEV_COORD_ALLOWED) {
        //if (p->debug_verbosity_level > 0) printf("c_round_dev clipping %g\n", v);
        return -iMAX_DEV_COORD_ALLOWED;
    }
    return ROUND(v);
}

void update_bbox(FM *p, double x, double y)
{
   if (x >= p->clip_left && x < bbox_llx) bbox_llx = x;
   if (y >= p->clip_bottom && y < bbox_lly) bbox_lly = y;
   if (x <= p->clip_right && x > bbox_urx) bbox_urx = x;
   if (y <= p->clip_top && y > bbox_ury) bbox_ury = y;
}

VALUE FM_update_bbox(VALUE fmkr, VALUE x, VALUE y)
{
   FM *p = Get_FM(fmkr);
   x = rb_Float(x);
   y = rb_Float(y);
   update_bbox(p, convert_figure_to_output_x(p,NUM2DBL(x)), convert_figure_to_output_y(p,NUM2DBL(y)));
   return fmkr;
}


VALUE FM_bbox_left(VALUE fmkr)
{
   return rb_float_new(bbox_llx); 
}

VALUE FM_bbox_right(VALUE fmkr)
{
   return rb_float_new(bbox_urx); 
}

VALUE FM_bbox_top(VALUE fmkr)
{
   return rb_float_new(bbox_ury); 
}

VALUE FM_bbox_bottom(VALUE fmkr)
{
   return rb_float_new(bbox_lly); 
}



void c_moveto(FM *p, double x, double y)
{
   ARE_OK_NUMBERS(x,y);
   if (writing_file) fprintf(TF, "%ld %ld m\n", c_round_dev(p,x), c_round_dev(p,y));
   update_bbox(p, x, y);
   have_current_point = constructing_path = true;
}

VALUE FM_move_to_point(VALUE fmkr, VALUE x, VALUE y)
{
   FM *p = Get_FM(fmkr);
   x = rb_Float(x);
   y = rb_Float(y);
   double dev_x = convert_figure_to_output_x(p,NUM2DBL(x)), dev_y = convert_figure_to_output_y(p,NUM2DBL(y));
   c_moveto(p, dev_x, dev_y);
   return fmkr;
}

void c_lineto(FM *p, double x, double y)
{
   ARE_OK_NUMBERS(x,y);
   if (!constructing_path) rb_raise(rb_eArgError, "Sorry: must start path with moveto before call lineto");
   if (writing_file) fprintf(TF, "%ld %ld l\n", c_round_dev(p,x), c_round_dev(p,y));
   update_bbox(p, x, y);
}

VALUE FM_append_point_to_path(VALUE fmkr, VALUE x, VALUE y)
{
   FM *p = Get_FM(fmkr);
   x = rb_Float(x);
   y = rb_Float(y);
   double dev_x = convert_figure_to_output_x(p,NUM2DBL(x)), dev_y = convert_figure_to_output_y(p,NUM2DBL(y));
   c_lineto(p, dev_x, dev_y);
   return fmkr;
}

void c_curveto(FM *p, double x1, double y1, double x2, double y2, double x3, double y3)
{
   ARE_OK_NUMBERS(x1,y1);
   ARE_OK_NUMBERS(x2,y2);
   ARE_OK_NUMBERS(x3,y3);
   if (!constructing_path) rb_raise(rb_eArgError, "Sorry: must start path with moveto before call curveto");
   if (writing_file) fprintf(TF, "%ld %ld %ld %ld %ld %ld c\n", 
            c_round_dev(p,x1), c_round_dev(p,y1), c_round_dev(p,x2), c_round_dev(p,y2), c_round_dev(p,x3), c_round_dev(p,y3));
   update_bbox(p, x1, y1);
   update_bbox(p, x2, y2);
   update_bbox(p, x3, y3);
}

VALUE FM_append_curve_to_path(VALUE fmkr, VALUE x1, VALUE y1, VALUE x2, VALUE y2, VALUE x3, VALUE y3)
{
   FM *p = Get_FM(fmkr);
   x1 = rb_Float(x1);
   y1 = rb_Float(y1);
   x2 = rb_Float(x2);
   y2 = rb_Float(y2);
   x3 = rb_Float(x3);
   y3 = rb_Float(y3);
   double dev_x1 = convert_figure_to_output_x(p,NUM2DBL(x1)), dev_y1 = convert_figure_to_output_y(p,NUM2DBL(y1));
   double dev_x2 = convert_figure_to_output_x(p,NUM2DBL(x2)), dev_y2 = convert_figure_to_output_y(p,NUM2DBL(y2));
   double dev_x3 = convert_figure_to_output_x(p,NUM2DBL(x3)), dev_y3 = convert_figure_to_output_y(p,NUM2DBL(y3));
   c_curveto(p, dev_x1, dev_y1, dev_x2, dev_y2, dev_x3, dev_y3);
   return fmkr;
}

void c_closepath(FM *p)
{
   if (!constructing_path) rb_raise(rb_eArgError, "Sorry: must be constructing path when call closepath");
   if (writing_file) fprintf(TF, "h\n");
   have_current_point = false;
   p = NULL;
}

VALUE FM_close_path(VALUE fmkr)
{
   c_closepath(Get_FM(fmkr));
   return fmkr;
}

void c_append_arc(FM *p, double x_start, double y_start, double x_corner, double y_corner,
   double x_end, double y_end, double radius)
{
   ARE_OK_NUMBERS(x_start,y_start);
   ARE_OK_NUMBERS(x_corner,y_corner);
   ARE_OK_NUMBERS(x_end,y_end);

   double x0, y0, x1, y1, x2, y2, x3, y3, udx, udy, vdx, vdy, wdx, wdy, len, x_center, y_center, tmp;
   double psi, sin_psi, cos_psi, theta, cos_alpha, sin_alpha;
   udx = x_start - x_corner; udy = y_start - y_corner;
   vdx = x_end - x_corner; vdy = y_end - y_corner;
   len = sqrt(udx*udx + udy*udy); udx /= len; udy /= len; // u is unit vector from corner to start
   len = sqrt(vdx*vdx + vdy*vdy); vdx /= len; vdy /= len; // v is unit vector from corner to end
   cos_psi = udx*vdx + udy*vdy; sin_psi = udy * vdx - udx * vdy;
   psi = atan2(sin_psi, cos_psi); // psi is angle between u and v
   if (psi > PI) psi = 2*PI - psi;
   theta = PI - psi; // theta is opening angle for the arc
   while (theta < 0) theta += 2*PI;
   if (theta >= PI) rb_raise(rb_eArgError, "Sorry: invalid control point for arc");
   // first compute control points for arc of opening theta with unit radius, bisected by positive x axis
   // based on note by Richard DeVeneza, "How to determine the control points of a Bezier curve that
   // approximates a small circular arc", Nov 2004.
   x0 = cos(theta/2); y0 = sin(theta/2);  x3 = x0; y3 = -y0;
   x1 = (4-x0)/3; y1 = (1-x0)*(3-x0)/(3*y0);  x2 = x1; y2 = -y1;
   if (sin_psi > 0) { y0 = -y0; y1 = -y1; y2 = -y2; y3 = -y3; }
   wdx = udx + vdx; wdy = udy + vdy;
   len = sqrt(wdx*wdx + wdy*wdy); wdx /= len; wdy /= len; // w is unit vector bisecting the corner angle
   cos_alpha = -wdx; sin_alpha = -wdy; // need to rotate the arc by alpha
   x_center = x_corner + radius * wdx / x0;
   y_center = y_corner + radius * wdy / x0;   
   // then translate arc to x_center, y_center
   tmp = x0; x0 = x0*cos_alpha - y0*sin_alpha; y0 = y0*cos_alpha + tmp*sin_alpha;
   tmp = x1; x1 = x1*cos_alpha - y1*sin_alpha; y1 = y1*cos_alpha + tmp*sin_alpha;
   tmp = x2; x2 = x2*cos_alpha - y2*sin_alpha; y2 = y2*cos_alpha + tmp*sin_alpha;
   tmp = x3; x3 = x3*cos_alpha - y3*sin_alpha; y3 = y3*cos_alpha + tmp*sin_alpha;
   x0 *= radius; y0 *= radius;
   x1 *= radius; y1 *= radius;
   x2 *= radius; y2 *= radius;
   x3 *= radius; y3 *= radius;
   x0 += x_center; y0 += y_center;
   x1 += x_center; y1 += y_center;
   x2 += x_center; y2 += y_center;
   x3 += x_center; y3 += y_center;
   if (have_current_point) c_lineto(p,x0,y0);
   else c_moveto(p,x0,y0);
   c_curveto(p,x1,y1,x2,y2,x3,y3);
}

double Get_Arc_Radius(FM *p, VALUE dx, VALUE dy)
{
   dx = rb_Float(dx);
   dy = rb_Float(dy);
   double rx = NUM2DBL(dx), ry = NUM2DBL(dy);
   rx = convert_figure_to_output_dx(p,rx);
   ry = convert_figure_to_output_dy(p,ry);
   if (rx < 0) rx = -rx;
   if (ry < 0) ry = -ry;
   return MIN(rx,ry);
}

VALUE FM_append_arc_to_path(VALUE fmkr, VALUE x_start, VALUE y_start, VALUE x_corner, VALUE y_corner,
   VALUE x_end, VALUE y_end, VALUE dx, VALUE dy)
{
   FM *p = Get_FM(fmkr);
   x_start = rb_Float(x_start);
   y_start = rb_Float(y_start);
   x_corner = rb_Float(x_corner);
   y_corner = rb_Float(y_corner);
   x_end = rb_Float(x_end);
   y_end = rb_Float(y_end);
   c_append_arc(p,
      convert_figure_to_output_x(p,NUM2DBL(x_start)), convert_figure_to_output_y(p,NUM2DBL(y_start)),
      convert_figure_to_output_x(p,NUM2DBL(x_corner)), convert_figure_to_output_y(p,NUM2DBL(y_corner)),
      convert_figure_to_output_x(p,NUM2DBL(x_end)), convert_figure_to_output_y(p,NUM2DBL(y_end)),
      Get_Arc_Radius(p,dx,dy));
   return fmkr;
}

void c_append_rect(FM *p, double x, double y, double width, double height)
{
   c_moveto(p,x,y);
   c_lineto(p,x+width,y);
   c_lineto(p,x+width,y+height);
   c_lineto(p,x,y+height);
   c_closepath(p);
}

VALUE FM_append_rect_to_path(VALUE fmkr, VALUE x, VALUE y, VALUE width, VALUE height)
{
   FM *p = Get_FM(fmkr);
   x = rb_Float(x);
   y = rb_Float(y);
   width = rb_Float(width);
   height = rb_Float(height);
   c_append_rect(p,
      convert_figure_to_output_x(p,NUM2DBL(x)), convert_figure_to_output_y(p,NUM2DBL(y)),
      convert_figure_to_output_dx(p,NUM2DBL(width)), convert_figure_to_output_dy(p,NUM2DBL(height)));
   return fmkr;
}

void c_append_rounded_rect(FM *p, double x, double y, double width, double height, double radius)
{
   double xc = x + width/2, yc = y + height/2, xp = x + width, yp = y + height;
   c_moveto(p,xc,y);
   c_append_arc(p,xc,y, xp,y, xp,yc, radius);
   c_append_arc(p,xp,yc, xp,yp, xc,yp, radius);
   c_append_arc(p,xc,yp, x,yp, x,yc, radius);
   c_append_arc(p,x,yc, x,y, xc,y, radius);
   c_closepath(p);
}

VALUE FM_append_rounded_rect_to_path(VALUE fmkr, VALUE x, VALUE y, VALUE width, VALUE height, VALUE dx, VALUE dy)
{
   FM *p = Get_FM(fmkr);
   x = rb_Float(x);
   y = rb_Float(y);
   width = rb_Float(width);
   height = rb_Float(height);
   c_append_rounded_rect(p,
      convert_figure_to_output_x(p,NUM2DBL(x)), convert_figure_to_output_y(p,NUM2DBL(y)),
      convert_figure_to_output_dx(p,NUM2DBL(width)), convert_figure_to_output_dy(p,NUM2DBL(height)), 
      Get_Arc_Radius(p,dx,dy));
   return fmkr;
}

#define ROTATE90(x,y) tmp = x; x = y; y = -tmp;
#define TRANSFORM(xp,yp,x,y) xp = a*(x)+c*(y)+e; yp = b*(x)+d*(y)+f;

void c_append_oval(FM *p, double x, double y, double dx, double dy, double angle)
{
   double cs = cos(angle/RADIANS_TO_DEGREES), sn = sin(angle/RADIANS_TO_DEGREES);
   double a = cs*dx, b = sn*dx, c = -sn*dy, d = cs*dy, e = x, f = y;
   double x0, y0, x1, y1, x2, y2, x3, y3, x0p, y0p, x1p, y1p, x2p, y2p, x3p, y3p, tmp;
   int i;
   x0 = 0.707107; y0 = 0.707107; x3 = x0;  y3 = -y0; 
   x1 = 1.09763; y1 = 0.316582; x2 = x1; y2 = -y1;
   TRANSFORM(x0p,y0p,x0,y0)
   TRANSFORM(x1p,y1p,x1,y1)
   TRANSFORM(x2p,y2p,x2,y2)
   TRANSFORM(x3p,y3p,x3,y3)
   c_moveto(p,x0p,y0p);
   c_curveto(p,x1p,y1p,x2p,y2p,x3p,y3p);
   for (i = 0; i < 3; i++) {
      ROTATE90(x0,y0)
      ROTATE90(x1,y1)
      ROTATE90(x2,y2)
      ROTATE90(x3,y3)
      TRANSFORM(x0p,y0p,x0,y0)
      TRANSFORM(x1p,y1p,x1,y1)
      TRANSFORM(x2p,y2p,x2,y2)
      TRANSFORM(x3p,y3p,x3,y3)
      c_curveto(p,x1p,y1p,x2p,y2p,x3p,y3p);
   }
   c_closepath(p);
}

VALUE FM_append_oval_to_path(VALUE fmkr, VALUE x, VALUE y, VALUE dx, VALUE dy, VALUE angle)
{
   FM *p = Get_FM(fmkr);
   x = rb_Float(x);
   y = rb_Float(y);
   dx = rb_Float(dx);
   dy = rb_Float(dy);
   angle = rb_Float(angle);
   c_append_oval(p,
      convert_figure_to_output_x(p,NUM2DBL(x)), convert_figure_to_output_y(p,NUM2DBL(y)),
      convert_figure_to_output_dx(p,NUM2DBL(dx)), convert_figure_to_output_dy(p,NUM2DBL(dy)), 
      NUM2DBL(angle));
   return fmkr;
}

VALUE FM_append_circle_to_path(VALUE fmkr, VALUE x, VALUE y, VALUE dx)
{
   FM *p = Get_FM(fmkr);
   x = rb_Float(x);
   y = rb_Float(y);
   dx = rb_Float(dx);
   double s = convert_figure_to_output_dx(p,NUM2DBL(dx));
   c_append_oval(p,
      convert_figure_to_output_x(p,NUM2DBL(x)), convert_figure_to_output_y(p,NUM2DBL(y)),
      s, s, 0.0);
   return fmkr;
}

VALUE FM_append_points_to_path(VALUE fmkr, VALUE x_vec, VALUE y_vec)
{
   FM *p = Get_FM(fmkr);
   long xlen, ylen, i;
   double x0, y0;
   double *xs = Dvector_Data_for_Read(x_vec, &xlen);
   double *ys = Dvector_Data_for_Read(y_vec, &ylen);
   if (xlen != ylen) rb_raise(rb_eArgError, "Sorry: must have same number xs and ys for append_points");
   if (xlen <= 0) return fmkr;
   x0 = convert_figure_to_output_x(p,xs[0]); y0 = convert_figure_to_output_y(p,ys[0]);
   if (have_current_point) c_lineto(p,x0,y0);
   else c_moveto(p,x0,y0);
   for (i=1; i<xlen; i++)
      c_lineto(p,convert_figure_to_output_x(p,xs[i]), convert_figure_to_output_y(p,ys[i]));
   return fmkr;
}

VALUE FM_private_append_points_with_gaps_to_path(VALUE fmkr, VALUE x_vec, VALUE y_vec, VALUE gaps, VALUE close_gaps)
    // where there's a gap, do a moveto instead of a lineto
{
   if (gaps == Qnil) return FM_append_points_to_path(fmkr, x_vec, y_vec);
   FM *p = Get_FM(fmkr);
   long xlen, ylen, glen, i, j;
   double x0, y0;
   double *xs = Dvector_Data_for_Read(x_vec, &xlen);
   double *ys = Dvector_Data_for_Read(y_vec, &ylen);
   double *gs = Dvector_Data_for_Read(gaps, &glen);
   bool do_close = (close_gaps == Qtrue);
   if (xlen != ylen) rb_raise(rb_eArgError, "Sorry: must have same number xs and ys for append_points_with_gaps");
   if (xlen <= 0) return fmkr;
   x0 = convert_figure_to_output_x(p,xs[0]); y0 = convert_figure_to_output_y(p,ys[0]);
   if (have_current_point) c_lineto(p,x0,y0);
   else c_moveto(p,x0,y0);
   for (i = 1, j = 0; j < glen; j++) {
      int gap_start = ROUND(gs[j]);
      if (gap_start == xlen) break;
      if (gap_start > xlen)
         rb_raise(rb_eArgError, "Sorry: gap value (%i) too large for vectors of length (%i)", gap_start, xlen);
      while (i < gap_start) {
         c_lineto(p,convert_figure_to_output_x(p,xs[i]), convert_figure_to_output_y(p,ys[i]));
         i++;
      }
      if (do_close) c_closepath(p);
      c_moveto(p,convert_figure_to_output_x(p,xs[i]), convert_figure_to_output_y(p,ys[i]));
      i++;
   }
   while (i < xlen) {
      c_lineto(p,convert_figure_to_output_x(p,xs[i]), convert_figure_to_output_y(p,ys[i]));
      i++;
   }
   if (do_close) c_closepath(p);
   return fmkr;
}

/* Path painting operators */

VALUE FM_stroke(VALUE fmkr)
{
   if (!constructing_path) return fmkr;
   if (writing_file) fprintf(TF, "S\n");
   have_current_point = constructing_path = false;
   return fmkr;
}

VALUE FM_close_and_stroke(VALUE fmkr)
{
   if (!constructing_path) return fmkr;
   if (writing_file) fprintf(TF, "s\n");
   have_current_point = constructing_path = false;
   return fmkr;
}

VALUE FM_fill(VALUE fmkr)
{
   if (!constructing_path) return fmkr;
   if (writing_file) fprintf(TF, "f\n");
   have_current_point = constructing_path = false;
   return fmkr;
}

VALUE FM_discard_path(VALUE fmkr)
{
   if (!constructing_path) return fmkr;
   if (writing_file) fprintf(TF, "n\n");
   have_current_point = constructing_path = false;
   return fmkr;
}

VALUE FM_eofill(VALUE fmkr)
{
   if (!constructing_path) return fmkr;
   if (writing_file) fprintf(TF, "f*\n");
   have_current_point = constructing_path = false;
   return fmkr;
}

VALUE FM_fill_and_stroke(VALUE fmkr)
{
   if (!constructing_path) return fmkr;
   if (writing_file) fprintf(TF, "B\n");
   have_current_point = constructing_path = false;
   return fmkr;
}

VALUE FM_eofill_and_stroke(VALUE fmkr)
{
   if (!constructing_path) return fmkr;
   if (writing_file) fprintf(TF, "B*\n");
   have_current_point = constructing_path = false;
   return fmkr;
}

VALUE FM_close_fill_and_stroke(VALUE fmkr)
{
   if (!constructing_path) return fmkr;
   if (writing_file) fprintf(TF, "b\n");
   have_current_point = constructing_path = false;
   return fmkr;
}

VALUE FM_close_eofill_and_stroke(VALUE fmkr)
{
   if (!constructing_path) return fmkr;
   if (writing_file) fprintf(TF, "b*\n");
   have_current_point = constructing_path = false;
   return fmkr;
}

void c_clip(FM *p)
{
   if (!constructing_path) return;
   if (writing_file) fprintf(TF, "W n\n");
   have_current_point = constructing_path = false;
   p = NULL;
}

VALUE FM_clip(VALUE fmkr)
{
   c_clip(Get_FM(fmkr));
   return fmkr;
}

VALUE FM_eoclip(VALUE fmkr)
{
   if (!constructing_path) return fmkr;
   if (writing_file) fprintf(TF, "W* n\n");
   have_current_point = constructing_path = false;
   return fmkr;
}

VALUE FM_fill_and_clip(VALUE fmkr)
{
   if (!constructing_path) return fmkr;
   if (writing_file) fprintf(TF, "q f Q\n");
   c_clip(Get_FM(fmkr));
   return fmkr;
}

VALUE FM_stroke_and_clip(VALUE fmkr)
{
   if (!constructing_path) return fmkr;
   if (writing_file) fprintf(TF, "q S Q\n");
   c_clip(Get_FM(fmkr));
   return fmkr;
}

VALUE FM_fill_stroke_and_clip(VALUE fmkr)
{
   if (!constructing_path) return fmkr;
   if (writing_file) fprintf(TF, "q B Q\n");
   c_clip(Get_FM(fmkr));
   return fmkr;
}

/* Combination Path Constructing and Using */

VALUE FM_stroke_line(VALUE fmkr, VALUE x1, VALUE y1, VALUE x2, VALUE y2)
{
   if (constructing_path) rb_raise(rb_eArgError, "Sorry: must finish with current path before calling stroke_line");
   FM_move_to_point(fmkr, x1, y1);
   FM_append_point_to_path(fmkr, x2, y2);
   FM_stroke(fmkr);
   return fmkr;
}

VALUE FM_fill_rect(VALUE fmkr, VALUE x, VALUE y, VALUE width, VALUE height)
{
   if (constructing_path) rb_raise(rb_eArgError, "Sorry: must finish with current path before calling fill_rect");
   FM_append_rect_to_path(fmkr, x, y, width, height);
   FM_fill(fmkr);
   return fmkr;
}

VALUE FM_stroke_rect(VALUE fmkr, VALUE x, VALUE y, VALUE width, VALUE height)
{
   if (constructing_path) rb_raise(rb_eArgError, "Sorry: must finish with current path before calling stroke_rect");
   FM_append_rect_to_path(fmkr, x, y, width, height);
   FM_stroke(fmkr);
   return fmkr;
}

VALUE FM_fill_and_stroke_rect(VALUE fmkr, VALUE x, VALUE y, VALUE width, VALUE height)
{
   if (constructing_path) rb_raise(rb_eArgError, "Sorry: must finish with current path before calling fill_and_stroke_rect");
   FM_append_rect_to_path(fmkr, x, y, width, height);
   FM_fill_and_stroke(fmkr);
   return fmkr;
}

void c_clip_rect(FM *p, double x, double y, double width, double height) // in output coords
{
   double clip_left=x, clip_right, clip_top, clip_bottom=y, clip_width=width, clip_height=height;
   if (clip_width < 0.0) { clip_right = clip_left; clip_width = -clip_width; clip_left -= clip_width; }
   else clip_right = clip_left + clip_width;
   if (clip_height < 0.0) { clip_top = clip_bottom; clip_height = -clip_height; clip_bottom -= clip_height; }
   else clip_top = clip_bottom + clip_height;
   c_append_rect(p, clip_left, clip_bottom, clip_width, clip_height);
   c_clip(p);
   if (clip_left > p->clip_left) p->clip_left = clip_left;
   if (clip_bottom > p->clip_bottom) p->clip_bottom = clip_bottom;
   if (clip_right < p->clip_right) p->clip_right = clip_right;
   if (clip_top < p->clip_top) p->clip_top = clip_top;
}

VALUE FM_clip_rect(VALUE fmkr, VALUE x, VALUE y, VALUE width, VALUE height)
{
   if (constructing_path) rb_raise(rb_eArgError, "Sorry: must finish with current path before calling clip_rect");
   FM *p = Get_FM(fmkr);
   x = rb_Float(x);
   y = rb_Float(y);
   width = rb_Float(width);
   height = rb_Float(height);
   c_clip_rect(p,
      convert_figure_to_output_x(p,NUM2DBL(x)), convert_figure_to_output_y(p,NUM2DBL(y)),
      convert_figure_to_output_dx(p,NUM2DBL(width)), convert_figure_to_output_dy(p,NUM2DBL(height)));
   return fmkr;
}

VALUE FM_clip_oval(VALUE fmkr, VALUE x, VALUE y, VALUE dx, VALUE dy, VALUE angle)
{
   if (constructing_path) rb_raise(rb_eArgError, "Sorry: must finish with current path before calling clip_oval");
   FM_append_oval_to_path(fmkr, x, y, dx, dy, angle);
   FM_clip(fmkr);
   return fmkr;
}

VALUE FM_fill_oval(VALUE fmkr, VALUE x, VALUE y, VALUE dx, VALUE dy, VALUE angle)
{
   if (constructing_path) rb_raise(rb_eArgError, "Sorry: must finish with current path before calling fill_oval");
   FM_append_oval_to_path(fmkr, x, y, dx, dy, angle);
   FM_fill(fmkr);
   return fmkr;
}

VALUE FM_stroke_oval(VALUE fmkr, VALUE x, VALUE y, VALUE dx, VALUE dy, VALUE angle)
{
   if (constructing_path) rb_raise(rb_eArgError, "Sorry: must finish with current path before calling stroke_oval");
   FM_append_oval_to_path(fmkr, x, y, dx, dy, angle);
   FM_stroke(fmkr);
   return fmkr;
}

VALUE FM_fill_and_stroke_oval(VALUE fmkr, VALUE x, VALUE y, VALUE dx, VALUE dy, VALUE angle)
{
   if (constructing_path) rb_raise(rb_eArgError, "Sorry: must finish with current path before calling fill_and_stroke_oval");
   FM_append_oval_to_path(fmkr, x, y, dx, dy, angle);
   FM_fill_and_stroke(fmkr);
   return fmkr;
}

VALUE FM_clip_rounded_rect(VALUE fmkr, VALUE x, VALUE y, VALUE width, VALUE height, VALUE dx, VALUE dy)
{
   if (constructing_path) rb_raise(rb_eArgError, "Sorry: must finish with current path before calling clip_rounded_rect");
   FM_append_rounded_rect_to_path(fmkr, x, y, width, height, dx, dy);
   FM_clip(fmkr);
   return fmkr;
}

VALUE FM_fill_rounded_rect(VALUE fmkr, VALUE x, VALUE y, VALUE width, VALUE height, VALUE dx, VALUE dy)
{
   if (constructing_path) rb_raise(rb_eArgError, "Sorry: must finish with current path before calling fill_rounded_rect");
   FM_append_rounded_rect_to_path(fmkr, x, y, width, height, dx, dy);
   FM_fill(fmkr);
   return fmkr;
}

VALUE FM_stroke_rounded_rect(VALUE fmkr, VALUE x, VALUE y, VALUE width, VALUE height, VALUE dx, VALUE dy)
{
   if (constructing_path) rb_raise(rb_eArgError, "Sorry: must finish with current path before calling stroke_rounded_rect");
   FM_append_rounded_rect_to_path(fmkr, x, y, width, height, dx, dy);
   FM_stroke(fmkr);
   return fmkr;
}

VALUE FM_fill_and_stroke_rounded_rect(VALUE fmkr, VALUE x, VALUE y, VALUE width, VALUE height, VALUE dx, VALUE dy)
{
   if (constructing_path) rb_raise(rb_eArgError, "Sorry: must finish with current path before calling fill_and_stroke_rounded_rect");
   FM_append_rounded_rect_to_path(fmkr, x, y, width, height, dx, dy);
   FM_fill_and_stroke(fmkr);
   return fmkr;
}

VALUE FM_clip_circle(VALUE fmkr, VALUE x, VALUE y, VALUE dx)
{
   if (constructing_path) rb_raise(rb_eArgError, "Sorry: must finish with current path before calling clip_circle");
   FM_append_circle_to_path(fmkr, x, y, dx);
   FM_clip(fmkr);
   return fmkr;
}

VALUE FM_fill_circle(VALUE fmkr, VALUE x, VALUE y, VALUE dx)
{
   if (constructing_path) rb_raise(rb_eArgError, "Sorry: must finish with current path before calling fill_circle");
   FM_append_circle_to_path(fmkr, x, y, dx);
   FM_fill(fmkr);
   return fmkr;
}

VALUE FM_stroke_circle(VALUE fmkr, VALUE x, VALUE y, VALUE dx)
{
   if (constructing_path) rb_raise(rb_eArgError, "Sorry: must finish with current path before calling stroke_circle");
   FM_append_circle_to_path(fmkr, x, y, dx);
   FM_stroke(fmkr);
   return fmkr;
}

VALUE FM_fill_and_stroke_circle(VALUE fmkr, VALUE x, VALUE y, VALUE dx)
{
   if (constructing_path) rb_raise(rb_eArgError, "Sorry: must finish with current path before calling fill_and_stroke_circle");
   FM_append_circle_to_path(fmkr, x, y, dx);
   FM_fill_and_stroke(fmkr);
   return fmkr;
}

static void c_append_frame(FM *p, bool clip)
{
   double frame_left = convert_page_to_output_x(p, p->frame_left);
   double frame_bottom = convert_page_to_output_y(p, p->frame_bottom);
   double frame_width = convert_page_to_output_dx(p, p->frame_width);
   double frame_height = convert_page_to_output_dy(p, p->frame_height);
   double frame_right = frame_left + frame_width;
   double frame_top = frame_bottom + frame_height;
   c_append_rect(p, frame_left, frame_bottom, frame_width, frame_height);
   if (!clip) return;
   if (frame_left > p->clip_left) p->clip_left = frame_left;
   if (frame_bottom > p->clip_bottom) p->clip_bottom = frame_bottom;
   if (frame_right < p->clip_right) p->clip_right = frame_right;
   if (frame_top < p->clip_top) p->clip_top = frame_top;
}

VALUE FM_append_frame_to_path(VALUE fmkr)
{
   FM *p = Get_FM(fmkr);
   c_append_frame(p, false);
   return fmkr;
}

VALUE FM_fill_frame(VALUE fmkr)
{
   FM *p = Get_FM(fmkr);
   if (constructing_path) rb_raise(rb_eArgError, "Sorry: must finish with current path before calling fill_frame");
   c_append_frame(p, false); FM_fill(fmkr);
   return fmkr;
}

VALUE FM_stroke_frame(VALUE fmkr)
{
   FM *p = Get_FM(fmkr);
   if (constructing_path) rb_raise(rb_eArgError, "Sorry: must finish with current path before calling stroke_frame");
   c_append_frame(p, false); FM_stroke(fmkr);
   return fmkr;
}

VALUE FM_fill_and_stroke_frame(VALUE fmkr)
{
   FM *p = Get_FM(fmkr);
   if (constructing_path) rb_raise(rb_eArgError, "Sorry: must finish with current path before calling fill_and_stroke_frame");
   c_append_frame(p, false); FM_fill_and_stroke(fmkr);
   return fmkr;
}

VALUE FM_clip_to_frame(VALUE fmkr)
{
   FM *p = Get_FM(fmkr);
   if (constructing_path) rb_raise(rb_eArgError, "Sorry: must finish with current path before calling clip_to_frame");
   c_append_frame(p, true); FM_clip(fmkr);
   return fmkr;
}
