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

/* This header file provides three OS-specific macros for the definition of
   extern (or NOT extern) symbols:

   * PUBLIC, which has to be used to mark objects that will be used
     outside the module

   * INTERN, for symbols which are shared among compilation units within a
     module, but are NOT exported to other modules.

   * PRIVATE, for symbols which are visible only within the containing
     compilation unit (i.e. C source file) and, of course, are not exported to
     other modules.

   Please don't add "extern" after the PRIVATE or PUBLIC declaration
   as this would break compilation on Darwin.
*/

#if __GNUC__ >= 4 /* we have the visibility attribute */
# define INTERN __attribute__ ((visibility ("hidden")))
# define PUBLIC __attribute__ ((visibility ("default"))) 
# define INTERN_EXTERN extern
#elif defined __APPLE__
# define INTERN __private_extern__
# define PUBLIC 
# define INTERN_EXTERN
#else /* not really good */
# define INTERN
# define PUBLIC 
# define INTERN_EXTERN
#endif /* __APPLE__  and __GNU_C_ >= 4*/

/* In any case, PRIVATE is static */

#define PRIVATE static

#endif
