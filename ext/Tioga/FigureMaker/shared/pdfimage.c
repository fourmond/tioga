/* pdfimage.c */
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

/*  Images

   invoke image by "/Image_name Do" in the content stream
   
   the resources dictionary must have image object names in XObject dictionary
      /ProcSet [/PDF /Text /ImageB /ImageC /ImageI]
      /XObject <<
         /Image_name 30 0 R
         ...
         >>
         
   30 0 obj <<
      /Type /XObject
      /Subtype /Image
      /Width <integer>  number of columns.  required for all.
      /Height <integer>  number of rows.  required for all.
      /Length <integer>  number of bytes in image data stream.  required for all.
      /Interpolate <boolean>  optional for all.
      /SMask <stream>  optional for all.
      ...
   >>
   stream
   ...image data...
   endstream
   endobj
   
   For JPEG2000 images:
      /Filter /JPXDecode
      /Mask <image_mask_name>  for explicit masking.  optional.
   
   For JPEG images:
      /Filter /DCTDecode
      /ColorSpace <name or array>.  required.  /DeviceRGB ?
      /BitsPerComponent 8
      /Mask <image_mask_name>  for explicit masking.  optional.
   
   For sampled images:
      /Filter <name>  whatever filter being used for the image data.  optional.
      /ColorSpace <name or array>.  required.
      /BitsPerComponent 8
      /Mask <image_mask_name>  for explicit masking.  optional.
      /Mask [min, max] for masking out specified sample values.
         for example, you can use 0 to stand for "undefined", then setting /Mask [0 0] will cause all
            0 valued samples to be masked out of the image.
      
   For image masks:
      /ImageMask true
      /BitsPerComponent 1
      /Decode [0 1] means sample values of 0 are included in the output, values of 1 are excluded
      /Decode [1 0] means sample values of 1 are included in the output, values of 0 are excluded
      
*/     

/*
  Reads next byte, and set *eof on end of file
*/
static int read_byte(FILE * file, int *eof)
{
  int c = fgetc(file);
  if(c == EOF)
    *eof = 1;
  else
    *eof = 0;
  return c;
}

/* Reads a 16 bits word */
static unsigned read_word(FILE * file, int *eof)
{
  int v = fgetc(file);
  int c = fgetc(file);
  if(v == EOF || c == EOF)
    *eof = 1;
  else
    *eof = 0;
  return ((unsigned) v) << 8 | ((unsigned) c);
}

/* Reads until the next tag, and returns its code */
static int read_next_tag(FILE * file, int *eof)
{
  int c;
  *eof = 0;

  /* Discard non 0xFF bytes */
  do {
    c = read_byte(file, eof);
    if(*eof)
      return 0xFF;
  } while(c != 0xFF);

  do {
    c = read_byte(file, eof);
    if(*eof)
      return 0xFF;
  } while (c == 0xFF);
  return c;
}

static void skip_variable_length(FILE * file, int *eof)
{
  *eof = 0;
  int len = read_word(file, eof);
  if(*eof)
    return;
  if(len < 2) {
    *eof = 1;
    return;
  }
  len -= 2;
  while(len > 0) {
    --len;
    read_byte(file, eof);
    if(*eof)
      return;
  }
}


/* Parses a JPEG file and extracts the resolution data from it, and
   returns a newly allocated JPG_Info structure.
*/
JPG_Info * Parse_JPG(const char * file) 
{

  FILE * f = fopen(file, "rb");
  if(! f)
    return NULL;

  int eof = 0;
  int tag = read_next_tag(f, &eof);
  if(tag != 0xD8 || eof) {
    fclose(f);
    return NULL;
  }

  while(1) {
    tag = read_next_tag(f, &eof);
    if(eof)  {
      fclose(f);
      return NULL;
    }
    switch(tag) {
    case 0xC0: /* image data */
      {
        int len = read_word(f, &eof);
        int bps = read_byte(f, &eof);
        int y = read_word(f, &eof);
        int x = read_word(f, &eof);
        int cmps = read_byte(f, &eof);
        fclose(f);
        if(eof)
          return NULL;
          
        JPG_Info * val = (JPG_Info *)calloc(1, sizeof(JPG_Info));
        val->filename = ALLOC_N_char(strlen(file)+1);
        strcpy(val->filename, file);
        val->width = x;
        val->height = y;
        return val;
      }
    default:
      skip_variable_length(f, &eof);
    }
  }
}



void
Free_JPG(JPG_Info *xo)
{
   if (xo->filename != NULL) free(xo->filename);
}


void
Free_Sampled(Sampled_Info *xo)
{
   if (xo->image_data != NULL) free(xo->image_data);
   if (xo->lookup != NULL) free(xo->lookup);
   if (xo->filters != NULL) free(xo->filters);
}


static bool
Is_monochrome(int obj_num)
{
   XObject_Info *xo;
   for (xo = xobj_list; xo != NULL; xo = xo->next) {
      if (xo->xobj_subtype == SAMPLED_SUBTYPE && xo->obj_num == obj_num) {
         Sampled_Info *p = (Sampled_Info *)xo;
         return (p->image_type == MONO_IMAGE);
      }
   }
   return false;
}


static void
Write_Image_From_File(char *filename, int width, int height, char *out_info,
                      int mask_obj_num, int *ierr)
{
   FILE *jpg = fopen(filename, "rb"); /* We read binary files ! */
   if (jpg == NULL) {
      RAISE_ERROR_s("Sorry: cannot open file for showing image (%s)\n",
                    filename, ierr);
      return;
   }
   unsigned char *buff;
   int len, rd_len;
   int buff_len = 256000;
   buff = ALLOC_N_unsigned_char(buff_len);
   len = 0;
   while ((rd_len = fread(buff, 1, buff_len, jpg)) == buff_len) {
      len += buff_len;
   }
   len += rd_len;
   fprintf(OF, "\t/Subtype /Image\n");
   if (mask_obj_num > 0) {
      if (!Is_monochrome(mask_obj_num))
         fprintf(OF, "\t/SMask %i 0 R\n", mask_obj_num);
      else
         fprintf(OF, "\t/Mask %i 0 R\n", mask_obj_num);
   }
   fprintf(OF, "\t/Width %i\n", width);
   fprintf(OF, "\t/Height %i\n", height);
   fprintf(OF, "%s", out_info);
   fprintf(OF, "\t/Length %i\n\t>>\nstream\n", len);
   if (len < buff_len) fwrite(buff, 1, len, OF);
   else {
      rewind(jpg);
      while ((rd_len = fread(buff, 1, buff_len, jpg)) == buff_len) {
         fwrite(buff, 1, buff_len, OF);
      }
      fwrite(buff, 1, rd_len, OF);
   }
   fprintf(OF, "\nendstream\n");
   fclose(jpg);
}


void
Write_JPG(JPG_Info *xo, int *ierr)
{
   Write_Image_From_File(xo->filename, xo->width, xo->height, 
                         "\t/Filter /DCTDecode\n\t/ColorSpace "
                         "/DeviceRGB\n\t/BitsPerComponent 8\n",
                         xo->mask_obj_num, ierr);
}

extern void
str_hls_to_rgb_bang(unsigned char* str, long len);
   
void
Write_Sampled(Sampled_Info *xo, int *ierr)
{
   fprintf(OF, "\n\t/Subtype /Image\n");
   fprintf(OF, "\t/Interpolate %s\n",
           (xo->interpolate)? "true":"false");
   fprintf(OF, "\t/Height %i\n", xo->height);
   fprintf(OF, "\t/Width %i\n", xo->width);
   int i, len;
   unsigned long new_len;
   unsigned char *image_data;
   unsigned char *buffer;
   unsigned char *wd;
   switch (xo->image_type) {
      case RGB_IMAGE:
      case HLS_IMAGE:
         fprintf(OF, "\t/ColorSpace /DeviceRGB\n");
         fprintf(OF, "\t/BitsPerComponent %d\n", xo->components);
         break;
      case CMYK_IMAGE:
         fprintf(OF, "\t/ColorSpace /DeviceCMYK\n");
         fprintf(OF, "\t/BitsPerComponent %d\n", xo->components);
         break;
      case GRAY_IMAGE:
         fprintf(OF, "\t/ColorSpace /DeviceGray\n");
         fprintf(OF, "\t/BitsPerComponent %d\n", xo->components);
         break;
      case MONO_IMAGE:
         fprintf(OF, "\t/ImageMask true\n");
         fprintf(OF, "\t/BitsPerComponent 1\n");
         if (!xo->reversed) fprintf(OF, "\t/Decode [0 1]\n");
         else fprintf(OF, "\t/Decode [1 0]\n");
         break;
      default:
         len = xo->lookup_len;
         fprintf(OF, "\t/ColorSpace [ /Indexed /DeviceRGB %i <", xo->hival);
         for (i = 0; i < len; i++) {
            unsigned char c = xo->lookup[i];
            if (c == 0) fprintf(OF, "00");
            else if (c < 16) fprintf(OF, "0%x", c);
            else fprintf(OF, "%x", c);
         }
         fprintf(OF, "> ]\n");
         fprintf(OF, "\t/BitsPerComponent %d\n", xo->components);
   }
   if (xo->mask_obj_num > 0) {
      if (xo->image_type == MONO_IMAGE) {
         RAISE_ERROR("Sorry: monochrome images must not have masks", ierr);
         return;
      }
      if (!Is_monochrome(xo->mask_obj_num))
         fprintf(OF, "\t/SMask %i 0 R\n", xo->mask_obj_num);
      else
         fprintf(OF, "\t/Mask %i 0 R\n", xo->mask_obj_num);
   }
   if (xo->value_mask_min >= 0 && xo->value_mask_max >= 0
       && xo->value_mask_min <= 255 && xo->value_mask_max <= 255
       && xo->value_mask_min <= xo->value_mask_max)
      fprintf(OF, "\t/Mask [%i %i]\n", xo->value_mask_min, xo->value_mask_max);
   
   if (xo->image_type == HLS_IMAGE) {
      image_data = ALLOC_N_unsigned_char(xo->length);
      memcpy(image_data, xo->image_data, xo->length);
      str_hls_to_rgb_bang(image_data, xo->length);
   } else {
      image_data = xo->image_data;
   }

   buffer = NULL;
   wd = image_data;
   
   if(xo->filters) {
     new_len = xo->length;
     fprintf(OF, "%s", xo->filters);
   }
   else {
     fprintf(OF, "\t/Filter /FlateDecode\n");
   
     new_len = (xo->length * 11)/10 + 100;
     buffer = ALLOC_N_unsigned_char(new_len);
     if (do_flate_compress(buffer, &new_len, image_data, xo->length)
         != FLATE_OK) {
       free(buffer);
       RAISE_ERROR("Error compressing image data", ierr); 
       return;
     }
     wd = buffer;
   }
   fprintf(OF, "\t/Length %li\n", new_len);
   fprintf(OF, "\t>>\nstream\n");
   if (fwrite(wd, 1, new_len, OF) < new_len) {
      RAISE_ERROR("Error writing image data", ierr);
      return;
   }
   if(buffer)
     free(buffer);
   if (xo->image_type == HLS_IMAGE) free(image_data);
   fprintf(OF, "\nendstream\nendobj\n");
}


// transform maps (0,0), (1,0), and (0,1) to the given points
static void
Create_Transform_from_Points(double llx, double lly, double lrx, double lry,
                             double ulx, double uly, double *a, double *b,
                             double *c, double *d, double *e, double *f)
{
   *e = llx; *f = lly; lrx -= llx; ulx -= llx; lry -= lly; uly -= lly;
   *a = lrx; *b = lry; *c = ulx; *d = uly;
}


/* Read the image here ?*/
int
c_private_register_jpg(OBJ_PTR fmkr, FM *p, char *filename, 
                       int width, int height,
                       int mask_obj_num, int *ierr)
{
  JPG_Info *xo = (JPG_Info *)calloc(1,sizeof(JPG_Info));
  xo->xobj_subtype = JPG_SUBTYPE;
  xo->next = xobj_list;
  xobj_list = (XObject_Info *)xo;
  xo->xo_num = next_available_xo_number++;
  xo->obj_num = next_available_object_number++;
  xo->filename = ALLOC_N_char(strlen(filename)+1);
  strcpy(xo->filename, filename);
  xo->width = width;
  xo->height = height;
  xo->mask_obj_num = mask_obj_num;
  return xo->obj_num;
}


static void
Expand_Array(OBJ_PTR image_destination, double *dest, int *ierr)
{
   int len = Array_Len(image_destination,ierr);
   if (*ierr != 0) return;
   if (len != 6) {
      RAISE_ERROR("Sorry: invalid image destination array: "
                  "must have 6 entries", ierr);
      return;
   }
   int i;
   for (i = 0; i < 6; i++) {
      OBJ_PTR entry = Array_Entry(image_destination, i, ierr);
      if (*ierr != 0) return;
      dest[i] = Number_to_double(entry, ierr);
      if (*ierr != 0) return;
   }
}

void
c_private_show_jpg(OBJ_PTR fmkr, FM *p, char *filename, 
                   int width, int height, OBJ_PTR image_destination,
                   int mask_obj_num, int *ierr)
{
   double dest[6];
   int ref;
   if (constructing_path) {
      RAISE_ERROR("Sorry: must finish with current path before "
                  "calling show_jpg", ierr);
      return;
   }
   ref = c_private_register_jpg(fmkr, p, filename, width, height, 
                                mask_obj_num, ierr);
   Expand_Array(image_destination, dest, ierr);
   if (*ierr != 0) return;

   c_private_show_image_from_ref(fmkr, p, ref, dest[0], dest[1], 
                                 dest[2], dest[3], dest[4], dest[5],
                                 ierr);
}


OBJ_PTR
c_private_create_image_data(OBJ_PTR fmkr, FM *p, OBJ_PTR table,
                            int first_row, int last_row, int first_column,
                            int last_column, double min_val, double max_val,
                            int max_code, int if_below_range,
                            int if_above_range, int *ierr)
{
   long num_cols, num_rows;
   double **data = Table_Data_for_Read(table, &num_cols, &num_rows, ierr);
   if (*ierr != 0) RETURN_NIL;
   if (first_column < 0) first_column += num_cols;
   if (first_column < 0 || first_column >= num_cols)
      RAISE_ERROR_i("Sorry: invalid first_column specification (%i)",
                    first_column, ierr);
   if (last_column < 0) last_column += num_cols;
   if (last_column < 0 || last_column >= num_cols)
      RAISE_ERROR_i("Sorry: invalid last_column specification (%i)",
                    last_column, ierr);
   if (first_row < 0) first_row += num_rows;
   if (first_row < 0 || first_row >= num_rows)
      RAISE_ERROR_i("Sorry: invalid first_row specification (%i)",
                    first_row, ierr);
   if (last_row < 0) last_row += num_rows;
   if (last_row < 0 || last_row >= num_rows)
      RAISE_ERROR_i("Sorry: invalid last_row specification (%i)",
                    last_row, ierr);
   if (min_val >= max_val)
      RAISE_ERROR_gg("Sorry: invalid range specification: min %g max %g",
                     min_val, max_val, ierr);
   if (max_code <= 0 || max_code > 255)
      RAISE_ERROR_i("Sorry: invalid max_code specification (%i)",
                    max_code, ierr);
   if (if_below_range < 0 || if_below_range > 255)
      RAISE_ERROR_i("Sorry: invalid if_below_range specification (%i)",
                    if_below_range, ierr);
   if (if_above_range < 0 || if_above_range > 255)
      RAISE_ERROR_i("Sorry: invalid if_above_range specification (%i)",
                    if_above_range, ierr);
   int i, j, k;
   int width = last_column - first_column + 1;
   int height = last_row - first_row + 1;
   int sz = width * height;
   if (sz <= 0)
      RAISE_ERROR_ii("Sorry: invalid data specification: width (%i) "
                     "height (%i)", width, height, ierr);
   if (*ierr != 0) RETURN_NIL;
   char *buff = ALLOC_N_char(sz);
   for (k = 0, i = first_row; i <= last_row; i++) {
      double *row = data[i];
      for (j = first_column; j <= last_column; j++) {
         double val = row[j];
         if (val < min_val) buff[k++] = if_below_range;
         else if (val > max_val) buff[k++] = if_above_range;
         else {
            val = max_code * (val - min_val)/(max_val - min_val);
            buff[k++] = ROUND(val);
         }
      }
   }
   OBJ_PTR result = String_New(buff, sz);
   free(buff);
   return result;
}


OBJ_PTR
c_private_create_monochrome_image_data(OBJ_PTR fmkr, FM *p, OBJ_PTR table,
                                       int first_row, int last_row,
                                       int first_column, int last_column,
                                       double boundary, bool reversed,
                                       int *ierr)
{
   long num_cols, num_rows;
   double **data = Table_Data_for_Read(table, &num_cols, &num_rows, ierr);
   if (*ierr != 0) RETURN_NIL;
   if (first_column < 0) first_column += num_cols;
   if (first_column < 0 || first_column >= num_cols)
      RAISE_ERROR_i("Sorry: invalid first_column specification (%i)",
                    first_column, ierr);
   if (last_column < 0) last_column += num_cols;
   if (last_column < 0 || last_column >= num_cols)
      RAISE_ERROR_i("Sorry: invalid last_column specification (%i)",
                    last_column, ierr);
   if (first_row < 0) first_row += num_rows;
   if (first_row < 0 || first_row >= num_rows)
      RAISE_ERROR_i("Sorry: invalid first_row specification (%i)",
                    first_row, ierr);
   if (last_row < 0) last_row += num_rows;
   if (last_row < 0 || last_row >= num_rows)
      RAISE_ERROR_i("Sorry: invalid last_row specification (%i)",
                    last_row, ierr);
   int i, j, k;
   int width = last_column - first_column + 1;
   int height = last_row - first_row + 1;
   int bytes_per_row = (width+7)/8;
   int sz = bytes_per_row * 8 * height;
   if (sz <= 0)
      RAISE_ERROR_ii("Sorry: invalid data specification: width (%i) "
                     "height (%i)", width, height, ierr);
   if (*ierr != 0) RETURN_NIL;
   // to simplify the process, do it in two stages: first get the
   // values and then pack the bits
   char *buff = ALLOC_N_char(sz);
   for (k = 0, i = first_row; i <= last_row; i++) {
      double *row = data[i];
      for (j = first_column; j <= last_column; j++) {
         double val = row[j];
         buff[k++] = (reversed)? (val <= boundary) : (val > boundary);
      }
      for (j = last_column+1; j < bytes_per_row * 8; j++) {
         buff[k++] = 0;
      }
   }
   int num_bytes = (sz+7) >> 3;
   char *bits = ALLOC_N_char(num_bytes), c = 0;
   int num_bits = num_bytes << 3;
   for (i = 0, k = -1; i < num_bits; i++) {
      int bit = (i < sz)? buff[i] : 0;
      int which_bit = i & 7;
      if (which_bit != 0) c |= bit << (7-which_bit);
      else {
         if (k >= 0) bits[k] = c;
         k++; c = bit << 7;
      }
   }
   bits[k] = c;
   OBJ_PTR result = String_New(bits, num_bytes);
   free(bits); free(buff);
   return result;
}


OBJ_PTR
c_private_show_image(OBJ_PTR fmkr, FM *p, int image_type, double llx,
                     double lly, double lrx, double lry, double ulx,
                     double uly, bool interpolate, bool reversed,
                     int w, int h, unsigned char* data, long len, 
                     OBJ_PTR mask_min, OBJ_PTR mask_max, OBJ_PTR hivalue,
                     OBJ_PTR lookup_data, int mask_obj_num, int components,
                     const char * filters,
                     int *ierr)
{
  int ref = c_private_register_image(fmkr, p, image_type,
                                     interpolate, reversed,
                                     w, h, data, len, mask_min, 
                                     mask_max, hivalue, lookup_data, 
                                     mask_obj_num, components, filters, ierr);
  if (mask_obj_num != -1)
    c_private_show_image_from_ref(fmkr, p, ref, llx, lly, 
                                  lrx, lry, ulx, uly, ierr);
  return Integer_New(ref);
}

int
c_private_register_image(OBJ_PTR fmkr, FM *p, int image_type,
                         bool interpolate, bool reversed,
                         int w, int h, unsigned char* data, long len, 
                         OBJ_PTR mask_min, OBJ_PTR mask_max, OBJ_PTR hivalue,
                         OBJ_PTR lookup_data, int mask_obj_num, int components,
                         const char * filters,
                         int *ierr)
{
   unsigned char *lookup = NULL;
   int value_mask_min = 256, value_mask_max = 256, lookup_len = 0, hival = 0;
   if (constructing_path) {
      RAISE_ERROR("Sorry: must finish with current path before calling "
                  "show_image", ierr);
      RETURN_NIL;
   }
   if (image_type == COLORMAP_IMAGE) {
      value_mask_min = Number_to_int(mask_min, ierr);
      value_mask_max = Number_to_int(mask_max, ierr);
      hival = Number_to_int(hivalue, ierr);
      lookup = (unsigned char *)(String_Ptr(lookup_data, ierr));
      lookup_len = String_Len(lookup_data, ierr);
      if (*ierr != 0) RETURN_NIL;
   }
   

   Sampled_Info *xo = (Sampled_Info *)calloc(1, sizeof(Sampled_Info));
   xo->xobj_subtype = SAMPLED_SUBTYPE;
   double a, b, c, d, e, f; // the transform to position the image
   //int ir, ic, id;
   xo->next = xobj_list;
   xobj_list = (XObject_Info *)xo;
   xo->xo_num = next_available_xo_number++;
   xo->obj_num = next_available_object_number++;
   xo->image_data = ALLOC_N_unsigned_char(len);
   xo->length = len;
   xo->interpolate = interpolate;
   xo->reversed = reversed;
   xo->components = components;
   memcpy(xo->image_data, data, len);
   xo->image_type = image_type;
   if(filters) {
     int len = strlen(filters) + 1;
     xo->filters = calloc(1, len);
     memcpy(xo->filters, filters, len);
   }
   else
     xo->filters = NULL;
   if (image_type != COLORMAP_IMAGE) xo->lookup = NULL;
   else {
      if ((hival+1)*3 > lookup_len) {
         RAISE_ERROR_ii("Sorry: color space hival (%i) is too large for "
                        "length of lookup table (%i)", hival, lookup_len,
                        ierr);
         RETURN_NIL;
      }
      xo->hival = hival;
      lookup_len = (hival+1) * 3;
      xo->lookup = ALLOC_N_unsigned_char(lookup_len);
      xo->lookup_len = lookup_len;
      memcpy(xo->lookup, lookup, lookup_len);
   }
   xo->width = w;
   xo->height = h;   
   xo->value_mask_min = value_mask_min;
   xo->value_mask_max = value_mask_max;
   xo->mask_obj_num = mask_obj_num;
   return xo->obj_num;
}

/* Goes through the xobject list and find the one whose object number
   matches the one given, and returns the Xobject number. -1 if not
   found. */

int Find_XObjRef(int ref)
{
  XObject_Info * info = xobj_list;
  while(1) {
    if(info->obj_num == ref)
      return info->xo_num;
    info = info->next;
    if(! info)
      break;
  }
  return -1;
}

void
c_private_show_image_from_ref(OBJ_PTR fmkr, FM *p, int ref, double llx,
                              double lly, double lrx, double lry, double ulx,
                              double uly,
                              int *ierr)
{
   if (constructing_path) {
      RAISE_ERROR("Sorry: must finish with current path before calling "
                  "show_image", ierr);
      return;
   }

   double a, b, c, d, e, f; // the transform to position the image
   int xo_num = Find_XObjRef(ref);
   if(xo_num < 0) {
     RAISE_ERROR_i("Could not find image PDF object %d", ref,
                    ierr);
     return;
   }

   llx = convert_figure_to_output_x(p, llx);
   lly = convert_figure_to_output_y(p, lly);
   lrx = convert_figure_to_output_x(p, lrx);
   lry = convert_figure_to_output_y(p, lry);
   ulx = convert_figure_to_output_x(p, ulx);
   uly = convert_figure_to_output_y(p, uly);

   Create_Transform_from_Points(llx, lly, lrx, lry, ulx, uly,
                                &a, &b, &c, &d, &e, &f);
   fprintf(TF, "q %0.2f %0.2f %0.2f %0.2f %0.2f %0.2f cm /XObj%i Do Q\n",
           a, b, c, d, e, f, xo_num);
   update_bbox(p, llx, lly);
   update_bbox(p, lrx, lry);
   update_bbox(p, ulx, uly);
   update_bbox(p, lrx+ulx-llx, lry+uly-lly);
}
