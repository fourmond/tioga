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
#include "generic.h"
#include "figures.h"
#include "pdfs.h"
#include "flate.h"



static OBJ_PTR rb_Integer_class, rb_Numeric_class;

void Init_generic(void) {
   rb_Numeric_class = rb_define_class("Numeric", rb_cObject);
   rb_Integer_class = rb_define_class("Integer", rb_Numeric_class);
}

bool Is_Kind_of_Integer(OBJ_PTR obj) { return rb_obj_is_kind_of(obj,rb_Integer_class); }
bool Is_Kind_of_Number(OBJ_PTR obj) { return rb_obj_is_kind_of(obj,rb_Numeric_class); }

void Call_Function(OBJ_PTR fmkr, ID_PTR fn, OBJ_PTR arg, int *ierr) {
   rb_funcall(fmkr, fn, 1, arg);
}

double Number_to_double(OBJ_PTR obj, int *ierr) { return NUM2DBL(obj); }
    
long Number_to_int(OBJ_PTR obj, int *ierr) { return NUM2INT(obj); }

OBJ_PTR Integer_New(long val) { return INT2FIX(val); }

OBJ_PTR Float_New(double val) { return rb_float_new(val); }

OBJ_PTR String_New(char *src, long len) { return rb_str_new(src,len); }

OBJ_PTR String_From_Cstring(char *src) { return rb_str_new2(src); }

char *String_Ptr(OBJ_PTR obj, int *ierr) {
   VALUE str = rb_String(obj);
   return StringValuePtr(str); }

long String_Len(OBJ_PTR obj, int *ierr) {
   VALUE str = rb_String(obj);
   return RSTRING_LEN(str); }

char *CString_Ptr(OBJ_PTR obj, int *ierr) {
   VALUE str = rb_String(obj);
   char *cs = StringValuePtr(str);
   long len = RSTRING_LEN(str);
   if (len != strlen(cs)) { RAISE_ERROR("invalid C string; contains NULL character",ierr); return NULL; }
   return cs; }

OBJ_PTR Array_New(long len) { return rb_ary_new2(len); }

long Array_Len(OBJ_PTR obj, int *ierr) { 
   return isa_Dvector(obj)? len_Dvector(obj) : (RARRAY_LEN(rb_Array(obj))); }

OBJ_PTR Array_Entry(OBJ_PTR obj, long indx, int *ierr) { return rb_ary_entry(obj,indx); }

void Array_Store(OBJ_PTR obj, long indx, OBJ_PTR val, int *ierr) { rb_ary_store(obj,indx,val); }

void Array_Push(OBJ_PTR obj, OBJ_PTR val, int *ierr) { rb_ary_push(obj,val); }

double Array_Entry_double(OBJ_PTR obj, long indx, int *ierr) {
   return isa_Dvector(obj)? access_Dvector(obj,indx) : Number_to_double(rb_ary_entry(obj,indx),ierr);
}

long Array_Entry_int(OBJ_PTR obj, long indx, int *ierr) {
   return isa_Dvector(obj)? round(access_Dvector(obj,indx)) : Number_to_int(rb_ary_entry(obj,indx),ierr);
}



ID_PTR ID_Get(char *name) { return rb_intern(name); }

char *ID_Name(ID_PTR id, int *ierr) { return rb_id2name(id); }

OBJ_PTR Obj_Attr_Get(OBJ_PTR obj, ID_PTR attr_ID, int *ierr) { return rb_ivar_get(obj,attr_ID); }

void Obj_Attr_Set(OBJ_PTR obj, ID_PTR attr_ID, OBJ_PTR val, int *ierr) { rb_ivar_set(obj,attr_ID,val); }
      
      
      
OBJ_PTR TEX_PREAMBLE(OBJ_PTR fmkr, int *ierr) { return rb_const_get(CLASS_OF(fmkr),ID_Get("TEX_PREAMBLE")); }
      
OBJ_PTR COLOR_PREAMBLE(OBJ_PTR fmkr, int *ierr) { return rb_const_get(CLASS_OF(fmkr),ID_Get("COLOR_PREAMBLE")); }


//#define Obj_Attr_Get_by_StringName(obj,attr_name_string) rb_iv_get(obj,attr_name_string)
    // returns value of the given attr of the obj (name_string is char *)
//#define Obj_Attr_Set_by_StringName(obj,attr_name_string,val) rb_iv_set(obj,attr_name_string,val)
    // sets the specified attr of the obj to val (name_string is char *)

void GIVE_WARNING(const char *fmt, const char *str) { rb_warn(fmt,str); }

void RAISE_ERROR(char *str, int *ierr) { *ierr = -1; rb_raise(rb_eArgError,"%s", str); }

#define err_buff_len 256
void RAISE_ERROR_s(char *fmt, char *s, int *ierr) {
   char buff[err_buff_len];
   snprintf(buff,sizeof(buff),fmt,s);
   RAISE_ERROR(buff,ierr);
}

void RAISE_ERROR_ss(char *fmt, char *s1, char *s2, int *ierr) {
   char buff[err_buff_len];
   snprintf(buff,sizeof(buff),fmt,s1,s2);
   RAISE_ERROR(buff,ierr);
}

void RAISE_ERROR_i(char *fmt, int x, int *ierr) {
   char buff[err_buff_len];
   snprintf(buff,sizeof(buff),fmt,x);
   RAISE_ERROR(buff,ierr);
}

void RAISE_ERROR_ii(char *fmt, int x1, int x2, int *ierr) {
   char buff[err_buff_len];
   snprintf(buff,sizeof(buff),fmt,x1,x2);
   RAISE_ERROR(buff,ierr);
}

void RAISE_ERROR_g(char *fmt, double x, int *ierr) {
   char buff[err_buff_len];
   snprintf(buff,sizeof(buff),fmt,x);
   RAISE_ERROR(buff,ierr);
}

void RAISE_ERROR_gg(char *fmt, double x1, double x2, int *ierr) {
   char buff[err_buff_len];
   snprintf(buff,sizeof(buff),fmt,x1,x2);
   RAISE_ERROR(buff,ierr);
}


/* generic interface for alloc */

char *ALLOC_N_char(long len) { return ALLOC_N(char,len); }
unsigned char *ALLOC_N_unsigned_char(long len) { return ALLOC_N(unsigned char,len); }

long *ALLOC_N_long(long len) { return ALLOC_N(long,len); }
unsigned long *ALLOC_N_unsigned_long(long len) { return ALLOC_N(unsigned long,len); }

void **ALLOC_N_pointer(long len) { return ALLOC_N(void *,len); }
bool *ALLOC_N_bool(long len) { return ALLOC_N(bool,len); }
double *ALLOC_N_double(long len) { return ALLOC_N(double,len); }

void REALLOC_long(long **ptr, long new_len) { 
   long *data_ptr = *ptr;
   REALLOC_N(data_ptr,long,new_len);
   *ptr = data_ptr;
}

void REALLOC_double(double **ptr, long new_len) { 
   double *data_ptr = *ptr;
   REALLOC_N(data_ptr,double,new_len);
   *ptr = data_ptr;
}


/* generic interface for vectors and tables */

double *Vector_Data_for_Read(OBJ_PTR obj, long *len_ptr, int *ierr) { 
   return Dvector_Data_for_Read(obj,len_ptr);
}

double **Table_Data_for_Read(OBJ_PTR tbl, long *num_col_ptr, long *num_row_ptr, int *ierr) {
   return Dtable_Ptr(tbl,num_col_ptr,num_row_ptr);
}

OBJ_PTR Vector_New(long len, double *vals) { 
   VALUE dv = Dvector_Create();
   double *data = Dvector_Data_Resize(dv,len);
   long i;
   for (i=0; i<len; i++) data[i] = vals[i];
   return dv;
}

OBJ_PTR Integer_Vector_New(long len, long *vals) {
   VALUE ar = rb_ary_new2(len);
   long i;
   for (i=0; i<len; i++) rb_ary_store(ar,i,LONG2NUM(vals[i]));
   return ar;
}



int do_flate_compress(unsigned char *new_ptr, unsigned long *new_len_ptr, unsigned char *ptr, long len) {
   return flate_compress(new_ptr, new_len_ptr, ptr, len); }

/* Hash-related functions: */

OBJ_PTR Hash_New() 
{
  return rb_hash_new();
}

OBJ_PTR Hash_Get_Obj_Obj(OBJ_PTR hash, OBJ_PTR key)
{
  return rb_hash_aref(hash, key);
}


OBJ_PTR Hash_Get_Obj(OBJ_PTR hash, const char * key)
{
  return rb_hash_aref(hash, rb_str_new2(key));
}

void Hash_Set_Obj(OBJ_PTR hash, const char * key, OBJ_PTR value)
{
  rb_hash_aset(hash, rb_str_new2(key), value);
}

void Hash_Set_Obj_Obj(OBJ_PTR hash, OBJ_PTR key, OBJ_PTR value)
{
  rb_hash_aset(hash, key, value);
}

double Hash_Get_Double(OBJ_PTR hash, const char * key)
{
  int err;
  return Number_to_double(Hash_Get_Obj(hash, key), &err);
}
    /* Same as Hash_Get_Obj, but returns a double */
void Hash_Set_Double(OBJ_PTR hash, const char * key, double value)
{
  Hash_Set_Obj(hash, key, Float_New(value));
}
void Hash_Delete(OBJ_PTR hash, const char * key)
{
  rb_hash_delete(hash, rb_str_new2(key));
}


bool Hash_Has_Key(OBJ_PTR hash, const char * key)
{
  return Hash_Has_Key_Obj(hash, rb_str_new2(key));
}

bool Hash_Has_Key_Obj(OBJ_PTR hash, OBJ_PTR key)
{
  return RTEST(rb_funcall(hash,rb_intern("has_key?"), 1, key));
}

