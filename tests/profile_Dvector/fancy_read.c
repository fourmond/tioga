/* 
   fancy_read.c: profiling support for dvector_fast_fancy_read

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


extern void Init_Dvector();
int main()
{
  ruby_init();
  ruby_init_loadpath();
  Init_Dvector();
  VALUE cls = rb_eval_string("Dobjects::Dvector");
  VALUE res;
  
  res = rb_eval_string("Dobjects::Dvector.fast_fancy_read"
		       "(File.open('tmp.dat', 'r'), "
		       "Dobjects::Dvector::FANCY_READ_DEFAULTS)");
  return 0;
}
