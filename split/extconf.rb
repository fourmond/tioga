# main Tioga installation file

require 'mkmf2'

# install Dvector include and library files into base dirs,
# and builds Dvector.so
setup_dir("Dvector", "Dobjects", "Dobjects/Dvector", "")
# the same for Dtable
setup_dir("Dtable", "Dobjects", "Dobjects/Dtable", "")

setup_dir("Flate", "", "Flate", "")

setup_dir("Tioga", "Tioga", "Tioga/FigureMaker", "")

have_header("zlib.h")
have_library("z", "compress", "zlib.h")

write_makefile



