/* 
   profile_fancy_read.c: profiling support for dvector_fast_fancy_read

   Copyright 2007 by Vincent Fourmond

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Library Public License as published
   by the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.
   
   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU Library General Public License for more details.
   
   You should have received a copy of the GNU Library General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
*/

#include <ruby.h>
#include <stdio.h>
#include <stdlib.h>

extern void Init_Dvector();
int main()
{
  int i;
  ruby_init();
  ruby_init_loadpath();
  Init_Dvector();
  VALUE cls = rb_eval_string("Dobjects::Dvector");
  VALUE res;
  
  /* Then, we prepare a file, tmp.dat */
  FILE * tmp = fopen("tmp.dat", "w");
  for(i = 0; i < 100000; i++) 
    fprintf(tmp, "%g\t%g\n", drand48(), drand48());
  fclose(tmp);
  /*for(i = 0; i < 10; i++)*/
  res = rb_eval_string("Dobjects::Dvector.fast_fancy_read"
		       "(File.open('tmp.dat', 'r'), "
		       "Dobjects::Dvector::FANCY_READ_DEFAULTS)");
  rb_p(res);
  return 0;
}
