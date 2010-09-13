# Dtable installation file
require 'mkmf'

# We add include files:
$CFLAGS += "-I../../includes"
create_makefile 'Dobjects/Dvector'
