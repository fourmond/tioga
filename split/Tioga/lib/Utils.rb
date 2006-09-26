#  Utils.rb

=begin
   Copyright (C) 2006  Vincent Fourmond

   This file is part of Tioga.

   Tioga is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Library Public License as published
   by the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.

   Tioga is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with Tioga; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
=end

# This file is meant to receive useful functions not strictly speaking
# part of the graphical library, but that can come in handy for it.


module Tioga
  
  # The Utils module contains some useful functions that can help Tioga
  # users. They are not strictly speaking part of the graphical library,
  # but provides functionnalities Tioga users will certainly need at
  # some point.
  module Utils

    # This function returns a string that is suitable for inclusion in
    # a TeX document: all nasty characters are escaped properly. It will
    # not work however if you redefine TeX character classes, but then
    # you should know what you're doing.
    def tex_quote_text(text)
      a = text.gsub("\\", "\\BS")
      a.gsub!(/([{}$%#_^~])/) do 
        "\\#{$1}"
      end
      return a
    end

    # Dimension conversion constants taken straight from the TeXbook
    DIMENSION_CONVERSION = {
      "pt" => (72.0/72.27),
      "bp" => 1.0,
      "in" => 72.0,
      "cm" => (72.0/2.54),
      "mm" => (72.0/25.4),
    }

    # Returns the value of the given TeX dimension in postscript points.
    def tex_dimension_to_bp(dim)
      for unit, val in DIMENSION_CONVERSION
        if dim =~ /^\s*([\d.]+)\s*#{unit}$/
          return $1.to_f * val
        end
      end
      # We take it to be centimeters by default ????
      if dim =~ /^\s*([\d.]+)\s*$/
        warn "tex_dimension_to_bp: No dimension was specified, " +
          "using centimeters"
        return $1.to_f * DIMENSION_CONVERSION["cm"] 
      end
      raise "'#{dim}' is not a valid TeX dimension"
    end


  end
end
