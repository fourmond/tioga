/* texout.c */
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

#define RADIANS_TO_DEGREES (180.0 / PI)

static FILE *fp; // for the TeX file

/* TeX text */

void c_text_scale_set(FM *p, double scale)
{
   double factor = scale / p->default_text_scale;
   if (factor <= 0) rb_raise(rb_eArgError, "Sorry: text scaling must be positive");
   p->default_text_height_dx *= factor;
   p->default_text_height_dy *= factor;
   p->default_text_scale = scale;
}

VALUE FM_rescale_text(VALUE fmkr, VALUE scaling_factor) // updates default text heights too
{
   FM *p = Get_FM(fmkr);
   scaling_factor = rb_Float(scaling_factor);
   c_text_scale_set(p, NUM2DBL(scaling_factor) * p->default_text_scale);
   return fmkr;
}

int String_Is_Blank(char  *str) {
   char c;
   if (str == NULL) return 1;
   while (1) {
      c = *str++;
      if (c == '\0') return 1;
      if (!isspace(c)) break;
      }
   return 0;
   }

void tex_show_rotated_text(FM *p, char *text, double x, double y, double scale, double angle, int justification, int alignment)
{  // x and y are the device coords for the reference point of the text
   char ref, jst;
   double ft_ht, sz;
   if (String_Is_Blank(text)) return; /* blank strings break TeX! */
   scale *= p->default_text_scale;
   ft_ht = scale * DEFAULT_FONT_HT;
   sz = ft_ht * ENLARGE;
   ref = (alignment == ALIGNED_AT_BASELINE)? 'B' : 
         (alignment == ALIGNED_AT_BOTTOM)? 'b' :
         (alignment == ALIGNED_AT_TOP)? 't' : 'c';
   if (justification == 0) jst = 'c';
   else if (justification > 0) jst = 'r';
   else jst = 'l';
   bbox_llx = MIN(bbox_llx, x - sz);
   bbox_lly = MIN(bbox_lly, y - sz);
   bbox_urx = MAX(bbox_urx, x + sz);
   bbox_ury = MAX(bbox_ury, y + sz);
   if (angle != 0.0)
      fprintf(fp,"\\put(%d,%d){\\rotatebox{%.1f}{\\scalebox{%.2f}{\\makebox(0,0)[%c%c]{\\FF",
            ROUND(x), ROUND(y), angle, scale, jst, ref);
   else
      fprintf(fp,"\\put(%d,%d){\\scalebox{%.2f}{\\makebox(0,0)[%c%c]{\\FF",
            ROUND(x), ROUND(y), scale, jst, ref);
   fprintf(fp, (alignment == ALIGNED_AT_BASELINE)? "{%s\\BS" : "{%s", text);
   fprintf(fp, angle != 0? "}}}}}\n" : "}}}}\n");
}

static void Convert_Frame_Text_Position_To_Output_Location(FM *p, int frame_side, double offset, 
   double fraction, double *xp, double *yp, double *base_angle, char *text)
{
   double page_x, page_y;
   switch (frame_side) {
      case LEFT:
         page_x = p->page_width * p->frame_left - offset;
         page_y = p->page_height * (p->frame_bottom + fraction * p->frame_height);
         *base_angle = 90;
         break;
      case RIGHT:
         page_x = p->page_width * p->frame_right + offset;
         page_y = p->page_height * (p->frame_bottom + fraction * p->frame_height);
         *base_angle = 90;
         break;
      case AT_X_ORIGIN:
         if (0.0 > p->bounds_xmax || 0.0 < p->bounds_xmin)
            rb_raise(rb_eArgError, "Sorry: x origin is not part of plot for (%s)", text);
         page_x = convert_figure_to_output_x(p, 0.0);
         if (p->xaxis_reversed) offset = -offset;
         page_x += offset;
         page_y = p->page_height * (p->frame_bottom + fraction * p->frame_height);
         *base_angle = 90;
         break;
      case TOP:
         page_y = p->page_height * p->frame_top + offset;
         page_x = p->page_width * (p->frame_left + fraction * p->frame_width);
         *base_angle = 0;
         break;
      case BOTTOM:
         page_y = p->page_height * p->frame_bottom - offset;
         page_x = p->page_width * (p->frame_left + fraction * p->frame_width);
         *base_angle = 0;
         break;
      case AT_Y_ORIGIN:
         if (0.0 > p->bounds_ymax || 0.0 < p->bounds_ymin)
            rb_raise(rb_eArgError, "Sorry: y origin is not part of plot for (%s)", text);
         page_y = convert_figure_to_output_y(p, 0.0);
         if (p->yaxis_reversed) offset = -offset;
         page_y += offset;
         page_x = p->page_width * (p->frame_left + fraction * p->frame_width);
         *base_angle = 0;
         break;
      default: rb_raise(rb_eArgError, "Sorry: invalid parameter for frame side in show text (%s)", text);
   }
   *xp = p->page_left + page_x; *yp = p->page_bottom + page_y;
}

void c_show_rotated_text(FM *p, char *text, int frame_side, double shift, double fraction,
   double scale, double angle, int justification, int alignment)
{
   double x, y, base_angle, ft_ht = p->default_text_scale * scale * DEFAULT_FONT_HT;
   Convert_Frame_Text_Position_To_Output_Location(p, frame_side, shift*ft_ht*ENLARGE, fraction, &x, &y, &base_angle, text);
   tex_show_rotated_text(p, text, x, y, scale, angle + base_angle, justification, alignment);
}
   
VALUE FM_show_rotated_text(VALUE fmkr, VALUE text, VALUE frame_side, VALUE shift,
   VALUE fraction, VALUE scale, VALUE angle, VALUE justification, VALUE alignment)
{
   FM *p = Get_FM(fmkr);
   text = rb_String(text);
   frame_side = rb_Integer(frame_side);
   shift = rb_Float(shift);
   fraction = rb_Float(fraction);
   scale = rb_Float(scale);
   angle = rb_Float(angle);
   justification = rb_Integer(justification);
   alignment = rb_Integer(alignment);
   c_show_rotated_text(p, RSTRING(text)->ptr, NUM2INT(frame_side), NUM2DBL(shift),
      NUM2DBL(fraction), NUM2DBL(scale), NUM2DBL(angle), NUM2INT(justification), NUM2INT(alignment));
   return fmkr;
}
         
void c_show_rotated_label(FM *p, char *text, 
   double xloc, double yloc, double scale, double angle, int justification, int alignment)
{
   tex_show_rotated_text(p, text, convert_figure_to_output_x(p, xloc), convert_figure_to_output_y(p, yloc),
      scale, angle, justification, alignment);
}
   
VALUE FM_show_rotated_label(VALUE fmkr, VALUE text,
   VALUE xloc, VALUE yloc, VALUE scale, VALUE angle, VALUE justification, VALUE alignment)
{
   FM *p = Get_FM(fmkr);
   text = rb_String(text);
   xloc = rb_Float(xloc);
   yloc = rb_Float(yloc);
   scale = rb_Float(scale);
   angle = rb_Float(angle);
   justification = rb_Integer(justification);
   alignment = rb_Integer(alignment);
   c_show_rotated_label(p, RSTRING(text)->ptr, NUM2DBL(xloc), NUM2DBL(yloc),
      NUM2DBL(scale), NUM2DBL(angle), NUM2INT(justification), NUM2INT(alignment));
   return fmkr;
}

VALUE FM_check_label_clip(VALUE fmkr, VALUE xloc, VALUE yloc)
{
   FM *p = Get_FM(fmkr);
   xloc = rb_Float(xloc);
   yloc = rb_Float(yloc);
   double x = NUM2DBL(xloc), y = NUM2DBL(yloc);
   x = convert_figure_to_frame_x(p,x);
   y = convert_figure_to_frame_y(p,y);
   if (x < p->label_left_margin || y < p->label_bottom_margin ||
         1.0 - x < p->label_right_margin || 1.0 - y < p->label_top_margin)  return Qfalse;
   return Qtrue;
}

/* TeX File Management */

static long cur_pos;

static void Get_tex_name(char *ofile, char *filename, int maxlen)
{
   char *dot;
   strncpy(ofile, filename, maxlen);
   dot = strrchr(ofile,'.');
   if (dot != NULL) dot[0] = '\0';
   strcat(ofile, "_figure.txt");
}

void Open_tex(VALUE fmkr, char *filename, bool quiet_mode) 
{
   char ofile[300];
   Get_tex_name(ofile, filename, 300);
   fp = fopen(ofile, "w");
   fprintf(fp,"\\setlength{\\unitlength}{%fbp}%%\n", 1.0/ENLARGE);
   fprintf(fp,"\\begingroup\\makeatletter\\ifx\\SetFigFont\\undefined%%\n");
   fprintf(fp,"\\gdef\\FF{%%\n");
   fprintf(fp,"\\reset@font\\fontsize{%0.1f}{12pt}%%\n", DEFAULT_FONT_HT);
   fprintf(fp,"\\fontfamily{\\rmdefault}\\fontseries{\\mddefault}\\fontshape{\\updefault}%%\n");
   fprintf(fp,"\\selectfont}%%\n");
   fprintf(fp,"\\fi\\endgroup%%\n"); 
   cur_pos = ftell(fp);
   fprintf(fp,"\\begin{picture}(xxxxxx,xxxxxx)            %% (width,height)(xoffset,yoffset) -- Adjust the 2nd pair for registration adjustments\n"); /* this line is rewritten at the end */
   fprintf(fp,"\\def\\BS{\\phantom{\\Huge\\scalebox{0}[2]{\\hbox{\\rotatebox{180}{O}O}}}}\n"); 
      // graphicx seems to vertically align baseline (B) like center (c), 
      // so we add BS (Big Strut) to make them look the same
   fmkr = Qnil; // unused
}

void Close_tex(VALUE fmkr, bool quiet_mode)
{
   double x, y, xoff, yoff;
   x = bbox_urx - bbox_llx; if (x < 0) x = bbox_urx = bbox_llx = 0;
   y = bbox_ury - bbox_lly; if (y < 0) y = bbox_ury = bbox_lly = 0;
   xoff = bbox_llx + Get_tex_xoffset(fmkr)*ENLARGE;
   yoff = bbox_lly + Get_tex_yoffset(fmkr)*ENLARGE;
   fprintf(fp,"\\end{picture}\n");
   fseek(fp, cur_pos, SEEK_SET);
   fprintf(fp,"\\begin{picture}(%d,%d)(%d,%d)", ROUND(x), ROUND(y), ROUND(xoff), ROUND(yoff));
   fclose(fp);
}   
   
void Create_wrapper(VALUE fmkr, char *fname, bool quiet_mode)
{  // create the wrapper TeX file to combine the text and graphics to make a figure
   double x, y;
   char *dot;
   char tex_fname[100], base_name[100], simple_name[100];
   float preview_paper_width, preview_paper_height;
   float preview_hoffset, preview_voffset;
   float preview_figure_width, preview_figure_height;
   float figure_width, figure_height, paperwidth, paperheight;
   FILE *file;
   if ((dot=strrchr(fname,'.')) != NULL) {
      strncpy(base_name, fname, dot-fname); base_name[dot-fname] = '\0';
      sprintf(tex_fname, "%s.tex", base_name);
      }
   else {
      strcpy(base_name, fname);
      sprintf(tex_fname, "%s.tex", fname);
      }
   if ((dot=strrchr(base_name,'/')) != NULL) {
      strcpy(simple_name, dot+1);
      }
   else {
      strcpy(simple_name, base_name);
      }
   file = fopen(tex_fname, "w");
   fprintf(file, "%%      Sample LaTeX file for combining %s_figure.pdf and %s_figure.txt\n\n", base_name, base_name);
   fprintf(file, "\\documentclass{%s}\n", Get_tex_preview_documentclass(fmkr));
   fprintf(file, "%s\n", Get_tex_preview_preamble(fmkr));
   fprintf(file, "%% Set lengths to make the preview page fit the figure.\n");
   
   x = bbox_urx - bbox_llx; if (x < 0) x = bbox_urx = bbox_llx = 0;
   y = bbox_ury - bbox_lly; if (y < 0) y = bbox_ury = bbox_lly = 0;
   figure_width = x/ENLARGE;
   figure_height = y/ENLARGE;

   paperwidth = figure_width;
   paperheight = figure_height; // plus margins?

   //paperwidth *= figure_scale;
   //paperheight *= figure_scale;
      
   fprintf(file, "\\setlength{\\paperwidth}{%s}\n", Get_tex_preview_paper_width(fmkr));
   fprintf(file, "\\setlength{\\paperheight}{%s}\n", Get_tex_preview_paper_height(fmkr));
   fprintf(file, "\\setlength{\\hoffset}{%s}\n", Get_tex_preview_hoffset(fmkr));
   fprintf(file, "\\setlength{\\voffset}{%s}\n", Get_tex_preview_voffset(fmkr));

   fprintf(file, "\n%% We need the graphicx package.\n");
   fprintf(file, "\\usepackage{graphicx}\n\n");
   fprintf(file, "%% Here are some commands for doing our figures.\n");
   fprintf(file, "\\newcommand{\\TiogaFigureShow}[1]{\n\t\\rotatebox{0.0}{\n");
   fprintf(file, "\t\\begin{picture}(0,0)(0,0)\n");
   fprintf(file, "\t\\includegraphics[scale=1.0,clip]{#1_figure.pdf}\n");
   fprintf(file, "\t\\end{picture}\n");
   fprintf(file, "\t\\input{#1_figure.txt}}}\n");
   fprintf(file, "\\newcommand{\\TiogaFigureSized}[3]{\n\t\\centering{\\resizebox{#2}{#3}{\\TiogaFigureShow{#1}}}}\n");
   fprintf(file, "\t%% The 1st arg is the base name for the pdf and txt files.\n");
   fprintf(file, "\t%% The 2nd arg is a width.\n");
   fprintf(file, "\t%% The 3rd arg is a height.\n");
   fprintf(file, "\\newcommand{\\TiogaFigure}[1]{\n\t\\TiogaFigureSized{#1}{\\columnwidth}{!}}\n\t%% The default is to resize to fit the column width.\n\n");

   fprintf(file, "\\begin{document}\n");
   fprintf(file, "\\pagestyle{%s}\n", Get_tex_preview_pagestyle(fmkr));   
   fprintf(file, "\\TiogaFigureSized{%s}{%s}{%s}\n", simple_name, Get_tex_preview_figure_width(fmkr), Get_tex_preview_figure_height(fmkr));   
   fprintf(file, "\\end{document}\n");
   fclose(file);
}

void Init_tex(void)
{
}

void Rename_tex(char *oldname, char *newname)
{
   char old_ofile[300], new_ofile[300];
   Get_tex_name(old_ofile, oldname, 300);
   Get_tex_name(new_ofile, newname, 300);
   rename(old_ofile, new_ofile); // from stdio.h
}



