#  Titles_and_Labels.rb

module Tioga

# These are the methods and attributes for doing plot titles and axis labels.

class Titles_and_Labels < Doc < FigureMaker

# If the _text_ argument is +nil+, then the current #title attribute is used instead.
# If the flag title_visible is +true+ and the text is not +nil+, then calls show_text
# with the text and the attributes controlling the appearance of the title (title_scale,
# title_shift, etc.).
    def show_title(text = nil)
    end
    
# If the _text_ argument is +nil+, then the current #xlabel attribute is used instead.
# If the flag xlabel_visible is +true+ and the text is not +nil+, then calls show_text
# with the text and the attributes controlling the appearance of the x label (xlabel_position,
# xlabel_scale, etc.).
    def show_xlabel(text = nil)
    end

# If the _text_ argument is +nil+, then the current #ylabel attribute is used instead.
# If the flag ylabel_visible is +true+ and the text is not +nil+, then calls show_text
# with the text and the attributes controlling the appearance of the y label (ylabel_position,
# ylabel_scale, etc.).
    def show_ylabel(text = nil)
    end

# Each argument, if it is not +nil+, is shown (using show_title, show_xlabel, or show_ylabel)
# and it's "visible" flag is set +false+ to stop any later attempts to "reshow" it.
    def do_box_labels(title, xlabel, ylabel)
    end

# :call-seq:
#               title_visible                                     
#               title_visible = false
#
# If +true+, then it is okay for tioga to show the title of a plot.  If +false+,
# then calls on show_title return immediately.
# This attribute is "one-way-only" in that it starts +true+ and can be
# set +false+, but cannot be reset to +true+ except by restoring the
# graphics state in which is was still +true+.  This is intended
# to help control the behavior of plots when embedded as subplots in a
# larger configuration.  Note that this does not effect calls on show_text;
# it only applies to calls on show_title.
   def title_visible
   end

# :call-seq:
#               xlabel_visible                                     
#               xlabel_visible = false
#
# If +true+, then it is okay for tioga to show the x label of a plot.  If +false+,
# then calls on show_xlabel return immediately.
# This attribute is "one-way-only" in that it starts +true+ and can be
# set +false+, but cannot be reset to +true+ except by restoring the
# graphics state in which is was still +true+.  This is intended
# to help control the behavior of plots when embedded as subplots in a
# larger configuration.  Note that this does not effect calls on show_text;
# it only applies to calls on show_xlabel.
   def xlabel_visible
   end

# :call-seq:
#               ylabel_visible                                     
#               ylabel_visible = false
#
# If +true+, then it is okay for tioga to show the y label of a plot.  If +false+,
# then calls on show_ylabel return immediately.
# This attribute is "one-way-only" in that it starts +true+ and can be
# set +false+, but cannot be reset to +true+ except by restoring the
# graphics state in which is was still +true+.  This is intended
# to help control the behavior of plots when embedded as subplots in a
# larger configuration.  Note that this does not effect calls on show_text;
# it only applies to calls on show_ylabel.
   def ylabel_visible
   end

# :call-seq:
#               title_shift                                     
#               title_shift = a_float
#
# Defines the shift distance away from the frame edge for the reference point of the title measured
# in units of text heights (default title_shift is 0.7).
   def title_shift
   end
    
# :call-seq:
#               xlabel_shift                                     
#               xlabel_shift = a_float
#
# Defines the shift distance away from the frame edge for the reference point of the x label measured
# in units of text heights (default xlabel_shift is 1.0).
   def xlabel_shift
   end
    
# :call-seq:
#               ylabel_shift                                     
#               ylabel_shift = a_float
#
# Defines the shift distance away from the frame edge for the reference point of the y label measured
# in units of text heights (default ylabel_shift is 1.8).
   def ylabel_shift
   end
    
# :call-seq:
#               title_scale                                     
#               title_scale = a_float
#
# Defines the text size scale for the title (default is 1.1).
   def title_scale
   end
   
# :call-seq:
#               xlabel_scale                                     
#               xlabel_scale = a_float
#
# Defines the text size scale for the x label (default is 1.0).
   def xlabel_scale
   end
   
# :call-seq:
#               ylabel_scale                                     
#               ylabel_scale = a_float
#
# Defines the text size scale for the y label (default is 1.0).
   def ylabel_scale
   end
   
# :call-seq:
#               title_alignment                                     
#               ylabel_alignment = an_alignment
#
# Defines the alignment for the title (default is +ALIGNED_AT_BASELINE+).
   def title_alignment
   end
   
# :call-seq:
#               xlabel_alignment                                     
#               xlabel_alignment = an_alignment
#
# Defines the alignment for the x label (default is +ALIGNED_AT_BASELINE+).
   def xlabel_alignment
   end
   
# :call-seq:
#               ylabel_alignment                                     
#               ylabel_alignment = an_alignment
#
# Defines the alignment for the y label (default is +ALIGNED_AT_BASELINE+).
   def ylabel_alignment
   end
   
# :call-seq:
#               title_justification                                     
#               title_justification = a_justification
#
# Defines the justification for the title (default is +CENTERED+).
   def title_justification
   end
   
# :call-seq:
#               xlabel_justification                                     
#               xlabel_justification = a_justification
#
# Defines the justification for the x label (default is +CENTERED+).
   def xlabel_justification
   end
   
# :call-seq:
#               ylabel_justification                                     
#               ylabel_justification = a_justification
#
# Defines the justification for the y label (default is +CENTERED+).
   def ylabel_justification
   end
   
# :call-seq:
#               title_color                                     
#               title_color = a_color_or_nil
#
# If not +nil+, then is used to add a TeX "\textcolor" specification to the title text.
   def title_color
   end
   
# :call-seq:
#               xlabel_color                                     
#               xlabel_color = a_color_or_nil
#
# If not +nil+, then is used to add a TeX "\textcolor" specification to the x label text.
   def xlabel_color
   end
   
# :call-seq:
#               ylabel_color                                     
#               ylabel_color = a_color_or_nil
#
# If not +nil+, then is used to add a TeX "\textcolor" specification to the y label text.
   def ylabel_color
   end
   
# :call-seq:
#               title_angle                                     
#               title_angle = a_float
#
# The angle of the baseline for the title with respect to the side of the frame (default is 0).
   def title_angle
   end
   
# :call-seq:
#               xlabel_angle                                     
#               xlabel_angle = a_float
#
# The angle of the baseline for the x label with respect to the side of the frame (default is 0).
   def xlabel_angle
   end
   
# :call-seq:
#               ylabel_angle                                     
#               ylabel_angle = a_float
#
# The angle of the baseline for the y label with respect to the side of the frame (default is 0).
   def ylabel_angle
   end
   
# :call-seq:
#               title_side                                     
#               title_side = a_side
#
# The side of the frame for the title (default is +TOP+).
   def title_side
   end
   
# :call-seq:
#               xlabel_side                                     
#               xlabel_side = a_side
#
# The side of the frame for the x label (default is +BOTTOM+).
   def xlabel_side
   end
   
# :call-seq:
#               ylabel_side                                     
#               ylabel_side = a_side
#
# The side of the frame for the y label (default is +LEFT+).
   def ylabel_side
   end
   
# :call-seq:
#               title_position                                     
#               title_position = a_float
#
# The fractional position along the edge of the frame for the title reference point.
# The default title_position is 0.5 with title_justification == +CENTERED+.
   def title_position
   end
   
# :call-seq:
#               xlabel_position                                     
#               xlabel_position = a_float
#
# The fractional position along the edge of the frame for the x label reference point.
# The default xlabel_position is 0.5 with xlabel_justification == +CENTERED+.
   def xlabel_position
   end
   
# :call-seq:
#               ylabel_position                                     
#               ylabel_position = a_float
#
# The fractional position along the edge of the frame for the y label reference point.
# The default ylabel_position is 0.5 with ylabel_justification == +CENTERED+.
   def ylabel_position
   end
   
# :call-seq:
#               title                                     
#               title = text_or_nil
#
# The text to be used for the title, or +nil+ indicating no title.
   def title
   end
   
# :call-seq:
#               xlabel                                     
#               xlabel = text_or_nil
#
# The text to be used for the x axis label, or +nil+ indicating no label.
   def xlabel
   end
   
# :call-seq:
#               ylabel                                     
#               ylabel = text_or_nil
#
# The text to be used for the y axis label, or +nil+ indicating no label.
   def ylabel
   end
   



end # class
end # module Tioga
