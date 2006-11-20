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
   ft_ht = scale * p->default_font_size;
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
      fprintf(fp,"\\put(%d,%d){\\rotatebox{%.1f}{\\scalebox{%.2f}{\\makebox(0,0)[%c%c]{\\tiogasetfont",
            ROUND(x), ROUND(y), angle, scale, jst, ref);
   else
      fprintf(fp,"\\put(%d,%d){\\scalebox{%.2f}{\\makebox(0,0)[%c%c]{\\tiogasetfont",
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
   double x, y, base_angle, ft_ht = p->default_text_scale * scale * p->default_font_size;
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
   fprintf(fp,"\\end{picture}");
   fseek(fp, cur_pos, SEEK_SET);
   fprintf(fp,"\\begin{picture}(%03d,%03d)(%02d,%d)", ROUND(x), ROUND(y), ROUND(xoff), ROUND(yoff));
   fclose(fp);
}   


void Write_preview_header(VALUE fmkr, FILE *file) {
   fprintf(file, "\\documentclass{%s}\n\n", Get_tex_preview_documentclass(fmkr));
   /* we print out the preamble generated from tioga.sty.in */
   fprintf(file, "%% Tioga preamble generated from tioga.sty.in\n");
   fprintf(file, "%s\n", Get_tex_preview_generated_preamble(fmkr));
   fprintf(file, "%% User-specified preamble\n");
   fprintf(file, "%s\n\n", Get_tex_preamble(fmkr));
   fprintf(file, "%% Set page margins, page size and orientation.\n");
   fprintf(file, "\t\\usepackage[pdftex,tmargin=0pt,lmargin=0pt,"
	   "rmargin=0pt,bmargin=0pt,\n");
   fprintf(file, "\tpaperwidth=%s,paperheight=%s,\n", 
	   Get_tex_preview_paper_width(fmkr),
	   Get_tex_preview_paper_height(fmkr));
   fprintf(file, "\thoffset=%s,voffset=%s\n", 
	   Get_tex_preview_hoffset(fmkr),
	   Get_tex_preview_voffset(fmkr));
   fprintf(file, "\t]{geometry}\n");

   fprintf(file, "\n%% We need the graphicx package and the calc package.\n");
   fprintf(file, "\t\\usepackage{graphicx}\n");
   fprintf(file, "\t\\usepackage{calc}\n\n");
   fprintf(file, "\t%% This is necessary to avoid getting the picture on the second page\n");
   fprintf(file, "\t\\topskip=0pt\n\n");

   /* now, the commands to customize the font used */
   fprintf(file, "\\settiogafontsize[10pt]{%s}\n", Get_tex_fontsize(fmkr));
   fprintf(file, "\\settiogafontfamily{\\%s}\n", Get_tex_fontfamily(fmkr));
   fprintf(file, "\\settiogafontseries{\\%s}\n", Get_tex_fontseries(fmkr));
   fprintf(file, "\\settiogafontshape{\\%s}\n", Get_tex_fontshape(fmkr));
}


void Write_figure_command(VALUE fmkr, char *simple_name, FILE *file) {
   char *minwhitespace;
   
   if (Get_tex_preview_fullpage(fmkr)) {
        minwhitespace = Get_tex_preview_minwhitespace(fmkr);
        if (minwhitespace == NULL) {
            fprintf(file, "\\tiogafigurefullpage{%s}\n", simple_name); 
        } else {
            fprintf(file, "\\tiogafigurefullpage[%s]{%s}\n", minwhitespace, simple_name); 
        }
   } else {
       const char * command = Get_tex_preview_tiogafigure_command(fmkr);
       if(strcmp(command, "tiogafigureshow")) {
        fprintf(file, "\\%s{%s}{%s}{%s}\n", Get_tex_preview_tiogafigure_command(fmkr), simple_name, 
            Get_tex_preview_figure_width(fmkr), Get_tex_preview_figure_height(fmkr)); 
       } else { /* no need for extra arguments for tiogafigureshow */
        fprintf(file, "\\%s{%s}\n", Get_tex_preview_tiogafigure_command(fmkr), simple_name); 
       }
   }
}

   
void Create_wrapper(VALUE fmkr, char *fname, bool quiet_mode)
{  // create the wrapper TeX file to combine the text and graphics to make a figure
   char *dot;
   char tex_fname[100], base_name[100], simple_name[100];
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
   fprintf(file, "%% Tioga preview LaTeX file for %s_figure.pdf and %s_figure.txt\n\n", base_name, base_name);

   Write_preview_header(fmkr, file);

   fprintf(file, "\n%% Here's the page with the figure.\n");
   fprintf(file, "\\begin{document}\n");
   fprintf(file, "\\pagestyle{%s}\n", Get_tex_preview_pagestyle(fmkr));
   /* necessary to get the position right */
   fprintf(file, "\\noindent");
   Write_figure_command(fmkr, simple_name, file);
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

VALUE FM_private_make_portfolio(VALUE fmkr, VALUE name, VALUE filename, VALUE fignames)
{
    /*
    FM *p = Get_FM(fmkr); // not in use
    */
    FILE *file;
    /*
    VALUE figname; // not in use
    */
    char *fname;
    int i, len;
    name = rb_String(name);
    filename = rb_String(filename);
    fname = RSTRING(filename)->ptr;
    file = fopen(fname, "w");
    fprintf(file, "%% Portfolio file %s\n", RSTRING(name)->ptr);
    Write_preview_header(fmkr, file);
    fprintf(file, "%% The document starts here.\n");
    fprintf(file, "\\begin{document}\n");
    fprintf(file, "\\pagestyle{%s}\n\n", Get_tex_preview_pagestyle(fmkr));   
    fprintf(file, "%% Start of figures, one per page\n\n");
    fignames = rb_Array(fignames);
    len = RARRAY(fignames)->len;
    for (i=0; i < len; i++) {
        fprintf(file, "\\begin{figure}\n");
        Write_figure_command(fmkr, Get_String(fignames, i), file);
        fprintf(file, "\\end{figure}\n\\clearpage\n\n");
    }
    fprintf(file, "\\end{document}\n");
    fclose(file);
    return fmkr;
}


