#  Executive.rb

module Tioga

# These are the methods and attributes for controlling the FigureMaker executive routines.

class Executive < Doc < FigureMaker

# :call-seq:
#   def_figure(name) do ... end
#
# The block of code is saved to be executed later when the _name_ string is given to make_figure.
# The name is entered in the table of figure_names.  If the name was already listed, then the new code
# replaces the previous definition.
#
    def def_figure(name, &cmd)
    end

# :call-seq:
#   make_figure(name)
#   make_figure(number)
#
# Executes the corresponding code that was previously saved by def_figure.
# Output is written to the currently specified save_dir directory. 
#
    def make_figure(name)
    end

# :call-seq:
#   make_portfolio_pdf(name=nil)
#
# Creates a simple TeX file holding all of the figures, one per page.
# Uses the _filename_ if it is given, otherwise, appends '.tex' to the
# name of the current '.rb' file.
#
    def make_portfolio_pdf(name=nil)
    end

# :call-seq:
#   make_preview_pdf(num)
#
# Calls make_figure, then calls pdflatex to make a 1 page pdf for previewing. 
#
    def make_preview_pdf(num)
    end

# :call-seq:
#   reset_figures
#
# Clears the list of figures.
    def reset_figures
    end

# Opens a file with the given _name_ and writes a small TeX program to include
# all of the currently defined figures, each on a separate page.
    def make_portfolio(name)
    end

# :call-seq:
#   reset_eval_function
#
# Removes the current definition for the eval_function.  See also def_eval_function.
#
    def reset_eval_function
    end

# :call-seq:
#   def_eval_function { |string| ... }
#
# The block of code is saved to be executed later when eval_function is called.  See also reset_eval_function.
#
    def def_eval_function(&cmd)
    end

# Calls the function block defined by a previous call to def_eval_function passing the string as argument.
# If no function is currently defined, it calls Ruby's +eval+ with _string_.
#
    def eval_function(string)
    end

# :call-seq:
#               run_dir                                     
#
# The path name for the FigureMaker working directory.
   def run_dir
   end

# :call-seq:
#               save_dir                                     
#               save_dir = a_string
#
# The path name for the directory where output files will be created.
   def save_dir
   end

# :call-seq:
#               which_pdflatex                                     
#               which_pdflatex = a_string
#
# The name for the pdflatex to use.  Defaults to 'pdflatex'.
   def which_pdflatex
   end

# :call-seq:
#               quiet_mode                                     
#               quiet_mode = true or false
#
# Defaults to +false+.
# If +true+, turns off all but essential messages.
   def quiet_mode
   end

# :call-seq:
#               model_number                                     
#               model_number = an_integer
#
# This can be set by your figure routines.  It is used by the output routines if add_model_number is +true+.
# The value should be an integer between 0 and 9999.
   def model_number
   end

# :call-seq:
#               get_save_filename(name) -> a_string
#
# Returns a string with the filename that will be used for saving the figure with the given _name_.
   def get_save_filename(name)
   end

# :call-seq:
#               add_model_number                                     
#               add_model_number = true_or_false
#
# This flag is initially +false+.  If it is set to +true+, then the output routines will append the
# current model_number to the file names.  For example, if the plot name is "sample", the output names
# when add_model_number is +false+ will be "sample_figure.pdf", "sample_figure.txt", "sample.tex", and
# "sample.pdf".  However, if add_model_number is +true+ and +model_number+ is 57, say, then the names will
# be "sample_0057_figure.pdf", "sample_0057_figure.txt", "sample_0057.tex", and
# "sample_0057.pdf".
# This can be useful if you are collecting several versions of
# a figure for different sets of input data.
   def add_model_number
   end

# :call-seq:
#               need_to_reload_data                                     
#               need_to_reload_data = true_or_false
#
# This flag is set to +true+ by the Tioga front end for "refresh" commands.  Plot routines that support
# auto-refresh can use this flag to avoid unnecessary reloading of data.  For example, a read_data method
# that is called by all of the plotting definitions
# might look like the following (in this example, the method "t" returns the default FigureMaker):
#
#     def read_data
#         return unless t.need_to_reload_data
#         ... read the data ...
#         t.need_to_reload_data = false
#     end
# 
   def need_to_reload_data
   end

# :call-seq:
#               auto_refresh_filename                                     
#               auto_refresh_filename = a_string
#
# If this is non-nil, the front end will only do an automatic refresh when this file has
# been modified since the time of the last refresh.
   def auto_refresh_filename
   end

# :call-seq:
#               num_figures                                     
#
# The current number of defined figures for this FigureMaker.
   def num_figures
   end

# :call-seq:
#               figure_names                                     
#
# An array of names for the currently defined figures.
   def figure_names
   end

# :call-seq:
#               figure_name(num) -> a_string                                     
#
# Returns the name from the figure_names array.
   def figure_name(num)
   end

# :call-seq:
#               tex_preview_documentclass                                     
#               tex_preview_documentclass = a_string
#
# This string will be used as the "documentclass" for the sample TeX file (default is "article").
   def tex_preview_documentclass
   end

# :call-seq:
#               tex_preview_pagestyle                                     
#               tex_preview_pagestyle = a_string
#
# This string will be used as the "pagestyle" for the sample TeX file (default is "empty").
   def tex_preview_pagestyle
   end

=begin rdoc
:call-seq:
               tex_preview_preamble                                     
               tex_preview_preamble = a_string

This string will be used as the "preamble" for the sample TeX file.
The default does "usepackage" commands for "color" and "geometry".
You may want to use some other packages as well.  Here's an example of how to do it which
adds the 'marvosym' package by inserting the following line into the 'initialize' routine.

        t.tex_preview_preamble = t.tex_preview_preamble + "\n\t\\usepackage{marvosym}\n"
        
=end 
   def tex_preview_preamble
   end

# :call-seq:
#               tex_xoffset                                     
#               tex_xoffset = a_float
#
# Defines the relative shift in the x direction necessary to make the PDF and TeX output align properly.
# Hopefully, you won't need to touch this, but it is here just in case.  See also tex_yoffset.
#
# You can check the alignment by running the test pattern in the sample figures.  It should look like this:
#
# http://theory.kitp.ucsb.edu/~paxton/tioga_jpegs/Test_Pattern.jpg
#
   def tex_xoffset
   end
   
# :call-seq:
#               tex_yoffset                                     
#               tex_yoffset = a_float
#
# Defines the relative shift in the y direction necessary to make the PDF and TeX output align properly.
# Hopefully, you won't need to touch this, but it is here just in case.  See also tex_xoffset.
   def tex_yoffset
   end
   


end # class
end # module Tioga
