# Function installation file
require 'mkmf'

# We add include directories
$INCFLAGS += " -I../../includes -I../Dvector/include"

create_makefile 'Dobjects/Function'
