/* pdfcoords.c */
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


/* Frame and Bounds */
void
Recalc_Font_Hts(FM *p)
{
   double dx, dy; // font height in output coords
   dx = dy = ENLARGE * p->default_font_size * p->default_text_scale;
   dx = convert_output_to_page_dx(p, dx);
   dx = convert_page_to_frame_dx(p, dx);
   p->default_text_height_dx = convert_frame_to_figure_dx(p, dx);
   dy = convert_output_to_page_dy(p, dy);
   dy = convert_page_to_frame_dy(p, dy);
   p->default_text_height_dy = convert_frame_to_figure_dy(p, dy);
}


void
c_set_subframe(OBJ_PTR fmkr, FM *p, 
               double left_margin, double right_margin, double top_margin,
               double bottom_margin, int *ierr)
{
   double x, y, w, h;
   x = p->frame_left += left_margin * p->frame_width;
   p->frame_right -= right_margin * p->frame_width;
   p->frame_top -= top_margin * p->frame_height;
   y = p->frame_bottom += bottom_margin * p->frame_height;
   w = p->frame_width = p->frame_right - p->frame_left;
   h = p->frame_height = p->frame_top - p->frame_bottom;
   Recalc_Font_Hts(p);
}


void
c_private_set_bounds(OBJ_PTR fmkr, FM *p, double left, double right,
                     double top, double bottom, int *ierr)
{
   if (constructing_path) {
      RAISE_ERROR("Sorry: must finish with current path before calling "
                  "set_bounds", ierr);
      return;
   }
   p->bounds_left = left; p->bounds_right = right;
   p->bounds_bottom = bottom; p->bounds_top = top;
   if (left < right) {
      p->xaxis_reversed = false;
      p->bounds_xmin = left; p->bounds_xmax = right;
   }
   else if (right < left) {
      p->xaxis_reversed = true;
      p->bounds_xmin = right; p->bounds_xmax = left;
   }
   else { // left == right
      p->xaxis_reversed = false;
      if (left > 0.0) {
         p->bounds_xmin = left * (1.0 - 1e-6);
         p->bounds_xmax = left * (1.0 + 1e-6);
      }
      else if (left < 0.0) {
        p->bounds_xmin = left * (1.0 + 1e-6);
        p->bounds_xmax = left * (1.0 - 1e-6);
      }
      else {
         p->bounds_xmin = -1e-6; p->bounds_xmax = 1e-6;
      }
   }
   if (bottom < top) {
      p->yaxis_reversed = false;
      p->bounds_ymin = bottom; p->bounds_ymax = top;
   }
   else if (top < bottom) {
      p->yaxis_reversed = true;
      p->bounds_ymin = top; p->bounds_ymax = bottom;
   }
   else { // top == bottom
      p->yaxis_reversed = false;
      if (bottom > 0.0) {
         p->bounds_ymin = bottom * (1.0 - 1e-6);
         p->bounds_ymax = bottom * (1.0 + 1e-6);
      }
      else if (bottom < 0.0) {
         p->bounds_ymin = bottom * (1.0 + 1e-6);
         p->bounds_ymax = bottom * (1.0 - 1e-6);
      }
      else {
         p->bounds_xmin = -1e-6; p->bounds_xmax = 1e-6;
      }
   }
   p->bounds_width = p->bounds_xmax - p->bounds_xmin;
   p->bounds_height = p->bounds_ymax - p->bounds_ymin;
   Recalc_Font_Hts(p);
}


// Conversions

OBJ_PTR
c_convert_to_degrees(OBJ_PTR fmkr, FM *p, double dx, double dy, int *ierr)
{ // dx and dy in figure coords
   double angle;
   if (dx == 0.0 && dy == 0.0) angle = 0.0;
   else if (dx > 0.0 && dy == 0.0)
      angle = (p->bounds_left > p->bounds_right)? 180.0 : 0.0;
   else if (dx < 0.0 && dy == 0.0)
      angle = (p->bounds_left > p->bounds_right)? 0.0 : 180.0;
   else if (dx == 0.0 && dy > 0.0)
      angle = (p->bounds_bottom > p->bounds_top)? -90.0 : 90.0;
   else if (dx == 0.0 && dy < 0.0)
      angle = (p->bounds_bottom > p->bounds_top)? 90.0 : -90.0;
   else
      angle = atan2(convert_figure_to_output_dy(p, dy),
                    convert_figure_to_output_dx(p, dx)) * RADIANS_TO_DEGREES;
   return Float_New(angle);
}


OBJ_PTR
c_convert_inches_to_output(OBJ_PTR fmkr, FM *p, double val, int *ierr)
{
   val = convert_inches_to_output(val);
   return Float_New(val);
}


OBJ_PTR
c_convert_output_to_inches(OBJ_PTR fmkr, FM *p, double val, int *ierr)
{
   val = convert_output_to_inches(val);
   return Float_New(val);
}


OBJ_PTR
c_convert_mm_to_output(OBJ_PTR fmkr, FM *p, double val, int *ierr)
{
   val = convert_mm_to_output(val);
   return Float_New(val);
}


OBJ_PTR
c_convert_output_to_mm(OBJ_PTR fmkr, FM *p, double val, int *ierr)
{
   val = convert_output_to_mm(val);
   return Float_New(val);
}


OBJ_PTR
c_convert_page_to_output_x(OBJ_PTR fmkr, FM *p, double val, int *ierr)
{
   val = convert_page_to_output_x(p, val);
   return Float_New(val);
}


OBJ_PTR
c_convert_page_to_output_y(OBJ_PTR fmkr, FM *p, double val, int *ierr)
{
   val = convert_page_to_output_y(p, val);
   return Float_New(val);
}


OBJ_PTR
c_convert_page_to_output_dx(OBJ_PTR fmkr, FM *p, double val, int *ierr)
{
   val = convert_page_to_output_dx(p, val);
   return Float_New(val);
}


OBJ_PTR
c_convert_page_to_output_dy(OBJ_PTR fmkr, FM *p, double val, int *ierr)
{
   val = convert_page_to_output_dy(p, val);
   return Float_New(val);
}


OBJ_PTR
c_convert_output_to_page_x(OBJ_PTR fmkr, FM *p, double val, int *ierr)
{
   val = convert_output_to_page_x(p, val);
   return Float_New(val);
}


OBJ_PTR
c_convert_output_to_page_y(OBJ_PTR fmkr, FM *p, double val, int *ierr)
{
   val = convert_output_to_page_y(p, val);
   return Float_New(val);
}


OBJ_PTR
c_convert_output_to_page_dx(OBJ_PTR fmkr, FM *p, double val, int *ierr)
{
   val = convert_output_to_page_dx(p, val);
   return Float_New(val);
}


OBJ_PTR
c_convert_output_to_page_dy(OBJ_PTR fmkr, FM *p, double val, int *ierr)
{
   val = convert_output_to_page_dy(p, val);
   return Float_New(val);
}


OBJ_PTR
c_convert_frame_to_page_x(OBJ_PTR fmkr, FM *p, double val, int *ierr)
{
   val = convert_frame_to_page_x(p, val);
   return Float_New(val);
}


OBJ_PTR
c_convert_frame_to_page_y(OBJ_PTR fmkr, FM *p, double val, int *ierr)
{
   val = convert_frame_to_page_y(p, val);
   return Float_New(val);
}


OBJ_PTR
c_convert_frame_to_page_dx(OBJ_PTR fmkr, FM *p, double val, int *ierr)
{
   val = convert_frame_to_page_dx(p, val);
   return Float_New(val);
}


OBJ_PTR
c_convert_frame_to_page_dy(OBJ_PTR fmkr, FM *p, double val, int *ierr)
{
   val = convert_frame_to_page_dy(p, val);
   return Float_New(val);
}


OBJ_PTR
c_convert_page_to_frame_x(OBJ_PTR fmkr, FM *p, double val, int *ierr)
{
   val = convert_page_to_frame_x(p, val);
   return Float_New(val);
}


OBJ_PTR
c_convert_page_to_frame_y(OBJ_PTR fmkr, FM *p, double val, int *ierr)
{
   val = convert_page_to_frame_y(p, val);
   return Float_New(val);
}


OBJ_PTR
c_convert_page_to_frame_dx(OBJ_PTR fmkr, FM *p, double val, int *ierr)
{
   val = convert_page_to_frame_dx(p, val);
   return Float_New(val);
}


OBJ_PTR
c_convert_page_to_frame_dy(OBJ_PTR fmkr, FM *p, double val, int *ierr)
{
   val = convert_page_to_frame_dy(p, val);
   return Float_New(val);
}

OBJ_PTR
c_convert_figure_to_frame_x(OBJ_PTR fmkr, FM *p, double val, int *ierr)
{
   val = convert_figure_to_frame_x(p, val);
   return Float_New(val);
}


OBJ_PTR
c_convert_figure_to_frame_y(OBJ_PTR fmkr, FM *p, double val, int *ierr)
{
   val = convert_figure_to_frame_y(p, val);
   return Float_New(val);
}


OBJ_PTR
c_convert_figure_to_frame_dx(OBJ_PTR fmkr, FM *p, double val, int *ierr)
{
   val = convert_figure_to_frame_dx(p, val);
   return Float_New(val);
}


OBJ_PTR
c_convert_figure_to_frame_dy(OBJ_PTR fmkr, FM *p, double val, int *ierr)
{
   val = convert_figure_to_frame_dy(p, val);
   return Float_New(val);
}


OBJ_PTR
c_convert_frame_to_figure_x(OBJ_PTR fmkr, FM *p, double val, int *ierr)
{
   val = convert_frame_to_figure_x(p, val);
   return Float_New(val);
}


OBJ_PTR
c_convert_frame_to_figure_y(OBJ_PTR fmkr, FM *p, double val, int *ierr)
{
   val = convert_frame_to_figure_y(p, val);
   return Float_New(val);
}


OBJ_PTR
c_convert_frame_to_figure_dx(OBJ_PTR fmkr, FM *p, double val, int *ierr)
{
   val = convert_frame_to_figure_dx(p, val);
   return Float_New(val);
}


OBJ_PTR
c_convert_frame_to_figure_dy(OBJ_PTR fmkr, FM *p, double val, int *ierr)
{
   val = convert_frame_to_figure_dy(p, val);
   return Float_New(val);
}


double
convert_figure_to_output_x(FM *p, double x)
{
   x = convert_figure_to_frame_x(p, x);
   x = convert_frame_to_page_x(p, x);
   x = convert_page_to_output_x(p, x);
   return x;
}


double
convert_figure_to_output_dy(FM *p, double dy)
{
   dy = convert_figure_to_frame_dy(p, dy);
   dy = convert_frame_to_page_dy(p, dy);
   dy = convert_page_to_output_dy(p, dy);
   return dy;
}


double
convert_figure_to_output_dx(FM *p, double dx)
{
   dx = convert_figure_to_frame_dx(p, dx);
   dx = convert_frame_to_page_dx(p, dx);
   dx = convert_page_to_output_dx(p, dx);
   return dx;
}


double
convert_figure_to_output_y(FM *p, double y)
{
   y = convert_figure_to_frame_y(p, y);
   y = convert_frame_to_page_y(p, y);
   y = convert_page_to_output_y(p, y);
   return y;
}


OBJ_PTR
c_convert_figure_to_output_x(OBJ_PTR fmkr, FM *p, double val, int *ierr)
{
   return Float_New(convert_figure_to_output_x(p, val));
}


OBJ_PTR
c_convert_figure_to_output_y(OBJ_PTR fmkr, FM *p, double val, int *ierr)
{
   return Float_New(convert_figure_to_output_y(p, val));
}


OBJ_PTR
c_convert_figure_to_output_dx(OBJ_PTR fmkr, FM *p, double val, int *ierr)
{
   return Float_New(convert_figure_to_output_dx(p, val));
}


OBJ_PTR
c_convert_figure_to_output_dy(OBJ_PTR fmkr, FM *p, double val, int *ierr)
{
   return Float_New(convert_figure_to_output_dy(p, val));
}


double
convert_output_to_figure_x(FM *p, double x)
{
   x = convert_output_to_page_x(p, x);
   x = convert_page_to_frame_x(p, x);
   x = convert_frame_to_figure_x(p, x);
   return x;
}


double
convert_output_to_figure_dy(FM *p, double dy)
{
   dy = convert_output_to_page_dy(p, dy);
   dy = convert_page_to_frame_dy(p, dy);
   dy = convert_frame_to_figure_dy(p, dy);
   return dy;
}


double
convert_output_to_figure_dx(FM *p, double dx)
{
   dx = convert_output_to_page_dx(p, dx);
   dx = convert_page_to_frame_dx(p, dx);
   dx = convert_frame_to_figure_dx(p, dx);
   return dx;
}


double
convert_output_to_figure_y(FM *p, double y)
{
   y = convert_output_to_page_y(p, y);
   y = convert_page_to_frame_y(p, y);
   y = convert_frame_to_figure_y(p, y);
   return y;
}


OBJ_PTR
c_convert_output_to_figure_x(OBJ_PTR fmkr, FM *p, double val, int *ierr)
{
   return Float_New(convert_output_to_figure_x(p, val));
}


OBJ_PTR
c_convert_output_to_figure_y(OBJ_PTR fmkr, FM *p, double val, int *ierr)
{
   return Float_New(convert_output_to_figure_y(p, val));
}


OBJ_PTR
c_convert_output_to_figure_dx(OBJ_PTR fmkr, FM *p, double val, int *ierr)
{
   return Float_New(convert_output_to_figure_dx(p, val));
}


OBJ_PTR
c_convert_output_to_figure_dy(OBJ_PTR fmkr, FM *p, double val, int *ierr)
{
   return Float_New(convert_output_to_figure_dy(p, val));
}


void
c_private_set_default_font_size(OBJ_PTR fmkr, FM *p, double size, int *ierr)
{
   p->default_font_size = size;
   Recalc_Font_Hts(p);
}


void
c_doing_subplot(OBJ_PTR fmkr, FM *p, int *ierr)
{
   p->in_subplot = true;
}


void
c_doing_subfigure(OBJ_PTR fmkr, FM *p, int *ierr)
{
   p->root_figure = false;
}
