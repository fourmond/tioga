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

void c_rescale_text(OBJ_PTR fmkr, FM *p, double scaling_factor, int *ierr) {
   double scale = scaling_factor * p->default_text_scale;
   if (scaling_factor <= 0) { RAISE_ERROR("Sorry: text scaling must be positive", ierr); return; }
   p->default_text_height_dx *= scaling_factor;
   p->default_text_height_dy *= scaling_factor;
   p->default_text_scale = scale;
}

static int String_Is_Blank(char  *str) {
   char c;
   if (str == NULL) return 1;
   while (1) {
      c = *str++;
      if (c == '\0') return 1;
      if (!isspace(c)) break;
      }
   return 0;
   }

static OBJ_PTR Get_Measure_Hash(OBJ_PTR fmkr, OBJ_PTR measure_name)
{
  OBJ_PTR value;
  int i;
  OBJ_PTR measures_info = Obj_Attr_Get(fmkr, measures_info_ID, &i);
  if(measure_name == OBJ_NIL) {
    return OBJ_NIL;
  }
  if(! Hash_Has_Key_Obj(measures_info, measure_name)) {
    value = Hash_New();
    Hash_Set_Obj_Obj(measures_info, measure_name, value);
  }
  else 
    value = Hash_Get_Obj_Obj(measures_info, measure_name);
  return value;
}


static void tex_show_rotated_text(OBJ_PTR fmkr, FM *p, char *text, 
				  double x, double y, double scale, 
				  double angle, int justification, 
				  int alignment, OBJ_PTR measure_name)
{  // x and y are the device coords for the reference point of the text
   char ref, jst;
   double ft_ht, sz;
   int dummy;
   OBJ_PTR measures = Get_Measure_Hash(fmkr, measure_name);
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
      fprintf(fp,"\\put(%d,%d){\\rotatebox{%.1f}{\\scalebox{%.2f}{\\makebox(0,0)[%c%c]{",
            ROUND(x), ROUND(y), angle, scale, jst, ref);
   else
      fprintf(fp,"\\put(%d,%d){\\scalebox{%.2f}{\\makebox(0,0)[%c%c]{",
            ROUND(x), ROUND(y), scale, jst, ref);
   if(measure_name != OBJ_NIL)
     fprintf(fp, "{\\tiogameasure{%s}{\\tiogasetfont{}", 
	     CString_Ptr(measure_name,&dummy));
   else
     fprintf(fp, "{{\\tiogasetfont{}");

   /* Moving the \BS out of the potential \tiogameasure input, so it does
      not disturb the measure.
   */
   fprintf(fp, (alignment == ALIGNED_AT_BASELINE)? "%s}\\BS" : "%s}", text);
   fprintf(fp, angle != 0? "}}}}}\n" : "}}}}\n");

   /* Now, we save measures informations if applicable*/
   if(measures != OBJ_NIL) {
     Hash_Set_Double(measures, "scale", scale);
     /* [xy]anchor are saved in postscript points */
     Hash_Set_Double(measures, "xanchor", ROUND(x) / ENLARGE);
     Hash_Set_Double(measures, "yanchor", ROUND(y) / ENLARGE);
     Hash_Set_Double(measures, "angle", angle);
     Hash_Set_Double(measures, "just", justification);
     Hash_Set_Double(measures, "align", alignment);
   }
}

static void Convert_Frame_Text_Position_To_Output_Location(FM *p, int frame_side, double offset, 
   double fraction, double *xp, double *yp, double *base_angle, char *text, int *ierr)
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
         if (0.0 > p->bounds_xmax || 0.0 < p->bounds_xmin) {
            RAISE_ERROR_s("Sorry: x origin is not part of plot for (%s)", text, ierr); return; }
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
         if (0.0 > p->bounds_ymax || 0.0 < p->bounds_ymin) {
            RAISE_ERROR_s("Sorry: y origin is not part of plot for (%s)", text, ierr); return; }
         page_y = convert_figure_to_output_y(p, 0.0);
         if (p->yaxis_reversed) offset = -offset;
         page_y += offset;
         page_x = p->page_width * (p->frame_left + fraction * p->frame_width);
         *base_angle = 0;
         break;
      default: 
         RAISE_ERROR_s("Sorry: invalid parameter for frame side in show text (%s)", text, ierr); 
         return;
   }
   *xp = p->page_left + page_x; *yp = p->page_bottom + page_y;
}


void c_show_rotated_text(OBJ_PTR fmkr, FM *p, char *text, int frame_side,
                         double shift, double fraction, double scale,
                         double angle, int justification, int alignment,
                         OBJ_PTR measure_name, int *ierr)
{
   double x = 0, y = 0, base_angle = 0;
   double ft_ht = p->default_text_scale * scale * p->default_font_size;
   Convert_Frame_Text_Position_To_Output_Location(p, frame_side,
                                                  shift * ft_ht * ENLARGE,
                                                  fraction, &x, &y,
                                                  &base_angle, text, ierr);
   tex_show_rotated_text(fmkr, p, text, x, y, scale, angle + base_angle,
                         justification, alignment, measure_name);
}

         
void c_show_rotated_label(OBJ_PTR fmkr, FM *p, char *text, 
   double xloc, double yloc, double scale, double angle, int justification, int alignment, OBJ_PTR measure_name, int *ierr) {
   tex_show_rotated_text(fmkr, p, text, convert_figure_to_output_x(p, xloc), convert_figure_to_output_y(p, yloc),
			 scale, angle, justification, alignment, measure_name);
}
   
OBJ_PTR c_check_label_clip(OBJ_PTR fmkr, FM *p, double x, double y, int *ierr) {
   x = convert_figure_to_frame_x(p,x);
   y = convert_figure_to_frame_y(p,y);
   if (x < p->label_left_margin || y < p->label_bottom_margin ||
         1.0 - x < p->label_right_margin || 1.0 - y < p->label_top_margin) RETURN_FALSE;
   RETURN_TRUE;
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

void Open_tex(OBJ_PTR fmkr, char *filename, bool quiet_mode, int *ierr) 
{
   char ofile[300];
   Get_tex_name(ofile, filename, 300);
   FM *p = Get_FM(fmkr,ierr);
   fp = fopen(ofile, "w");
   fprintf(fp,"\\setlength{\\unitlength}{%fbp}%%\n", 1.0/ENLARGE);
   cur_pos = ftell(fp);
   fprintf(fp,"\\begin{picture}(xxxxxx,xxxxxx)            %% (width,height)(xoffset,yoffset) -- Adjust the 2nd pair for registration adjustments\n"); /* this line is rewritten at the end */
   fprintf(fp,"\\def\\BS{\\phantom{\\Huge\\scalebox{0}[2]{\\hbox{\\rotatebox{180}{O}O}}}}\n"); 
      // graphicx seems to vertically align baseline (B) like center (c), 
      // so we add BS (Big Strut) to make them look the same
}

void Close_tex(OBJ_PTR fmkr, bool quiet_mode, int *ierr)
{
   double x, y, xoff, yoff;
   FM *p = Get_FM(fmkr,ierr);
   x = bbox_urx - bbox_llx; if (x < 0) x = bbox_urx = bbox_llx = 0;
   y = bbox_ury - bbox_lly; if (y < 0) y = bbox_ury = bbox_lly = 0;
   xoff = bbox_llx + Get_tex_xoffset(fmkr,ierr)*ENLARGE;
   yoff = bbox_lly + Get_tex_yoffset(fmkr,ierr)*ENLARGE;
   fprintf(fp,"\\end{picture}");
   fseek(fp, cur_pos, SEEK_SET);
   fprintf(fp,"\\begin{picture}(%03d,%03d)(%02d,%d)", ROUND(x), ROUND(y), ROUND(xoff), ROUND(yoff));
   fclose(fp);
}   


static void Write_preview_header(OBJ_PTR fmkr, FILE *file, int *ierr) {
   fprintf(file, "\\documentclass{%s}\n\n", Get_tex_preview_documentclass(fmkr,ierr));   
   /* we print out the preamble generated from tioga.sty.in */
   fprintf(file, "%% Tioga preamble generated from tioga.sty.in\n");
   fprintf(file, "%s\n", Get_tex_preview_generated_preamble(fmkr,ierr));
   fprintf(file, "%% User-specified preamble\n");
   fprintf(file, "%s\n\n", Get_tex_preamble(fmkr,ierr));
   fprintf(file, "%% Command to format numeric labels on xaxis\n");
   fprintf(file, "\\newcommand{\\tiogaxaxisnumericlabel}[1]{%s}\n\n", Get_xaxis_numeric_label_tex(fmkr,ierr));
   fprintf(file, "%% Command to format numeric labels on yaxis\n");
   fprintf(file, "\\newcommand{\\tiogayaxisnumericlabel}[1]{%s}\n\n", Get_yaxis_numeric_label_tex(fmkr,ierr));
   fprintf(file, "%% Color constants definitions\n");
   fprintf(file, "%s\n\n", CString_Ptr(COLOR_PREAMBLE(fmkr,ierr),ierr)); 
   fprintf(file, "%% Set page margins, page size and orientation.\n");
   fprintf(file, "\t\\usepackage[pdftex,tmargin=0pt,lmargin=0pt,"
	   "rmargin=0pt,bmargin=0pt,\n");
   fprintf(file, "\tpaperwidth=%s,paperheight=%s,\n", 
	   Get_tex_preview_paper_width(fmkr,ierr),
	   Get_tex_preview_paper_height(fmkr,ierr));
   fprintf(file, "\thoffset=%s,voffset=%s\n", 
	   Get_tex_preview_hoffset(fmkr,ierr),
	   Get_tex_preview_voffset(fmkr,ierr));
   fprintf(file, "\t]{geometry}\n");

   fprintf(file, "\n%% We need the graphicx package and the calc package.\n");
   fprintf(file, "\t\\usepackage{graphicx}\n");
   fprintf(file, "\t\\usepackage{calc}\n\n");
   fprintf(file, "\t%% This is necessary to avoid getting the picture on the second page\n");
   fprintf(file, "\t\\topskip=0pt\n\n");

   /* now, the commands to customize the font used */
   fprintf(file, "\\settiogafontsize[10pt]{%s}\n", Get_tex_fontsize(fmkr,ierr));
   fprintf(file, "\\settiogafontfamily{\\%s}\n", Get_tex_fontfamily(fmkr,ierr));
   fprintf(file, "\\settiogafontseries{\\%s}\n", Get_tex_fontseries(fmkr,ierr));
   fprintf(file, "\\settiogafontshape{\\%s}\n", Get_tex_fontshape(fmkr,ierr));
}


static void Write_figure_command(OBJ_PTR fmkr, char *simple_name, FILE *file, int *ierr) {
   char *minwhitespace;
   
   if (Get_tex_preview_fullpage(fmkr,ierr)) {
        minwhitespace = Get_tex_preview_minwhitespace(fmkr,ierr);
        if (minwhitespace == NULL) {
            fprintf(file, "\\tiogafigurefullpage{%s}\n", simple_name); 
        } else {
            fprintf(file, "\\tiogafigurefullpage[%s]{%s}\n", minwhitespace, simple_name); 
        }
   } else {
       const char * command = Get_tex_preview_tiogafigure_command(fmkr,ierr);
       if(strcmp(command, "tiogafigureshow")) {
        fprintf(file, "\\%s{%s}{%s}{%s}\n", Get_tex_preview_tiogafigure_command(fmkr,ierr), simple_name, 
            Get_tex_preview_figure_width(fmkr,ierr), Get_tex_preview_figure_height(fmkr,ierr)); 
       } else { /* no need for extra arguments for tiogafigureshow */
        fprintf(file, "\\%s{%s}\n", Get_tex_preview_tiogafigure_command(fmkr,ierr), simple_name); 
       }
   }
}

   
void Create_wrapper(OBJ_PTR fmkr, char *fname, bool quiet_mode, int *ierr)
{  // create the wrapper TeX file to combine the text and graphics to make a figure
   char *dot;
   char tex_fname[100], base_name[100], simple_name[100];
   FILE *file;
   if ((dot=strrchr(fname,'.')) != NULL) {
      strncpy(base_name, fname, dot-fname); base_name[dot-fname] = '\0';
      snprintf(tex_fname, sizeof(tex_fname), "%s.tex", base_name);
      }
   else {
      strcpy(base_name, fname);
      snprintf(tex_fname, sizeof(tex_fname), "%s.tex", fname);
      }
   if ((dot=strrchr(base_name,'/')) != NULL) {
      strcpy(simple_name, dot+1);
      }
   else {
      strcpy(simple_name, base_name);
      }
   file = fopen(tex_fname, "w");
   fprintf(file, "%% Tioga preview LaTeX file for %s_figure.pdf and %s_figure.txt\n\n", base_name, base_name);

   Write_preview_header(fmkr, file, ierr);

   fprintf(file, "\n%% Here's the page with the figure.\n");
   fprintf(file, "\\begin{document}\n");
   fprintf(file, "\\pagestyle{%s}\n", Get_tex_preview_pagestyle(fmkr,ierr));
   /* necessary to get the position right */
   fprintf(file, "\\noindent");
   Write_figure_command(fmkr, simple_name, file, ierr);
   fprintf(file, "\\end{document}\n");
   fclose(file);
}

void Init_tex(int *ierr)
{
}

void Rename_tex(char *oldname, char *newname, int *ierr)
{
   char old_ofile[300], new_ofile[300];
   Get_tex_name(old_ofile, oldname, 300);
   Get_tex_name(new_ofile, newname, 300);
   rename(old_ofile, new_ofile); // from stdio.h
}

void private_make_portfolio(char *name, OBJ_PTR fignums, OBJ_PTR fignames, int *ierr)
{
    FILE *file;
    int i, len, numfigs, j;
    char tex_fname[256];
    snprintf(tex_fname, sizeof(tex_fname), "%s.tex", name);
    file = fopen(tex_fname, "w");
    if (file == NULL) {
       RAISE_ERROR_s("Sorry: can't open %s.\n", tex_fname, ierr); return; }
    fprintf(file, "%% Tioga Portfolio %s\n\n", name);
    fprintf(file, "\\documentclass{article}\n");
    fprintf(file, "\\usepackage{pdfpages}\n");
    fprintf(file, "\\begin{document}\n");
    fprintf(file, "%% Start of figures, one per page\n\n");
    len = Array_Len(fignames,ierr);
    if (fignums == OBJ_NIL) {
        for (i=0; i < len; i++) {
            fprintf(file, "\\includepdf{%s.pdf}\n", Get_String(fignames, i, ierr));
            if (*ierr != 0) return;
        }
    } else {
        numfigs = Array_Len(fignums,ierr);
        if (*ierr != 0) return;
        for (i=0; i < numfigs; i++) {
            OBJ_PTR n = Array_Entry(fignums,i,ierr);
            if (*ierr != 0) return;
            j = Number_to_int(n,ierr);
            if (j >= 0 && j < len) {
               fprintf(file, "\\includepdf{%s.pdf}\n", Get_String(fignames, j, ierr));
               if (*ierr != 0) return;
            }
            else {
                fclose(file);
                RAISE_ERROR("Requested figure numbers must be >= 0 and < num_figures.", ierr);
                return;
            }
        }
    }
    fprintf(file, "\n\\end{document}\n");
    fclose(file);
}


/* 
   Stores and transforms measures as printed by pdflatex.

   Takes sizes in bp.

*/
void c_private_save_measure(OBJ_PTR fmkr, OBJ_PTR measure_name,
                            double width, double height, double depth)
{
  double angle, scale;
  int just, align;
  /* Page coordinates in bp before applying rotation ! */
  double xl,xr,yt,yb;
  double xa,ya;
  OBJ_PTR measures = Get_Measure_Hash(fmkr, measure_name);
  int dummy;

  /* The following really should not happen */
  if(measures == OBJ_NIL) {
    fprintf(stderr, "Warning: got hash = OBJ_NIL in %s, line %d\n",
	    __FILE__, __LINE__);
    return;
  }

  /* Storing measured sizes  */
  Hash_Set_Double(measures, "tex_measured_width", width);
  Hash_Set_Double(measures, "tex_measured_height", height);
  Hash_Set_Double(measures, "tex_measured_depth", depth);

  angle = Hash_Get_Double(measures, "angle");
  scale = Hash_Get_Double(measures, "scale");
  just = Hash_Get_Double(measures, "just");
  align = Hash_Get_Double(measures, "align");
  
  /* Setting the appropriate scale */
  width *= scale;
  height *= scale;
  depth *= scale;
  /* Now setting the width and height in  */
  Hash_Set_Double(measures, "width", width);
  Hash_Set_Double(measures, "height", height);
  Hash_Set_Double(measures, "depth", depth);

  xa = Hash_Get_Double(measures, "xanchor");
  ya = Hash_Get_Double(measures, "yanchor");
  
  /* Now, we try to compute the precise position of
     the points of the box surrounding the text.
  */

  /* First, before rotation: */
  switch(just) {
  case 1:			/* Right-justified */
    xr = xa;
    xl = xa - width;
    break;
  case 0:			/* Centered */
    xr = xa + width/2;
    xl = xa - width/2;
    break;
  case -1:			/* Left-justified */
    xl = xa;
    xr = xa + width;
    break;
  default:
    fprintf(stderr, "Invalid justification = %d at %s, line %d\n",
	    just, __FILE__, __LINE__);
    xl = xa;
    xr = xa + width/2;
  }

  /* First, before rotation: */
  switch(align) {
  case ALIGNED_AT_BASELINE:
    yt = ya + height;
    yb = ya - depth;
    break;
  case ALIGNED_AT_BOTTOM:
    yt = ya + height + depth;
    yb = ya;
    break;
  case ALIGNED_AT_TOP:
    yb = ya - height - depth;
    yt = ya;
    break;
  default:			/* Centered */
    yb = ya - 0.5*(height + depth);
    yt = ya + 0.5*(height + depth);
  }

  /* Now, rotation */
  if(angle == 0.0) {
    /* xbl = x of 'bottom left' */
    Hash_Set_Double(measures, "xbl", xl);
    Hash_Set_Double(measures, "ybl", yb);
    Hash_Set_Double(measures, "xtl", xl);
    Hash_Set_Double(measures, "ytl", yt);
    Hash_Set_Double(measures, "xbr", xr);
    Hash_Set_Double(measures, "ybr", yb);
    Hash_Set_Double(measures, "xtr", xr);
    Hash_Set_Double(measures, "ytr", yt);
  }
  else {
    double s = - sin(angle * PI/180);
    double c = cos(angle * PI/180);
    Hash_Set_Double(measures, "xbl", 
		    xa + (xl-xa)*c + (yb - ya) * s);
    Hash_Set_Double(measures, "ybl",
		    ya - (xl-xa)*s + (yb - ya) * c);
    Hash_Set_Double(measures, "xtl", 
		    xa + (xl-xa)*c + (yt - ya) * s);
    Hash_Set_Double(measures, "ytl",
		    ya - (xl-xa)*s + (yt - ya) * c);
    Hash_Set_Double(measures, "xbr", 
		    xa + (xr-xa)*c + (yb - ya) * s);
    Hash_Set_Double(measures, "ybr",
		    ya - (xr-xa)*s + (yb - ya) * c);
    Hash_Set_Double(measures, "xtr", 
		    xa + (xr-xa)*c + (yt - ya) * s);
    Hash_Set_Double(measures, "ytr",
		    ya - (xr-xa)*s + (yt - ya) * c);
  }
  
  /* We transform coordinates into an array 
     (topleft, topright,  botright, botleft)
     of arrays (xy) of doubles 
  */
  OBJ_PTR points = Array_New(0);
  OBJ_PTR current_point = OBJ_NIL;
  int i;
  for(i = 0; i < 8; i++) {
    char buf[4];
    if(! (i % 2)) {
      current_point = Array_New(0);
      Array_Push(points, current_point, &dummy);
    }
    snprintf(buf, sizeof(buf), "%c%c%c", 
	     (i%2 ? 'y' : 'x'),
	     (i/4 ? 't' : 'b'),
	     ((i >= 2) && (i < 6) ? 'r' : 'l'));
    Array_Push(current_point, Hash_Get_Obj(measures, buf), &dummy);
  }
  Hash_Set_Obj(measures, "points", points);
}
