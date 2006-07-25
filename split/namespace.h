/* namespace.h: an attempt at rationalizing shared objects
   namespace use.
  
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
*/

#ifndef _NAMESPACE_H
#define _NAMESPACE_H

/* This header file provides two OS-specific macros for the definition of
   extern symbols:

   * PUBLIC, which has to be used to mark objects that will be used
   outside the module

   * PRIVATE, for symbols which are "extern" but intern to the module

   Please don't add "extern" after the PRIVATE or PUBLIC declaration
   as this would break compilation on Darwin.
*/

#ifdef __APPLE__
# define PRIVATE __private_extern__
# define PUBLIC 
#elif __GNUC__ >= 4 /* we have the visibility attribute */
# define PRIVATE __attribute__ ((visibility ("hidden"))) 
# define PUBLIC __attribute__ ((visibility ("default"))) 
#else /* not really good */
# define PRIVATE 
# define PUBLIC 
#endif /* __APPLE__  and __GNU_C_ >= 4*/

#endif
