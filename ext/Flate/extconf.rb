# Flate installation file
require 'mkmf'

if have_header("zlib.h") and have_library("z", "compress", "zlib.h")
  puts "Using the system zlib library"
else
  require 'fileutils'
  puts "Using the private copy of zlib: copying the files from zlib/"
  files = Dir["zlib/*.[ch]"]
  for f in files do
    target = File::basename(f)
    begin
      FileUtils::symlink(f, target)
    rescue NotImplementedError => e  # For platforms when that isn't implemented
      FileUtils::copy(f, target)
    end
    $distcleanfiles << target
  end
end

# We add include directories
$INCFLAGS += " -I../includes"

create_makefile 'Flate'
