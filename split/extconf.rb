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


setup_dir("Function", "Dobjects", 
          "Dobjects/Function", "") do |l,b,i|
  b.add_sources("symbols.c")
end


# We declare Tioga by hand, as an automatic generation would not
# take lib/TexPreamble.rb into accound (missing).
declare_library("Tioga", "Tioga/lib/**/*.rb", 
                "Tioga/lib/TexPreamble.rb")
declare_binary_library("Tioga/FigureMaker", 
                       "Tioga/**/*.c", "symbols.c")

# The preamble stuff:
custom_rule("Tioga/lib/TexPreamble.rb", 
            [ "cd Tioga; " + Mkmf2.config_var("RUBY_INSTALL_NAME") + 
              " mk_tioga_sty.rb"],
            ["Tioga/lib/ColorConstants.rb",
             "Tioga/tioga.sty.in"]
            )

# we check the presence of zlib library
have_header("zlib.h")
have_library("z", "compress", "zlib.h")

write_makefile



