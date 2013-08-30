/* pdffile.c */
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

#include <time.h>
#include "figures.h"
#include "pdfs.h"

#define FLATE_ENCODE 1

#define Get_pdf_xoffset()  5.0
#define Get_pdf_yoffset()  5.0


/* must match the font numbers in FigureConstants.rb */
char *predefined_Fonts[] = { 
   NULL,
   "Times-Roman",
   "Times-Italic", // 2
   "Times-Bold",
   "Times-BoldItalic",
   "Helvetica",
   "Helvetica-Oblique", // 6
   "Helvetica-Bold",
   "Helvetica-BoldOblique",
   "Courier",
   "Courier-Oblique",  // 10
   "Courier-Bold",
   "Courier-BoldOblique",
   "Symbol",
   "ZapfDingbats" // 14
};
int num_predefined_fonts = 14;
int num_pdf_standard_fonts = 14;

long *obj_offsets, capacity_obj_offsets, stream_start, stream_end;
long length_offset, xref_offset;
long num_objects, next_available_object_number, next_available_gs_number;
long next_available_xo_number;
long next_available_shade_number, next_available_font_number;
Stroke_Opacity_State *stroke_opacities = NULL;
Fill_Opacity_State *fill_opacities = NULL;
XObject_Info *xobj_list = NULL;
Function_Info *functions_list;
Shading_Info *shades_list = NULL;
Font_Dictionary *font_dictionaries = NULL;
Old_Font_Dictionary *old_font_dictionaries = NULL;
FILE *OF = NULL; // for the PDF file
FILE *TF = NULL; // for the temp file


/* PDF File Management */

static void
Free_XObjects(int *ierr)
{
   XObject_Info *xo;
   while (xobj_list != NULL) {
      xo = xobj_list;
      xobj_list = xo->next;
      switch (xo->xobj_subtype) {
      case JPG_SUBTYPE:
         Free_JPG((JPG_Info *)xo);
         break;
      case SAMPLED_SUBTYPE:
         Free_Sampled((Sampled_Info *)xo);
         break;
      default:
         RAISE_ERROR_i("Invalid XObject subtype (%i)",
                       xo->xobj_subtype, ierr);
         return;
      }
      free(xo);
   }
}


void
Init_pdf(int *ierr)
{
   int i;
   writing_file = false;
   capacity_obj_offsets = 1000;
   num_objects = 0;
   obj_offsets = ALLOC_N_long(capacity_obj_offsets);
   for (i=0; i < capacity_obj_offsets; i++) obj_offsets[i] = 0;
}


void
Record_Object_Offset(int obj_number)
{
   long int offset = ftell(OF);
   if (obj_number >= capacity_obj_offsets) {
      int size_increment = 50, i;
      REALLOC_long(&obj_offsets, obj_number + size_increment);
      capacity_obj_offsets = obj_number + size_increment;
      for (i=num_objects; i < capacity_obj_offsets; i++) obj_offsets[i] = 0;
   }
   obj_offsets[obj_number] = offset;
   if (obj_number >= num_objects) num_objects = obj_number + 1;
}


static void
Write_XObjects(int *ierr)
{
   XObject_Info *xo;
   for (xo = xobj_list; xo != NULL; xo = xo->next) {
      Record_Object_Offset(xo->obj_num);
      fprintf(OF, "%i 0 obj << /Type /XObject ", xo->obj_num);
      switch (xo->xobj_subtype) {
      case JPG_SUBTYPE:
         Write_JPG((JPG_Info *)xo, ierr);
         break;
      case SAMPLED_SUBTYPE:
         Write_Sampled((Sampled_Info *)xo, ierr);
         break;
      default:
         RAISE_ERROR_i("Invalid XObject subtype (%i)", xo->xobj_subtype, ierr);
      }
      if (*ierr != 0) return;
      fprintf(OF, ">> endobj\n");
   }
}


#define INFO_OBJ 1
#define PAGES_OBJ 2
#define STREAM_OBJ 3
#define PAGE_OBJ 4
#define CATALOG_OBJ 5
#define FIRST_OTHER_OBJ 6


static void
Free_Records(int *ierr)
{
   Free_Stroke_Opacities();
   Free_Fill_Opacities();
   Free_XObjects(ierr);
   Free_Shadings();
   Free_Functions();
}


static void
Get_pdf_name(char *ofile, char *filename, int maxlen)
{
   char *dot;
   strncpy(ofile, filename, maxlen);
   dot = strrchr(ofile, '.');
   if (dot != NULL) dot[0] = '\0';
   strcat(ofile, "_figure.pdf");
}


void
Open_pdf(OBJ_PTR fmkr, FM *p, char *filename, bool quiet_mode, int *ierr)
{
   int i;
   if (writing_file) {
      RAISE_ERROR("Sorry: cannot start a new output file until finish "
                  "current one.", ierr);
      return;
   }
   Clear_Fonts_In_Use_Flags();
   Free_Records(ierr);
   if (*ierr != 0) return;
   next_available_object_number = FIRST_OTHER_OBJ;
   next_available_font_number = num_predefined_fonts + 1;
   next_available_gs_number = 1;
   next_available_xo_number = 1;
   next_available_shade_number = 1;
   writing_file = true;
   time_t now = time(NULL);
   char ofile[300], timestring[100];
   Get_pdf_name(ofile, filename, 300);
   if ((OF = fopen(ofile, "wb")) == NULL) { /* Write binary file ! */
      RAISE_ERROR_s("Sorry: can't open %s.\n", filename, ierr);
      return;
   }
   if ((TF = tmpfile()) == NULL) {
      RAISE_ERROR_s("Sorry: can't create temp file for writing PDF file %s.\n",
                    filename, ierr);
      return;
   }
   /* open PDF file and write header */
   fprintf(OF, "%%PDF-1.4\n");
   strcpy(timestring, ctime(&now));
   i = strlen(timestring);
   if (i > 0) timestring[i-1] = '\0';
   Record_Object_Offset(INFO_OBJ);
   fprintf(OF,
           "%i 0 obj <<\n/Creator (Tioga)\n/CreationDate (%s)\n>>\nendobj\n",
           INFO_OBJ, timestring);
   Record_Object_Offset(PAGES_OBJ);
   fprintf(OF,
           "%i 0 obj <<\n/Type /Pages\n/Kids [%i 0 R]\n/Count 1\n>> endobj\n",
           PAGES_OBJ, PAGE_OBJ);
   Record_Object_Offset(STREAM_OBJ);
   if (FLATE_ENCODE)
      fprintf(OF, "%i 0 obj <<\t/Filter /FlateDecode   /Length ", STREAM_OBJ);
   else
      fprintf(OF, "%i 0 obj <<\t/Length ", STREAM_OBJ);
   length_offset = ftell(OF);
   fprintf(OF, "             \n>>\nstream\n");
   stream_start = ftell(OF);
   fprintf(TF, "%.2f 0 0 %.2f %.2f %.2f cm\n", 1.0/ENLARGE, 1.0/ENLARGE,
           Get_pdf_xoffset(), Get_pdf_yoffset());
   /* set stroke and fill colors to black */
   have_current_point = constructing_path = false;
   c_line_width_set(fmkr, p, p->line_width, ierr);
   c_line_cap_set(fmkr, p, p->line_cap, ierr);
   c_line_join_set(fmkr, p, p->line_join, ierr);
   c_miter_limit_set(fmkr, p, p->miter_limit, ierr);
   c_line_type_set(fmkr, p, Get_line_type(fmkr, ierr), ierr);
   c_stroke_color_set_RGB(fmkr, p, p->stroke_color_R, p->stroke_color_G,
                          p->stroke_color_B, ierr);
   c_fill_color_set_RGB(fmkr, p, p->fill_color_R, p->fill_color_G,
                        p->fill_color_B, ierr);
   // initialize clip region
   bbox_llx = bbox_lly = 1e5;
   bbox_urx = bbox_ury = -1e5;
}


void
Start_Axis_Standard_State(OBJ_PTR fmkr, FM *p, double r, double g, double b,
                          double line_width, int *ierr)
{
   fprintf(TF, "q 2 J [] 0 d\n");
   c_line_width_set(fmkr, p, line_width, ierr);
   c_stroke_color_set_RGB(fmkr, p, r, g, b, ierr);
   /* 2 J sets the line cap style to square cap */
   /* set stroke and fill colors to black.  set line type to solid */
}


void
End_Axis_Standard_State(void)
{
   Write_grestore();
}


void
Write_gsave(void)
{
   fprintf(TF, "q\n");
}


void
Write_grestore(void)
{
   fprintf(TF, "Q\n");
}


void
c_pdf_gsave(OBJ_PTR fmkr, FM *p, int *ierr)
{
   Write_gsave();
}


void
c_pdf_grestore(OBJ_PTR fmkr, FM *p, int *ierr)
{
   Write_grestore();
}


static void
Print_Xref(long int offset)
{
   char line[80];
   int i, len;
   snprintf(line,sizeof(line), "%li", offset);
   len = strlen(line);
   for (i=0; i < 10-len; i++) fputc('0', OF);
   fprintf(OF, "%s 00000 n \n", line);
}


static void
Write_Stream(int *ierr)
{
   long int len = ftell(TF);
   unsigned long int new_len = (len * 11) / 10 + 100;
   unsigned char *buffer, *dest_buffer;
   rewind(TF);
   buffer = ALLOC_N_unsigned_char(len+1);
   dest_buffer = ALLOC_N_unsigned_char(new_len+1);
   fread(buffer, 1, len, TF);
   fclose(TF);
   if (FLATE_ENCODE) {
      if (do_flate_compress(dest_buffer, &new_len, buffer, len) != FLATE_OK) {
         free(buffer); free(dest_buffer);
         RAISE_ERROR("Error compressing PDF stream data", ierr); 
         return;
      }
      fwrite(dest_buffer, 1, new_len, OF);
   }
   else {
      fwrite(buffer, 1, len, OF);
   }
   free(buffer);
   free(dest_buffer);
}


void
Close_pdf(OBJ_PTR fmkr, FM *p, bool quiet_mode, int *ierr)
{
   int i;
   double llx, lly, urx, ury, xoff, yoff;
   if (!writing_file) {
      RAISE_ERROR("Sorry: cannot End_Output if not writing file.", ierr);
      return;
   }
   writing_file = false;
   if (constructing_path) {
      RAISE_ERROR("Sorry: must finish with current path before ending file",
                  ierr);
      return;
   }
   Write_Stream(ierr);
   if (*ierr != 0) return;
   stream_end = ftell(OF);
   fprintf(OF, "endstream\nendobj\n");
   Record_Object_Offset(PAGE_OBJ);
   fprintf(OF, "%i 0 obj <<\n/Type /Page\n/Parent %i 0 R\n/MediaBox [ ",
           PAGE_OBJ, PAGES_OBJ);
   if (bbox_llx < p->page_left) bbox_llx = p->page_left;
   if (bbox_lly < p->page_bottom) bbox_lly = p->page_bottom;
   if (bbox_urx > p->page_left + p->page_width)
      bbox_urx = p->page_left + p->page_width;
   if (bbox_ury > p->page_bottom + p->page_height)
      bbox_ury = p->page_bottom + p->page_height;
//#define MARGIN 3
#define MARGIN 0
   xoff = Get_pdf_xoffset();
   yoff = Get_pdf_yoffset();
   llx = bbox_llx / ENLARGE + xoff - MARGIN;  // convert back to points
   lly = bbox_lly / ENLARGE + yoff - MARGIN;
   urx = bbox_urx / ENLARGE + xoff + MARGIN;
   ury = bbox_ury / ENLARGE + yoff + MARGIN;
   if (urx < llx || ury < lly) {
      RAISE_ERROR("Sorry: Empty plot!", ierr);
      return;
   }
   fprintf(OF, "%d %d %d %d", ROUND(llx), ROUND(lly), ROUND(urx), ROUND(ury));
   fprintf(OF, " ]\n/Contents %i 0 R\n/Resources << "
           "/ProcSet [/PDF /Text /ImageB /ImageC /ImageI]\n", STREAM_OBJ);
   if (Used_Any_Fonts()) {
      Font_Dictionary *f;      
      fprintf(OF, "    /Font <<\n     ");
      for (f = font_dictionaries; f != NULL; f = f->next) {
         if (!f->in_use) continue;
         fprintf(OF, "      /F%i %i 0 R\n", f->font_num, f->obj_num);
      }
      fprintf(OF, "    >>\n"); // end of /Font
   }
   if (fill_opacities != NULL || stroke_opacities != NULL) {
      // ExtGstate objects go here
      Fill_Opacity_State *pf;
      Stroke_Opacity_State *ps;
      fprintf(OF, "    /ExtGState <<\n");
      for (ps = stroke_opacities; ps != NULL; ps = ps->next) {
         fprintf(OF, "      /GS%i %i 0 R\n", ps->gs_num, ps->obj_num);
      }
      for (pf = fill_opacities; pf != NULL; pf = pf->next) {
         fprintf(OF, "      /GS%i %i 0 R\n", pf->gs_num, pf->obj_num);
      }
      fprintf(OF, "    >>\n"); // end of /ExtGState
   }
   if (xobj_list != NULL) {
      // Xobjects go here
      XObject_Info *xo;
      fprintf(OF, "    /XObject <<\n");
      for (xo = xobj_list; xo != NULL; xo = xo->next) {
         fprintf(OF, "      /XObj%i %i 0 R\n", xo->xo_num, xo->obj_num);
      }
      fprintf(OF, "    >>\n"); // end of /XObject
   }
   if (shades_list != NULL) {
      // Shadings go here
      Shading_Info *so;
      fprintf(OF, "    /Shading <<\n");
      for (so = shades_list; so != NULL; so = so->next) {
         fprintf(OF, "      /Shade%i %i 0 R\n", so->shade_num, so->obj_num);
      }
      fprintf(OF, "    >>\n"); // end of /Shading
   }
   fprintf(OF, "  >>\n"); // end of /Resources
   fprintf(OF, ">> endobj\n");
   Record_Object_Offset(CATALOG_OBJ);
   fprintf(OF, "%i 0 obj <<\n/Type /Catalog\n/Pages %i 0 R\n>> endobj\n",
           CATALOG_OBJ, PAGES_OBJ);
   Write_Font_Dictionaries();
   Write_Font_Descriptors();
   Write_Font_Widths();
   Write_Stroke_Opacity_Objects();
   Write_Fill_Opacity_Objects();
   Write_XObjects(ierr);
   if (*ierr != 0) return;
   Write_Functions(ierr);
   if (*ierr != 0) return;
   Write_Shadings();
   xref_offset = ftell(OF);
   fprintf(OF, "xref\n0 %li\n0000000000 65535 f \n", num_objects);
   for (i = 1; i < num_objects; i++)
      Print_Xref(obj_offsets[i]); // NB: DONT USE OBJECT 0
   fprintf(OF, "trailer\n<<\n/Size %li\n/Root %i 0 R\n/Info %i 0 "
           "R\n>>\nstartxref\n%li\n%%%%EOF\n",
           num_objects, CATALOG_OBJ, INFO_OBJ, xref_offset);
   fseek(OF, length_offset, SEEK_SET);
   fprintf(OF, "%li", stream_end - stream_start);
   fclose(OF);
   Free_Records(ierr);
}


void
Rename_pdf(char *oldname, char *newname)
{
   char old_ofile[300], new_ofile[300];
   Get_pdf_name(old_ofile, oldname, 300);
   Get_pdf_name(new_ofile, newname, 300);
   rename(old_ofile, new_ofile); // from stdio.h
}

