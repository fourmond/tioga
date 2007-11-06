/* 
   prepare_file.c: prepare a tmp.dat file with a given number of data lines

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

#include <stdlib.h>
#include <stdio.h>

int main(int argc, char * argv[])
{
  int i, imax = atoi(argv[1]);
  FILE * tmp = fopen("tmp.dat", "w");
  for(i = 0; i < imax; i++) 
    fprintf(tmp, "%g\t%g\n", drand48(), drand48());
  fclose(tmp);
}
