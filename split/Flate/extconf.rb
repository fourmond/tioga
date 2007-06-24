# Flate installation file
require 'mkmf'

unless have_header("zlib.h") and have_library("z", "compress", "zlib.h")
  puts <<"EON"
Error: you should have zlib (including development files) installed to
build and run Tioga. You can get it there:

  http://www.zlib.net/

If that doesn't solve your problem, please report it on the Tioga tracker:
 
  http://rubyforge.org/tracker/?group_id=701

EON
  exit 1 
end

create_makefile 'Flate'
