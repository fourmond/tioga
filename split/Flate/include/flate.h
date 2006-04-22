/* flate.h -- c interface for the Ruby Flate extension
*/

/*
   Copyright (C) 2005  Bill Paxton

   Flate is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Library Public License as published
   by the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.

   Flate is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with Flate; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
*/


/* This file has been heavily modified by Vincent Fourmond for the 'RCR330'
   symbol export scheme 
*/
#ifndef __flate_H__
#define __flate_H__

#include <symbols.h>

DECLARE_SYMBOL(int, flate_compress, 
	       (unsigned char *new_ptr, unsigned long *new_len_ptr, 
		unsigned char *ptr, long len));
// source is given by ptr and is len bytes in length.
// new_ptr is destination buffer of size *new_len_ptr.  
// NOTE: the destination buffer for flate_compress should be LARGER than the source buffer to be safe.
// The minimal extra is 0.1% larger than the source plus 12 bytes.
// My rule is to use (len * 11)/10 + 100 just to be sure.
DECLARE_SYMBOL(int, flate_expand,
	       (unsigned char **new_ptr_ptr, unsigned long *new_len_ptr, 
		unsigned char *ptr, long len));
// source is given by ptr and is len bytes in length.
// *new_ptr_ptr is destination buffer of size *new_len_ptr.  
// NOTE: the destination buffer for flate_expand will be reallocated if it isn't large enough.
// So you MUST allocate a buffer rather than using a static one.
// And you MUST be prepared for the buffer to change location and size.

// Here are the return codes for the compression/expansion functions. Negative
// values are errors, positive values are used for special but normal events.

#define FLATE_OK              0
#define FLATE_STREAM_END      1
#define FLATE_NEED_DICT       2
#define FLATE_ERRNO         (-1)
#define FLATE_STREAM_ERROR  (-2)
#define FLATE_DATA_ERROR    (-3)
#define FLATE_MEM_ERROR     (-4)
#define FLATE_BUF_ERROR     (-5)
#define FLATE_VERSION_ERROR (-6)

// If this simple interface isn't enough for your needs, the next stop is zlib.h
// for the full interface.

// The internals of this package come from the 'zlib' compression library which
// carries the following copyright:

/* 'zlib' general purpose compression library
  version 1.2.2, October 3rd, 2004

  Copyright (C) 1995-2004 Jean-loup Gailly and Mark Adler

  This software is provided 'as-is', without any express or implied
  warranty.  In no event will the authors be held liable for any damages
  arising from the use of this software.

  Permission is granted to anyone to use this software for any purpose,
  including commercial applications, and to alter it and redistribute it
  freely, subject to the following restrictions:

  1. The origin of this software must not be misrepresented; you must not
     claim that you wrote the original software. If you use this software
     in a product, an acknowledgment in the product documentation would be
     appreciated but is not required.
  2. Altered source versions must be plainly marked as such, and must not be
     misrepresented as being the original software.
  3. This notice may not be removed or altered from any source distribution.

  Jean-loup Gailly        Mark Adler
  jloup@gzip.org          madler@alumni.caltech.edu


  The data format used by the zlib library is described by RFCs (Request for
  Comments) 1950 to 1952 in the files ftp://ds.internic.net/rfc/rfc1950.txt
  (zlib format), rfc1951.txt (deflate format) and rfc1952.txt (gzip format).
*/


#endif   /* __flate_H__ */
