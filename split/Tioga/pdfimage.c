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
   

void Free_JPG(JPG_Info *xo)
{
   if (xo->filename != NULL) free(xo->filename);
}

void Free_Sampled(Sampled_Info *xo)
{
   if (xo->image_data != NULL) free(xo->image_data);
   if (xo->lookup != NULL) free(xo->lookup);
}

static bool Is_monochrome(int obj_num)
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

static void Write_Image_From_File(char *filename, int width, int height, char *out_info, int mask_obj_num)
{
   FILE *jpg = fopen(filename, "r");
   if (jpg == NULL) RAISE_ERROR_s("Sorry: cannot open file for showing image (%s)\n", filename);
   unsigned char *buff;
   int len, rd_len;
   int buff_len = 256000;
   buff = ALLOC_N(unsigned char, buff_len);
   len = 0;
   while ((rd_len = fread(buff, 1, buff_len, jpg)) == buff_len) len += buff_len;
   len += rd_len;
   fprintf(OF, "\t/Subtype /Image\n");
   if (mask_obj_num > 0) {
      if (!Is_monochrome(mask_obj_num)) fprintf(OF, "\t/SMask %i 0 R\n", mask_obj_num);
      else fprintf(OF, "\t/Mask %i 0 R\n", mask_obj_num);
   }
   fprintf(OF, "\t/Width %i\n", width);
   fprintf(OF, "\t/Height %i\n", height);
   fprintf(OF, "%s", out_info);
   fprintf(OF, "\t/Length %i\n\t>>\nstream\n", len);
   if (len < buff_len) fwrite(buff, 1, len, OF);
   else {
      rewind(jpg);
      while ((rd_len = fread(buff, 1, buff_len, jpg)) == buff_len) fwrite(buff, 1, buff_len, OF);
      fwrite(buff, 1, rd_len, OF);
   }
   fprintf(OF, "\nendstream\n");
   fclose(jpg);
}

void Write_JPG(JPG_Info *xo)
{
   Write_Image_From_File(xo->filename, xo->width, xo->height, 
      "\t/Filter /DCTDecode\n\t/ColorSpace /DeviceRGB\n\t/BitsPerComponent 8\n", xo->mask_obj_num);
}

void Write_Sampled(Sampled_Info *xo)
{
   fprintf(OF, "\n\t/Subtype /Image\n");
   fprintf(OF, "\t/Filter /FlateDecode\n\t/Interpolate %s\n", (xo->interpolate)? "true":"false");
   fprintf(OF, "\t/Height %i\n", xo->height);
   fprintf(OF, "\t/Width %i\n", xo->width);
   int i, len;
   unsigned long new_len;
   unsigned char *buffer;
   switch (xo->image_type) {
      case RGB_IMAGE:
         fprintf(OF, "\t/ColorSpace /DeviceRGB\n");
         fprintf(OF, "\t/BitsPerComponent 8\n");
         break;
      case CMYK_IMAGE:
         fprintf(OF, "\t/ColorSpace /DeviceCMYK\n");
         fprintf(OF, "\t/BitsPerComponent 8\n");
         break;
      case GRAY_IMAGE:
         fprintf(OF, "\t/ColorSpace /DeviceGray\n");
         fprintf(OF, "\t/BitsPerComponent 8\n");
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
         fprintf(OF, "\t/BitsPerComponent 8\n");
   }
   if (xo->mask_obj_num > 0) {
      if (xo->image_type == MONO_IMAGE)
         RAISE_ERROR("Sorry: monochrome images must not have masks");
      if (!Is_monochrome(xo->mask_obj_num)) fprintf(OF, "\t/SMask %i 0 R\n", xo->mask_obj_num);
      else fprintf(OF, "\t/Mask %i 0 R\n", xo->mask_obj_num);
   }
   if (xo->value_mask_min >= 0 && xo->value_mask_max >= 0 &&
      xo->value_mask_min <= 255 && xo->value_mask_max <= 255 && xo->value_mask_min <= xo->value_mask_max)
      fprintf(OF, "\t/Mask [%i %i]\n", xo->value_mask_min, xo->value_mask_max);
   new_len = (xo->length * 11)/10 + 100;
   buffer = ALLOC_N(unsigned char, new_len);
   if (flate_compress(buffer, &new_len, xo->image_data, xo->length) != FLATE_OK) {
      free(buffer);
      RAISE_ERROR("Error compressing image data");
   }
   fprintf(OF, "\t/Length %li\n", new_len);
   fprintf(OF, "\t>>\nstream\n");
   if (fwrite(buffer, 1, new_len, OF) < new_len)
      RAISE_ERROR("Error writing image data");
   free(buffer);
   fprintf(OF, "\nendstream\nendobj\n");
}

void Create_Transform_from_Points( // transform maps (0,0), (1,0), and (0,1) to the given points
   double llx, double lly, double lrx, double lry, double ulx, double uly,
   double *a, double *b, double *c, double *d, double *e, double *f)
{
   *e = llx; *f = lly; lrx -= llx; ulx -= llx; lry -= lly; uly -= lly;
   *a = lrx; *b = lry; *c = ulx; *d = uly;
}

void Get_Image_Dest(FM *p, OBJ_PTR image_destination, double *dest)
{
   if (Array_Len(image_destination) != 6)
      RAISE_ERROR("Sorry: invalid image destination array: must have 6 entries");
   int i;
   for (i = 0; i < 6; i++) {
      OBJ_PTR entry = Array_Entry(image_destination, i);
      if (i % 2 == 0)
         dest[i] = convert_figure_to_output_x(p,Number_to_double(entry));
      else
         dest[i] = convert_figure_to_output_y(p,Number_to_double(entry));
   }
}

static void Show_JPEG(FM *p, char *filename, int width, int height, double *dest, int subtype, int mask_obj_num)
{
   JPG_Info *xo = ALLOC(JPG_Info);
   xo->xobj_subtype = subtype;
   double llx = dest[0], lly = dest[1], lrx = dest[2], lry = dest[3], ulx = dest[4],  uly = dest[5];
   double a, b, c, d, e, f; // the transform to position the image
   xo->next = xobj_list;
   xobj_list = (XObject_Info *)xo;
   xo->xo_num = next_available_xo_number++;
   xo->obj_num = next_available_object_number++;
   xo->filename = ALLOC_N(char, strlen(filename)+1);
   strcpy(xo->filename, filename);
   xo->width = width;
   xo->height = height;
   xo->mask_obj_num = mask_obj_num;
   Create_Transform_from_Points(llx, lly, lrx, lry, ulx, uly, &a, &b, &c, &d, &e, &f);
   fprintf(TF, "q %0.2f %0.2f %0.2f %0.2f %0.2f %0.2f cm /XObj%i Do Q\n", a, b, c, d, e, f, xo->xo_num);
   update_bbox(p, llx, lly);
   update_bbox(p, lrx, lry);
   update_bbox(p, ulx, uly);
   update_bbox(p, lrx+ulx-llx, lry+uly-lly);
}

void c_show_jpg(FM *p, char *filename, int width, int height, double *dest, int mask_obj_num)
{
   Show_JPEG(p, filename, width, height, dest, JPG_SUBTYPE, mask_obj_num);
}

OBJ_PTR FM_private_show_jpg(OBJ_PTR fmkr, OBJ_PTR filename, OBJ_PTR width, OBJ_PTR height, OBJ_PTR image_destination, OBJ_PTR mask_obj_num)
{
   double dest[6];
   FM *p = Get_FM(fmkr);
   if (constructing_path) RAISE_ERROR("Sorry: must finish with current path before calling show_jpg");
   Get_Image_Dest(p, image_destination, dest);
   c_show_jpg(p, String_Ptr(filename), Number_to_int(width), Number_to_int(height), dest, Number_to_int(mask_obj_num));
   return fmkr;
}

OBJ_PTR c_private_create_image_data(FM *p, double **data, long num_cols, long num_rows,
            int first_row, int last_row, int first_column, int last_column,
            double min_val, double max_val, int max_code, int if_below_range, int if_above_range)
{
   if (first_column < 0) first_column += num_cols;
   if (first_column < 0 || first_column >= num_cols)
      RAISE_ERROR_i("Sorry: invalid first_column specification (%i)", first_column);
   if (last_column < 0) last_column += num_cols;
   if (last_column < 0 || last_column >= num_cols)
      RAISE_ERROR_i("Sorry: invalid last_column specification (%i)", last_column);
   if (first_row < 0) first_row += num_rows;
   if (first_row < 0 || first_row >= num_rows)
      RAISE_ERROR_i("Sorry: invalid first_row specification (%i)", first_row);
   if (last_row < 0) last_row += num_rows;
   if (last_row < 0 || last_row >= num_rows)
      RAISE_ERROR_i("Sorry: invalid last_row specification (%i)", last_row);
   if (min_val >= max_val)
      RAISE_ERROR_gg("Sorry: invalid range specification: min %g max %g", min_val, max_val);
   if (max_code <= 0 || max_code > 255)
      RAISE_ERROR_i("Sorry: invalid max_code specification (%i)", max_code);
   if (if_below_range < 0 || if_below_range > 255)
      RAISE_ERROR_i("Sorry: invalid if_below_range specification (%i)", if_below_range);
   if (if_above_range < 0 || if_above_range > 255)
      RAISE_ERROR_i("Sorry: invalid if_above_range specification (%i)", if_above_range);
   int i, j, k, width = last_column - first_column + 1, height = last_row - first_row + 1;
   int sz = width * height;
   if (sz <= 0) RAISE_ERROR_ii("Sorry: invalid data specification: width (%i) height (%i)", width, height);
   char *buff = ALLOC_N(char, sz);
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

OBJ_PTR FM_private_create_image_data(OBJ_PTR fmkr, OBJ_PTR data,
            OBJ_PTR first_row, OBJ_PTR last_row, OBJ_PTR first_column, OBJ_PTR last_column,
            OBJ_PTR min_val, OBJ_PTR max_val, OBJ_PTR max_code, OBJ_PTR if_below_range, OBJ_PTR if_above_range)
{
   FM *p = Get_FM(fmkr);
   long num_cols, num_rows;
   double **ary = Table_Info(data, &num_cols, &num_rows);
   return c_private_create_image_data(p, ary, num_cols, num_rows, 
      Number_to_int(first_row), Number_to_int(last_row), Number_to_int(first_column), Number_to_int(last_column),
      Number_to_double(min_val), Number_to_double(max_val), Number_to_int(max_code), Number_to_int(if_below_range), Number_to_int(if_above_range));
}

static OBJ_PTR c_private_create_monochrome_image_data(FM *p, double **data, long num_cols, long num_rows,
            int first_row, int last_row, int first_column, int last_column,
            double boundary, bool reversed)
{
   if (first_column < 0) first_column += num_cols;
   if (first_column < 0 || first_column >= num_cols)
      RAISE_ERROR_i("Sorry: invalid first_column specification (%i)", first_column);
   if (last_column < 0) last_column += num_cols;
   if (last_column < 0 || last_column >= num_cols)
      RAISE_ERROR_i("Sorry: invalid last_column specification (%i)", last_column);
   if (first_row < 0) first_row += num_rows;
   if (first_row < 0 || first_row >= num_rows)
      RAISE_ERROR_i("Sorry: invalid first_row specification (%i)", first_row);
   if (last_row < 0) last_row += num_rows;
   if (last_row < 0 || last_row >= num_rows)
      RAISE_ERROR_i("Sorry: invalid last_row specification (%i)", last_row);
   int i, j, k, width = last_column - first_column + 1, height = last_row - first_row + 1, bytes_per_row = (width+7)/8;
   int sz = bytes_per_row * 8 * height;
   if (sz <= 0) RAISE_ERROR_ii("Sorry: invalid data specification: width (%i) height (%i)", width, height);
   // to simplify the process, do it in two stages: first get the values and then pack the bits
   char *buff = ALLOC_N(char, sz);
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
   char *bits = ALLOC_N(char, num_bytes), c = 0;
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

OBJ_PTR FM_private_create_monochrome_image_data(OBJ_PTR fmkr, OBJ_PTR data,
            OBJ_PTR first_row, OBJ_PTR last_row, OBJ_PTR first_column, OBJ_PTR last_column,
            OBJ_PTR boundary, OBJ_PTR reverse)
{
   FM *p = Get_FM(fmkr);
   long num_cols, num_rows;
   double **ary = Table_Info(data, &num_cols, &num_rows);
   return c_private_create_monochrome_image_data(p, ary, num_cols, num_rows, 
      Number_to_int(first_row), Number_to_int(last_row), Number_to_int(first_column), Number_to_int(last_column),
      Number_to_double(boundary), reverse != Qfalse);
}

int c_private_show_image(FM *p, int image_type, double *dest, bool interpolate, bool reversed, int w, int h, unsigned char* data, int len, 
   int value_mask_min, int value_mask_max, int hival, unsigned char* lookup, int lookup_len, int mask_obj_num)
{
   Sampled_Info *xo = ALLOC(Sampled_Info);
   xo->xobj_subtype = SAMPLED_SUBTYPE;
   double llx = dest[0], lly = dest[1], lrx = dest[2], lry = dest[3], ulx = dest[4],  uly = dest[5];
   double a, b, c, d, e, f; // the transform to position the image
   int ir, ic, id;
   xo->next = xobj_list;
   xobj_list = (XObject_Info *)xo;
   xo->xo_num = next_available_xo_number++;
   xo->obj_num = next_available_object_number++;
   xo->image_data = ALLOC_N(unsigned char, len);
   xo->length = len;
   xo->interpolate = interpolate;
   xo->reversed = reversed;
   MEMCPY(xo->image_data, data, unsigned char, len);
   xo->image_type = image_type;
   if (image_type != COLORMAP_IMAGE) xo->lookup = NULL;
   else {
      if ((hival+1)*3 > lookup_len)
         RAISE_ERROR_ii("Sorry: color space hival (%i) is too large for length of lookup table (%i)", hival, lookup_len);
      xo->hival = hival;
      lookup_len = (hival+1) * 3;
      xo->lookup = ALLOC_N(unsigned char, lookup_len);
      xo->lookup_len = lookup_len;
      MEMCPY(xo->lookup, lookup, unsigned char, lookup_len);
   }
   xo->width = w;
   xo->height = h;
   
   if (0) {
    printf("len=%i  w=%i  h=%i\ndata\n\n", len, w, h);
   for (ir=0; ir<h; ir++) {
    for (ic=0; ic<w; ic++) {
        id = (int)data[ir*h+ic];
        printf("%3i ",id);
    }
    printf("\n");
   }
   
   
    printf("\n\nxo->image_data\n");
   for (ir=0; ir<h; ir++) {
    for (ic=0; ic<w; ic++) {
        id = (int)xo->image_data[ir*h+ic];
        printf("%3i ",id);
    }
    printf("\n\n");
   }
   }
   
   
   xo->value_mask_min = value_mask_min;
   xo->value_mask_max = value_mask_max;
   xo->mask_obj_num = mask_obj_num;
   if (mask_obj_num == -1) return xo->obj_num; // this image is being used as an opacity mask
   Create_Transform_from_Points(llx, lly, lrx, lry, ulx, uly, &a, &b, &c, &d, &e, &f);
   fprintf(TF, "q %0.2f %0.2f %0.2f %0.2f %0.2f %0.2f cm /XObj%i Do Q\n", a, b, c, d, e, f, xo->xo_num);
   update_bbox(p, llx, lly);
   update_bbox(p, lrx, lry);
   update_bbox(p, ulx, uly);
   update_bbox(p, lrx+ulx-llx, lry+uly-lly);
   return xo->obj_num;
}

static OBJ_PTR private_show_image(int image_type, OBJ_PTR fmkr, OBJ_PTR llx, OBJ_PTR lly, OBJ_PTR lrx, OBJ_PTR lry,
    OBJ_PTR ulx, OBJ_PTR uly, OBJ_PTR interpolate, OBJ_PTR reversed, OBJ_PTR w, OBJ_PTR h, OBJ_PTR data, OBJ_PTR value_mask_min, OBJ_PTR value_mask_max,
    OBJ_PTR hival, OBJ_PTR lookup, OBJ_PTR mask_obj_num)
{
   double dest[6];
   unsigned char *lookup_str=NULL;
   int mask_min = 256, mask_max = 256, lookup_len=0, high_val=0;
   FM *p = Get_FM(fmkr);
   if (constructing_path) RAISE_ERROR("Sorry: must finish with current path before calling show_image");
   if (image_type == COLORMAP_IMAGE) {
      mask_min = Number_to_int(value_mask_min);
      mask_max = Number_to_int(value_mask_max);
      high_val = Number_to_int(hival);
      lookup_str = (unsigned char *)(String_Ptr(lookup));
      lookup_len = String_Len(lookup);
   }
   dest[0] = convert_figure_to_output_x(p,Number_to_double(llx));
   dest[1] = convert_figure_to_output_y(p,Number_to_double(lly));
   dest[2] = convert_figure_to_output_x(p,Number_to_double(lrx));
   dest[3] = convert_figure_to_output_y(p,Number_to_double(lry));
   dest[4] = convert_figure_to_output_x(p,Number_to_double(ulx));
   dest[5] = convert_figure_to_output_y(p,Number_to_double(uly));
   int obj_num = c_private_show_image(p, image_type, dest, (interpolate != Qfalse), (reversed == Qtrue), Number_to_int(w), Number_to_int(h), 
      (unsigned char *)String_Ptr(data), String_Len(data), mask_min, mask_max, high_val, lookup_str, lookup_len, Number_to_int(mask_obj_num));
   return Integer_New(obj_num);
}

OBJ_PTR FM_private_show_rgb_image(OBJ_PTR fmkr, OBJ_PTR llx, OBJ_PTR lly, OBJ_PTR lrx, OBJ_PTR lry,
    OBJ_PTR ulx, OBJ_PTR uly, OBJ_PTR interpolate, OBJ_PTR w, OBJ_PTR h, OBJ_PTR data, OBJ_PTR mask_obj_num)
{
   return private_show_image(RGB_IMAGE, fmkr, llx, lly, lrx, lry, ulx, uly, interpolate, Qfalse, w, h, data, Qnil, Qnil, Qnil, Qnil, mask_obj_num);
}

OBJ_PTR FM_private_show_cmyk_image(OBJ_PTR fmkr, OBJ_PTR llx, OBJ_PTR lly, OBJ_PTR lrx, OBJ_PTR lry,
    OBJ_PTR ulx, OBJ_PTR uly, OBJ_PTR interpolate, OBJ_PTR w, OBJ_PTR h, OBJ_PTR data, OBJ_PTR mask_obj_num)
{
   return private_show_image(CMYK_IMAGE, fmkr, llx, lly, lrx, lry, ulx, uly, interpolate, Qfalse, w, h, data, Qnil, Qnil, Qnil, Qnil, mask_obj_num);
}

OBJ_PTR FM_private_show_grayscale_image(OBJ_PTR fmkr, OBJ_PTR llx, OBJ_PTR lly, OBJ_PTR lrx, OBJ_PTR lry,
    OBJ_PTR ulx, OBJ_PTR uly, OBJ_PTR interpolate, OBJ_PTR w, OBJ_PTR h, OBJ_PTR data, OBJ_PTR mask_obj_num)
{
   return private_show_image(GRAY_IMAGE, fmkr, llx, lly, lrx, lry, ulx, uly, interpolate, Qfalse, w, h, data, Qnil, Qnil, Qnil, Qnil, mask_obj_num);
}

OBJ_PTR FM_private_show_monochrome_image(OBJ_PTR fmkr, OBJ_PTR llx, OBJ_PTR lly, OBJ_PTR lrx, OBJ_PTR lry,
    OBJ_PTR ulx, OBJ_PTR uly, OBJ_PTR interpolate, OBJ_PTR reversed, OBJ_PTR w, OBJ_PTR h, OBJ_PTR data, OBJ_PTR mask_obj_num)
{
   return private_show_image(MONO_IMAGE, fmkr, llx, lly, lrx, lry, ulx, uly, interpolate, reversed, 
         w, h, data, Qnil, Qnil, Qnil, Qnil, mask_obj_num);
}

OBJ_PTR FM_private_show_image(OBJ_PTR fmkr, OBJ_PTR llx, OBJ_PTR lly, OBJ_PTR lrx, OBJ_PTR lry,
    OBJ_PTR ulx, OBJ_PTR uly, OBJ_PTR interpolate, OBJ_PTR w, OBJ_PTR h, OBJ_PTR data,
    OBJ_PTR value_mask_min, OBJ_PTR value_mask_max, OBJ_PTR hival, OBJ_PTR lookup, OBJ_PTR mask_obj_num)
{
   return private_show_image(COLORMAP_IMAGE, fmkr, llx, lly, lrx, lry, ulx, uly, interpolate, Qfalse, w, h, data, 
      value_mask_min, value_mask_max, hival, lookup, mask_obj_num);
}

