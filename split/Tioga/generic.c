/* generic.c */
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

#include <ctype.h>
#include "dvector.h"
#include "dtable.h"
#include "ruby.h"
#include "intern.h"
#include "generic.h"


double Number_to_double(OBJ_PTR obj) { return NUM2DBL(obj); }
    
long Number_to_int(OBJ_PTR obj) { return NUM2INT(obj); }

OBJ_PTR Integer_New(long val) { return INT2FIX(val); }

OBJ_PTR Float_New(double val) { return rb_float_new(val); }

OBJ_PTR String_New(char *src, long len) { return rb_str_new(src,len); }

OBJ_PTR String_From_Cstring(char *src) { return rb_str_new2(src); }

char *String_Ptr(OBJ_PTR obj) {
   VALUE str = rb_String(obj);
   return RSTRING(str)->ptr; }

long String_Len(OBJ_PTR obj) {
   VALUE str = rb_String(obj);
   return RSTRING(str)->len; }

OBJ_PTR Array_New(long len) { return rb_ary_new2(len); }

long Array_Len(OBJ_PTR obj) { return (RARRAY(rb_Array(obj))->len); }

OBJ_PTR Array_Entry(OBJ_PTR obj, long indx) { return rb_ary_entry(obj,indx); }

OBJ_PTR Array_Store(OBJ_PTR obj, long indx, OBJ_PTR val) { rb_ary_store(obj,indx,val); return val; }

OBJ_PTR Array_Push(OBJ_PTR obj, OBJ_PTR val) { rb_ary_push(obj,val); return val; }

ID_PTR ID_Get(char *name) { return rb_intern(name); }

OBJ_PTR Obj_Attr_Get(OBJ_PTR obj, ID_PTR attr_ID) { return rb_ivar_get(obj,attr_ID); }

OBJ_PTR Obj_Attr_Set(OBJ_PTR obj, ID_PTR attr_ID, OBJ_PTR val) {
      return rb_ivar_set(obj,attr_ID,val); }

//#define Obj_Attr_Get_by_StringName(obj,attr_name_string) rb_iv_get(obj,attr_name_string)
    // returns value of the given attr of the obj (name_string is char *)
//#define Obj_Attr_Set_by_StringName(obj,attr_name_string,val) rb_iv_set(obj,attr_name_string,val)
    // sets the specified attr of the obj to val (name_string is char *)



void RAISE_ERROR(char *str) { rb_raise(rb_eArgError,str); }

#define err_buff_len 256
void RAISE_ERROR_s(char *fmt, char *s) {
   char buff[err_buff_len];
   sprintf(buff,fmt,s);
   RAISE_ERROR(buff);
}

void RAISE_ERROR_ss(char *fmt, char *s1, char *s2) {
   char buff[err_buff_len];
   sprintf(buff,fmt,s1,s2);
   RAISE_ERROR(buff);
}

void RAISE_ERROR_i(char *fmt, int x) {
   char buff[err_buff_len];
   sprintf(buff,fmt,x);
   RAISE_ERROR(buff);
}

void RAISE_ERROR_ii(char *fmt, int x1, int x2) {
   char buff[err_buff_len];
   sprintf(buff,fmt,x1,x2);
   RAISE_ERROR(buff);
}

void RAISE_ERROR_g(char *fmt, double x) {
   char buff[err_buff_len];
   sprintf(buff,fmt,x);
   RAISE_ERROR(buff);
}

void RAISE_ERROR_gg(char *fmt, double x1, double x2) {
   char buff[err_buff_len];
   sprintf(buff,fmt,x1,x2);
   RAISE_ERROR(buff);
}



/* generic interface for vectors and tables */

double *Vector_Data_for_Read(OBJ_PTR obj, long *len_ptr) { return Dvector_Data_for_Read(obj,len_ptr); }

OBJ_PTR Vector_New(long len, double *vals) { return Qnil; }

OBJ_PTR Integer_Vector_New(long len, long *vals) { return Qnil; }

double **Table_Data_for_Read(OBJ_PTR tbl, long *num_col_ptr, long *num_row_ptr) {
   return Dtable_Ptr(tbl,num_col_ptr,num_row_ptr); }
