/* pdfs.h */
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

#ifndef __pdfs_H__
#define __pdfs_H__

#include <namespace.h>

extern long *obj_offsets, capacity_obj_offsets, stream_start, stream_end, length_offset, xref_offset;
extern long num_objects, next_available_object_number, next_available_gs_number, next_available_xo_number;
extern long next_available_shade_number, next_available_font_number;
extern void Record_Object_Offset(int obj_number);
extern char *predefined_Fonts[];
extern int num_pdf_standard_fonts, num_predefined_fonts;

typedef struct stroke_opacity_state {
   struct stroke_opacity_state *next;
   int gs_num;
   int obj_num;
   double stroke_opacity;
} Stroke_Opacity_State;
extern Stroke_Opacity_State *stroke_opacities;

typedef struct fill_opacity_state {
   struct fill_opacity_state *next;
   int gs_num;
   int obj_num;
   double fill_opacity;
} Fill_Opacity_State;
extern Fill_Opacity_State *fill_opacities;

typedef struct xobj_info {
   struct xobj_info *next;
   int xo_num;
   int obj_num;
   int xobj_subtype;
} XObject_Info;
extern XObject_Info *xobj_list;

typedef struct jpg_info {
   // start must match start of xobj_info
   struct xobj_info *next;
   int xo_num;
   int obj_num;
   int xobj_subtype;
   // remainder is for this subtype of xobj
   int width, height;
   int mask_obj_num;
   char *filename;
} JPG_Info;


/* Parses a JPEG file */
JPG_Info * Parse_JPG(const char * file);
extern void Write_JPG(JPG_Info *xo, int *ierr);
extern void Free_JPG(JPG_Info *xo);



typedef struct sampled_info {
   // start must match start of xobj_info
   struct xobj_info *next;
   int xo_num;
   int obj_num;
   int xobj_subtype;
   // remainder is for this subtype of xobj
   int width, height;
   int length; // number of bytes of image data
   unsigned char *image_data;
   bool interpolate;
   bool reversed; // only applies to mono images
   int mask_obj_num;
   int image_type;
   int value_mask_min;
   int value_mask_max;
   int hival;
   int lookup_len;
   unsigned char *lookup;
  int components; /* number of bits per element (one color) */
  char * filters; /* PDF filters to be used "as-is" */
} Sampled_Info;
extern void Write_Sampled(Sampled_Info *xo, int *ierr);
extern void Free_Sampled(Sampled_Info *xo);

#define JPG_SUBTYPE 1
#define SAMPLED_SUBTYPE 2

#define RGB_IMAGE 0
#define HLS_IMAGE 5
#define CMYK_IMAGE 4
#define GRAY_IMAGE 1
#define MONO_IMAGE 2
#define COLORMAP_IMAGE 3

typedef struct function_info {
   struct function_info *next;
   int obj_num;
   int hival;
   int lookup_len;
   unsigned char *lookup;
} Function_Info;
extern Function_Info *functions_list;

typedef struct shading_info {
   struct shading_info *next;
   int shade_num;
   int obj_num;
   bool axial;
   double x0;
   double y0;
   double x1;
   double y1;
   double r0;
   double r1;
   int function;
   bool extend_start;
   bool extend_end;
} Shading_Info;
extern Shading_Info *shades_list;

typedef struct { 
   int font_num; // for making font resource name such as /F7
   char *font_name;
   int firstChar, lastChar;  // firstChar typically is 0 and lastChar 255
   int char_width[256], char_llx[256], char_lly[256], char_urx[256], char_ury[256];
   int widths_obj_num;
   /* FontDescriptor */
   int flags; // describe the font
   int fnt_llx, fnt_lly, fnt_urx, fnt_ury;  // FontBBox
   int italicAngle, ascent, descent, capHeight, stemV;
} Font_Afm_Info;

extern Font_Afm_Info afm_array[];

typedef struct font_dictionary { 
   struct font_dictionary *next;
   int font_num; // for making font resource name such as /F7
   int obj_num;
   bool in_use;
   int widths_obj_num;
   int descriptor_obj_num;
   Font_Afm_Info *afm;
} Font_Dictionary;
extern Font_Dictionary *font_dictionaries;

typedef struct old_font_dictionary { 
   struct old_font_dictionary *next;
   int font_num; // for making font resource name such as /F7
   int obj_num;
   bool in_use;
   char *font_name;
   int firstChar, lastChar;  // firstChar typically is 0 and lastChar 255
   int char_width[256], char_llx[256], char_lly[256], char_urx[256], char_ury[256];
   int widths_obj_num;
   /* FontDescriptor */
   int descriptor_obj_num;
   int flags; // describe the font
   int fnt_llx, fnt_lly, fnt_urx, fnt_ury;  // FontBBox
   int italicAngle, ascent, descent, capHeight, stemV;
} Old_Font_Dictionary;
extern Old_Font_Dictionary *old_font_dictionaries;

#define FixedPitchFlag 1
#define SerifFlag 2
#define SymbolicFlag 4
#define ScriptFlag 8
#define NonsymbolicFlag (1<<5)
#define ItalicFlag (1<<6)
#define AllCapFlag (1<<16)
#define SmallCapFlag (1<<17)
#define ForceBoldFlag (1<<18)

#define RADIANS_TO_DEGREES (180.0 / PI)

extern bool have_current_point, constructing_path, writing_file;

extern FILE *OF; // for the PDF file
extern FILE *TF; // for the temp file holding the uncompressed stream


#endif   /* __pdfs_H__ */

