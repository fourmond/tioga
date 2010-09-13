/**********************************************************************

   defs.h: 
   some definitions that are used everywhere and that depend on some
   HAVE_* macros.
   
   Copyright (C) 2006  Vincent Fourmond

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
   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
   
**********************************************************************/


#ifdef HAVE_ISNAN
/* we use isnan and isinf, which are much faster than the workaround */
#define is_okay_number(x) (! isnan(x) && ! isinf(x))
#else
#define is_okay_number(x) ((x) - (x) == 0.0)
#define isnan(x) ((x) != (x))
/* yes, as funny as it may look  NaN != NaN, and that's the only one */
#endif

/* Fix old versions of ruby.h */
#ifndef RSTRING_PTR
# define RSTRING_PTR(x) (RSTRING(x)->ptr)
# define RSTRING_LEN(x) (RSTRING(x)->len)
#endif
