#  Legends.rb

module Tioga

# These are the methods and attributes for plot legends.

class Legends < Doc < FigureMaker

# :call-seq:
#               reset_legend_info                         
#                   
# Clears the current legend information.
    def reset_legend_info
    end

# Creates a vertical space of _dy_ text heights in the legend.
    def save_legend_separator(dy)
    end

# :call-seq:
#               legend_height                         
#                   
# Returns the height (in units of text heights) of the currently saved legend information.
    def legend_height
    end

# :call-seq:
#               show_legend(legend_background_function=nil)                        
#                   
# Shows the legend in the current frame.  If given a function arguement,
# it calls the function with bounds [left, right, top, bottom] for the
# rectangle where the legend will be shown.  You can use this opportunity
# to provide a background for the legend info and a frame around it too
# if you'd like.
    def show_legend(legend_background_function=nil)
    end

=begin rdoc
:call-seq:
    save_legend_info(text)
    save_legend_info(dict)

Saves information for later use in creating a legend.  If called with a string as argument,
is equivalent to calling with a dictionary having a single entry with key 'text' and value the string.

Dictionary Entries
    'text'          => a_string      # text of the legend
    'line_color'    => a_color       # defaults to self.line_color
    'line_width'    => a_float       # defaults to self.line_width
    'line_cap'      => a_line_cap    # defaults to self.line_cap
    'line_type'     => a_line_type or 'None'   # defaults to self.line_type
    'dy'            => a_float       # defaults to self.legend_text_dy
    'marker'        => a_marker      # defaults to nil
    'marker_color'  => a_color       # defaults to self.line_color
    'marker_scale'  => a_float       # defaults to 0.5
    'marker_dict'   => a_dictionary  # defaults to nil

The dictionary holding the information is appended to the legend_info array.  If the 'marker' entry is present,
then the marker with the given 'marker_color' and 'marker_scale' will be shown in the middle of the
legend line.  If the 'marker_dict' entry is present, it will have the values for 'x' and 'y' set to the location
of the middle of the legend line and then it will be passed to the show_marker method.  This gives you the option of using
the full range of marker functionality in legends -- perhaps you'd like to use a line marker that is rotated, 
stretched, filled in one color, and stroked in another!  Sometimes you just want to have a marker in
the legend without a line; do this by setting line_type to 'None'.

=end
    def save_legend_info(arg)
    end

# :call-seq:
#               legend_defaults                                     
#               legend_defaults = a_dictionary
#
# This dictionary holds defaults for +show_plot_with_legend+.
   def legend_defaults
   end

# :call-seq:
#               legend_info                                     
#
# The current array of saved legend dictionaries.  See save_legend_info for details.
   def legend_info
   end

# :call-seq:
#               legend_line_x0                                     
#               legend_line_x0 = a_float
#
# The x position for the start of lines in a legend, measured in units
# of text height.  The corresponding position in figure coordinates is
# (legend_line_x0 * legend_scale * default_text_height_dx).
   def legend_line_x0
   end
   
# :call-seq:
#               legend_line_x1                                     
#               legend_line_x1 = a_float
#
# The x position for the end of lines in a legend, measured in units
# of text height.  The corresponding position in figure coordinates is
# (legend_line_x1 * legend_scale * default_text_height_dx).
   def legend_line_x1
   end
   
# :call-seq:
#               legend_line_dy                                     
#               legend_line_dy = a_float
#
# Each line in a legend is shifted up from the text baseline by this amount
# measured in units of text height.  The corresponding distance in figure coordinates is
# (legend_line_dy * legend_scale * default_text_height_dy).
   def legend_line_dy
   end

# :call-seq:
#               legend_text_width                                     
#               legend_text_width = a_float
#
# The assumed width of text in a legend, measured in units
# of text height.  The corresponding width in figure coordinates is
# (legend_text_width * legend_scale * default_text_height_dx).
# The figure bounding box is expanded if necessary to provide for this
# much space.  If legend_text_width is negative, then the system will
# provide a default value depending on the placement of the legend.
   def legend_text_width
   end

# :call-seq:
#               legend_text_xstart                                     
#               legend_text_xstart = a_float
#
# The x position for the start of text in a legend, measured in units
# of text height.  The corresponding position in figure coordinates is
# (legend_text_xstart * legend_scale * default_text_height_dx).
   def legend_text_xstart
   end

# :call-seq:
#               legend_text_ystart                                     
#               legend_text_ystart = a_float
#
# The y position for the start of text in a legend, measured in units
# of text height.  The corresponding position in figure coordinates is
# (legend_text_ystart * legend_scale * default_text_height_dy).
   def legend_text_ystart
   end

# :call-seq:
#               legend_text_dy                                     
#               legend_text_dy = a_float
#
# The distance in y to move down after a legend line entry, measured in
# units of text height.  The corresponding distance in figure coordinates is
# (legend_text_dy * legend_scale * default_text_height_dy).
   def legend_text_dy
   end

# :call-seq:
#               legend_line_width                                     
#               legend_line_width = a_float
#
# If this is non-negative, then it is used as the line_width attribute when
# stroking the lines in a legend.  If it is negative, then each legend
# line is stroked using the value of the line_width attribute at the time
# the legend information for the entry was saved by save_legend_info.
   def legend_line_width
   end

# :call-seq:
#               legend_scale                                     
#               legend_scale = a_float
#
# Used for determining positions in the legend and for showing  legend text.
   def legend_scale
   end

# :call-seq:
#               legend_alignment                                     
#               legend_alignment = an_alignment
#
# Used as the #alignment value in showing  legend text.
   def legend_alignment
   end

# :call-seq:
#               legend_justification                                     
#               legend_justification = a_justification
#
# Used as the #justification value in showing legend text.
   def legend_justification
   end



end # class
end # module Tioga
