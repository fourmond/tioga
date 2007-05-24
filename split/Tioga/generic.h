/* generic.h */
/*
   Copyright (C) 2007  Bill Paxton

   This file is part of Tioga.

   Tioga is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Library Public License as published
   by the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.

   Tioga is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with Tioga; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
*/

#ifndef __generic_H__
#define __generic_H__


/* most of the c code should use this interface 
   rather than directly depending on ruby interfaces */

/* similarly, use routines from this interface
   instead of directly using Dvector or Dtable */


/* generic interpreter interface */

#define String_New(src,len) rb_str_new(src,len)
    // returns a new string object initialized with len chars from src
#define String_From_Cstring(src) rb_str_new2(src)
    // returns a new string object initialized from null-terminated Cstring src
#define String_Ptr(obj) RSTRING_PTR(rb_String(obj))
    // returns char * pointer to storage buffer for string obj
    // tries to convert obj to string if necessary
    // raises error if obj not a kind of string and cannot be converted to one
#define String_Len(obj) RSTRING_LEN(rb_String(obj))
    // returns int length of contents of string obj
    // tries to convert obj to string if necessary
    // raises error if obj not a kind of string and cannot be converted to one

#define Array_New(len) rb_ary_new2(len)
    // returns a new array object of length len
#define Array_Len(obj) (RARRAY(rb_Array(obj))->len)
    // returns length of the array obj
    // tries to convert obj to array if necessary
    // raises error if obj not a kind of array and cannot be converted to one
#define Array_Entry(obj,indx) rb_ary_entry(obj,indx)
    // returns array obj entry indx
    // tries to convert obj to array if necessary
    // raises error if obj not a kind of array and cannot be converted to one
#define Array_Store(obj,indx,val) rb_ary_store(obj,indx,val)
    // sets array obj entry indx to val
    // raises error if obj not a kind of array
#define Array_Push(obj,val) rb_ary_push(obj,val)
    // pushes val onto the end of array obj
    // raises error if obj not a kind of array

#define Float_New(val) rb_float_new(val)
    // returns a new float object with given val

#define ID_Get(name) rb_intern(name)
    // returns internal ID for the given name
#define Obj_Attr_Get(obj,attr_ID) rb_ivar_get(obj,attr_ID)
    // returns value of the given attr of the obj
#define Obj_Attr_Set(obj,attr_ID,val) rb_ivar_set(obj,attr_ID,val)
    // sets the specified attr of the obj to val
//#define Obj_Attr_Get_by_StringName(obj,attr_name_string) rb_iv_get(obj,attr_name_string)
    // returns value of the given attr of the obj (name_string is char *)
//#define Obj_Attr_Set_by_StringName(obj,attr_name_string,val) rb_iv_set(obj,attr_name_string,val)
    // sets the specified attr of the obj to val (name_string is char *)

#define RAISE_ERROR(str) rb_raise(rb_eArgError, (str))
extern void RAISE_ERROR_s(char *fmt, char *s);
extern void RAISE_ERROR_ss(char *fmt, char *s1, char *s2);
extern void RAISE_ERROR_i(char *fmt, int x);
extern void RAISE_ERROR_ii(char *fmt, int x1, int x2);
extern void RAISE_ERROR_g(char *fmt, double x);
extern void RAISE_ERROR_gg(char *fmt, double x1, double x2);


/* generic double array interface */

#define Vector_Data_for_Read(obj,len_ptr) Dvector_Data_for_Read(obj,len_ptr) 
    //returns double *
#define Vector_Data_for_Write(obj,len_ptr) Dvector_Data_for_Write(obj,len_ptr) 
    //returns double * after ensuring that is okay to write into it
#define Vector_Data_Resize(obj,new_len) Dvector_Data_Resize(obj,new_len) 
    //returns double * after resizing and making okay to write
#define Vector_Store(ary,indx,val) Dvector_Store_Double(ary,indx,val)
    // VALUE ary, long indx, double val
    // sets vector object ary entry indx to double val
    // raises error if ary not a kind of vector

#define Table_Info(tbl,num_col_ptr,num_row_ptr) Dtable_Ptr(tbl,num_col_ptr,num_row_ptr)
    // VALUE tbl, long *num_col_ptr, long *num_row_ptr
    // returns double ** for data stored in table
    // sets number of cols and rows 


#endif   /* __generic_H__ */
