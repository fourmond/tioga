#  Tioga::Dvector_extras.rb

class Numeric

    def mod(y)
        self.modulo(y)
    end

end

# Modified by Vincent Fourmond to have a nice Dvector module
# instead of the Tioga module.
# add arithmetic operators for Float and Fixnum with Dvec

class Array
    def to_dvector
        Dobjects::Dvector[*self]
    end
end

class Float
    alias :pre_dvec_add :+
    def +(a)
        if a.class == Dobjects::Dvector
            a+self
        else
            pre_dvec_add(a)
        end
    end
    alias :pre_dvec_sub :-
    def -(a)
        if a.class == Dobjects::Dvector
            a.neg+self
        else
            pre_dvec_sub(a)
        end
    end
    alias :pre_dvec_mult :*
    def *(a)
        if a.class == Dobjects::Dvector
            a*self
        else
            pre_dvec_mult(a)
        end
    end
    alias :pre_dvec_div :/
    def /(a)
        if a.class == Dobjects::Dvector
            a.inv*self
        else
            pre_dvec_div(a)
        end
    end
    alias :pre_dvec_pow :**
    def **(a)
        if a.class == Dobjects::Dvector
            a.as_exponent_of(self)
        else
            pre_dvec_pow(a)
        end
    end
    
    # a safe function that will return nil if the string is not a valid
    # number
    def Float.from_s(str)
      if str =~ /^\s*[\d.]/
        return str.to_s
      else
        return nil
      end
    end
end

class Fixnum
    alias :pre_dvec_add :+
    def +(a)
        if a.class == Dobjects::Dvector
            a+self
        else
            pre_dvec_add(a)
        end
    end
    alias :pre_dvec_sub :-
    def -(a)
        if a.class == Dobjects::Dvector
            a.neg+self
        else
            pre_dvec_sub(a)
        end
    end
    alias :pre_dvec_mult :*
    def *(a)
        if a.class == Dobjects::Dvector
            a*self
        else
            pre_dvec_mult(a)
        end
    end
    alias :pre_dvec_div :/
    def /(a)
        if a.class == Dobjects::Dvector
            a.inv*self
        else
            pre_dvec_div(a)
        end
    end
    alias :pre_dvec_pow :**
    def **(a)
        if a.class == Dobjects::Dvector
            a.as_exponent_of(self)
        else
            pre_dvec_pow(a)
        end
    end
end

module Dobjects
  class Dvector

    # Dvector.fancy_read's defaults options. See that function for
    # more details
    FANCY_READ_DEFAULTS = { 'sep' => /\s+/,
      'comments' => /^\s*\#/,
      'skip_first' => 0,
      'headers' => nil, # not used for now.
      'default' => 0.0/0.0, # defaults to NaN
    }

    # This function reads in +stream+ (can an IO object or a String,
    # in which case it represents the name of a file to be opened)
    # the columns specified by +cols+ and returns them. column 0 is the
    # first column. If +cols+ is +nil+, then fancy_read attempts to find
    # all the columns in the file, while filling absent data with NaN.
    #
    # +opts+ is a hash for tuning the behavior of the reading. It can hold
    # the following keys:
    # 'sep':: the record separator
    # 'comments':: a regular expression matching comment lines
    # 'skip_first':: how many lines to skip at the beginning of the file,
    # 'default':: the value taken for missing elements
    # ...

    def Dvector.fancy_read(stream, cols = nil, opts = {})
      # first, we turn the stream into a real IO stream
      if stream.is_a?(String)
        stream = IO.open(stream)
      end
      raise ArgumentError.new("'stream' should have a gets method") unless 
        stream.respond_to? :gets
      
      # we take default options and override them with opts
      o = FANCY_READ_DEFAULTS.dup
      o.update(opts)
      
      # strip off the first lines.
      while o["skip_first"] > 0
        stream.gets
        o["skip_first"] -= 1
      end

      # then, parsing the lines. We store the results in an array. We read up
      # all columns, regardless of what is asked (it doesn't slow that much
      # the process -- or does it ?)
      
      columns = []
      line_number = 0 # the number of the significant lines read so far
      
      while line = stream.gets
        next if line =~ o["comments"]
        next if line =~ /^\s*$/ # skip empty lines
        elements = line.split(o["sep"])
        # now, the fun: the actual reading.
        # we first turn this elements into floats:
        numbers = elements.collect do |s| 
          a = Float.from_s(s)
          if a
            a
          else
            o["default"]
          end
        end

        if numbers.size < columns.size 
          # we pad it with default values
          while numbers.size < columns.size
            numbers << o["default"]
          end
        else
          # in that case, we need to create new Dvectors to match the
          # size of numbers
          while columns.size < numbers.size
            columns << Dvector.new(line_number, o["default"]) 
          end
        end
        # now, we should have the same number of elements both
        # in numbers and in columns
        columns.size.times do |i|
          columns[i] << numbers[i]
        end
        # and it's done ;-) !

        line_number += 1
      end

      return columns unless cols
      return cols.collect { |i| 
        columns[i]
      }
    end

  end
end
