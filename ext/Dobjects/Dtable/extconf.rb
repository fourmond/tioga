# Dtable installation file
require 'mkmf'

# We add include directories
$CFLAGS += "-I../../includes -I../Dvector/include"

create_makefile 'Dobjects/Dtable'
