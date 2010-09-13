/* dtable.h */
/*
   Copyright (C) 2004  Bill Paxton

   This file is part of Dtable.

   Dtable is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Library Public License as published
   by the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.

   Dtable is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with Dtable; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
*/

#ifndef __dtable_H__
#define __dtable_H__

#include <symbols.h>

/*======================================================================*/


DECLARE_SYMBOL(VALUE, Read_Dtable, 
	       (VALUE dest, char *filename, int skip_lines));
DECLARE_SYMBOL(double **, Dtable_Ptr, 
	       (VALUE dtable, long *num_cols, long *num_rows));

#endif
