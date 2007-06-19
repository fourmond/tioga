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


/* Ruby definitions for the generic interpreter interface */

// if ruby, OBJ_PTR is VALUE
// if python, OBJ_PTR is PyObject*
#define OBJ_PTR VALUE

extern double Number_to_double(OBJ_PTR obj);
    // returns a C double
    // raises error if obj not a kind of number
extern long Number_to_int(OBJ_PTR obj);
    // returns a C int
    // raises error if obj not a kind of integer

extern OBJ_PTR Integer_New(long val);
    // returns a new integer object with given val
extern OBJ_PTR Float_New(double val);
    // returns a new float object with given val

extern OBJ_PTR String_New(char *src, long len);
    // returns a new string object initialized with len chars from src
extern OBJ_PTR String_From_Cstring(char *src);
    // returns a new string object initialized from null-terminated Cstring src


extern char *String_Ptr(OBJ_PTR obj);
    // returns pointer to storage buffer for string obj
    // tries to convert obj to string if necessary
    // raises error if obj not a kind of string and cannot be converted to one

extern long String_Len(OBJ_PTR obj);
    // returns int length of contents of string obj
    // tries to convert obj to string if necessary
    // raises error if obj not a kind of string and cannot be converted to one

extern OBJ_PTR Array_New(long len);
    // returns a new array object of length len
extern long Array_Len(OBJ_PTR obj);
    // returns length of the array obj
    // tries to convert obj to array if necessary
    // raises error if obj not a kind of array and cannot be converted to one
extern OBJ_PTR Array_Entry(OBJ_PTR obj, long indx);
    // returns array obj entry indx
    // tries to convert obj to array if necessary
    // raises error if obj not a kind of array and cannot be converted to one
extern OBJ_PTR Array_Store(OBJ_PTR obj, long indx, OBJ_PTR val);
    // sets array obj entry indx to val and then returns val.
    // raises error if obj not a kind of array
extern OBJ_PTR Array_Push(OBJ_PTR obj, OBJ_PTR val);
    // pushes val onto the end of array obj and then returns val.
    // raises error if obj not a kind of array


// if ruby, ID_PTR is ID
// if python, ID_PTR is PyObject*  ???
#define ID_PTR ID

extern ID_PTR ID_Get(char *name);
    // returns internal ID for the given name
extern OBJ_PTR Obj_Attr_Get(OBJ_PTR obj, ID_PTR attr_ID);
    // returns value of the given attr of the obj
extern OBJ_PTR Obj_Attr_Set(OBJ_PTR obj, ID_PTR attr_ID, OBJ_PTR val);
    // sets the specified attr of the obj to val, and then returns val.
    
//#define Obj_Attr_Get_by_StringName(obj,attr_name_string) rb_iv_get(obj,attr_name_string)
    // returns value of the given attr of the obj (name_string is char *)
//#define Obj_Attr_Set_by_StringName(obj,attr_name_string,val) rb_iv_set(obj,attr_name_string,val)
    // sets the specified attr of the obj to val (name_string is char *)

extern void RAISE_ERROR(char *str);
// The following do sprintf to a local string, and then call RAISE_ERROR.
extern void RAISE_ERROR_s(char *fmt, char *s);
extern void RAISE_ERROR_ss(char *fmt, char *s1, char *s2);
extern void RAISE_ERROR_i(char *fmt, int x);
extern void RAISE_ERROR_ii(char *fmt, int x1, int x2);
extern void RAISE_ERROR_g(char *fmt, double x);
extern void RAISE_ERROR_gg(char *fmt, double x1, double x2);


/* generic interface for vectors and tables */

extern OBJ_PTR Vector_New(long len, double *vals);
    // creates a 1D vector and initializes it with given values
    // returns OBJ_PTR for the new vector of doubles

extern OBJ_PTR Integer_Vector_New(long len, long *vals);
    // creates a 1D vector and initializes it with given values
    // returns OBJ_PTR for the new vector of integers

extern double *Vector_Data_for_Read(OBJ_PTR obj, long *len_ptr);
    // returns (double *) pointer to data (read access only)
    // also returns length of vector via len_ptr

extern double **Table_Data_for_Read(OBJ_PTR tbl, long *num_col_ptr, long *num_row_ptr);
    // returns (double **) pointer to data (read access only)
    // also returns number of cols and rows via num_col_ptr and num_row_ptr
    
    
    
// the following will go away
#define Vector_Data_for_Write(obj,len_ptr) Dvector_Data_for_Write(obj,len_ptr) 
    //returns double * after ensuring that is okay to write into it
#define Vector_Data_Resize(obj,new_len) Dvector_Data_Resize(obj,new_len) 
    //returns double * after resizing and making okay to write
#define Vector_Store(ary,indx,val) Dvector_Store_Double(ary,indx,val)
    // VALUE ary, long indx, double val
    // sets vector object ary entry indx to double val
    // raises error if ary not a kind of vector


#endif   /* __generic_H__ */
