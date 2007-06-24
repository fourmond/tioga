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
      RAISE_ERROR("Error writing function sample data");
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
   fo->lookup = ALLOC_N_unsigned_char(lookup_len);
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

OBJ_PTR FM_stroke_opacity_set(OBJ_PTR fmkr, OBJ_PTR val)
{  // /GSi gs for ExtGState obj with /CS set to stroke opacity val
   FM *p = Get_FM(fmkr);
   if (constructing_path) RAISE_ERROR("Sorry: must not be constructing a path when change stroke opacity");
   double stroke_opacity = Number_to_double(val);
   if (stroke_opacity == p->stroke_opacity) return val;
   int gs_num = Get_Stroke_Opacity_XGS(stroke_opacity);
   fprintf(TF, "/GS%i gs\n", gs_num);
   p->stroke_opacity = stroke_opacity;
   return val;
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

OBJ_PTR FM_fill_opacity_set(OBJ_PTR fmkr, OBJ_PTR val)
{  // /GSi gs for ExtGState obj with /cs set to fill opacity val
   FM *p = Get_FM(fmkr);
   if (constructing_path) RAISE_ERROR("Sorry: must not be constructing a path when change fill opacity");
   double fill_opacity = Number_to_double(val);
   if (fill_opacity == p->fill_opacity) return val;
   int gs_num = Get_Fill_Opacity_XGS(fill_opacity);
   fprintf(TF, "/GS%i gs\n", gs_num);
   p->fill_opacity = fill_opacity;
   return val;
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


static void c_axial_shading(FM *p, double x0, double y0, double x1, double y1,
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

OBJ_PTR FM_private_axial_shading(OBJ_PTR fmkr, OBJ_PTR x0, OBJ_PTR y0, OBJ_PTR x1, OBJ_PTR y1,
   OBJ_PTR colormap, OBJ_PTR extend_start, OBJ_PTR extend_end)
{
   FM *p = Get_FM(fmkr);
   if (Array_Len(colormap) != 2)
         RAISE_ERROR("Sorry: colormap must be array [hivalue, lookup]");
   OBJ_PTR hival = Array_Entry(colormap, 0);
   OBJ_PTR lookup = Array_Entry(colormap, 1);
   c_axial_shading(p, convert_figure_to_output_x(p,Number_to_double(x0)), convert_figure_to_output_y(p,Number_to_double(y0)),
      convert_figure_to_output_x(p,Number_to_double(x1)), convert_figure_to_output_y(p,Number_to_double(y1)),
      Number_to_int(hival), String_Len(lookup), (unsigned char *)(String_Ptr(lookup)),
      extend_start == OBJ_TRUE, extend_end == OBJ_TRUE);
   return fmkr;
}

static void c_radial_shading(FM *p, double x0, double y0, double r0, double x1, double y1, double r1,
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

OBJ_PTR FM_private_radial_shading(OBJ_PTR fmkr,
        OBJ_PTR x0, OBJ_PTR y0, OBJ_PTR r0,
        OBJ_PTR x1, OBJ_PTR y1, OBJ_PTR r1, OBJ_PTR colormap,
        OBJ_PTR a, OBJ_PTR b, OBJ_PTR c, OBJ_PTR d, OBJ_PTR extend_start, OBJ_PTR extend_end)
{
   FM *p = Get_FM(fmkr);
   if (Array_Len(colormap) != 2)
         RAISE_ERROR("Sorry: colormap must be array [hivalue, lookup]");
   OBJ_PTR hival = Array_Entry(colormap, 0);
   OBJ_PTR lookup = Array_Entry(colormap, 1);
   c_radial_shading(p,
      Number_to_double(x0), Number_to_double(y0), Number_to_double(r0),
      Number_to_double(x1), Number_to_double(y1), Number_to_double(r1), 
      Number_to_int(hival), String_Len(lookup), (unsigned char *)(String_Ptr(lookup)),
      convert_figure_to_output_dx(p,Number_to_double(a)), convert_figure_to_output_dy(p,Number_to_double(b)),
      convert_figure_to_output_dx(p,Number_to_double(c)), convert_figure_to_output_dy(p,Number_to_double(d)),
      convert_figure_to_output_x(p,0.0), convert_figure_to_output_y(p,0.0),
      extend_start == OBJ_TRUE, extend_end == OBJ_TRUE);
   return fmkr;
}

/*  Colormaps
*/

static double clr_value(double n1, double n2, double hue)  // from plplot plctrl.c
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

static void c_hls_to_rgb(double h, double l, double s, double *p_r, double *p_g, double *p_b)  // from plplot plctrl.c
{
   double m1, m2;
   if (l <= .5) m2 = l * (s + 1.);
   else m2 = l + s - l * s;
   m1 = 2 * l - m2;
   *p_r = clr_value(m1, m2, h + 120.);
   *p_g = clr_value(m1, m2, h);
   *p_b = clr_value(m1, m2, h - 120.);
}

static void c_rgb_to_hls(double r, double g, double b, double *p_h, double *p_l, double *p_s)  // from plplot plctrl.c
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

static double linear_interpolate(int num_pts, double *xs, double *ys, double x)
{
   int i;
   if (num_pts == 1) return ys[0];
   for (i = 0; i < num_pts; i++) {
      if (xs[i] <= x && x < xs[i+1]) {
         return ys[i] + (ys[i+1]-ys[i])*(x-xs[i])/(xs[i+1]-xs[i]);
      }
   }
   return ys[num_pts-1];
}

static OBJ_PTR c_create_colormap(FM *p, bool rgb_flag, int length,
   int num_pts, double *ps, double *c1s, double *c2s, double *c3s)
{
   int i;
   if (ps[0] != 0.0 || ps[num_pts-1] != 1.0)
      RAISE_ERROR("Sorry: first control point for create colormap must be at 0.0 and last must be at 1.0");
   for (i = 1; i < num_pts; i++) {
      if (ps[i-1] > ps[i])
         RAISE_ERROR("Sorry: control points for create colormap must be increasing from 0 to 1");
   }
   int j, buff_len = length * 3, hival = length-1;
   unsigned char *buff;
   buff = ALLOC_N_unsigned_char(buff_len);
   for (j = 0, i = 0; j < length; j++) {
      double x = j; x /= (length-1);
      double c1, c2, c3, r, g, b;
      c1 = linear_interpolate(num_pts, ps, c1s, x);
      c2 = linear_interpolate(num_pts, ps, c2s, x);
      c3 = linear_interpolate(num_pts, ps, c3s, x);
      if (rgb_flag) { r = c1; g = c2; b = c3; }
      else c_hls_to_rgb(c1, c2, c3, &r, &g, &b);
      buff[i++] = ROUND(hival * r);
      buff[i++] = ROUND(hival * g);
      buff[i++] = ROUND(hival * b);
   }
   OBJ_PTR lookup = String_New((char *)buff, buff_len);
   free(buff);
   OBJ_PTR result = Array_New(2);
   Array_Store(result, 0, Integer_New(hival));
   Array_Store(result, 1, lookup);
   return result;
}

OBJ_PTR FM_private_create_colormap(OBJ_PTR fmkr, OBJ_PTR rgb_flag,
             OBJ_PTR length, OBJ_PTR Ps, OBJ_PTR C1s, OBJ_PTR C2s, OBJ_PTR C3s)
{
        /* 
            create mappings from 'position' (0 to 1) to color (in HLS or RGB color spaces)
            the length parameter determines the number of entries in the color map (any integer between 2 and 256).
            for rgb, the colors are given as (red, green, blue) intensities from 0.0 to 1.0
            for hls, the colors are given as (hue, lightness, saturation)
                lightness and saturation given as values from 0.0 to 1.0
                hue given as degrees (0 to 360) around the color wheel from red->green->blue->red
            Ps are the locations in (0 to 1) for the control points -- in increasing order
            must have Ps[0] == 0.0 and Ps[num_ps-1] == 1.0
        */
   FM *p = Get_FM(fmkr);
   bool rgb = rgb_flag != OBJ_FALSE;
   long p_len, c1_len, c2_len, c3_len;
   double *p_ptr = Vector_Data_for_Read(Ps, &p_len);
   double *c1_ptr = Vector_Data_for_Read(C1s, &c1_len);
   double *c2_ptr = Vector_Data_for_Read(C2s, &c2_len);
   double *c3_ptr = Vector_Data_for_Read(C3s, &c3_len);
   if (p_len < 2 || p_len != c1_len || p_len != c2_len || p_len != c3_len)
      RAISE_ERROR("Sorry: vectors for create colormap must all be os same length (with at least 2 entries)");
   return c_create_colormap(p, rgb, Number_to_int(length), p_len, p_ptr, c1_ptr, c2_ptr, c3_ptr);
}
            
OBJ_PTR FM_get_color_from_colormap(OBJ_PTR fmkr, OBJ_PTR color_map, OBJ_PTR color_position)
{
        /* color_position is from 0 to 1.  this returns a vector for the RGB color from the given colormap */
   double x = Number_to_double(color_position);
   unsigned char *buff = (unsigned char *)(String_Ptr(color_map)), r, g, b, i;
   int len = String_Len(color_map);
   if (len % 3 != 0) RAISE_ERROR("Sorry: color_map length must be a multiple of 3 (for R G B components)");
   i = 3 * ROUND(x * ((len/3)-1));
   r = buff[i]; g = buff[i+1]; b = buff[i+2];
   OBJ_PTR result = Array_New(3);
   Array_Store(result, 0, Float_New(r/255.0));
   Array_Store(result, 1, Float_New(g/255.0));
   Array_Store(result, 2, Float_New(b/255.0));
   return result;
   fmkr = OBJ_NIL;
}

OBJ_PTR FM_convert_to_colormap(OBJ_PTR fmkr, OBJ_PTR Rs, OBJ_PTR Gs, OBJ_PTR Bs)
{
        /* this creates an arbitrary mapping from positions to colors given as (r,g,b) triples */
        /* the colormap size is set to the length of the vectors */
        /* the Rs, Gs, and Bs are VALUEs from 0 to 1 representing the intensity of the color component */
   long r_len, g_len, b_len;
   double *r_ptr = Vector_Data_for_Read(Rs, &r_len);
   double *g_ptr = Vector_Data_for_Read(Gs, &g_len);
   double *b_ptr = Vector_Data_for_Read(Bs, &b_len);
   if (r_len <= 0 || r_len != g_len || b_len != g_len)
      RAISE_ERROR("Sorry: vectors for convert_to_colormap must all be of same length");
   int i, j, buff_len = r_len * 3;
   unsigned char *buff;
   buff = ALLOC_N_unsigned_char(buff_len);
   for (i = 0, j = 0; j < r_len; j++) {
      buff[i++] = ROUND(r_ptr[j]*255);
      buff[i++] = ROUND(g_ptr[j]*255);
      buff[i++] = ROUND(b_ptr[j]*255);
   }
   OBJ_PTR lookup = String_New((char *)buff, buff_len);
   free(buff);
   OBJ_PTR result = Array_New(2);
   Array_Store(result, 0, Integer_New(r_len-1));
   Array_Store(result, 1, lookup);
   return result;
}

static void Unpack_HLS(OBJ_PTR hls, double *hp, double *lp, double *sp)
{
   if (Array_Len(hls) != 3) RAISE_ERROR("Sorry: invalid hls array: must have 3 entries");
   OBJ_PTR entry = Array_Entry(hls, 0);
   double h = Number_to_double(entry);
   entry = Array_Entry(hls, 1);
   double l = Number_to_double(entry);
   entry = Array_Entry(hls, 2);
   double s = Number_to_double(entry);
   if (l < 0.0 || l > 1.0) RAISE_ERROR_g("Sorry: invalid lightness (%g) for hls: must be between 0 and 1", l);
   if (s < 0.0 || s > 1.0) RAISE_ERROR_g("Sorry: invalid saturation (%g) for hls: must be between 0 and 1", s);
   *hp = h; *lp = l; *sp = s;
}

OBJ_PTR FM_hls_to_rgb(OBJ_PTR fmkr, OBJ_PTR hls_vec)
{
   double h, l, s, r, g, b;
   Unpack_HLS(hls_vec, &h, &l, &s);
   c_hls_to_rgb(h, l, s, &r, &g, &b);
   OBJ_PTR result = Array_New(3);
   Array_Store(result, 0, Float_New(r));
   Array_Store(result, 1, Float_New(g));
   Array_Store(result, 2, Float_New(b));
   return result; 
}

OBJ_PTR FM_rgb_to_hls(OBJ_PTR fmkr, OBJ_PTR rgb_vec)
{
    /* hue is given as an angle from 0 to 360 around the color wheel.
        0, 60, 120, 180, 240, and 300 are respectively red, yellow, green, cyan, blue, and magenta. */
    /* lightness and saturation are given as numbers from 0 to 1 */
   double h, l, s, r, g, b;
   Unpack_RGB(rgb_vec, &r, &g, &b);
   c_rgb_to_hls(r, g, b, &h, &l, &s);
   OBJ_PTR result = Array_New(3);
   Array_Store(result, 0, Float_New(h));
   Array_Store(result, 1, Float_New(l));
   Array_Store(result, 2, Float_New(s));
   return result; 
}


static void c_title_color_set(FM *p, double r, double g, double b)
{
   p->title_color_R = r;
   p->title_color_G = g;
   p->title_color_B = b;
}

OBJ_PTR FM_title_color_set(OBJ_PTR fmkr, OBJ_PTR val)
{ // r g b rg
   FM *p = Get_FM(fmkr);
   double r, g, b;
   Unpack_RGB(val, &r, &g, &b);
   c_title_color_set(p, r, g, b);
   return val;
}

OBJ_PTR FM_title_color_get(OBJ_PTR fmkr) // value is array of [r, g, b] intensities from 0 to 1
{  // r g b RG
   FM *p = Get_FM(fmkr);
   double r, g, b;
   r = p->title_color_R;
   g = p->title_color_G;
   b = p->title_color_B;
   OBJ_PTR result = Array_New(3);
   Array_Store(result, 0, Float_New(r));
   Array_Store(result, 1, Float_New(g));
   Array_Store(result, 2, Float_New(b));
   return result;
}

static void c_xlabel_color_set(FM *p, double r, double g, double b)
{
   p->xlabel_color_R = r;
   p->xlabel_color_G = g;
   p->xlabel_color_B = b;
}

OBJ_PTR FM_xlabel_color_set(OBJ_PTR fmkr, OBJ_PTR val)
{ // r g b rg
   FM *p = Get_FM(fmkr);
   double r, g, b;
   Unpack_RGB(val, &r, &g, &b);
   c_xlabel_color_set(p, r, g, b);
   return val;
}

OBJ_PTR FM_xlabel_color_get(OBJ_PTR fmkr) // value is array of [r, g, b] intensities from 0 to 1
{  // r g b RG
   FM *p = Get_FM(fmkr);
   double r, g, b;
   r = p->xlabel_color_R;
   g = p->xlabel_color_G;
   b = p->xlabel_color_B;
   OBJ_PTR result = Array_New(3);
   Array_Store(result, 0, Float_New(r));
   Array_Store(result, 1, Float_New(g));
   Array_Store(result, 2, Float_New(b));
   return result;
}

static void c_ylabel_color_set(FM *p, double r, double g, double b)
{
   p->ylabel_color_R = r;
   p->ylabel_color_G = g;
   p->ylabel_color_B = b;
}

OBJ_PTR FM_ylabel_color_set(OBJ_PTR fmkr, OBJ_PTR val)
{ // r g b rg
   FM *p = Get_FM(fmkr);
   double r, g, b;
   Unpack_RGB(val, &r, &g, &b);
   c_ylabel_color_set(p, r, g, b);
   return val;
}

OBJ_PTR FM_ylabel_color_get(OBJ_PTR fmkr) // value is array of [r, g, b] intensities from 0 to 1
{  // r g b RG
   FM *p = Get_FM(fmkr);
   double r, g, b;
   r = p->ylabel_color_R;
   g = p->ylabel_color_G;
   b = p->ylabel_color_B;
   OBJ_PTR result = Array_New(3);
   Array_Store(result, 0, Float_New(r));
   Array_Store(result, 1, Float_New(g));
   Array_Store(result, 2, Float_New(b));
   return result;
}

static void c_xaxis_stroke_color_set(FM *p, double r, double g, double b)
{
   p->xaxis_stroke_color_R = r;
   p->xaxis_stroke_color_G = g;
   p->xaxis_stroke_color_B = b;
}

OBJ_PTR FM_xaxis_stroke_color_set(OBJ_PTR fmkr, OBJ_PTR val)
{ // r g b rg
   FM *p = Get_FM(fmkr);
   double r, g, b;
   Unpack_RGB(val, &r, &g, &b);
   c_xaxis_stroke_color_set(p, r, g, b);
   return val;
}

OBJ_PTR FM_xaxis_stroke_color_get(OBJ_PTR fmkr) // value is array of [r, g, b] intensities from 0 to 1
{  // r g b RG
   FM *p = Get_FM(fmkr);
   double r, g, b;
   r = p->xaxis_stroke_color_R;
   g = p->xaxis_stroke_color_G;
   b = p->xaxis_stroke_color_B;
   OBJ_PTR result = Array_New(3);
   Array_Store(result, 0, Float_New(r));
   Array_Store(result, 1, Float_New(g));
   Array_Store(result, 2, Float_New(b));
   return result;
}

static void c_yaxis_stroke_color_set(FM *p, double r, double g, double b)
{
   p->yaxis_stroke_color_R = r;
   p->yaxis_stroke_color_G = g;
   p->yaxis_stroke_color_B = b;
}

OBJ_PTR FM_yaxis_stroke_color_set(OBJ_PTR fmkr, OBJ_PTR val)
{ // r g b rg
   FM *p = Get_FM(fmkr);
   double r, g, b;
   Unpack_RGB(val, &r, &g, &b);
   c_yaxis_stroke_color_set(p, r, g, b);
   return val;
}

OBJ_PTR FM_yaxis_stroke_color_get(OBJ_PTR fmkr) // value is array of [r, g, b] intensities from 0 to 1
{  // r g b RG
   FM *p = Get_FM(fmkr);
   double r, g, b;
   r = p->yaxis_stroke_color_R;
   g = p->yaxis_stroke_color_G;
   b = p->yaxis_stroke_color_B;
   OBJ_PTR result = Array_New(3);
   Array_Store(result, 0, Float_New(r));
   Array_Store(result, 1, Float_New(g));
   Array_Store(result, 2, Float_New(b));
   return result;
}
