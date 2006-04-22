/* pdfcolor.c */
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

/* functions */

void Free_Functions()
{
   Function_Info *fo;
   while (functions_list != NULL) {
      fo = functions_list;
      functions_list = fo->next;
      if (fo->lookup != NULL) free(fo->lookup);
      free(fo);
   }
}

static void Write_Sampled_Function(Function_Info *fo)
{
   fprintf(OF, "%i 0 obj << /FunctionType 0\n", fo->obj_num);
   fprintf(OF, "\t/Domain [0 1]\n");
   fprintf(OF, "\t/Range [0 1 0 1 0 1]\n");
   fprintf(OF, "\t/Size [%i]\n", fo->hival + 1);
   fprintf(OF, "\t/BitsPerSample 8\n");
   fprintf(OF, "\t/Order 1\n");
   fprintf(OF, "\t/Length %i\n\t>>\nstream\n", fo->lookup_len);
   if (fwrite(fo->lookup, 1, fo->lookup_len, OF) < fo->lookup_len)
      rb_raise(rb_eArgError, "Error writing function sample data");
   fprintf(OF, "\nendstream\nendobj\n");
}

void Write_Functions(void)
{
   Function_Info *fo;
   for (fo = functions_list; fo != NULL; fo = fo->next) {
      Record_Object_Offset(fo->obj_num);
      Write_Sampled_Function(fo);
   }
}

static int create_function(int hival, int lookup_len, unsigned char *lookup)
{
   Function_Info *fo = ALLOC(Function_Info);
   fo->next = functions_list;
   functions_list = fo;
   fo->lookup = ALLOC_N(unsigned char, lookup_len);
   MEMCPY(fo->lookup, lookup, unsigned char, lookup_len);
   fo->lookup_len = lookup_len;
   fo->hival = hival;
   fo->obj_num = next_available_object_number++;
   return fo->obj_num;
}

/* Opacity */

void Free_Stroke_Opacities(void)
{
   Stroke_Opacity_State *p;
   while (stroke_opacities != NULL) {
      p = stroke_opacities;  stroke_opacities = p->next; free(p);
   }
}

static int Get_Stroke_Opacity_XGS(double stroke_opacity)
{
   Stroke_Opacity_State *p;
   for (p = stroke_opacities; p != NULL; p = p->next) {
      if (p->stroke_opacity == stroke_opacity) return p->gs_num;
   }
   p = ALLOC(Stroke_Opacity_State);
   p->stroke_opacity = stroke_opacity;
   p->gs_num = next_available_gs_number++;
   p->obj_num = next_available_object_number++;
   p->next = stroke_opacities;
   stroke_opacities = p;
   return p->gs_num;
}

VALUE FM_stroke_opacity_set(VALUE fmkr, VALUE value)
{  // /GSi gs for ExtGState obj with /CS set to stroke opacity value
   FM *p = Get_FM(fmkr);
   if (constructing_path) rb_raise(rb_eArgError, "Sorry: must not be constructing a path when change stroke opacity");
   value = rb_Float(value);
   double stroke_opacity = NUM2DBL(value);
   if (stroke_opacity == p->stroke_opacity) return value;
   int gs_num = Get_Stroke_Opacity_XGS(stroke_opacity);
   fprintf(TF, "/GS%i gs\n", gs_num);
   p->stroke_opacity = stroke_opacity;
   return value;
}

void Free_Fill_Opacities(void)
{
   Fill_Opacity_State *p;
   while (fill_opacities != NULL) {
      p = fill_opacities;  fill_opacities = p->next; free(p);
   }
}

static int Get_Fill_Opacity_XGS(double fill_opacity)
{
   Fill_Opacity_State *p;
   for (p = fill_opacities; p != NULL; p = p->next) {
      if (p->fill_opacity == fill_opacity) return p->gs_num;
   }
   p = ALLOC(Fill_Opacity_State);
   p->fill_opacity = fill_opacity;
   p->gs_num = next_available_gs_number++;
   p->obj_num = next_available_object_number++;
   p->next = fill_opacities;
   fill_opacities = p;
   return p->gs_num;
}

VALUE FM_fill_opacity_set(VALUE fmkr, VALUE value)
{  // /GSi gs for ExtGState obj with /cs set to fill opacity value
   FM *p = Get_FM(fmkr);
   if (constructing_path) rb_raise(rb_eArgError, "Sorry: must not be constructing a path when change fill opacity");
   value = rb_Float(value);
   double fill_opacity = NUM2DBL(value);
   if (fill_opacity == p->fill_opacity) return value;
   int gs_num = Get_Fill_Opacity_XGS(fill_opacity);
   fprintf(TF, "/GS%i gs\n", gs_num);
   p->fill_opacity = fill_opacity;
   return value;
}

void Write_Stroke_Opacity_Objects(void)
{
   Stroke_Opacity_State *p;
   for (p = stroke_opacities; p != NULL; p = p->next) {
      Record_Object_Offset(p->obj_num);
      fprintf(OF, "%2i 0 obj << /Type /ExtGState /CA %g >> endobj\n", p->obj_num, p->stroke_opacity);
   }
}

void Write_Fill_Opacity_Objects(void)
{
   Fill_Opacity_State *p;
   for (p = fill_opacities; p != NULL; p = p->next) {
      Record_Object_Offset(p->obj_num);
      fprintf(OF, "%2i 0 obj << /Type /ExtGState /ca %g >> endobj\n", p->obj_num, p->fill_opacity);
   }
}

/* Shading */

void Free_Shadings()
{
   Shading_Info *so;
   while (shades_list != NULL) {
      so = shades_list;
      shades_list = so->next;
      free(so);
   }
}

void Write_Shadings(void)
{
   Shading_Info *so;
   for (so = shades_list; so != NULL; so = so->next) {
      Record_Object_Offset(so->obj_num);
      fprintf(OF, "%i 0 obj <<\n", so->obj_num);
      if (so->axial) {
         fprintf(OF, "\t/ShadingType 2\n\t/Coords [%0.2f %0.2f %0.2f %0.2f]\n",
            so->x0, so->y0, so->x1, so->y1);
      } else {
         fprintf(OF, "\t/ShadingType 3\n\t/Coords [%0.2f %0.2f %0.2f %0.2f %0.2f %0.2f]\n",
            so->x0, so->y0, so->r0, so->x1, so->y1, so->r1);
      }
      if (so->extend_start || so->extend_end) fprintf(OF, "\t/Extend [ %s %s ]\n",
         (so->extend_start)? "true" : "false", (so->extend_end)? "true" : "false");
      fprintf(OF, "\t/ColorSpace /DeviceRGB\n");
      fprintf(OF, "\t/Function %i 0 R\n", so->function);
      fprintf(OF, ">> endobj\n");
   }
}

void c_axial_shading(FM *p, double x0, double y0, double x1, double y1,
      int hival, int lookup_len, unsigned char *lookup, bool extend_start, bool extend_end)
{
   Shading_Info *so = ALLOC(Shading_Info);
   so->next = shades_list;
   shades_list = so;
   so->shade_num = next_available_shade_number++;
   so->obj_num = next_available_object_number++;
   so->function = create_function(hival, lookup_len, lookup);
   so->axial = true;
   so->x0 = x0;
   so->y0 = y0;
   so->x1 = x1;
   so->y1 = y1;
   so->extend_start = extend_start;
   so->extend_end = extend_end;
   fprintf(TF, "/Shade%i sh\n", so->shade_num);
}

VALUE FM_private_axial_shading(VALUE fmkr, VALUE x0, VALUE y0, VALUE x1, VALUE y1,
   VALUE colormap, VALUE extend_start, VALUE extend_end)
{
   FM *p = Get_FM(fmkr);
   x0 = rb_Float(x0);
   y0 = rb_Float(y0);
   x1 = rb_Float(x1);
   y1 = rb_Float(y1);
   colormap = rb_Array(colormap);
   if (RARRAY(colormap)->len != 2)
         rb_raise(rb_eArgError, "Sorry: colormap must be array [hivalue, lookup]");
   VALUE hival = rb_ary_entry(colormap, 0);
   hival = rb_Integer(hival);
   VALUE lookup = rb_ary_entry(colormap, 1);
   lookup = rb_String(lookup);
   c_axial_shading(p, convert_figure_to_output_x(p,NUM2DBL(x0)), convert_figure_to_output_y(p,NUM2DBL(y0)),
      convert_figure_to_output_x(p,NUM2DBL(x1)), convert_figure_to_output_y(p,NUM2DBL(y1)),
      NUM2INT(hival), RSTRING(lookup)->len, (unsigned char *)(RSTRING(lookup)->ptr),
      extend_start == Qtrue, extend_end == Qtrue);
   return fmkr;
}

void c_radial_shading(FM *p, double x0, double y0, double r0, double x1, double y1, double r1,
      int hival, int lookup_len, unsigned char *lookup,
      double a, double b, double c, double d, double e, double f, bool extend_start, bool extend_end)
{
   Shading_Info *so = ALLOC(Shading_Info);
   so->next = shades_list;
   shades_list = so;
   so->shade_num = next_available_shade_number++;
   so->obj_num = next_available_object_number++;
   so->function = create_function(hival, lookup_len, lookup);
   so->axial = false;
   so->x0 = x0;
   so->y0 = y0;
   so->r0 = r0;
   so->x1 = x1;
   so->y1 = y1;
   so->r1 = r1;
   so->extend_start = extend_start;
   so->extend_end = extend_end;
   if (a != 1.0 || b != 0.0 || c != 0.0 || d != 1.0 || e != 0 || f != 0) {
      fprintf(TF, "q %0.2f %0.2f %0.2f %0.2f %0.2f %0.2f cm /Shade%i sh Q\n",
         a, b, c, d, e, f, so->shade_num);
   } else {
      fprintf(TF, "/Shade%i sh\n", so->shade_num);
   }
}

VALUE FM_private_radial_shading(VALUE fmkr,
        VALUE x0, VALUE y0, VALUE r0,
        VALUE x1, VALUE y1, VALUE r1, VALUE colormap,
        VALUE a, VALUE b, VALUE c, VALUE d, VALUE extend_start, VALUE extend_end)
{
   FM *p = Get_FM(fmkr);
   x0 = rb_Float(x0);
   y0 = rb_Float(y0);
   r0 = rb_Float(r0);
   x1 = rb_Float(x1);
   y1 = rb_Float(y1);
   r1 = rb_Float(r1);
   a = rb_Float(a);
   b = rb_Float(b);
   c = rb_Float(c);
   d = rb_Float(d);
   colormap = rb_Array(colormap);
   if (RARRAY(colormap)->len != 2)
         rb_raise(rb_eArgError, "Sorry: colormap must be array [hivalue, lookup]");
   VALUE hival = rb_ary_entry(colormap, 0);
   hival = rb_Integer(hival);
   VALUE lookup = rb_ary_entry(colormap, 1);
   lookup = rb_String(lookup);
   c_radial_shading(p,
      NUM2DBL(x0), NUM2DBL(y0), NUM2DBL(r0),
      NUM2DBL(x1), NUM2DBL(y1), NUM2DBL(r1), 
      NUM2INT(hival), RSTRING(lookup)->len, (unsigned char *)(RSTRING(lookup)->ptr),
      convert_figure_to_output_dx(p,NUM2DBL(a)), convert_figure_to_output_dy(p,NUM2DBL(b)),
      convert_figure_to_output_dx(p,NUM2DBL(c)), convert_figure_to_output_dy(p,NUM2DBL(d)),
      convert_figure_to_output_x(p,0.0), convert_figure_to_output_y(p,0.0),
      extend_start == Qtrue, extend_end == Qtrue);
   return fmkr;
}

/*  Colormaps
*/

VALUE c_create_colormap(FM *p, bool rgb_flag, int length,
   int num_pts, double *ps, double *c1s, double *c2s, double *c3s)
{
   int i;
   if (ps[0] != 0.0 || ps[num_pts-1] != 1.0)
      rb_raise(rb_eArgError, "Sorry: first control point for create colormap must be at 0.0 and last must be at 1.0");
   for (i = 1; i < num_pts; i++) {
      if (ps[i-1] > ps[i])
         rb_raise(rb_eArgError, "Sorry: control points for create colormap must be increasing from 0 to 1");
   }
   int j, buff_len = length * 3, hival = length-1;
   unsigned char *buff;
   buff = ALLOC_N(unsigned char, buff_len);
   for (j = 0, i = 0; j < length; j++) {
      double x = j; x /= (length-1);
      double c1, c2, c3, r, g, b;
      c1 = c_dvector_linear_interpolate(num_pts, ps, c1s, x);
      c2 = c_dvector_linear_interpolate(num_pts, ps, c2s, x);
      c3 = c_dvector_linear_interpolate(num_pts, ps, c3s, x);
      if (rgb_flag) { r = c1; g = c2; b = c3; }
      else c_hls_to_rgb(c1, c2, c3, &r, &g, &b);
      buff[i++] = ROUND(hival * r);
      buff[i++] = ROUND(hival * g);
      buff[i++] = ROUND(hival * b);
   }
   VALUE lookup = rb_str_new((char *)buff, buff_len);
   free(buff);
   VALUE result = rb_ary_new2(2);
   rb_ary_store(result, 0, INT2FIX(hival));
   rb_ary_store(result, 1, lookup);
   return result;
}

VALUE FM_private_create_colormap(VALUE fmkr, VALUE rgb_flag,
             VALUE length, VALUE Ps, VALUE C1s, VALUE C2s, VALUE C3s)
{
   FM *p = Get_FM(fmkr);
   bool rgb = rgb_flag != Qfalse;
   length = rb_Integer(length);
   long p_len, c1_len, c2_len, c3_len;
   double *p_ptr = Dvector_Data_for_Read(Ps, &p_len);
   double *c1_ptr = Dvector_Data_for_Read(C1s, &c1_len);
   double *c2_ptr = Dvector_Data_for_Read(C2s, &c2_len);
   double *c3_ptr = Dvector_Data_for_Read(C3s, &c3_len);
   if (p_len < 2 || p_len != c1_len || p_len != c2_len || p_len != c3_len)
      rb_raise(rb_eArgError, "Sorry: vectors for create colormap must all be os same length (with at least 2 entries)");
   return c_create_colormap(p, rgb, NUM2INT(length), p_len, p_ptr, c1_ptr, c2_ptr, c3_ptr);
}
            
VALUE FM_get_color_from_colormap(VALUE fmkr, VALUE color_map, VALUE color_position)
{
   color_position = rb_Float(color_position);
   double x = NUM2DBL(color_position);
   color_map = rb_String(color_map);
   unsigned char *buff = (unsigned char *)(RSTRING(color_map)->ptr), r, g, b, i;
   int len = RSTRING(color_map)->len;
   if (len % 3 != 0) rb_raise(rb_eArgError, "Sorry: color_map length must be a multiple of 3 (for R G B components)");
   i = 3 * ROUND(x * ((len/3)-1));
   r = buff[i]; g = buff[i+1]; b = buff[i+2];
   VALUE result = rb_ary_new2(3);
   rb_ary_store(result, 0, rb_float_new(r/255.0));
   rb_ary_store(result, 1, rb_float_new(g/255.0));
   rb_ary_store(result, 2, rb_float_new(b/255.0));
   return result;
   fmkr = Qnil;
}

VALUE FM_convert_to_colormap(VALUE fmkr, VALUE Rs, VALUE Gs, VALUE Bs)
{
   long r_len, g_len, b_len;
   double *r_ptr = Dvector_Data_for_Read(Rs, &r_len);
   double *g_ptr = Dvector_Data_for_Read(Gs, &g_len);
   double *b_ptr = Dvector_Data_for_Read(Bs, &b_len);
   if (r_len <= 0 || r_len != g_len || b_len != g_len)
      rb_raise(rb_eArgError, "Sorry: vectors for convert_to_colormap must all be of same length");
   int i, j, buff_len = r_len * 3;
   unsigned char *buff;
   buff = ALLOC_N(unsigned char, buff_len);
   for (i = 0, j = 0; j < r_len; j++) {
      buff[i++] = ROUND(r_ptr[j]*255);
      buff[i++] = ROUND(g_ptr[j]*255);
      buff[i++] = ROUND(b_ptr[j]*255);
   }
   VALUE lookup = rb_str_new((char *)buff, buff_len);
   free(buff);
   VALUE result = rb_ary_new2(2);
   rb_ary_store(result, 0, INT2FIX(r_len-1));
   rb_ary_store(result, 1, lookup);
   return result;
}

static double value(double n1, double n2, double hue)  // from plplot plctrl.c
{
   double val;
   while (hue >= 360.) hue -= 360.;
   while (hue < 0.) hue += 360.;
   if (hue < 60.) val = n1 + (n2 - n1) * hue / 60.;
   else if (hue < 180.) val = n2;
   else if (hue < 240.) val = n1 + (n2 - n1) * (240. - hue) / 60.;
   else val = n1;
   return (val);
}

void c_hls_to_rgb(double h, double l, double s, double *p_r, double *p_g, double *p_b)  // from plplot plctrl.c
{
   double m1, m2;
   if (l <= .5) m2 = l * (s + 1.);
   else m2 = l + s - l * s;
   m1 = 2 * l - m2;
   *p_r = value(m1, m2, h + 120.);
   *p_g = value(m1, m2, h);
   *p_b = value(m1, m2, h - 120.);
}

void c_rgb_to_hls(double r, double g, double b, double *p_h, double *p_l, double *p_s)  // from plplot plctrl.c
{
   double h, l, s, d, rc, gc, bc, rgb_min, rgb_max;
   rgb_min = MIN( r, MIN( g, b ));
   rgb_max = MAX( r, MAX( g, b ));
   l = (rgb_min+rgb_max) / 2.0;
   if (rgb_min == rgb_max) s = h = 0;
   else {
      d = rgb_max - rgb_min;
      if (l < 0.5) s = 0.5 * d / l;
      else s = 0.5* d / (1.-l);
      rc = (rgb_max-r) / d;
      gc = (rgb_max-g) / d;
      bc = (rgb_max-b) / d;
      if (r == rgb_max) h = bc-gc;
      else if (g == rgb_max) h = rc-bc+2;
      else h = gc-rc-2;
      h = h*60;
      if (h <  0) h = h+360;
      else if (h >= 360) h = h-360;
   }
   *p_h = h;
   *p_l = l;
   *p_s = s;
}

static void Unpack_HLS(VALUE hls, double *hp, double *lp, double *sp)
{
   hls = rb_Array(hls);
   if (RARRAY(hls)->len != 3) rb_raise(rb_eArgError, "Sorry: invalid hls array: must have 3 entries");
   VALUE entry = rb_ary_entry(hls, 0);
   entry = rb_Float(entry);
   double h = NUM2DBL(entry);
   entry = rb_ary_entry(hls, 1);
   entry = rb_Float(entry);
   double l = NUM2DBL(entry);
   entry = rb_ary_entry(hls, 2);
   entry = rb_Float(entry);
   double s = NUM2DBL(entry);
   if (l < 0.0 || l > 1.0) rb_raise(rb_eArgError, "Sorry: invalid lightness (%g) for hls: must be between 0 and 1", l);
   if (s < 0.0 || s > 1.0) rb_raise(rb_eArgError, "Sorry: invalid saturation (%g) for hls: must be between 0 and 1", s);
   *hp = h; *lp = l; *sp = s;
}

VALUE FM_hls_to_rgb(VALUE fmkr, VALUE hls_vec)
{
   double h, l, s, r, g, b;
   Unpack_HLS(hls_vec, &h, &l, &s);
   c_hls_to_rgb(h, l, s, &r, &g, &b);
   VALUE result = rb_ary_new2(3);
   rb_ary_store(result, 0, rb_float_new(r));
   rb_ary_store(result, 1, rb_float_new(g));
   rb_ary_store(result, 2, rb_float_new(b));
   return result; 
}

VALUE FM_rgb_to_hls(VALUE fmkr, VALUE rgb_vec)
{
   double h, l, s, r, g, b;
   Unpack_RGB(rgb_vec, &r, &g, &b);
   c_rgb_to_hls(r, g, b, &h, &l, &s);
   VALUE result = rb_ary_new2(3);
   rb_ary_store(result, 0, rb_float_new(h));
   rb_ary_store(result, 1, rb_float_new(l));
   rb_ary_store(result, 2, rb_float_new(s));
   return result; 
}

