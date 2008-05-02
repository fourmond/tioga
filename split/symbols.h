#ifndef _SYMBOLS_H
#define _SYMBOLS_H

#include <ruby.h>
/* A small layer for exporting and importing symbols from
   a compiled module */

#include <namespace.h>


PRIVATE void rb_export_symbol(VALUE module, const char * symbol_name,
		      void * symbol);
PRIVATE void * rb_import_symbol(VALUE module, const char * symbol_name);
PRIVATE void * rb_import_symbol_no_raise(VALUE module, 
						const char * symbol_name);

/* a shortcut for exporting something with the same name */
#define RB_EXPORT_SYMBOL(module, name) \
  rb_export_symbol(module, #name, name)

/* A shortcut for getting a symbol */
		   
#define DECLARE_SYMBOL(ret_type,name,args) \
  typedef ret_type (*rb_export_##name##_type) args;\
  INTERN INTERN_EXTERN rb_export_##name##_type name

#define IMPLEMENT_SYMBOL(name)\
  INTERN rb_export_##name##_type name = 0;

#define RB_IMPORT_SYMBOL(module, name) \
  name = (rb_export_##name##_type) rb_import_symbol(module, #name)


/* These three macro enable a simple use of function declaration: this way,
   you can type in a header file that is used by the importer:

   DECLARE_SYMBOL(int, biniou, (int, int));

   in one file of the importer code, you write 

   IMPLEMENT_SYMBOL(biniou);

   and in the Init_... function, you write:

   RB_IMPORT_SYMBOL(module, biniou);

   and you can just use biniou as if it was a function declared as
   usual in the header file
*/
#endif   
   

