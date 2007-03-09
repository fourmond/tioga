# main Tioga installation file

require './mkmf2.rb'

# Now, if you want to install the include file, you need to
# set the EXTCONF_RB_INCLUDE
if ENV.key?("EXTCONF_RB_INCLUDE")
  include = ENV["EXTCONF_RB_INCLUDE"]
else
  include = nil
end

# install Dvector include and library files into base dirs,
# and builds Dvector.so
setup_dir("Dvector", "Dobjects", 
          "Dobjects/Dvector", include) do |l,b,i|
#  b.add_sources("symbols.c")
end
# the same for Dtable
setup_dir("Dtable", "Dobjects", 
          "Dobjects/Dtable", include) do |l,b,i|
#  b.add_sources("symbols.c")
end

setup_dir("Flate", "", "Flate", include) do |l,b,i|
#  b.add_sources("symbols.c")
end


setup_dir("Function", "Dobjects", 
          "Dobjects/Function", include) do |l,b,i|
#  b.add_sources("symbols.c")
end


# We declare Tioga by hand, as an automatic generation would not
# take lib/TexPreamble.rb into accound (missing).
declare_library("Tioga", 
                "Tioga/lib/tioga.rb",
                "Tioga/lib/ColorConstants.rb",
                "Tioga/lib/FigMkr.rb",
                "Tioga/lib/FigureConstants.rb",
                "Tioga/lib/MarkerConstants.rb",
                "Tioga/lib/maker.rb",
                "Tioga/lib/irb_tioga.rb",
                "Tioga/lib/tioga_ui_cmds.rb",
                "Tioga/lib/tioga_ui.rb",
                "Tioga/lib/Utils.rb",
                "Tioga/lib/TexPreamble.rb")

declare_binary_library("Tioga/FigureMaker", 
                       "Tioga/**/*.c")

# The preamble stuff:
custom_rule("Tioga/lib/TexPreamble.rb", 
            [ "cd Tioga; " + Mkmf2.config_var("RUBY_INSTALL_NAME") + 
              " mk_tioga_sty.rb"],
            ["Tioga/lib/ColorConstants.rb",
             "Tioga/tioga.sty.in", "Tioga/mk_tioga_sty.rb"]
            )

# we check the presence of zlib library
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

unless have_header("ieee754.h")
  puts "You lack the ieee754.h header file, which might mean lower " +
    "reliability when Marshalling Dvectors and Dtables"
end

# Looking for the presence of the is_nan (implies the rest -- isfinite)
have_func("isnan","math.h")

# Installing scripts
declare_exec 'scripts/tioga'
declare_exec 'scripts/irb_tioga'


if Config::CONFIG["target"] =~ /darwin/i
  # Installing MacOS specific scripts:
  # Creating the script
  puts "MacOS specific installation"
  custom_rule("repreview", 
              [ "echo '#!/bin/sh' > repreview",
                "echo \"osascript '$(EXEC_INSTALL_DIR)/Reload_Preview_Document.scpt'\" '$$*' >> repreview"
              ]) 
  declare_exec 'repreview'
  declare_exec 'scripts/Reload_Preview_Document.scpt'
else
  puts "Skipping MacOS-specific files"
end


write_makefile



