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

void Unpack_RGB(OBJ_PTR rgb, double *rp, double *gp, double *bp)
{
   if (rgb == Qnil) { *rp = *gp = *bp = 0.0; return; }
   if (Array_Len(rgb) != 3) RAISE_ERROR("Sorry: invalid rgb array for setting color: must have 3 entries");
   OBJ_PTR entry = Array_Entry(rgb, 0);
   double r = Number_to_double(entry);
   entry = Array_Entry(rgb, 1);
   double g = Number_to_double(entry);
   entry = Array_Entry(rgb, 2);
   double b = Number_to_double(entry);
   if (r < 0.0 || r > 1.0) RAISE_ERROR_g("Sorry: invalid red (%g) for color: must be between 0 and 1", r);
   if (g < 0.0 || g > 1.0) RAISE_ERROR_g("Sorry: invalid green (%g) for color: must be between 0 and 1", g);
   if (b < 0.0 || b > 1.0) RAISE_ERROR_g("Sorry: invalid blue (%g) for color: must be between 0 and 1", b);
   *rp = r; *gp = g; *bp = b;
}

void c_stroke_color_set(FM *p, double r, double g, double b)
{
   if (writing_file) fprintf(TF, "%0.3f %0.3f %0.3f RG\n", r, g, b);
   p->stroke_color_R = r;
   p->stroke_color_G = g;
   p->stroke_color_B = b;
}

OBJ_PTR FM_stroke_color_set(OBJ_PTR fmkr, OBJ_PTR value) // value is array of [r, g, b] intensities from 0 to 1
{  // r g b RG
   FM *p = Get_FM(fmkr);
   double r, g, b;
   Unpack_RGB(value, &r, &g, &b);
   c_stroke_color_set(p, r, g, b);
   return value;
}

OBJ_PTR FM_stroke_color_get(OBJ_PTR fmkr) // OBJ_PTR is array of [r, g, b] intensities from 0 to 1
{  // r g b RG
   FM *p = Get_FM(fmkr);
   double r, g, b;
   r = p->stroke_color_R;
   g = p->stroke_color_G;
   b = p->stroke_color_B;
   OBJ_PTR result = Array_New(3);
   Array_Store(result, 0, Float_New(r));
   Array_Store(result, 1, Float_New(g));
   Array_Store(result, 2, Float_New(b));
   return result;
}

void c_fill_color_set(FM *p, double r, double g, double b)
{
   if (writing_file) fprintf(TF, "%0.3f %0.3f %0.3f rg\n", r, g, b);
   p->fill_color_R = r;
   p->fill_color_G = g;
   p->fill_color_B = b;
}

OBJ_PTR FM_fill_color_set(OBJ_PTR fmkr, OBJ_PTR value) // value is array of [r, g, b] intensities from 0 to 1
{ // r g b rg
   FM *p = Get_FM(fmkr);
   double r, g, b;
   Unpack_RGB(value, &r, &g, &b);
   c_fill_color_set(p, r, g, b);
   return value;
}

OBJ_PTR FM_fill_color_get(OBJ_PTR fmkr) // OBJ_PTR is array of [r, g, b] intensities from 0 to 1
{  // r g b RG
   FM *p = Get_FM(fmkr);
   double r, g, b;
   r = p->fill_color_R;
   g = p->fill_color_G;
   b = p->fill_color_B;
   OBJ_PTR result = Array_New(3);
   Array_Store(result, 0, Float_New(r));
   Array_Store(result, 1, Float_New(g));
   Array_Store(result, 2, Float_New(b));
   return result;
}

void c_line_width_set(FM *p, double line_width)
{
   if (line_width < 0.0) RAISE_ERROR_g("Sorry: invalid line width (%g points): must be positive", line_width);
   if (line_width > 1e3) RAISE_ERROR_g("Sorry: too large line width (%g points)", line_width);
   if (writing_file) fprintf(TF, "%0.3f w\n", line_width * ENLARGE * p->default_line_scale);
   p->line_width = line_width;
}

OBJ_PTR FM_line_width_set(OBJ_PTR fmkr, OBJ_PTR val)  // val is thickness in points
{ // w
   FM *p = Get_FM(fmkr);
   c_line_width_set(p, Number_to_double(val));
   return val;
}

static void c_line_scale_set(FM *p, double new_scale)
{
   if (new_scale <= 0) RAISE_ERROR("Sorry: line scale must be positive");
   p->default_line_scale = new_scale;
   c_line_width_set(p, p->line_width);
}

OBJ_PTR FM_rescale_lines(OBJ_PTR fmkr, OBJ_PTR scaling_factor)
{
   FM *p = Get_FM(fmkr);
   c_line_scale_set(p, Number_to_double(scaling_factor) * p->default_line_scale);
   return fmkr;
}

void c_line_cap_set(FM *p, int line_cap)
{
   if (line_cap < 0 || line_cap > 3) RAISE_ERROR_i("Sorry: invalid arg for setting line_cap (%i)", line_cap);
   if (writing_file) fprintf(TF, "%d J\n", line_cap);
   p->line_cap = line_cap;
}

OBJ_PTR FM_line_cap_set(OBJ_PTR fmkr, OBJ_PTR val)
{ // J
   FM *p = Get_FM(fmkr);
   c_line_cap_set(p, Number_to_int(val));
   return val;
}

void c_line_join_set(FM *p, int line_join)
{
   if (line_join < 0 || line_join > 3) RAISE_ERROR_i("Sorry: invalid arg for setting line_join (%i)", line_join);
   if (writing_file) fprintf(TF, "%d j\n", line_join);
   p->line_join = line_join;
}

OBJ_PTR FM_line_join_set(OBJ_PTR fmkr, OBJ_PTR val)
{  // j
   FM *p = Get_FM(fmkr);
   c_line_join_set(p, Number_to_int(val));
   return val;
}

void c_miter_limit_set(FM *p, double miter_limit)
{
   if (miter_limit < 0.0)
      RAISE_ERROR_g(
         "Sorry: invalid miter limit (%g): must be positive ratio for max miter length to line width", miter_limit);
   if (writing_file) fprintf(TF, "%0.3f M\n", miter_limit);
   p->miter_limit = miter_limit;
}

OBJ_PTR FM_miter_limit_set(OBJ_PTR fmkr, OBJ_PTR val) // val is max ratio of miter length to line width
{ // M
   FM *p = Get_FM(fmkr);
   if (constructing_path) RAISE_ERROR("Sorry: must not be constructing a path when change miter limit");
   c_miter_limit_set(p, Number_to_double(val));
   return val;
}

OBJ_PTR FM_line_type_set(OBJ_PTR fmkr, OBJ_PTR line_type)
{ // array phase d  (distances given in points)
   FM *p = Get_FM(fmkr);
   double sz;
   if (constructing_path) RAISE_ERROR("Sorry: must not be constructing a path when change line_type");
   if (line_type == Qnil) {
      fprintf(TF, "[] 0 d\n");
   } else {
      if (writing_file) {
         if (Array_Len(line_type) != 2)
            RAISE_ERROR("Sorry: invalid line_type.  Must be [ [dash pattern] dash phase ]");
         OBJ_PTR dashArray = Array_Entry(line_type, 0), dashPhase = Array_Entry(line_type, 1);
         fprintf(TF, "[ ");
         if (dashArray != Qnil) {
            long i, len = Array_Len(dashArray);
            for (i=0; i < len; i++) {
               OBJ_PTR entry = Array_Entry(dashArray, i);
               sz = Number_to_double(entry);
               if (sz < 0.0) RAISE_ERROR_g("Sorry: invalid dash array entry (%g): must be positive", sz);
               fprintf(TF, "%0.3f ", sz * ENLARGE);
            }
         }
         sz = Number_to_double(dashPhase);
         if (sz < 0.0) RAISE_ERROR_g("Sorry: invalid dash phase (%g): must be positive", sz);
         fprintf(TF, "] %0.3f d\n", sz * ENLARGE);
      }
   }
   Set_line_type(fmkr, line_type);
   return fmkr;
}


/* Path construction operators */

    /* all locations and vectors are in figure coordinates */

static long c_round_dev(FM *p, double v) { // make sure that we don't get too far out or can overflow!
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

OBJ_PTR FM_update_bbox(OBJ_PTR fmkr, OBJ_PTR x, OBJ_PTR y)
{
   FM *p = Get_FM(fmkr);
   update_bbox(p, convert_figure_to_output_x(p,Number_to_double(x)), convert_figure_to_output_y(p,Number_to_double(y)));
   return fmkr;
}


OBJ_PTR FM_bbox_left(OBJ_PTR fmkr)
{
   return Float_New(bbox_llx); 
}

OBJ_PTR FM_bbox_right(OBJ_PTR fmkr)
{
   return Float_New(bbox_urx); 
}

OBJ_PTR FM_bbox_top(OBJ_PTR fmkr)
{
   return Float_New(bbox_ury); 
}

OBJ_PTR FM_bbox_bottom(OBJ_PTR fmkr)
{
   return Float_New(bbox_lly); 
}



void c_moveto(FM *p, double x, double y)
{
   ARE_OK_NUMBERS(x,y);
   if (writing_file) fprintf(TF, "%ld %ld m\n", c_round_dev(p,x), c_round_dev(p,y));
   update_bbox(p, x, y);
   have_current_point = constructing_path = true;
}

OBJ_PTR FM_move_to_point(OBJ_PTR fmkr, OBJ_PTR x, OBJ_PTR y)
{
   FM *p = Get_FM(fmkr);
   double dev_x = convert_figure_to_output_x(p,Number_to_double(x)), dev_y = convert_figure_to_output_y(p,Number_to_double(y));
   c_moveto(p, dev_x, dev_y);
   return fmkr;
}

void c_lineto(FM *p, double x, double y)
{
   ARE_OK_NUMBERS(x,y);
   if (!constructing_path) RAISE_ERROR("Sorry: must start path with moveto before call lineto");
   if (writing_file) fprintf(TF, "%ld %ld l\n", c_round_dev(p,x), c_round_dev(p,y));
   update_bbox(p, x, y);
}

OBJ_PTR FM_append_point_to_path(OBJ_PTR fmkr, OBJ_PTR x, OBJ_PTR y)
{
   FM *p = Get_FM(fmkr);
   double dev_x = convert_figure_to_output_x(p,Number_to_double(x)), dev_y = convert_figure_to_output_y(p,Number_to_double(y));
   c_lineto(p, dev_x, dev_y);
   return fmkr;
}


void c_bezier_control_points(double *data, double x0, double y0, double delta_x, double a, double b, double c)
{
   double x1, y1, x2, y2, x3, y3;
   double cx = delta_x, ay = a * delta_x * delta_x * delta_x, by = b * delta_x * delta_x, cy = c * delta_x;
   x1 = x0 + cx/3.0; x2 = x1 + cx/3.0; x3 = x0 + delta_x;
   y1 = y0 + cy/3.0; y2 = y1 + (cy + by)/3.0; y3 = y0 + ay + by + cy;
   data[0] = x1; data[1] = y1; data[2] = x2; data[3] = y2; data[4] = x3; data[5] = y3;
}

OBJ_PTR FM_bezier_control_points(OBJ_PTR fmkr, VALUE x0, VALUE y0, VALUE delta_x, VALUE a, VALUE b, VALUE c)
{
   double data[6];
   c_bezier_control_points(data,
      Number_to_double(x0), Number_to_double(y0), Number_to_double(delta_x),
      Number_to_double(a), Number_to_double(b), Number_to_double(c));
   return Vector_New(data,6);
}


void c_curveto(FM *p, double x1, double y1, double x2, double y2, double x3, double y3)
{
   ARE_OK_NUMBERS(x1,y1);
   ARE_OK_NUMBERS(x2,y2);
   ARE_OK_NUMBERS(x3,y3);
   if (!constructing_path) RAISE_ERROR("Sorry: must start path with moveto before call curveto");
   if (writing_file) fprintf(TF, "%ld %ld %ld %ld %ld %ld c\n", 
            c_round_dev(p,x1), c_round_dev(p,y1), c_round_dev(p,x2), c_round_dev(p,y2), c_round_dev(p,x3), c_round_dev(p,y3));
   update_bbox(p, x1, y1);
   update_bbox(p, x2, y2);
   update_bbox(p, x3, y3);
}

OBJ_PTR FM_append_curve_to_path(OBJ_PTR fmkr, OBJ_PTR x1, OBJ_PTR y1, OBJ_PTR x2, OBJ_PTR y2, OBJ_PTR x3, OBJ_PTR y3)
{
   FM *p = Get_FM(fmkr);
   double dev_x1 = convert_figure_to_output_x(p,Number_to_double(x1)), dev_y1 = convert_figure_to_output_y(p,Number_to_double(y1));
   double dev_x2 = convert_figure_to_output_x(p,Number_to_double(x2)), dev_y2 = convert_figure_to_output_y(p,Number_to_double(y2));
   double dev_x3 = convert_figure_to_output_x(p,Number_to_double(x3)), dev_y3 = convert_figure_to_output_y(p,Number_to_double(y3));
   c_curveto(p, dev_x1, dev_y1, dev_x2, dev_y2, dev_x3, dev_y3);
   return fmkr;
}

static void c_closepath(FM *p)
{
   if (!constructing_path) RAISE_ERROR("Sorry: must be constructing path when call closepath");
   if (writing_file) fprintf(TF, "h\n");
   have_current_point = false;
   p = NULL;
}

OBJ_PTR FM_close_path(OBJ_PTR fmkr)
{
   c_closepath(Get_FM(fmkr));
   return fmkr;
}

static void c_append_arc(FM *p, double x_start, double y_start, double x_corner, double y_corner,
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
   if (theta >= PI) RAISE_ERROR("Sorry: invalid control point for arc");
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

static double Get_Arc_Radius(FM *p, OBJ_PTR dx, OBJ_PTR dy)
{
   double rx = Number_to_double(dx), ry = Number_to_double(dy);
   rx = convert_figure_to_output_dx(p,rx);
   ry = convert_figure_to_output_dy(p,ry);
   if (rx < 0) rx = -rx;
   if (ry < 0) ry = -ry;
   return MIN(rx,ry);
}

OBJ_PTR FM_append_arc_to_path(OBJ_PTR fmkr, OBJ_PTR x_start, OBJ_PTR y_start, OBJ_PTR x_corner, OBJ_PTR y_corner,
   OBJ_PTR x_end, OBJ_PTR y_end, OBJ_PTR dx, OBJ_PTR dy)
{
    // dx and dy are converted to output coords and smaller is used as radius
   FM *p = Get_FM(fmkr);
   c_append_arc(p,
      convert_figure_to_output_x(p,Number_to_double(x_start)), convert_figure_to_output_y(p,Number_to_double(y_start)),
      convert_figure_to_output_x(p,Number_to_double(x_corner)), convert_figure_to_output_y(p,Number_to_double(y_corner)),
      convert_figure_to_output_x(p,Number_to_double(x_end)), convert_figure_to_output_y(p,Number_to_double(y_end)),
      Get_Arc_Radius(p,dx,dy));
   return fmkr;
}

static void c_append_rect(FM *p, double x, double y, double width, double height)
{
   c_moveto(p,x,y);
   c_lineto(p,x+width,y);
   c_lineto(p,x+width,y+height);
   c_lineto(p,x,y+height);
   c_closepath(p);
}

OBJ_PTR FM_append_rect_to_path(OBJ_PTR fmkr, OBJ_PTR x, OBJ_PTR y, OBJ_PTR width, OBJ_PTR height)
{
   FM *p = Get_FM(fmkr);
   c_append_rect(p,
      convert_figure_to_output_x(p,Number_to_double(x)), convert_figure_to_output_y(p,Number_to_double(y)),
      convert_figure_to_output_dx(p,Number_to_double(width)), convert_figure_to_output_dy(p,Number_to_double(height)));
   return fmkr;
}

static void c_append_rounded_rect(FM *p, double x, double y, double width, double height, double radius)
{
   double xc = x + width/2, yc = y + height/2, xp = x + width, yp = y + height;
   c_moveto(p,xc,y);
   c_append_arc(p,xc,y, xp,y, xp,yc, radius);
   c_append_arc(p,xp,yc, xp,yp, xc,yp, radius);
   c_append_arc(p,xc,yp, x,yp, x,yc, radius);
   c_append_arc(p,x,yc, x,y, xc,y, radius);
   c_closepath(p);
}

OBJ_PTR FM_append_rounded_rect_to_path(OBJ_PTR fmkr, OBJ_PTR x, OBJ_PTR y, OBJ_PTR width, OBJ_PTR height, OBJ_PTR dx, OBJ_PTR dy)
{
    // dx and dy are converted to output coords and smaller is used as radius
   FM *p = Get_FM(fmkr);
   c_append_rounded_rect(p,
      convert_figure_to_output_x(p,Number_to_double(x)), convert_figure_to_output_y(p,Number_to_double(y)),
      convert_figure_to_output_dx(p,Number_to_double(width)), convert_figure_to_output_dy(p,Number_to_double(height)), 
      Get_Arc_Radius(p,dx,dy));
   return fmkr;
}

#define ROTATE90(x,y) tmp = x; x = y; y = -tmp;
#define TRANSFORM(xp,yp,x,y) xp = a*(x)+c*(y)+e; yp = b*(x)+d*(y)+f;

static void c_append_oval(FM *p, double x, double y, double dx, double dy, double angle)
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

OBJ_PTR FM_append_oval_to_path(OBJ_PTR fmkr, OBJ_PTR x, OBJ_PTR y, OBJ_PTR dx, OBJ_PTR dy, OBJ_PTR angle)
{
   FM *p = Get_FM(fmkr);
   c_append_oval(p,
      convert_figure_to_output_x(p,Number_to_double(x)), convert_figure_to_output_y(p,Number_to_double(y)),
      convert_figure_to_output_dx(p,Number_to_double(dx)), convert_figure_to_output_dy(p,Number_to_double(dy)), 
      Number_to_double(angle));
   return fmkr;
}

OBJ_PTR FM_append_circle_to_path(OBJ_PTR fmkr, OBJ_PTR x, OBJ_PTR y, OBJ_PTR dx)
{
   FM *p = Get_FM(fmkr);
   double s = convert_figure_to_output_dx(p,Number_to_double(dx));
   c_append_oval(p,
      convert_figure_to_output_x(p,Number_to_double(x)), convert_figure_to_output_y(p,Number_to_double(y)),
      s, s, 0.0);
   return fmkr;
}

OBJ_PTR FM_append_points_to_path(OBJ_PTR fmkr, OBJ_PTR x_vec, OBJ_PTR y_vec)
{
   FM *p = Get_FM(fmkr);
   long xlen, ylen, i;
   double x0, y0;
   double *xs = Vector_Data_for_Read(x_vec, &xlen);
   double *ys = Vector_Data_for_Read(y_vec, &ylen);
   if (xlen != ylen) RAISE_ERROR("Sorry: must have same number xs and ys for append_points");
   if (xlen <= 0) return fmkr;
   x0 = convert_figure_to_output_x(p,xs[0]); y0 = convert_figure_to_output_y(p,ys[0]);
   if (have_current_point) c_lineto(p,x0,y0);
   else c_moveto(p,x0,y0);
   for (i=1; i<xlen; i++)
      c_lineto(p,convert_figure_to_output_x(p,xs[i]), convert_figure_to_output_y(p,ys[i]));
   return fmkr;
}

OBJ_PTR FM_private_append_points_with_gaps_to_path(OBJ_PTR fmkr, OBJ_PTR x_vec, OBJ_PTR y_vec, OBJ_PTR gaps, OBJ_PTR close_gaps)
    // where there's a gap, do a moveto instead of a lineto
{
   if (gaps == Qnil) return FM_append_points_to_path(fmkr, x_vec, y_vec);
   FM *p = Get_FM(fmkr);
   long xlen, ylen, glen, i, j;
   double x0, y0;
   double *xs = Vector_Data_for_Read(x_vec, &xlen);
   double *ys = Vector_Data_for_Read(y_vec, &ylen);
   double *gs = Vector_Data_for_Read(gaps, &glen);
   bool do_close = (close_gaps == Qtrue);
   if (xlen != ylen) RAISE_ERROR("Sorry: must have same number xs and ys for append_points_with_gaps");
   if (xlen <= 0) return fmkr;
   x0 = convert_figure_to_output_x(p,xs[0]); y0 = convert_figure_to_output_y(p,ys[0]);
   if (have_current_point) c_lineto(p,x0,y0);
   else c_moveto(p,x0,y0);
   for (i = 1, j = 0; j < glen; j++) {
      int gap_start = ROUND(gs[j]);
      if (gap_start == xlen) break;
      if (gap_start > xlen)
         RAISE_ERROR_ii("Sorry: gap value (%i) too large for vectors of length (%i)", gap_start, xlen);
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

OBJ_PTR FM_stroke(OBJ_PTR fmkr)
{
   if (!constructing_path) return fmkr;
   if (writing_file) fprintf(TF, "S\n");
   have_current_point = constructing_path = false;
   return fmkr;
}

OBJ_PTR FM_close_and_stroke(OBJ_PTR fmkr)
{
   if (!constructing_path) return fmkr;
   if (writing_file) fprintf(TF, "s\n");
   have_current_point = constructing_path = false;
   return fmkr;
}

OBJ_PTR FM_fill(OBJ_PTR fmkr)
{
   if (!constructing_path) return fmkr;
   if (writing_file) fprintf(TF, "f\n");
   have_current_point = constructing_path = false;
   return fmkr;
}

OBJ_PTR FM_discard_path(OBJ_PTR fmkr)
{
   if (!constructing_path) return fmkr;
   if (writing_file) fprintf(TF, "n\n");
   have_current_point = constructing_path = false;
   return fmkr;
}

OBJ_PTR FM_eofill(OBJ_PTR fmkr)
{
   if (!constructing_path) return fmkr;
   if (writing_file) fprintf(TF, "f*\n");
   have_current_point = constructing_path = false;
   return fmkr;
}

OBJ_PTR FM_fill_and_stroke(OBJ_PTR fmkr)
{
   if (!constructing_path) return fmkr;
   if (writing_file) fprintf(TF, "B\n");
   have_current_point = constructing_path = false;
   return fmkr;
}

OBJ_PTR FM_eofill_and_stroke(OBJ_PTR fmkr)
{
   if (!constructing_path) return fmkr;
   if (writing_file) fprintf(TF, "B*\n");
   have_current_point = constructing_path = false;
   return fmkr;
}

OBJ_PTR FM_close_fill_and_stroke(OBJ_PTR fmkr)
{
   if (!constructing_path) return fmkr;
   if (writing_file) fprintf(TF, "b\n");
   have_current_point = constructing_path = false;
   return fmkr;
}

OBJ_PTR FM_close_eofill_and_stroke(OBJ_PTR fmkr)
{
   if (!constructing_path) return fmkr;
   if (writing_file) fprintf(TF, "b*\n");
   have_current_point = constructing_path = false;
   return fmkr;
}

static void c_clip(FM *p)
{
   if (!constructing_path) return;
   if (writing_file) fprintf(TF, "W n\n");
   have_current_point = constructing_path = false;
   p = NULL;
}

OBJ_PTR FM_clip(OBJ_PTR fmkr)
{
   c_clip(Get_FM(fmkr));
   return fmkr;
}

OBJ_PTR FM_eoclip(OBJ_PTR fmkr)
{
   if (!constructing_path) return fmkr;
   if (writing_file) fprintf(TF, "W* n\n");
   have_current_point = constructing_path = false;
   return fmkr;
}

OBJ_PTR FM_fill_and_clip(OBJ_PTR fmkr)
{
   if (!constructing_path) return fmkr;
   if (writing_file) fprintf(TF, "q f Q\n");
   c_clip(Get_FM(fmkr));
   return fmkr;
}

OBJ_PTR FM_stroke_and_clip(OBJ_PTR fmkr)
{
   if (!constructing_path) return fmkr;
   if (writing_file) fprintf(TF, "q S Q\n");
   c_clip(Get_FM(fmkr));
   return fmkr;
}

OBJ_PTR FM_fill_stroke_and_clip(OBJ_PTR fmkr)
{
   if (!constructing_path) return fmkr;
   if (writing_file) fprintf(TF, "q B Q\n");
   c_clip(Get_FM(fmkr));
   return fmkr;
}

/* Combination Path Constructing and Using */

OBJ_PTR FM_stroke_line(OBJ_PTR fmkr, OBJ_PTR x1, OBJ_PTR y1, OBJ_PTR x2, OBJ_PTR y2)
{
   if (constructing_path) RAISE_ERROR("Sorry: must finish with current path before calling stroke_line");
   FM_move_to_point(fmkr, x1, y1);
   FM_append_point_to_path(fmkr, x2, y2);
   FM_stroke(fmkr);
   return fmkr;
}

OBJ_PTR FM_fill_rect(OBJ_PTR fmkr, OBJ_PTR x, OBJ_PTR y, OBJ_PTR width, OBJ_PTR height)
{
   if (constructing_path) RAISE_ERROR("Sorry: must finish with current path before calling fill_rect");
   FM_append_rect_to_path(fmkr, x, y, width, height);
   FM_fill(fmkr);
   return fmkr;
}

OBJ_PTR FM_stroke_rect(OBJ_PTR fmkr, OBJ_PTR x, OBJ_PTR y, OBJ_PTR width, OBJ_PTR height)
{
   if (constructing_path) RAISE_ERROR("Sorry: must finish with current path before calling stroke_rect");
   FM_append_rect_to_path(fmkr, x, y, width, height);
   FM_stroke(fmkr);
   return fmkr;
}

OBJ_PTR FM_fill_and_stroke_rect(OBJ_PTR fmkr, OBJ_PTR x, OBJ_PTR y, OBJ_PTR width, OBJ_PTR height)
{
   if (constructing_path) RAISE_ERROR("Sorry: must finish with current path before calling fill_and_stroke_rect");
   FM_append_rect_to_path(fmkr, x, y, width, height);
   FM_fill_and_stroke(fmkr);
   return fmkr;
}

static void c_clip_rect(FM *p, double x, double y, double width, double height) // in output coords
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

OBJ_PTR FM_clip_rect(OBJ_PTR fmkr, OBJ_PTR x, OBJ_PTR y, OBJ_PTR width, OBJ_PTR height)
{
   if (constructing_path) RAISE_ERROR("Sorry: must finish with current path before calling clip_rect");
   FM *p = Get_FM(fmkr);
   c_clip_rect(p,
      convert_figure_to_output_x(p,Number_to_double(x)), convert_figure_to_output_y(p,Number_to_double(y)),
      convert_figure_to_output_dx(p,Number_to_double(width)), convert_figure_to_output_dy(p,Number_to_double(height)));
   return fmkr;
}

OBJ_PTR FM_clip_oval(OBJ_PTR fmkr, OBJ_PTR x, OBJ_PTR y, OBJ_PTR dx, OBJ_PTR dy, OBJ_PTR angle)
{
   if (constructing_path) RAISE_ERROR("Sorry: must finish with current path before calling clip_oval");
   FM_append_oval_to_path(fmkr, x, y, dx, dy, angle);
   FM_clip(fmkr);
   return fmkr;
}

OBJ_PTR FM_fill_oval(OBJ_PTR fmkr, OBJ_PTR x, OBJ_PTR y, OBJ_PTR dx, OBJ_PTR dy, OBJ_PTR angle)
{
   if (constructing_path) RAISE_ERROR("Sorry: must finish with current path before calling fill_oval");
   FM_append_oval_to_path(fmkr, x, y, dx, dy, angle);
   FM_fill(fmkr);
   return fmkr;
}

OBJ_PTR FM_stroke_oval(OBJ_PTR fmkr, OBJ_PTR x, OBJ_PTR y, OBJ_PTR dx, OBJ_PTR dy, OBJ_PTR angle)
{
   if (constructing_path) RAISE_ERROR("Sorry: must finish with current path before calling stroke_oval");
   FM_append_oval_to_path(fmkr, x, y, dx, dy, angle);
   FM_stroke(fmkr);
   return fmkr;
}

OBJ_PTR FM_fill_and_stroke_oval(OBJ_PTR fmkr, OBJ_PTR x, OBJ_PTR y, OBJ_PTR dx, OBJ_PTR dy, OBJ_PTR angle)
{
   if (constructing_path) RAISE_ERROR("Sorry: must finish with current path before calling fill_and_stroke_oval");
   FM_append_oval_to_path(fmkr, x, y, dx, dy, angle);
   FM_fill_and_stroke(fmkr);
   return fmkr;
}

OBJ_PTR FM_clip_rounded_rect(OBJ_PTR fmkr, OBJ_PTR x, OBJ_PTR y, OBJ_PTR width, OBJ_PTR height, OBJ_PTR dx, OBJ_PTR dy)
{
   if (constructing_path) RAISE_ERROR("Sorry: must finish with current path before calling clip_rounded_rect");
   FM_append_rounded_rect_to_path(fmkr, x, y, width, height, dx, dy);
   FM_clip(fmkr);
   return fmkr;
}

OBJ_PTR FM_fill_rounded_rect(OBJ_PTR fmkr, OBJ_PTR x, OBJ_PTR y, OBJ_PTR width, OBJ_PTR height, OBJ_PTR dx, OBJ_PTR dy)
{
   if (constructing_path) RAISE_ERROR("Sorry: must finish with current path before calling fill_rounded_rect");
   FM_append_rounded_rect_to_path(fmkr, x, y, width, height, dx, dy);
   FM_fill(fmkr);
   return fmkr;
}

OBJ_PTR FM_stroke_rounded_rect(OBJ_PTR fmkr, OBJ_PTR x, OBJ_PTR y, OBJ_PTR width, OBJ_PTR height, OBJ_PTR dx, OBJ_PTR dy)
{
   if (constructing_path) RAISE_ERROR("Sorry: must finish with current path before calling stroke_rounded_rect");
   FM_append_rounded_rect_to_path(fmkr, x, y, width, height, dx, dy);
   FM_stroke(fmkr);
   return fmkr;
}

OBJ_PTR FM_fill_and_stroke_rounded_rect(OBJ_PTR fmkr, OBJ_PTR x, OBJ_PTR y, OBJ_PTR width, OBJ_PTR height, OBJ_PTR dx, OBJ_PTR dy)
{
   if (constructing_path) RAISE_ERROR("Sorry: must finish with current path before calling fill_and_stroke_rounded_rect");
   FM_append_rounded_rect_to_path(fmkr, x, y, width, height, dx, dy);
   FM_fill_and_stroke(fmkr);
   return fmkr;
}

OBJ_PTR FM_clip_circle(OBJ_PTR fmkr, OBJ_PTR x, OBJ_PTR y, OBJ_PTR dx)
{
   if (constructing_path) RAISE_ERROR("Sorry: must finish with current path before calling clip_circle");
   FM_append_circle_to_path(fmkr, x, y, dx);
   FM_clip(fmkr);
   return fmkr;
}

OBJ_PTR FM_fill_circle(OBJ_PTR fmkr, OBJ_PTR x, OBJ_PTR y, OBJ_PTR dx)
{
   if (constructing_path) RAISE_ERROR("Sorry: must finish with current path before calling fill_circle");
   FM_append_circle_to_path(fmkr, x, y, dx);
   FM_fill(fmkr);
   return fmkr;
}

OBJ_PTR FM_stroke_circle(OBJ_PTR fmkr, OBJ_PTR x, OBJ_PTR y, OBJ_PTR dx)
{
   if (constructing_path) RAISE_ERROR("Sorry: must finish with current path before calling stroke_circle");
   FM_append_circle_to_path(fmkr, x, y, dx);
   FM_stroke(fmkr);
   return fmkr;
}

OBJ_PTR FM_fill_and_stroke_circle(OBJ_PTR fmkr, OBJ_PTR x, OBJ_PTR y, OBJ_PTR dx)
{
   if (constructing_path) RAISE_ERROR("Sorry: must finish with current path before calling fill_and_stroke_circle");
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

OBJ_PTR FM_append_frame_to_path(OBJ_PTR fmkr)
{
   FM *p = Get_FM(fmkr);
   c_append_frame(p, false);
   return fmkr;
}

OBJ_PTR FM_fill_frame(OBJ_PTR fmkr)
{
   FM *p = Get_FM(fmkr);
   if (constructing_path) RAISE_ERROR("Sorry: must finish with current path before calling fill_frame");
   c_append_frame(p, false); FM_fill(fmkr);
   return fmkr;
}

OBJ_PTR FM_stroke_frame(OBJ_PTR fmkr)
{
   FM *p = Get_FM(fmkr);
   if (constructing_path) RAISE_ERROR("Sorry: must finish with current path before calling stroke_frame");
   c_append_frame(p, false); FM_stroke(fmkr);
   return fmkr;
}

OBJ_PTR FM_fill_and_stroke_frame(OBJ_PTR fmkr)
{
   FM *p = Get_FM(fmkr);
   if (constructing_path) RAISE_ERROR("Sorry: must finish with current path before calling fill_and_stroke_frame");
   c_append_frame(p, false); FM_fill_and_stroke(fmkr);
   return fmkr;
}

OBJ_PTR FM_clip_to_frame(OBJ_PTR fmkr)
{
   FM *p = Get_FM(fmkr);
   if (constructing_path) RAISE_ERROR("Sorry: must finish with current path before calling clip_to_frame");
   c_append_frame(p, true); FM_clip(fmkr);
   return fmkr;
}
