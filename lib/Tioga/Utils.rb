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
    module_function :tex_quote_text

    class ::String
      def quotex
        Tioga::Utils.tex_quote_text(self)
      end      
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
    def self.tex_dimension_to_bp(dim)
      for unit, val in DIMENSION_CONVERSION
        if dim =~ /^\s*([+-]?[\d.]+)\s*#{unit}$/
          return $1.to_f * val
        end
      end
      # We take it to be centimeters by default ????
      if dim =~ /^\s*([+-]?[\d.]+)\s*$/
        warn "tex_dimension_to_bp: No dimension was specified, " +
          "using centimeters"
        return $1.to_f * DIMENSION_CONVERSION["cm"] 
      end
      raise "'#{dim}' is not a valid TeX dimension"
    end

    def tex_dimension_to_bp(dim)
      return Utils::tex_dimension_to_bp(dim)
    end


  end


  # This module is used internally by Tioga for handling the hash
  # arguments that most of the functions take
  module HashArguments

    def check_dict(dict,names,str)
        dict.each_key do |name|
            if names[name] == nil
                raise "Sorry: Invalid dictionary key for #{str} (#{name})."
            end
        end
    end
    
    def set_if_given(name, dict)
        val = dict[name]
        return if val == nil
        eval "self." + name + " = val"
    end
        
    def alt_names(dict, name1, name2)
        val = dict[name1]
        val = dict[name2] if val == nil
        return val
    end
        
    def get_if_given_else_use_default_dict(dict, name, default_dict)
        if dict != nil
            val = dict[name]
            return val if val != nil
        end
        val = default_dict[name]
        if val == nil
            raise "Sorry: failed to find value for '#{name}' in the defaults dictionary."
        end
        return val
    end
    
    def get_if_given_else_default(dict, name, default)
        return default if dict == nil
        val = dict[name]
        return val if val != nil
        return default
    end
    
    def complain_if_missing_numeric_arg(dict, name, alt_name, who_called)
        val = dict[name]
        val = dict[alt_name] if val == nil
        if val == nil
            raise "Sorry: Must supply '#{name}' in call to '#{who_called}'"
        end
        if !(val.kind_of?Numeric)
            raise "Sorry: Must supply numeric value for '#{name}' in call to '#{who_called}'"
        end
        return val
    end

    def check_pair(ary, name, who_called)
        return false if ary == nil
        if !(ary.kind_of?(Array) || ary.kind_of?(Dobjects::Dvector)) and ary.size == 2
            raise "Sorry: '#{name}' must be array [x,y] for #{who_called}."
        end
        return true
    end
    
    def get_dvec(dict, name, who_called)
        val = dict[name]
        if val == nil || !(val.kind_of? Dobjects::Dvector)
            raise "Sorry: '#{name}' must be a Dvector for '#{who_called}'"
        end
        return val
    end

  end
end
