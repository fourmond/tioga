/**********************************************************************

   safe_double.h: small abstraction for storing double without endianess
   problems
   
   Copyright (C) 2006 Vincent Fourmond

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
   
**********************************************************************/

/* This file provides two functions: store_double and get_double, which
   can be used in a 'safe' way to store doubles  and retrive them
   in a hopefully platform-independent form. However, it just 
   stores it without regards to that if ieee754.h isn't found...
*/

#ifndef _DOUBLE_H
#define _DOUBLE_H

#define STORE_LOWER_BYTE(a,p) do {\
*(p++) = (a) & 0xFF; (a) >>= 8; }\
while(0)
#define STORE_UNSIGNED(a,p)   for(i = 0; i < 4; i++) STORE_LOWER_BYTE(a,p);
#define GET_UNSIGNED(a,p) do { a = 0; for(i = 0; i < 4; i++) \
(a) |= *(p++) << (i * 8); } while (0)

#ifdef HAVE_IEEE754_H
#include <ieee754.h>


static inline void store_double(double a, unsigned char * p)
{
  unsigned int tmp;
  int i;
  union ieee754_double d;
  d.d = a;
  /* we store it with lower bytes firts */
  tmp = d.ieee.mantissa1;
  STORE_UNSIGNED(tmp, p);
  tmp = d.ieee.negative << 31 |
    d.ieee.exponent << 20 | 
    d.ieee.mantissa0;
  STORE_UNSIGNED(tmp, p);
}


static inline double get_double(const unsigned char * p)
{
  unsigned int tmp;
  int i;
  union ieee754_double d;
  GET_UNSIGNED(tmp, p);
  d.ieee.mantissa1 = tmp;
  GET_UNSIGNED(tmp, p);
  d.ieee.mantissa0 = tmp & 0xFFFFF;
  d.ieee.exponent = (tmp >> 20) & 0x7FF;
  d.ieee.negative = (tmp >> 31) & 0x1;
  return d.d;
}

#else

union basic_double{
  double d;
  struct {
    unsigned int a:32;
    unsigned int b:32;
  } i;
};

static inline void store_double(double a, unsigned char * p)
{
  union basic_double d;
  int i;
  d.d = a;
  STORE_UNSIGNED(d.i.a, p);
  STORE_UNSIGNED(d.i.b, p);
}

static inline double get_double(const unsigned char * p)
{
  union basic_double d;
  int i;
  GET_UNSIGNED(d.i.a, p);
  GET_UNSIGNED(d.i.b, p);
  return d.d;
}

#endif /* HAVE_IEEE754_H */

#endif /* _DOUBLE_H */
