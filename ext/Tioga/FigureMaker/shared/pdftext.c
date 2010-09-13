/* pdftext.c */
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


void
Init_Font_Dictionary(void)
{
   int i, num_fonts = num_pdf_standard_fonts;
   Font_Dictionary *font_info;
   for (i = 0; i < num_fonts; i++) {
      font_info = (Font_Dictionary *)calloc(1, sizeof(Font_Dictionary));
      font_info->afm = &afm_array[i];
      font_info->font_num = font_info->afm->font_num;
      font_info->in_use = false;
      font_info->next = font_dictionaries;
      font_dictionaries = font_info;
   }
}


static void
Write_Font_Dictionary(FILE *file, Old_Font_Dictionary *fi)
{
   int i;
   fprintf(file, "{\n");
   fprintf(file, "\t%i, // font_num\n", fi->font_num);
   fprintf(file, "\t\"%s\", // font_name\n", fi->font_name);
   fprintf(file, "\t%4i, // firstChar\n", fi->firstChar);
   fprintf(file, "\t%4i, // lastChar\n", fi->lastChar);

   fprintf(file, "\t{ // char_width\n");
   for (i=0; i<255; i++)
      fprintf(file, "\t\t%4i, // %i\n", fi->char_width[i], i);
   fprintf(file, "\t\t%4i }, // char_width\n", fi->char_width[255]);

   fprintf(file, "\t{ // char_llx\n");
   for (i=0; i<255; i++) fprintf(file, "\t\t%4i, // %i\n", fi->char_llx[i], i);
   fprintf(file, "\t\t%4i }, // char_llx\n", fi->char_llx[255]);

   fprintf(file, "\t{ // char_lly\n");
   for (i=0; i<255; i++) fprintf(file, "\t\t%4i, // %i\n", fi->char_lly[i], i);
   fprintf(file, "\t\t4%i }, // char_lly\n", fi->char_lly[255]);

   fprintf(file, "\t{ // char_urx\n");
   for (i=0; i<255; i++) fprintf(file, "\t\t%4i, // %i\n", fi->char_urx[i], i);
   fprintf(file, "\t\t%4i }, // char_urx\n", fi->char_urx[255]);

   fprintf(file, "\t{ // char_ury\n");
   for (i=0; i<255; i++) fprintf(file, "\t\t%4i, // %i\n", fi->char_ury[i], i);
   fprintf(file, "\t\t%4i }, // char_ury\n", fi->char_ury[255]);

   fprintf(file, "\t%i, // flags\n", fi->flags);
   fprintf(file, "\t%i, // fnt_llx\n", fi->fnt_llx);
   fprintf(file, "\t%i, // fnt_lly\n", fi->fnt_lly);
   fprintf(file, "\t%i, // fnt_urx\n", fi->fnt_urx);
   fprintf(file, "\t%i, // fnt_ury\n", fi->fnt_ury);
   fprintf(file, "\t%i, // italicAngle\n", fi->italicAngle);
   fprintf(file, "\t%i, // ascent\n", fi->ascent);
   fprintf(file, "\t%i, // descent\n", fi->descent);
   fprintf(file, "\t%i, // capHeight\n", fi->capHeight);
   fprintf(file, "\t%i  // stemV\n", fi->stemV);

   fprintf(file, "} // %s\n", fi->font_name);
}


static void
WriteFontDictsToFile(void)
{
   Old_Font_Dictionary *font_info;
   FILE *file;
   file = fopen("pdf_font_dicts.c", "w");
   fprintf(file, "Font_Dict_Array font_dictionaries[]"
           " = { // afm info for PDF fonts \n");
   for (font_info = old_font_dictionaries; font_info != NULL;
        font_info = font_info->next) {
      Write_Font_Dictionary(file, font_info);
      if (font_info->next != NULL) fprintf(file, ",\n");
   }
   fprintf(file, "}; // end of font_dictionaries declaration \n");
   fclose(file);
}


static void
Record_Font_In_Use(Font_Dictionary *font_info, int font_number)
{
   if (font_info->in_use) return;
   font_info->afm->font_num = font_number;
   font_info->obj_num = next_available_object_number++;
   font_info->in_use = true;
   if (font_number > num_pdf_standard_fonts) {
      font_info->widths_obj_num = next_available_object_number++;
      font_info->descriptor_obj_num = next_available_object_number++;
   }
}


#define DEBUG 0
#define MAXSTR 100

static Font_Dictionary *
GetFontDict(char *font_name, int font_number, int *ierr)
{
   Font_Dictionary *font_info;
   for (font_info = font_dictionaries; font_info != NULL;
        font_info = font_info->next) {
      if (strcmp(font_name, font_info->afm->font_name) == 0) {
         Record_Font_In_Use(font_info, font_number);
         return font_info;
      }
   }
   RAISE_ERROR_s("Sorry: invalid font name (%s)", font_name, ierr);
   return NULL;
}


static Font_Dictionary *
GetFontInfo(int font_number, int *ierr)
{
   Font_Dictionary *f;
   for (f = font_dictionaries; f != NULL; f = f->next) {
      if (f->font_num == font_number) {
         Record_Font_In_Use(f, font_number);
         return f;
      }
   }
   if (font_number > 0 && font_number <= num_predefined_fonts)
      return GetFontDict(predefined_Fonts[font_number], font_number, ierr);
   return NULL;
}


OBJ_PTR
c_register_font(OBJ_PTR fmkr, FM *p, char *font_name, int *ierr)
{
   Font_Dictionary *f;
   int i;
   for (f = font_dictionaries; f != NULL; f = f->next) {
      if (strcmp(f->afm->font_name, font_name) == 0)
         return Integer_New(f->afm->font_num);
   }
   for (i = 1; i <= num_predefined_fonts; i++) {
      if (strcmp(predefined_Fonts[i], font_name)==0) {
         f = GetFontDict(font_name, i, ierr);
         if (f == NULL)
            RAISE_ERROR_s("Error in reading font metrics for %s", font_name,
                          ierr);
         return Integer_New(i);
      }
   }
   f = GetFontDict(font_name, next_available_font_number, ierr);
   if (f == NULL) RAISE_ERROR_s("Error in reading font metrics for %s",
                                font_name, ierr);
   next_available_font_number++;
   return Integer_New(next_available_font_number); 
}


bool
Used_Any_Fonts(void)
{
   Font_Dictionary *f;
   for (f = font_dictionaries; f != NULL; f = f->next) {
      if (f->in_use) return true;
   }
   return false;
}


void
Clear_Fonts_In_Use_Flags(void)
{
   Font_Dictionary *f;
   for (f = font_dictionaries; f != NULL; f = f->next) {
      f->in_use = false;
   }
}


void
Write_Font_Descriptors(void)
{
   Font_Dictionary *f;
   for (f = font_dictionaries; f != NULL; f = f->next) {
      if (!f->in_use || f->font_num <= num_pdf_standard_fonts) continue;
      Record_Object_Offset(f->descriptor_obj_num);
      fprintf(OF, "%i 0 obj << /Type /FontDescriptor /FontName /%s\n",
              f->descriptor_obj_num, f->afm->font_name);
      fprintf(OF, "           /Flags %i /FontBBox [ %i %i %i %i ]\n",
              f->afm->flags, f->afm->fnt_llx, f->afm->fnt_lly, f->afm->fnt_urx,
              f->afm->fnt_ury);
      fprintf(OF, "           /ItalicAngle %i /Ascent %i /Descent %i "
              "/CapHeight %i /StemV %i\n", f->afm->italicAngle, f->afm->ascent,
              f->afm->descent, f->afm->capHeight, f->afm->stemV);
      fprintf(OF, ">> endobj\n");
   }
}


void
Write_Font_Widths(void)
{
   Font_Dictionary *f;
   int i, cnt = 0;
   for (f = font_dictionaries; f != NULL; f = f->next) {
      if (!f->in_use || f->font_num <= num_pdf_standard_fonts) continue;
      Record_Object_Offset(f->widths_obj_num);
      fprintf(OF, "%i 0 obj [\n    ", f->widths_obj_num);
      for (i = f->afm->firstChar; i <= f->afm->lastChar; i++) {
         fprintf(OF, "%i ", f->afm->char_width[i]);
         if (++cnt % 16 == 0) fprintf(OF, "\n    ");
      }
      fprintf(OF, "\n] endobj\n");
   }
}


void
Write_Font_Dictionaries(void)
{
   if (0) WriteFontDictsToFile(); // creates pdf_font_dicts.c 
   Font_Dictionary *f;
   for (f = font_dictionaries; f != NULL; f = f->next) {
      if (!f->in_use) continue;
      Record_Object_Offset(f->obj_num);
      fprintf(OF, "%i 0 obj << /Type /Font /Subtype /Type1 /BaseFont /%s",
              f->obj_num, f->afm->font_name);
      if (strcmp(f->afm->font_name,"Symbol") != 0
          && strcmp(f->afm->font_name,"ZapfDingbats") != 0)
         fprintf(OF, " /Encoding /MacRomanEncoding\n");
      else
         fprintf(OF, "\n");
      if (f->font_num > num_pdf_standard_fonts)
         fprintf(OF, "           /FirstChar %i /LastChar %i /Widths %i 0 R "
                 "/FontDescriptor %i 0 R\n", f->afm->firstChar,
                 f->afm->lastChar, f->widths_obj_num, f->descriptor_obj_num);
      fprintf(OF, ">> endobj\n");
   }
}


static void
GetStringInfo(FM *p, int font_number, unsigned char *text, double ft_ht,
              double *llx_ptr, double *lly_ptr, double *urx_ptr,
              double *ury_ptr, double *width_ptr, int *ierr)
{
   Font_Dictionary *fontinfo = GetFontInfo(font_number, ierr);
   if (*ierr != 0) return;
   if (fontinfo == NULL) {
      RAISE_ERROR_i("Sorry: invalid font number (%i): "
                    "must register font before use it.", font_number, ierr);
      return;
   }
   unsigned char *c_ptr = text, c;
   double width = 0, llx, lly, urx, ury;
   if (fontinfo == NULL || text == NULL || text[0] == '\0') {
      *width_ptr = *llx_ptr = *lly_ptr = *urx_ptr = *ury_ptr = 0;
      return;
   }
   c = *c_ptr;
   llx = fontinfo->afm->char_llx[c];
   lly = fontinfo->afm->char_lly[c];
   ury = fontinfo->afm->char_ury[c];
   while ((c = *c_ptr++) != '\0') {
      width += fontinfo->afm->char_width[c];
      if (fontinfo->afm->char_ury[c] > ury) ury = fontinfo->afm->char_ury[c];
      if (fontinfo->afm->char_lly[c] < lly) lly = fontinfo->afm->char_lly[c];
   }
   urx = llx + width;
   *width_ptr = width * ft_ht * 1e-3;
   *llx_ptr = ft_ht * 1e-3 * llx;
   *lly_ptr = ft_ht * 1e-3 * lly;
   *ury_ptr = ft_ht * 1e-3 * ury;
   *urx_ptr = ft_ht * 1e-3 * (urx - 70.0); // adjust for extra white
                                           // space on right
}


OBJ_PTR
c_marker_string_info(OBJ_PTR fmkr, FM *p, int fnt, unsigned char *text,
                     double scale, int *ierr)
{
   double ft_ht = (p->default_text_scale * scale * p->default_font_size
                   * ENLARGE);
   int ft_height = ROUND(ft_ht);
   ft_ht = ft_height;
   double llx, lly, urx, ury, width;
   GetStringInfo(p, fnt, text, ft_ht, &llx, &lly, &urx, &ury, &width, ierr);
   if (*ierr != 0) RETURN_NIL;
   OBJ_PTR result = Array_New(5);
   width = convert_output_to_figure_dx(p, width);
   llx = convert_output_to_figure_dx(p, llx);
   urx = convert_output_to_figure_dx(p, urx);
   lly = convert_output_to_figure_dy(p, lly);
   ury = convert_output_to_figure_dy(p, ury);
   Array_Store(result, 0, Float_New(width), ierr);
   Array_Store(result, 1, Float_New(llx), ierr);
   Array_Store(result, 2, Float_New(lly), ierr);
   Array_Store(result, 3, Float_New(urx), ierr);
   Array_Store(result, 4, Float_New(ury), ierr);
   return result;
}


#define TRANSFORM_VEC(dx,dy) tmp = dx; dx = (dx) * a + (dy) * c; dy = tmp * b + (dy) * d;


static void
c_rotated_string_at_points(OBJ_PTR fmkr, FM *p, double rotation,
                           int font_number, unsigned char *text, double scale,
                           int n, double *xs, double *ys, int alignment,
                           int just, double horizontal_scaling,
                           double vertical_scaling, double italic_angle,
                           double ascent_angle, int *ierr)
{
   double ft_ht = (p->default_text_scale * scale * p->default_font_size
                   * ENLARGE);
   int i, ft_height = ROUND(ft_ht), justification = just - 1;
   ft_ht = ft_height;
   if (constructing_path) {
      RAISE_ERROR("Sorry: must not be constructing a path when show marker",
                  ierr);
      return;
   }
   double llx, lly, urx, ury, width, shiftx, shifty, tmp;
   // the initial transform
   double a = horizontal_scaling, b = 0.0, c = 0.0, d = vertical_scaling;
   GetStringInfo(p, font_number, text, ft_ht, &llx, &lly, &urx, &ury, &width,
                 ierr);
   if (*ierr != 0) return;
   // translate according to justification and alignment
   // note that we use the bbox to calculate shifts so 'center' means
   // center of bbox
   if (italic_angle != 0) {
      double skew = sin(italic_angle / RADIANS_TO_DEGREES);
      c -= skew * a;
      d -= skew * b;
   }
   if (ascent_angle != 0) { 
      double skew = sin(ascent_angle / RADIANS_TO_DEGREES);
      a += skew * c;
      b += skew * d;
   }
   if (rotation != 0) {
      double xa, xb, xc, xd; // new transform
      double cs = cos(rotation / RADIANS_TO_DEGREES);
      double sn = sin(rotation / RADIANS_TO_DEGREES);
      xa = cs * a + sn * c;
      xb = cs * b + sn * d;
      xc = -sn * a + cs * c;
      xd = -sn * b + cs * d;
      a = xa;
      b = xb;
      c = xc;
      d = xd;
   }
   switch (justification) {
      case LEFT_JUSTIFIED:
         shiftx = 0; break;
      case CENTERED: // CENTERED for marker means centered on BBOX
         shiftx = -(urx + llx) / 2;
         break;
      case RIGHT_JUSTIFIED: 
         shiftx = -width;
         // the following hack compensates for Arrowhead bbox in
         // ZaphDingbats needed to make tip of arrowhead fall on the
         // reference point correctly
         if (font_number == 14 && strlen((char *)text) == 1
             && text[0] == 0344) {
            shiftx *= 0.9;
         }
         break;
      default: 
         RAISE_ERROR_i("Sorry: invalid setting for marker justification (%i)",
                       justification, ierr);
         return;
   }
   switch (alignment) {
      case ALIGNED_AT_TOP: shifty = -ury; break;
      case ALIGNED_AT_MIDHEIGHT: shifty = -(ury + lly)/2; break;
      case ALIGNED_AT_BASELINE: shifty = 0; break;
      case ALIGNED_AT_BOTTOM: shifty = -lly; break;
      default: 
         RAISE_ERROR_i("Sorry: invalid setting for marker alignment (%i)",
                       alignment, ierr);
         return;
   }
   // transform the bbox
   // if we're rotated we'll need all 4 corners of bbox
   double llx2 = llx, lly2 = lly, urx2 = urx, ury2 = ury;
   TRANSFORM_VEC(llx, lly)
   TRANSFORM_VEC(urx, ury)
   TRANSFORM_VEC(llx2, ury2)
   TRANSFORM_VEC(urx2, lly2)
   TRANSFORM_VEC(shiftx, shifty)
   fprintf(TF, "BT /F%i %i Tf\n", font_number, ft_height);
   if (0 && horizontal_scaling != 1.0) {
      fprintf(TF, "%d Tz\n", ROUND(100 * ABS(horizontal_scaling)));
   }
   double x, y, prev_x = 0, prev_y = 0, dx, dy;
   //int idx, idy;
   for (i = 0; i < n; i++) {
      unsigned char *cp = text, char_code;
      x = convert_figure_to_output_x(p, xs[i]) + shiftx;
      y = convert_figure_to_output_y(p, ys[i]) + shifty;
      if(!is_okay_number(x) || !is_okay_number(y))
         continue; // we forget this point if at least one coordinate
                   // is not 'real'
      update_bbox(p, x + llx, y + lly);
      update_bbox(p, x + urx, y + ury);
      update_bbox(p, x + llx2, y + ury2);
      update_bbox(p, x + urx2, y + lly2);
      dx = x - prev_x; dy = y - prev_y;
      //idx = ROUND(dx); idy = ROUND(dy);
      //prev_x = prev_x + idx; prev_y = prev_y + idy;
      prev_x = prev_x + dx; prev_y = prev_y + dy;
      if (b == 0 && c == 0 && a == 1 && d == 1) {
         //fprintf(TF, "%i %i Td (", idx, idy);
         fprintf(TF, "%0.6f %0.6f Td (", dx, dy);
      } 
      else { // need high precision when doing rotations
         fprintf(TF, "%0.6f %0.6f %0.6f %0.6f %0.6f %0.6f Tm (",
                 a, b, c, d, x, y);
      }
      while ((char_code = *cp++) != '\0') {
         if (char_code == '\\')
            fprintf(TF, "\\\\");
         else if (char_code == '(' || char_code == ')')
            fprintf(TF, "\\%c", char_code);
         else
            fprintf(TF, "%c", char_code);
      }
      fprintf(TF, ") Tj\n");
   }
   fprintf(TF, "ET\n");
}


static OBJ_PTR get1_obj(bool is_list, OBJ_PTR obj, int indx, int* ierr) {
   if (is_list) {
      int len;
      len = Array_Len(obj, ierr); if (*ierr != 0) return 0.0;
      obj = Array_Entry(obj, indx % len, ierr); if (*ierr != 0) return 0.0;
   }
   return obj;
}


static double get1_dbl(bool is_list, OBJ_PTR obj, int indx, int* ierr) {
   if (is_list) {
      int len = Array_Len(obj, ierr); if (*ierr != 0) return 0.0;      
      return Array_Entry_double(obj, indx % len, ierr); if (*ierr != 0) return 0.0;
   }
   return Number_to_double(obj, ierr);
}


static int get1_int(bool is_list, OBJ_PTR obj, int indx, int* ierr) {
   if (is_list) {
      int len = Array_Len(obj, ierr); if (*ierr != 0) return 0.0;      
      return Array_Entry_int(obj, indx % len, ierr); if (*ierr != 0) return 0.0;
   }
   return Number_to_int(obj, ierr);
}


void
c_private_show_marker(OBJ_PTR fmkr, FM *p, OBJ_PTR args, int *ierr)
{
   OBJ_PTR marker_obj, font_obj, mode_obj, align_obj, just_obj, stroke_width_obj, string, x, y, x_vec, y_vec,
       h_scale_obj, v_scale_obj, scale_obj, it_angle_obj, ascent_angle_obj, angle_obj,
       fill_color_obj, stroke_color_obj;
   int c, alignment, justification, fnt_num, n, num_times, num_per_call, i, mode, len;
   double h_scale, v_scale, scale, it_angle, ascent_angle, angle;
   unsigned char *text = NULL, buff[2];
   double *xs, *ys, xloc, yloc, prev_line_width = -1;
   double stroke_color_R = 0.0, stroke_color_G = 0.0, stroke_color_B = 0.0;
   double fill_color_R = 0.0, fill_color_G = 0.0, fill_color_B = 0.0;
   double prev_stroke_color_R, prev_stroke_color_G, prev_stroke_color_B;
   double prev_fill_color_R, prev_fill_color_G, prev_fill_color_B;
   OBJ_PTR fill_color, stroke_color, marker;
   bool do_lists, fill_color_is_list, stroke_color_is_list, stroke_width_is_list, h_scale_is_list;
   bool it_angle_is_list, ascent_angle_is_list, angle_is_list, v_scale_is_list, scale_is_list;
   bool align_is_list, just_is_list, marker_is_list, mode_is_list, font_is_list;

   i = 0;
   marker_obj = Array_Entry(args, i, ierr); i++;
   font_obj = Array_Entry(args, i, ierr); i++;
   mode_obj = Array_Entry(args, i, ierr); i++;
   align_obj = Array_Entry(args, i, ierr); i++;
   just_obj = Array_Entry(args, i, ierr); i++;
   stroke_width_obj = Array_Entry(args, i, ierr); i++;
   string = Array_Entry(args, i, ierr); i++;
   x = Array_Entry(args, i, ierr); i++;
   y = Array_Entry(args, i, ierr); i++;
   x_vec = Array_Entry(args, i, ierr); i++;
   y_vec = Array_Entry(args, i, ierr); i++;
   h_scale_obj = Array_Entry(args, i, ierr); i++;
   v_scale_obj = Array_Entry(args, i, ierr); i++;
   scale_obj = Array_Entry(args, i, ierr); i++;
   it_angle_obj = Array_Entry(args, i, ierr); i++;
   ascent_angle_obj = Array_Entry(args, i, ierr); i++;
   angle_obj = Array_Entry(args, i, ierr); i++;
   fill_color_obj = Array_Entry(args, i, ierr); i++;
   stroke_color_obj = Array_Entry(args, i, ierr); i++;

   if (x == OBJ_NIL) {
      long xlen, ylen;
      xs = Vector_Data_for_Read(x_vec, &xlen, ierr);
      if (*ierr != 0) return;
      ys = Vector_Data_for_Read(y_vec, &ylen, ierr);
      if (*ierr != 0) return;
      if (xlen != ylen) { 
         RAISE_ERROR("Sorry: must have same number xs and ys "
                     "for showing markers", ierr);
         return;
      }
      if (xlen <= 0) return;
      n = xlen;
   }
   else {
      xloc = Number_to_double(x, ierr); xs = &xloc;
      yloc = Number_to_double(y, ierr); ys = &yloc;
      if (*ierr != 0) return;
      n = 1;
   }

   prev_stroke_color_R = p->stroke_color_R;
   prev_stroke_color_G = p->stroke_color_G;
   prev_stroke_color_B = p->stroke_color_B;
   prev_fill_color_R = p->fill_color_R;
   prev_fill_color_G = p->fill_color_G;
   prev_fill_color_B = p->fill_color_B;
   prev_line_width = p->line_width;
   
   if (marker_obj == OBJ_NIL) {
      marker_is_list = false;
   } else {
      marker = Array_Entry(marker_obj, 0, ierr); if (*ierr != 0) return;
      marker_is_list = !Is_Kind_of_Number(marker);
   } 

   fill_color = Array_Entry(fill_color_obj, 0, ierr); if (*ierr != 0) return;
   stroke_color = Array_Entry(stroke_color_obj, 0, ierr); if (*ierr != 0) return;   
   fill_color_is_list = !Is_Kind_of_Number(fill_color);
   stroke_color_is_list = !Is_Kind_of_Number(stroke_color);
   
   font_is_list = (font_obj != OBJ_NIL) && (!Is_Kind_of_Integer(font_obj));
   align_is_list = !Is_Kind_of_Integer(align_obj);
   just_is_list = !Is_Kind_of_Integer(just_obj);
   mode_is_list = (mode_obj != OBJ_NIL) && (!Is_Kind_of_Number(mode_obj));
   stroke_width_is_list = stroke_width_obj != OBJ_NIL && !Is_Kind_of_Number(stroke_width_obj);
   h_scale_is_list = !Is_Kind_of_Number(h_scale_obj);
   v_scale_is_list = !Is_Kind_of_Number(v_scale_obj);
   scale_is_list = !Is_Kind_of_Number(scale_obj);
   it_angle_is_list = !Is_Kind_of_Number(it_angle_obj);
   ascent_angle_is_list = !Is_Kind_of_Number(ascent_angle_obj);
   angle_is_list = !Is_Kind_of_Number(angle_obj);
   
   do_lists = fill_color_is_list || stroke_color_is_list || h_scale_is_list || v_scale_is_list || scale_is_list || 
              align_is_list || just_is_list || it_angle_is_list || ascent_angle_is_list || angle_is_list;

   if (do_lists) {
      num_times = n; num_per_call = 1;
   } else {
      num_times = 1; num_per_call = n;
   }

   for (i=0; i<num_times; i++) {
      alignment = get1_int(align_is_list, align_obj, i, ierr); if (*ierr != 0) return;
      justification = 1+get1_int(just_is_list, just_obj, i, ierr); if (*ierr != 0) return;
      h_scale = get1_dbl(h_scale_is_list, h_scale_obj, i, ierr); if (*ierr != 0) return;
      v_scale = get1_dbl(v_scale_is_list, v_scale_obj, i, ierr); if (*ierr != 0) return;
      scale = get1_dbl(scale_is_list, scale_obj, i, ierr); if (*ierr != 0) return;
      it_angle = get1_dbl(it_angle_is_list, it_angle_obj, i, ierr); if (*ierr != 0) return;
      ascent_angle = get1_dbl(ascent_angle_is_list, ascent_angle_obj, i, ierr); if (*ierr != 0) return;
      angle = get1_dbl(angle_is_list, angle_obj, i, ierr); if (*ierr != 0) return;
      fill_color = get1_obj(fill_color_is_list, fill_color_obj, i, ierr); if (*ierr != 0) return;
      stroke_color = get1_obj(stroke_color_is_list, stroke_color_obj, i, ierr); if (*ierr != 0) return;
      
      if (mode_obj != OBJ_NIL) {
         mode = get1_int(mode_is_list, mode_obj, i, ierr); if (*ierr != 0) return;
      } else {
         mode = FILL;
      }
      
      if (font_obj != OBJ_NIL) {
         fnt_num = get1_int(font_is_list, font_obj, i, ierr); if (*ierr != 0) return;
      } else {
         fnt_num = 1; // Times_Roman = 1
      }

      if (marker_obj == OBJ_NIL) {
         text = (unsigned char *)(String_Ptr(string, ierr));
         if (*ierr != 0) return;
      } else {
         marker = get1_obj(marker_is_list, marker_obj, i, ierr); if (*ierr != 0) return;
         font_obj = Array_Entry(marker, 0, ierr); if (*ierr != 0) return;
         fnt_num = Number_to_int(font_obj, ierr); if (*ierr != 0) return;
         
         OBJ_PTR glyph_obj = Array_Entry(marker, 1, ierr); if (*ierr != 0) return;
         c = Number_to_int(glyph_obj, ierr); if (*ierr != 0) return;

         if (mode_obj == OBJ_NIL) {
            len = Array_Len(marker, ierr); if (*ierr != 0) return;
            if (len == 3) {
               mode = STROKE;
               if (stroke_width_obj == OBJ_NIL) {
                  OBJ_PTR s = Array_Entry(marker, 2, ierr); if (*ierr != 0) return;
                  double width = Number_to_double(s,ierr); if (*ierr != 0) return;
                  if (*ierr != 0) return;
                  fprintf(TF, "%0.6f w\n", width * ENLARGE);
               }
            }
         }
         
         font_obj = Array_Entry(marker, 0, ierr);
         if (*ierr != 0) return;
         fnt_num = Number_to_int(font_obj, ierr); if (*ierr != 0) return;
         
         if (c < 0 || c > 255) {
            RAISE_ERROR_i("Sorry: invalid character code (%i) : "
                          "must be between 0 and 255", c, ierr);
            return;
         }
         text = buff; text[0] = c; text[1] = '\0';
      }

      if (stroke_width_obj != OBJ_NIL) {
         double width = get1_dbl(stroke_width_is_list, stroke_width_obj, i, ierr); if (*ierr != 0) return;
         if (*ierr != 0) return;
         fprintf(TF, "%0.6f w\n", width * ENLARGE);
      }
      
      if (mode_obj != OBJ_NIL) {
         mode = get1_int(mode_is_list, mode_obj, i, ierr); if (*ierr != 0) return;
      }
      
      fprintf(TF, "%d Tr\n", mode);
      
      if (stroke_color != OBJ_NIL &&
          (mode == STROKE || mode == FILL_AND_STROKE
           || mode == STROKE_AND_CLIP || mode == FILL_STROKE_AND_CLIP)) {
         Unpack_RGB(stroke_color, &stroke_color_R, &stroke_color_G,
                    &stroke_color_B, ierr);
         if (*ierr != 0) return;
         if (stroke_color_R != p->stroke_color_R
             || stroke_color_G != p->stroke_color_G
             || stroke_color_B != p->stroke_color_B) {
            c_stroke_color_set_RGB(fmkr, p, stroke_color_R, stroke_color_G,
                                   stroke_color_B, ierr);
            if (*ierr != 0) return;
         }
      }
      
      if (fill_color != OBJ_NIL &&
          (mode == FILL || mode == FILL_AND_STROKE
           || mode == FILL_AND_CLIP || mode == FILL_STROKE_AND_CLIP)) {
         Unpack_RGB(fill_color, &fill_color_R, &fill_color_G, &fill_color_B,
                    ierr);
         if (*ierr != 0) return;
         if (fill_color_R != p->fill_color_R
             || fill_color_G != p->fill_color_G
             || fill_color_B != p->fill_color_B) {
            c_fill_color_set_RGB(fmkr, p, fill_color_R, fill_color_G,
                                 fill_color_B, ierr);
            if (*ierr != 0) return;
         }
      }

      c_rotated_string_at_points(fmkr, p, angle, fnt_num, text, scale, num_per_call, xs+i, ys+i,
                        alignment, justification, h_scale, v_scale, it_angle, ascent_angle, ierr);
                              
      if (prev_line_width != p->line_width) c_line_width_set(fmkr, p, prev_line_width, ierr);
      if (prev_fill_color_R != p->fill_color_R
             || prev_fill_color_G != p->fill_color_G
             || prev_fill_color_B != p->fill_color_B)
         c_fill_color_set_RGB(fmkr, p, prev_fill_color_R, prev_fill_color_G,
                              prev_fill_color_B, ierr);
      if (prev_stroke_color_R != p->stroke_color_R
             || prev_stroke_color_G != p->stroke_color_G
             || prev_stroke_color_B != p->stroke_color_B)
         c_stroke_color_set_RGB(fmkr, p, prev_stroke_color_R, prev_stroke_color_G,
                                prev_stroke_color_B, ierr);
      
   }
   
}
