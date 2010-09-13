/* flate.c -- main file for Ruby extension
 */

/* Internal files */
#include "symbols.h"
#include "symbols.c"
#include "flate_intern.h"

#include <defs.h>
/* End of internal include files */

#include "zlib.h"

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

PRIVATE
/* 
 *  call-seq:
 *     Flate.compress(str)  ->  string
 *
 *  Returns a compressed verion of _str_ in a new string.
 */
VALUE do_compress(VALUE klass, VALUE str) {
   str = rb_String(str);
   unsigned char *ptr = (unsigned char *)RSTRING_PTR(str);
   long len = RSTRING_LEN(str);
   unsigned long new_len = (len * 11) / 10 + 100;
   unsigned char *new_ptr = ALLOC_N(unsigned char, new_len);
   if (flate_compress(new_ptr, &new_len, ptr, len) != Z_OK) {
      free(new_ptr);
      rb_raise(rb_eArgError, "Error in Flate.compress");
   }
   VALUE new_str = rb_str_new((char *)new_ptr, new_len);
   free(new_ptr);
   return new_str;
}

PRIVATE int flate_compress(unsigned char *new_ptr, unsigned long *new_len_ptr, unsigned char *ptr, long len) {
   return compress(new_ptr, new_len_ptr, ptr, len); // updates new_len to the actual length
}

PRIVATE  
/* 
 *  call-seq:
 *     Flate.expand(str)  ->  string
 *
 *  Returns a decompressed verion of _str_ in a new string.
 *  Assumes that _str_ was compressed using <code>Flate.compress</code>.
 *  
 */
 
VALUE do_expand(VALUE klass, VALUE str) {
   str = rb_String(str);
   unsigned char *ptr = (unsigned char *)RSTRING_PTR(str);
   long len = RSTRING_LEN(str);
   unsigned long new_len = len * 4 + 100;
   unsigned char *new_ptr = ALLOC_N(unsigned char, new_len);
   if (flate_expand(&new_ptr, &new_len, ptr, len) != Z_OK) {
      free(new_ptr);
      rb_raise(rb_eArgError, "Error in Flate.expand");
   }
   VALUE new_str = rb_str_new((char *)new_ptr, new_len);
   free(new_ptr);  
   return new_str;
}


// NOTE: the destination buffer will be reallocated if it isn't large enough.
// So you MUST allocate a buffer rather than using a static one.
// And you MUST be prepared for the buffer to change location and size.
PRIVATE int flate_expand(unsigned char **new_ptr_ptr, unsigned long *new_len_ptr, unsigned char *ptr, long len) {
   unsigned char *new_ptr = *new_ptr_ptr;
   int result = Z_MEM_ERROR;
   while (*new_len_ptr < 9999999) {
      result = uncompress(new_ptr, new_len_ptr, ptr, len); // updates new_len
      if (result != Z_MEM_ERROR && result != Z_BUF_ERROR) break;
      // new_len not big enough
      *new_len_ptr *= 2;
      if (*new_len_ptr >= 9999999) break;
      free(new_ptr);
      new_ptr = ALLOC_N(unsigned char, *new_len_ptr);
   }
   *new_ptr_ptr = new_ptr;
   return result;
}
 
/* 
 * Document-module: Flate
 *
 * The Flate module implements an efficient lossless compression/decompression algorithm suitable for text and data.
 * It is a Ruby-wrapper around code from the 'zlib' compression library, written by
 * Jean-loup Gailly and Mark Adler, who kindly grant permission for free use of their work (visit {the zlib site}[http://www.gzip.org/zlib/]).  The algorithm for flate 
 * is based on huffman encoding and LZ77 compression.  For an introduction to the details, see the {description by
 * Gailly and Adler}[link:algorithm.txt].  Compression to 1/3 original size
 * is not unusual for text files, which explains the popularity of this algorithm and its use in
 * applications such as 'Zip' and 'gzip', and in PDF files where it is used for lossless compression of text
 * along with JPEG for lossy compression of images.
 * Note that short strings, fewer than
 * 100 bytes or so, may actually 'compress' to a larger string due to the overhead of compression tables.
 */
 
PUBLIC void Init_Flate() {
   /* modified by Vincent Fourmond, for the splitting out */
   /*VALUE mTioga = rb_define_module("Tioga"); 
     VALUE mFlate = rb_define_module_under(mTioga, "Flate"); */
   VALUE mFlate = rb_define_module("Flate");
   rb_define_singleton_method(mFlate, "compress", do_compress, 1);
   rb_define_singleton_method(mFlate, "expand", do_expand, 1);

   /* exporting the symbols that might be needed by other modules */
   RB_EXPORT_SYMBOL(mFlate, flate_expand);
   RB_EXPORT_SYMBOL(mFlate, flate_compress);
}

