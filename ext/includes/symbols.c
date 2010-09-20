/*
   Copyright (C) 2006 Vincent Fourmond

   Symbols is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Library Public License as published
   by the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.

   Symbols is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with Dvector; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
*/


/* Simple code for sharing C symbols across different Ruby libraries */

#include <ruby.h>

#include <namespace.h>


/* MV stands for Module Variable */
#define MV_SYMBOLS "@_exported_C_symbols"
/* modified to use instance variables instead of global class variables:
   this way, children don't overwrite the parent's export table
*/


/* makes sure that the hash is registered for the given
   module and returns it */
static VALUE get_symbol_hash(VALUE module)
{
  VALUE hash;
  ID mv_id = rb_intern(MV_SYMBOLS);
  if(RTEST(rb_ivar_defined(module, mv_id)))
    return rb_ivar_get(module, mv_id);
  else
    {
      /* module variable uninitialized, we need to make sure it's here */
      hash = rb_hash_new();
      rb_ivar_set(module, mv_id, hash);
      return hash;
    }
}

/* registers a symbol in the given module. This one is the internal
   function */
PRIVATE void rb_export_symbol(VALUE module, const char * symbol_name,
			    void * symbol)
{
  VALUE hash = get_symbol_hash(module);
  rb_hash_aset(hash, rb_str_new2(symbol_name),LONG2NUM((long) symbol));
}

PRIVATE void * rb_import_symbol_no_raise(VALUE module, 
					 const char * symbol_name)
{
  VALUE hash = rb_iv_get(module, MV_SYMBOLS);
  VALUE symbol = rb_hash_aref(hash, rb_str_new2(symbol_name));
  
  if(TYPE(symbol) == T_FIXNUM || TYPE(symbol) == T_BIGNUM)
    return (void *) NUM2LONG(symbol);
  return NULL;
}

/* same as before, but raises something is the return value is NULL,
   which is probably best as a default behavior*/
PRIVATE void * rb_import_symbol(VALUE module, const char * symbol_name)
{
  void * symbol = rb_import_symbol_no_raise(module, symbol_name);
  if(symbol)
    return symbol;
  /* we get the name of the module: */
  VALUE module_name = rb_funcall(module, rb_intern("to_s"), 0);
  rb_raise(rb_eRuntimeError, "The symbol %s was not found in "
	   "module %s", symbol_name, 
	   rb_string_value_cstr(&module_name));
}
