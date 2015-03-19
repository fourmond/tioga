#  Tioga::Dvector_extras.rb

module Dobjects 

  # MathEvaluator enables one to evaluate a simple mathematical expression
  # such as "x[0] + cos(x[1])", where the array x is given at each call to
  # compute, or "x + y**z", or...
  # 
  # This class acts as a backend for Dvector.compute_formula, to make sure
  # that the Math module is included, without the drawback of cluttering
  # all Math functions in Dvector, which would admittedly be quite stupid.

  class MathEvaluator
    include Math

    # Creates an evaluator for a formula. +formula+ is the formula. It is
    # transformed into a block that takes +argname+ as an argument --
    # +argname+ can be whatever you want. +mods+ are the modules you would
    # like the formula to include. Math is included by default, but you
    # can include other ones to make other kinds of functions available.
    #
    #  MathEvaluator.new("col[0] + col[1]", "col")
    #  MathEvaluator.new("x*cos(y)", "x,y")
    def initialize(formula, argname, mods = [])
      for mod in mods
        self.extend mod
      end
      @block = eval "proc { |#{argname}| #{formula} }"
    end

    # This function does the actual evaluation with the blocks
    # given.
    #
    #  e = MathEvaluator.new("x*y", "x,y")
    #  e.compute(1,2)         -> 2
    #
    # If an exception arises, NaN is returned, unless the problem is a
    # NameError (which means syntax problems). Other compilation
    # problems should be caught before that.
    def compute(*args)
      begin
        return compute_unsafe(*args)
      rescue NameError => e
        raise e
      rescue 
        return 0.0/0.0
      end
    end

    # This function does the actual evaluation with the blocks
    # given.
    #
    #  e = MathEvaluator.new("x*y", "x,y")
    #  e.compute(1,2)         -> 2
    # 
    # No care is taken to intercept exceptions.
    def compute_unsafe(*args)
      return @block.call(*args)
    end

  end

  class Dvector
  
    # thanks to Dave MacMahon for from_na and to_na
    # Create a Dvector from an NArray.
    def Dvector.from_na(na)
      _load([1, na.length, na.to_s].pack('CIa*'))
    end

    # Create an NArray with the same length and contents as +self+.
    def to_na
      ::NArray.to_na(_dump(nil)[5..-1], ::NArray::DFLOAT)
    end
    
      
    def to_dvector
      self
    end

    # Dvector.fancy_read's defaults options. See that function for
    # more details
    FANCY_READ_DEFAULTS = { 
      'sep' => /\s+/,
      'comments' => /^\s*\#/,
      'skip_first' => 0,
      'index_col' => false,
      'headers' => nil, # not used for now.
      'default' => 0.0/0.0, # defaults to NaN
      'initial_size' => 5001,
      'remove_space' => true ,# removes spaces at the beginning of the lines
      'last_col' => -1,       # Read all columns
      'text_columns' => [],   # Not a single column is text
    }

    # This function is a wrapper for #fast_fancy_read that reflects the
    # look-and-feel of #old_fancy_read
    def Dvector.fancy_read(stream, cols = nil, opts = {}) # :doc:
      o = FANCY_READ_DEFAULTS.dup
      o.update(opts)

      if stream.is_a?(String)
        stream = File.open(stream)
      end
      raise ArgumentError.new("'stream' should have a gets method") unless 
        stream.respond_to? :gets
      
      o['sep'] = Regexp.new(o['sep']) unless o['sep'].is_a? Regexp
      
      res = Dvector.fast_fancy_read(stream, o)

      # Adding the index columns if necessary
      if o["index_col"] 
        res.unshift(Dvector.new(res[0].length) { |i| i})
      end

      if cols
        return cols.map {|i| res[i] }
      else
        return res
      end
    end

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
    # 'index_col':: if set to true, the first column contains the
    #               indices of the elements (starting from 0 for
    #               first and so on)

    def Dvector.old_fancy_read(stream, cols = nil, opts = {}) # :doc:
      # first, we turn the stream into a real IO stream
      if stream.is_a?(String)
        stream = File.open(stream)
      end
      raise ArgumentError.new("'stream' should have a gets method") unless 
        stream.respond_to? :gets
      
      # we take default options and override them with opts
      o = FANCY_READ_DEFAULTS.merge(opts)
      
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
        if o["remove_space"]
          line.gsub!(/^\s+/,'')
        end
  
        elements = line.split(o["sep"])
        # now, the fun: the actual reading.
        # we first turn this elements into floats:
        numbers = elements.collect do |s| 
          begin 
            a = Float(s)
          rescue
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
      # Adding the index columns if necessary
      if o["index_col"] 
        columns.unshift(Dvector.new(columns[0].length) { |i| i})
      end

      return columns unless cols
      return cols.collect { |i| 
        columns[i]
      }
    end

    # This function is a rudimentary formula computing stuff. Give it
    # a text _formula_ and an array of Dvectors (_a_), and it returns a
    # Dvector with the result. The formula should contain the following;
    # column[n]:: represents the current element of the n th
    #             Dvector of the array
    #
    # This is just a try, and should be implemented in C rather than in
    # Ruby. But if you're looking for simplicity, here you go ;-) !
    #
    # _modules_ are the modules you would wish the evaluator to +include+. 
    # This feature enables one to make sure custom functions are included

    def Dvector.compute_formula(formula, a, modules = []) # :doc:

      evaluator = MathEvaluator.new(formula, "column", modules)
      # if we reach this place, it means that there a no big syntax errors ;-)
      
      # we now need to inspect the array given, and make sure that there is
      # and transform it into a clean stuff (an array with only Dvectors and
      # nil elements).
      
      target = []
      last = nil
      a.each { |elem| 
        if elem.is_a? Dvector
          target << elem
          last = elem
        else
          target << nil
        end
      }
      
      raise "No Dvector found" unless last
      
      # we check all the vectors have the same length
      target.each {|x| 
        if x && x.length != last.length
          raise "Dvectors should have all the same length !" 
        end
      }
      
      res = Dvector.new
      
      last.each_index { |i|
        args = target.collect { |val| 
          if val
            val[i]
          else 
            nil
          end
        }
        # we add the index at the beginning:
        #         args.unshift(i) 
        # Commented out for simplicity
        
        # then we call the block:
        elem = evaluator.compute(args)
        res << elem
      }
      
      return res
    end

    WRITE_DEFAULTS = {
      'sep' => "\t",
      'write_mode' => "a",
    }

    # Writes an array of Dvectors into a text _file_ 
    def Dvector.write(file, cols, options = {})
      ops = WRITE_DEFAULTS.update(options)
      if file.is_a?(String)
        file = File.open(file, ops["write_mode"])
      end 
      nb = cols.map {|d| d.size}.max # The number of lines
      nb.times do |i|
        file.puts(cols.map {|d| d[i].to_s }.join(ops["sep"]))
      end
    end
    
  end
end

# Modified by Vincent Fourmond to have a nice Dvector module
# instead of the Tioga module.
# add arithmetic operators for Float and Fixnum with Dvec

class Array # :nodoc: all
    def to_dvector
        Dobjects::Dvector[*self]
    end
end

class Float # :nodoc: all
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

class Fixnum # :nodoc: all
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

