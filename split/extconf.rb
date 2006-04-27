# main Tioga installation file

require './mkmf2.rb'

# install Dvector include and library files into base dirs,
# and builds Dvector.so
setup_dir("Dvector", "Dobjects", 
          "Dobjects/Dvector", "") do |l,b,i|
  b.add_sources("symbols.c")
end
# the same for Dtable
setup_dir("Dtable", "Dobjects", 
          "Dobjects/Dtable", "") do |l,b,i|
  b.add_sources("symbols.c")
end
setup_dir("Flate", "", "Flate", "") do |l,b,i|
  b.add_sources("symbols.c")
end
setup_dir("Tioga", "Tioga", 
          "Tioga/FigureMaker", "") do |l,b,i|
  b.add_sources("symbols.c")
end

# we check the presence of zlib library
have_header("zlib.h")
have_library("z", "compress", "zlib.h")

write_makefile



