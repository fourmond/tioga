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

void Recalc_Font_Hts(FM *p)
{
   double dx, dy;
   dx = dy = ENLARGE * DEFAULT_FONT_HT * p->default_text_scale;  // font height in output coords
   dx = convert_output_to_page_dx(p,dx);
   dx = convert_page_to_frame_dx(p,dx);
   p->default_text_height_dx = convert_frame_to_figure_dx(p,dx);
   dy = convert_output_to_page_dy(p,dy);
   dy = convert_page_to_frame_dy(p,dy);
   p->default_text_height_dy = convert_frame_to_figure_dy(p,dy);
}

void c_set_subframe(FM *p, double left_margin, double right_margin, double top_margin, double bottom_margin)
{
   double x, y, w, h;
   if (left_margin < 0 || right_margin < 0 || top_margin < 0 || bottom_margin < 0)
      rb_raise(rb_eArgError, "Sorry: margins for set_subframe must be non-negative");
   if (left_margin + right_margin >= 1.0)
      rb_raise(rb_eArgError, "Sorry: margins too large: left_margin (%g) right_margin (%g)", left_margin, right_margin);
   if (top_margin + bottom_margin >= 1.0)
      rb_raise(rb_eArgError, "Sorry: margins too large: top_margin (%g) bottom_margin (%g)", top_margin, bottom_margin);
   x = p->frame_left += left_margin * p->frame_width;
   p->frame_right -= right_margin * p->frame_width;
   p->frame_top -= top_margin * p->frame_height;
   y = p->frame_bottom += bottom_margin * p->frame_height;
   w = p->frame_width = p->frame_right - p->frame_left;
   h = p->frame_height = p->frame_top - p->frame_bottom;
   Recalc_Font_Hts(p);
}

VALUE FM_private_set_subframe(VALUE fmkr, VALUE left_margin, VALUE right_margin, VALUE top_margin, VALUE bottom_margin)
{
   FM *p = Get_FM(fmkr);
   left_margin = rb_Float(left_margin);
   right_margin = rb_Float(right_margin);
   top_margin = rb_Float(top_margin);
   bottom_margin = rb_Float(bottom_margin);
   c_set_subframe(p, NUM2DBL(left_margin), NUM2DBL(right_margin), NUM2DBL(top_margin), NUM2DBL(bottom_margin));
   return fmkr;
}

// We need to do some extra work to use 'ensure' around 'context'
// which is necessary in case code does 'return' from the block being executed in the context
// otherwise we won't get a chance to restore the state
typedef struct {
   VALUE fmkr;
   FM *p;
   FM saved;
   VALUE cmd;
} Context_Info;

static VALUE do_context_body(VALUE args)
{
   Context_Info *cp = (Context_Info *)args; // nasty but it works
   fprintf(TF, "q\n");
   return do_cmd(cp->fmkr, cp->cmd);
}

static VALUE do_context_ensure(VALUE args)
{
   Context_Info *cp = (Context_Info *)args;
   fprintf(TF, "Q\n");
   *cp->p = cp->saved;
   return Qnil; // what should we be returning here?
}

VALUE FM_private_context(VALUE fmkr, VALUE cmd)
{
   Context_Info c;
   c.fmkr = fmkr;
   c.cmd = cmd;
   c.p = Get_FM(fmkr);
   c.saved = *c.p;
   return rb_ensure(do_context_body, (VALUE)&c, do_context_ensure, (VALUE)&c);
}

VALUE FM_doing_subplot(VALUE fmkr)
{
   FM *p = Get_FM(fmkr);
   p->in_subplot = true;
   return fmkr;
}

VALUE FM_doing_subfigure(VALUE fmkr)
{
   FM *p = Get_FM(fmkr);
   p->root_figure = false;
   return fmkr;
}

void c_set_bounds(FM *p, double left, double right, double top, double bottom)
{
   if (constructing_path) rb_raise(rb_eArgError, "Sorry: must finish with current path before calling set_bounds");
   p->bounds_left = left; p->bounds_right = right;
   p->bounds_bottom = bottom; p->bounds_top = top;
   if (left < right) {
      p->xaxis_reversed = false;
      p->bounds_xmin = left; p->bounds_xmax = right;
   } else if (right < left) {
      p->xaxis_reversed = true;
      p->bounds_xmin = right; p->bounds_xmax = left;
   } else rb_raise(rb_eArgError, "Sorry: left and right bounds cannot be the same (%g)", left);
   if (bottom < top) {
      p->yaxis_reversed = false;
      p->bounds_ymin = bottom; p->bounds_ymax = top;
   } else if (top < bottom) {
      p->yaxis_reversed = true;
      p->bounds_ymin = top; p->bounds_ymax = bottom;
   } else rb_raise(rb_eArgError, "Sorry: top and bottom bounds cannot be the same (%g)", top);
   p->bounds_width = p->bounds_xmax - p->bounds_xmin;
   p->bounds_height = p->bounds_ymax - p->bounds_ymin;
   Recalc_Font_Hts(p);
}

VALUE FM_private_set_bounds(VALUE fmkr, VALUE left, VALUE right, VALUE top, VALUE bottom)
{
   FM *p = Get_FM(fmkr);
   double left_boundary, right_boundary, top_boundary, bottom_boundary;
   left = rb_Float(left);
   left_boundary = NUM2DBL(left);
   right = rb_Float(right);
   right_boundary = NUM2DBL(right);
   top = rb_Float(top);
   top_boundary = NUM2DBL(top);
   bottom = rb_Float(bottom);
   bottom_boundary = NUM2DBL(bottom);
   c_set_bounds(p, left_boundary, right_boundary, top_boundary, bottom_boundary);
   return fmkr;
}


// Conversions

double c_convert_to_degrees(FM *p, double dx, double dy)  // dx and dy in figure coords
{
   double angle;
   if (dx == 0.0 && dy == 0.0) angle = 0.0;
   else if (dx > 0.0 && dy == 0.0) angle = (p->bounds_left > p->bounds_right)? 180.0 : 0.0;
   else if (dx < 0.0 && dy == 0.0) angle = (p->bounds_left > p->bounds_right)? 0.0 : 180.0;
   else if (dx == 0.0 && dy > 0.0) angle = (p->bounds_bottom > p->bounds_top)? -90.0 : 90.0;
   else if (dx == 0.0 && dy < 0.0) angle = (p->bounds_bottom > p->bounds_top)? 90.0 : -90.0;
   else angle = atan2(convert_figure_to_output_dy(p,dy),convert_figure_to_output_dx(p,dx))*RADIANS_TO_DEGREES;
   return angle;
}

VALUE FM_convert_to_degrees(VALUE fmkr, VALUE dx, VALUE dy) // dx and dy in figure coords
{
   FM *p = Get_FM(fmkr);
   dx = rb_Float(dx);
   dy = rb_Float(dy);
   return rb_float_new(c_convert_to_degrees(p, NUM2DBL(dx), NUM2DBL(dy)));
}

VALUE FM_convert_page_to_output_x(VALUE fmkr, VALUE v)
{
   FM *p = Get_FM(fmkr); v = rb_Float(v); double val = NUM2DBL(v);
   val = convert_page_to_output_x(p,val);
   return rb_float_new(val);
}

VALUE FM_convert_page_to_output_y(VALUE fmkr, VALUE v)
{
   FM *p = Get_FM(fmkr); v = rb_Float(v); double val = NUM2DBL(v);
   val = convert_page_to_output_y(p,val);
   return rb_float_new(val);
}

VALUE FM_convert_page_to_output_dx(VALUE fmkr, VALUE v)
{
   FM *p = Get_FM(fmkr); v = rb_Float(v); double val = NUM2DBL(v);
   val = convert_page_to_output_dx(p,val);
   return rb_float_new(val);
}

VALUE FM_convert_page_to_output_dy(VALUE fmkr, VALUE v)
{
   FM *p = Get_FM(fmkr); v = rb_Float(v); double val = NUM2DBL(v);
   val = convert_page_to_output_dy(p,val);
   return rb_float_new(val);
}

VALUE FM_convert_output_to_page_x(VALUE fmkr, VALUE v)
{
   FM *p = Get_FM(fmkr); v = rb_Float(v); double val = NUM2DBL(v);
   val = convert_output_to_page_x(p,val);
   return rb_float_new(val);
}

VALUE FM_convert_output_to_page_y(VALUE fmkr, VALUE v)
{
   FM *p = Get_FM(fmkr); v = rb_Float(v); double val = NUM2DBL(v);
   val = convert_output_to_page_y(p,val);
   return rb_float_new(val);
}

VALUE FM_convert_output_to_page_dx(VALUE fmkr, VALUE v)
{
   FM *p = Get_FM(fmkr); v = rb_Float(v); double val = NUM2DBL(v);
   return convert_output_to_page_dx(p,val);
   return rb_float_new(val);
}

VALUE FM_convert_output_to_page_dy(VALUE fmkr, VALUE v)
{
   FM *p = Get_FM(fmkr); v = rb_Float(v); double val = NUM2DBL(v);
   val = convert_output_to_page_dy(p,val);
   return rb_float_new(val);
}

VALUE FM_convert_frame_to_page_x(VALUE fmkr, VALUE v)
{
   FM *p = Get_FM(fmkr); v = rb_Float(v); double val = NUM2DBL(v);
   val = convert_frame_to_page_x(p,val);
   return rb_float_new(val);
}

VALUE FM_convert_frame_to_page_y(VALUE fmkr, VALUE v)
{
   FM *p = Get_FM(fmkr); v = rb_Float(v); double val = NUM2DBL(v);
   val = convert_frame_to_page_y(p,val);
   return rb_float_new(val);
}

VALUE FM_convert_frame_to_page_dx(VALUE fmkr, VALUE v)
{
   FM *p = Get_FM(fmkr); v = rb_Float(v); double val = NUM2DBL(v);
   val = convert_frame_to_page_dx(p,val);
   return rb_float_new(val);
}

VALUE FM_convert_frame_to_page_dy(VALUE fmkr, VALUE v)
{
   FM *p = Get_FM(fmkr); v = rb_Float(v); double val = NUM2DBL(v);
   val = convert_frame_to_page_dy(p,val);
   return rb_float_new(val);
}

VALUE FM_convert_page_to_frame_x(VALUE fmkr, VALUE v)
{
   FM *p = Get_FM(fmkr); v = rb_Float(v); double val = NUM2DBL(v);
   val = convert_page_to_frame_x(p,val);
   return rb_float_new(val);
}

VALUE FM_convert_page_to_frame_y(VALUE fmkr, VALUE v)
{
   FM *p = Get_FM(fmkr); v = rb_Float(v); double val = NUM2DBL(v);
   val = convert_page_to_frame_y(p,val);
   return rb_float_new(val);
}

VALUE FM_convert_page_to_frame_dx(VALUE fmkr, VALUE v)
{
   FM *p = Get_FM(fmkr); v = rb_Float(v); double val = NUM2DBL(v);
   val = convert_page_to_frame_dx(p,val);
   return rb_float_new(val);
}

VALUE FM_convert_page_to_frame_dy(VALUE fmkr, VALUE v)
{
   FM *p = Get_FM(fmkr); v = rb_Float(v); double val = NUM2DBL(v);
   val = convert_page_to_frame_dy(p,val);
   return rb_float_new(val);
}

VALUE FM_convert_figure_to_frame_x(VALUE fmkr, VALUE v)
{
   FM *p = Get_FM(fmkr); v = rb_Float(v); double val = NUM2DBL(v);
   val = convert_figure_to_frame_x(p,val);
   return rb_float_new(val);
}

VALUE FM_convert_figure_to_frame_y(VALUE fmkr, VALUE v)
{
   FM *p = Get_FM(fmkr); v = rb_Float(v); double val = NUM2DBL(v);
   val = convert_figure_to_frame_y(p,val);
   return rb_float_new(val);
}

VALUE FM_convert_figure_to_frame_dx(VALUE fmkr, VALUE v)
{
   FM *p = Get_FM(fmkr); v = rb_Float(v); double val = NUM2DBL(v);
   val = convert_figure_to_frame_dx(p,val);
   return rb_float_new(val);
}

VALUE FM_convert_figure_to_frame_dy(VALUE fmkr, VALUE v)
{
   FM *p = Get_FM(fmkr); v = rb_Float(v); double val = NUM2DBL(v);
   val = convert_figure_to_frame_dy(p,val);
   return rb_float_new(val);
}

VALUE FM_convert_frame_to_figure_x(VALUE fmkr, VALUE v)
{
   FM *p = Get_FM(fmkr); v = rb_Float(v); double val = NUM2DBL(v);
   val = convert_frame_to_figure_x(p,val);
   return rb_float_new(val);
}

VALUE FM_convert_frame_to_figure_y(VALUE fmkr, VALUE v)
{
   FM *p = Get_FM(fmkr); v = rb_Float(v); double val = NUM2DBL(v);
   val = convert_frame_to_figure_y(p,val);
   return rb_float_new(val);
}

VALUE FM_convert_frame_to_figure_dx(VALUE fmkr, VALUE v)
{
   FM *p = Get_FM(fmkr); v = rb_Float(v); double val = NUM2DBL(v);
   val = convert_frame_to_figure_dx(p,val);
   return rb_float_new(val);
}

VALUE FM_convert_frame_to_figure_dy(VALUE fmkr, VALUE v)
{
   FM *p = Get_FM(fmkr); v = rb_Float(v); double val = NUM2DBL(v);
   val = convert_frame_to_figure_dy(p,val);
   return rb_float_new(val);
}

double convert_figure_to_output_x(FM *p, double x)
{
   x = convert_figure_to_frame_x(p,x);
   x = convert_frame_to_page_x(p,x);
   x = convert_page_to_output_x(p,x);
   return x;
}

double convert_figure_to_output_dy(FM *p, double dy)
{
   dy = convert_figure_to_frame_dy(p,dy);
   dy = convert_frame_to_page_dy(p,dy);
   dy = convert_page_to_output_dy(p,dy);
   return dy;
}

double convert_figure_to_output_dx(FM *p, double dx)
{
   dx = convert_figure_to_frame_dx(p,dx);
   dx = convert_frame_to_page_dx(p,dx);
   dx = convert_page_to_output_dx(p,dx);
   return dx;
}

double convert_figure_to_output_y(FM *p, double y)
{
   y = convert_figure_to_frame_y(p,y);
   y = convert_frame_to_page_y(p,y);
   y = convert_page_to_output_y(p,y);
   return y;
}

VALUE FM_convert_figure_to_output_x(VALUE fmkr, VALUE v)
{
   FM *p = Get_FM(fmkr); v = rb_Float(v); double val = NUM2DBL(v);
   return rb_float_new(convert_figure_to_output_x(p,val));
}

VALUE FM_convert_figure_to_output_y(VALUE fmkr, VALUE v)
{
   FM *p = Get_FM(fmkr); v = rb_Float(v); double val = NUM2DBL(v);
   return rb_float_new(convert_figure_to_output_y(p,val));
}

VALUE FM_convert_figure_to_output_dx(VALUE fmkr, VALUE v)
{
   FM *p = Get_FM(fmkr); v = rb_Float(v); double val = NUM2DBL(v);
   return rb_float_new(convert_figure_to_output_dx(p,val));
}

VALUE FM_convert_figure_to_output_dy(VALUE fmkr, VALUE v)
{
   FM *p = Get_FM(fmkr); v = rb_Float(v); double val = NUM2DBL(v);
   return rb_float_new(convert_figure_to_output_dy(p,val));
}

double convert_output_to_figure_x(FM *p, double x)
{
   x = convert_output_to_page_x(p,x);
   x = convert_page_to_frame_x(p,x);
   x = convert_frame_to_figure_x(p,x);
   return x;
}

double convert_output_to_figure_dy(FM *p, double dy)
{
   dy = convert_output_to_page_dy(p,dy);
   dy = convert_page_to_frame_dy(p,dy);
   dy = convert_frame_to_figure_dy(p,dy);
   return dy;
}

double convert_output_to_figure_dx(FM *p, double dx)
{
   dx = convert_output_to_page_dx(p,dx);
   dx = convert_page_to_frame_dx(p,dx);
   dx = convert_frame_to_figure_dx(p,dx);
   return dx;
}

double convert_output_to_figure_y(FM *p, double y)
{
   y = convert_output_to_page_y(p,y);
   y = convert_page_to_frame_y(p,y);
   y = convert_frame_to_figure_y(p,y);
   return y;
}

VALUE FM_convert_output_to_figure_x(VALUE fmkr, VALUE v)
{
   FM *p = Get_FM(fmkr); v = rb_Float(v); double val = NUM2DBL(v);
   return rb_float_new(convert_output_to_figure_x(p,val));
}

VALUE FM_convert_output_to_figure_y(VALUE fmkr, VALUE v)
{
   FM *p = Get_FM(fmkr); v = rb_Float(v); double val = NUM2DBL(v);
   return rb_float_new(convert_output_to_figure_y(p,val));
}

VALUE FM_convert_output_to_figure_dx(VALUE fmkr, VALUE v)
{
   FM *p = Get_FM(fmkr); v = rb_Float(v); double val = NUM2DBL(v);
   return rb_float_new(convert_output_to_figure_dx(p,val));
}

VALUE FM_convert_output_to_figure_dy(VALUE fmkr, VALUE v)
{
   FM *p = Get_FM(fmkr); v = rb_Float(v); double val = NUM2DBL(v);
   return rb_float_new(convert_output_to_figure_dy(p,val));
}

