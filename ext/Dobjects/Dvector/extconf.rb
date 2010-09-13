# Dtable installation file
require 'mkmf'

# Conditional use of fftw3
if have_header("fftw3.h") and have_library("fftw3", "fftw_execute", "fftw3.h")
  puts "fftw3 was found on this system: Fourier transforms will be available"
else
  puts "fftw3 was not found on this system: no Fourier transforms"
end

# We add include directories
$INCFLAGS += " -I../../includes"


create_makefile 'Dobjects/Dvector'
