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
#   make_pdf(name)
#   make_pdf(number)
#
# Executes the corresponding code that was previously saved by def_figure.
# Output is written to the currently specified save_dir directory.
# Alias for make_figure.
#
    def make_pdf(n)
    end


# :call-seq:
#   figure_index(name)
#
# Returns the index of the figure with the given name.


    def figure_index(name)
    end

# :call-seq:
#   reset_state
#
# Reinitializes the FigureMaker.  This is automatically called when you load or reload a tioga document file.
    def reset_state
    end
   
# :call-seq:
#   reset_figures                                     
#
# Alias for reset_state.
   def reset_figures
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
#   def_enter_page_function { |string| ... }
#
# The block of code is saved to be executed later whenever make_pdf is called to create a page for a figure.  
# See also reset_enter_page_function.
#
    def def_enter_page_function(&cmd)
    end

# :call-seq:
#   reset_enter_page_function
#
# Removes the current definition for the enter_page_function.  See also def_enter_page_function.
#
    def reset_enter_page_function
    end



# :call-seq:
#   def_exit_page_function { |string| ... }
#
# The block of code is saved to be executed later after make_pdf has returned from creating a page for a figure.  
# See also reset_exit_page_function.
#
    def def_exit_page_function(&cmd)
    end

# :call-seq:
#   reset_exit_page_function
#
# Removes the current definition for the exit_page_function.  See also def_exit_page_function.
#
    def reset_exit_page_function
    end



# :call-seq:
#   def_enter_show_plot_function { |string| ... }
#
# The block of code is saved to be executed later whenever show_plot is called.  
# See also reset_enter_show_plot_function.
#
    def def_enter_show_plot_function(&cmd)
    end

# :call-seq:
#   reset_enter_show_plot_function
#
# Removes the current definition for the enter_show_plot_function.  See also def_enter_show_plot_function.
#
    def reset_enter_show_plot_function
    end



# :call-seq:
#   def_exit_show_plot_function { |string| ... }
#
# The block of code is saved to be executed whenever show_plot returns.  
# See also reset_exit_show_plot_function.
#
    def def_exit_show_plot_function(&cmd)
    end

# :call-seq:
#   reset_exit_show_plot_function
#
# Removes the current definition for the exit_show_plot_function.  See also def_exit_show_plot_function.
#
    def reset_exit_show_plot_function
    end






# :call-seq:
#   def_enter_subfigure_function { |string| ... }
#
# The block of code is saved to be executed later whenever subfigure is called.  
# See also reset_enter_subfigure_function.
#
    def def_enter_subfigure_function(&cmd)
    end

# :call-seq:
#   reset_enter_subfigure_function
#
# Removes the current definition for the enter_subfigure_function.  See also def_enter_subfigure_function.
#
    def reset_enter_subfigure_function
    end



# :call-seq:
#   def_exit_subfigure_function { |string| ... }
#
# The block of code is saved to be executed whenever subfigure returns.  
# See also reset_exit_subfigure_function.
#
    def def_exit_subfigure_function(&cmd)
    end

# :call-seq:
#   reset_exit_subfigure_function
#
# Removes the current definition for the exit_subfigure_function.  See also def_exit_subfigure_function.
#
    def reset_exit_subfigure_function
    end






# :call-seq:
#   def_enter_subplot_function { |string| ... }
#
# The block of code is saved to be executed later whenever subplot is called.  
# See also reset_enter_subplot_function.
#
    def def_enter_subplot_function(&cmd)
    end

# :call-seq:
#   reset_enter_subplot_function
#
# Removes the current definition for the enter_subplot_function.  See also def_enter_subplot_function.
#
    def reset_enter_subplot_function
    end



# :call-seq:
#   def_exit_subplot_function { |string| ... }
#
# The block of code is saved to be executed whenever subplot returns.  
# See also reset_exit_subplot_function.
#
    def def_exit_subplot_function(&cmd)
    end

# :call-seq:
#   reset_exit_subplot_function
#
# Removes the current definition for the exit_subplot_function.  See also def_exit_subplot_function.
#
    def reset_exit_subplot_function
    end










# :call-seq:
#   def_enter_context_function { |string| ... }
#
# The block of code is saved to be executed later whenever context is called.  
# See also reset_enter_context_function.
#
    def def_enter_context_function(&cmd)
    end

# :call-seq:
#   reset_enter_context_function
#
# Removes the current definition for the enter_context_function.  See also def_enter_context_function.
#
    def reset_enter_context_function
    end



# :call-seq:
#   def_exit_context_function { |string| ... }
#
# The block of code is saved to be executed whenever context returns.  
# See also reset_exit_context_function.
#
    def def_exit_context_function(&cmd)
    end

# :call-seq:
#   reset_exit_context_function
#
# Removes the current definition for the exit_context_function.  See also def_exit_context_function.
#
    def reset_exit_context_function
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
#               create_save_dir                                     
#               create_save_dir = a_boolean
#
# Whether or not to create +save_dir+ if it doesn't exist.
   def create_save_dir
   end

# :call-seq:
#               autocleanup                                     
#               autocleanup = a_boolean
#
# Whether or not do do automatic cleanup of the temporary files when create a PDF for a figure.
   def autocleanup
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
# This flag is set to +true+ by the tioga front end for "refresh" commands.  Plot routines that support
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


end # class
end # module Tioga
