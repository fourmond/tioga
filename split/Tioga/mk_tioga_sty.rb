# This small script makes tioga.sty and lib/TexPreamble.rb from
# tioga.sty.in

require 'date'

# We make up the color constants from the Tioga file.
require './lib/ColorConstants.rb'

# slurp up the lines from tioga.sty.in
i = File.open("tioga.sty.in")
lines = i.readlines
i.close

puts "Generating lib/TexPreamble.rb"
out = File.open("lib/TexPreamble.rb", "w")
out.print "module Tioga
  class FigureMaker
    TEX_PREAMBLE = <<'End_of_preamble'\n" + 
"\\makeatletter\n" +  
lines.join +  
"\n\\makeatother\nEnd_of_preamble\nend\nend"

out.close

date = Date::today
str_date = sprintf "%04d/%02d/%02d", date.year, date.month, date.day


puts "Generating tioga.sty"
out = File.open("../../tioga.sty", "w")
out.puts "\\ProvidesPackage{tioga}[#{str_date}]"
out.puts lines.join

out.puts
out.puts "% Color constants, generated from ColorConstants.rb"



for const in Tioga::ColorConstants.constants
  r,g,b = *Tioga::ColorConstants.const_get(const)
  color_spec = sprintf "{%0.3f,%0.3f,%0.3f}", r,g,b
  out.puts "\\definecolor{#{const}}{rgb}#{color_spec}"
end
out.close
