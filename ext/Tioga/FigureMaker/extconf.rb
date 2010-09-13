# Dtable installation file
require 'mkmf'

# We add include directories
$INCFLAGS += " -I../../includes -I../../Dobjects/Dvector/include -I../../Dobjects/Dtable/include -I../../Flate/include"

create_makefile 'Tioga/FigureMaker'
