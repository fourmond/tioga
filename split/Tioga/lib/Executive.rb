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
#   require_pdf(name)
#   require_pdf(number)
#
# Calls make_pdf if the pdf for the figure has not yet been created.
# Returns the full file name of the pdf.
#
    def require_pdf(name)
    end

# :call-seq:
#   require_all(fignums=nil,report=false)
#
# Calls require_pdf for each of the figures listed in _fignums_.  
# Does all the defined figures if _fignums_ is +nil+.
# Writes pdf information to terminal as it goes if _report_ is true.
#
    def require_all(fignums=nil,report=false)
    end

# :call-seq:
#   make_all(fignums=nil,report=false)
#
# Calls make_pdf for each of the figures listed in _fignums_.  
# Does all the defined figures if _fignums_ is +nil+. See also require_all.
# Writes pdf information to terminal as it goes if _report_ is true.
#
    def make_all(fignums=nil,report=false)
    end

# :call-seq:
#   make_figure(name)
#   make_figure(number)
#
# Executes the corresponding code that was previously saved by def_figure.
# Output is written to the currently specified save_dir directory. 
# Alias for make_pdf.
#
    def make_figure(name)
    end

# :call-seq:
#   make_pdf(name) do |t| ... end
#   make_pdf(name)
#   make_pdf(number)
#
# First, if there is a command block given, def_figure is called to save it.
# Then make_pdf executes the command corresponding to the given name or number.
# Output is written to the currently specified save_dir directory.
# See also require_pdf.
#
    def make_pdf(n,&cmd)
    end

# :call-seq:
#   make_portfolio(name,fignums=nil,report=false)
#
# Creates a multipage pdf file containing the figures listed in _fignums_.  
# Does all the defined figures if _fignums_ is +nil+. 
# Writes pdf information to terminal as it goes if _report_ is true.
#
# First, make_portfolio calls require_all(fignums) so that all the pdfs will be available.  
# The portfolio will
# have the given _name_ with a ".pdf" extension and will be placed in the save_dir.
#
    def make_portfolio(name,fignums=nil,report=false)
    end

# :call-seq:
#   make_portfolio_pdf(name,fignums=nil,report=false)
#
# Alias for make_portfolio.
#
    def make_portfolio_pdf(name,fignums=nil,report=false)
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
# See also reset_enter_page_function, default_enter_page_function.
#
    def def_enter_page_function(&cmd)
    end

# :call-seq:
#   reset_enter_page_function
#
# Removes the current definition for the enter_page_function.  See also def_enter_page_function, default_enter_page_function.
#
    def reset_enter_page_function
    end

# :call-seq:
#   default_enter_page_function
#
# Calls page_setup with the current default_page_width and default_page_height.
# Then calls set_frame_sides with default_frame_left, default_frame_right, default_frame_top, and default_frame_bottom.
#
    def default_enter_page_function
    end

# :call-seq:
#   default_page_width
#   default_page_width = a_float
#
# The page width for use by default_enter_page_function.
#
    def default_page_width
    end

# :call-seq:
#   default_page_height
#   default_page_height = a_float
#
# The page height for use by default_enter_page_function.
#
    def default_page_height
    end

# :call-seq:
#   default_frame_left
#   default_frame_left = a_float
#
# The frame left for use by default_enter_page_function.
#
    def default_frame_left
    end

# :call-seq:
#   default_frame_right
#   default_frame_right = a_float
#
# The frame right for use by default_enter_page_function.
#
    def default_frame_right
    end

# :call-seq:
#   default_frame_top
#   default_frame_top = a_float
#
# The frame top for use by default_enter_page_function.
#
    def default_frame_top
    end

# :call-seq:
#   default_frame_bottom
#   default_frame_bottom = a_float
#
# The frame bottom for use by default_enter_page_function.
#
    def default_frame_bottom
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
# Whether or not to do automatic cleanup of the temporary files when create a PDF for a figure (default is true).
   def autocleanup
   end

# :call-seq:
#               multithreads_okay_for_tioga                                     
#               multithreads_okay_for_tioga = a_boolean
#
# Whether or not to use multithreading wherever possible (default is true).  Currently, this only is used when making
# a batch of pdfs -- if multithreads_okay_for_tioga is true, then we launch all the pdflatex shells in parallel.
   def multithreads_okay_for_tioga
   end

# :call-seq:
#               pdflatex                                     
#               pdflatex = a_string
#
# The name for the pdflatex to use.  Defaults to 'pdflatex'.
   def pdflatex
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
#               get_save_filename(name) -> a_string
#
# Returns a string with the filename that will be used for saving the figure with the given _name_.
   def get_save_filename(name)
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
#               figure_pdfs                                     
#
# An array of full pdf filenames for the currently defined figures.
# All entries in the array initialized to +nil+.  When a pdf is created
# for a figure, the full filename for the pdf is placed in this array
# in position corresponding to the figure's location in figure_names. 
   def figure_pdfs
   end

# :call-seq:
#               figure_pdf(num) -> a_string or nil                                 
#               figure_pdf(name) -> a_string or nil                    
#
# Returns the corresponding entry from the figure_pdfs array.
   def figure_pdf(num)
   end
   



end # class
end # module Tioga
