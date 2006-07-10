#  Page_Frame_Bounds.rb

module Tioga
    
=begin rdoc

This section describes the coordinate systems used by tioga for page layout.

The "output page" is defined in "output coordinates"
having units equal to 1/720 of an inch.  This unit is 1/10 of a "big point" that is the basic size used in PostScript and PDF.
By making the unit this size, we can write output coordinates to the PDF file as integers, getting a significant size reduction in
the file without giving up noticable accuracy.  The (0, 0) point of the output coordinate system is at the lower-left hand corner
of the output page.  The x axis increases horizontally, and the y axis increases vertically.  The  dimensions of the page
are given by the attributes page_width and page_height.  
The attributes page_right and page_top are aliases for these.
The attributes page_left and page_bottom are always zero.  The default page size is 5 inches square, 
but you can set the size to anything you like using the set_device_pagesize routine. 

While the output coordinates have a fixed physical size, all the other coordinate systems are relative rather than absolute.
At the next level comes "page coordinates" that are defined relative to the output page with (0, 0) in page coordinates 
at the lower left corner  of the output page and (1, 1) at the upper right.  Page coordinates are used to define the location of
the current "frame".  The current frame location is held in the attributes frame_left, frame_right, frame_top, and frame_bottom,
all in page coordinates.  In addition, the attribute frame_width is defined to be frame_right - frame_left, and frame_height is frame_top -
frame_bottom.  The defaults are (0.2, 0.2) for the lower left corner of the frame and (0.8, 0.8) for the upper right.
You can change these by calling the routine set_frame_sides.

The "frame coordinates" are defined with (0, 0) at the lower left corner of the frame and (1, 1) at the upper right.  Subframes are
sized and located using frame coordinates.  The routine set_subframe does this job.  In addition, subframes are used to
give a desired aspect ratio.  The routine set_aspect_ratio_relative_to_frame does this in terms of frame coordinates -- in
other words, it creates a subframe having the requested ratio of width to height relative to the frame.  In some cases
that will be what you want, but it is more common to want to specify the width to height ratio relative to the output page, i.e.,
in absolute rather than relative terms.   This is provided by the routine set_physical_aspect_ratio (with set_aspect_ratio as an alias).

When doing a plot, you want yet another coordinate system, one that matches the data.  This is called the "figure coordinate system"
and is set by the "bounds" attributes that give the locations in figure coordinates of the edges of the frame.  These attributes are
called bounds_right, bounds_left, bounds_top, and bounds_bottom.  Note that you can "reverse" the x axis, for example, by making
bounds_right smaller than bounds_left.  To help with the bookkeeping for this, the attribute bounds_xmin holds the minimum
of bounds_left and bounds_right, while bounds_ymin has the minimum of bounds_top and bounds_bottom.  Finally, bounds_width holds
the absolute value of bounds_right - bounds_left and bounds_height has abs(bounds_top - bounds_bottom).  The default bounds
are 0 for left and bottom and 1 for right and top, making figure coordinates identical to frame coordinates.  The bounds can be
changed by calling the set_bounds routine.

That takes care of getting the page layout done, but we still need to put the tioga output page into a TeX document, whether
as part of a larger document or only for previewing by itself.  In either case, we get one more chance to specify size and location.
Let's discuss the previewing case first.  The preview TeX document that is constructed automatically has a very simple structure --
it has no content except the figure!  You can specify the size of the TeX page in tioga by setting tex_preview_paper_width and
tex_preview_paper_height.  These attributes hold string values that are directly passed to TeX, so you can use any units of length
that TeX understands.  There are several routines to set these to standard values, such as set_USLetter_landscape and set_USLetter_portrait
("landscape" meaning width greater than height and "portrait" meaning the reverse).  Similarly, you can control the location of the
tioga output page on the TeX page using tex_preview_hoffset and tex_preview_voffset.

Independently of what size the
tioga output page had when it was created, you can now rescale it to whatever size you want on the TeX page using the
attributes tex_preview_figure_width and tex_preview_figure_height.  These sizes are also given as TeX strings.  I suggest opening
a preview TeX file in your favorite TeX editor and taking a look.  In the heading of the file, you'll find various "setlength" commands for setting the
paper size and the offsets.  Try changing these by hand in the TeX editor and retypesetting the page.  The values for the figure
size will be found in the call on the tiogafiguresized command from inside the document body at the bottom of the file.
It looks something like this:

        % Here's the page with the figure.
        \begin{document}
        \pagestyle{empty}

        \tiogafiguresized{MyPlot}{!}{184mm}

        \end{document}

The tiogafiguresized routine is included in the preamble section -- it takes the name of the plot ("MyPlot" in this case),
and specification strings for the width and height in the second and third arguments.  In the example, we're
setting the height to 184mm; the "!" for the width means that the width will be scaled the same as the height to keep
the aspect ratio of the figure unchanged.  As you did for the page size, try editing the figure size arguments
in the TeX editor.  If you want to give a relative scale for the figure size, use the tiogafigurescaled command;
if you want different horizontal and vertical scales, use tiogafigurescaledxy.

Once you've finished creating a figure and have previewed it, you'll probably want to put it in a document.  One option
is to use the PDF file created for previewing directly in the document with something like "includegraphics" -- you can see how this is done
by looking at the tiogafigureshow command in the preview TeX file.  However, if you are doing a set of figures
that should have a coordinated appearance, such as sharing the same fonts, it is probably better to recreate the
figure when the document is typeset in the same manner that the PDF preview is produced in the preview TeX file: include
a copy of the tiogafigure commands and use calls on them to place the figures in the document.  By doing this, you
"delay binding" of the selection of fonts for the figures until the last possible moment.  An entire set of figures
can be redone with a different typeface simply by retypesetting the TeX  with the help of the various settiogafont... 
LaTeX commands.

For more information about font selection, see TeX_Text.

=end


class Page_Frame_Bounds < Doc < FigureMaker

# :call-seq:
#               set_device_pagesize(width, height) # measured in output page coordinates (1/720 inch)                                 
#
# The page coordinates go from 0.0 to 1.0 with (0,0) at the lower left and (1,1) at the upper right.
# This command sets the physical size of this rectangle in the output coordinate 
   def set_device_pagesize(width, height)
   end

# :call-seq:
#               page_left                                     
#
# The position of the left of the page in the device coordinate system -- measured in output page coordinates (1/720 inch).
   def page_left
   end

# :call-seq:
#               page_right                                     
#
# The position of the right of the page in the device coordinate system -- measured in output page coordinates (1/720 inch).
   def page_right
   end

# :call-seq:
#               page_bottom                                     
#
# The position of the bottom of the page in the device coordinate system -- measured in output page coordinates (1/720 inch).
   def page_bottom
   end

# :call-seq:
#               page_top                                     
#
# The position of the top of the page in the device coordinate system -- measured in output page coordinates (1/720 inch).
   def page_top
   end

# :call-seq:
#               page_width                                     
#
# The width of the page in the device coordinate system -- measured in output page coordinates (1/720 inch).
   def page_width
   end

# :call-seq:
#               page_height                                     
#
# The height of the page in the device coordinate system -- measured in output page coordinates (1/720 inch).
   def page_height
   end
   

# :call-seq:
#               set_frame_sides(left, right, top, bottom) # sizes in page coords [0..1]                                 
#
# This command sets frame_left, frame_right, frame_top, and frame_bottom to the given values. 
   def set_frame_sides(left, right, top, bottom)
   end

# :call-seq:
#               frame_left                                     
#
# The position of the left of the frame in the page x coordinate system which runs from 0 at the left to 1 at the right.
# Initialized to 0.2 and changed by set_subframe.
   def frame_left
   end

# :call-seq:
#               frame_right                                     
#
# The position of the right of the frame in the page x coordinate system which runs from 0 at the left to 1 at the right.
# Initialized to 0.8 and changed by set_subframe.
   def frame_right
   end

# :call-seq:
#               frame_bottom                                     
#
# The position of the bottom of the frame in the page y coordinate system which runs from 0 at the bottom to 1 at the top.
# Initialized to 0.2 and changed by set_subframe.
   def frame_bottom
   end

# :call-seq:
#               frame_top                                     
#
# The position of the top of the frame in the page y coordinate system which runs from 0 at the bottom to 1 at the top.
# Initialized to 0.8 and changed by set_subframe.
   def frame_top
   end

# :call-seq:
#               frame_width                                     
#
# The width of the frame in page coordinates.
# Initialized to 0.6 and changed by set_subframe.
   def frame_width
   end

# :call-seq:
#               frame_height                                     
#
# The height of the frame in page coordinates.
# Initialized to 0.6 and changed by set_subframe.
   def frame_height
   end

# :call-seq:
#               bounds_left                                     
#
# The position of the left of the frame in the figure coordinate system.
# Initialized to 0.0 and changed by set_bounds.
   def bounds_left
   end

# :call-seq:
#               bounds_right                                     
#
# The position of the right of the frame in the figure coordinate system.
# Initialized to 1.0 and changed by set_bounds.
   def bounds_right
   end

# :call-seq:
#               bounds_bottom                                     
#
# The position of the bottom of the frame in the figure coordinate system.
# Initialized to 0.0 and changed by set_bounds.
   def bounds_bottom
   end

# :call-seq:
#               bounds_top                                     
#
# The position of the top of the frame in the figure coordinate system.
# Initialized to 1.0 and changed by set_bounds.
   def bounds_top
   end

# :call-seq:
#               bounds_width                                     
#
# The width of the frame in figure coordinates.
# Initialized to 1.0 and changed by set_bounds.
   def bounds_width
   end

# :call-seq:
#               bounds_height                                     
#
# The height of the frame in figure coordinates.
# Initialized to 1.0 and changed by set_bounds.
   def bounds_height
   end

# :call-seq:
#               bounds_xmin                                     
#
# The minimum x figure coordinate that is inside the frame.
# Initialized to 0.0 and changed by set_bounds.
   def bounds_xmin
   end

# :call-seq:
#               bounds_xmax                                     
#
# The maximum x figure coordinate that is inside the frame.
# Initialized to 1.0 and changed by set_bounds.
   def bounds_xmax
   end

# :call-seq:
#               bounds_ymin                                     
#
# The minimum y figure coordinate that is inside the frame.
# Initialized to 0.0 and changed by set_bounds.
   def bounds_ymin
   end

# :call-seq:
#               bounds_ymax                                     
#
# The maximum y figure coordinate that is inside the frame.
# Initialized to 1.0 and changed by set_bounds.
   def bounds_ymax
   end

=begin rdoc
Adjusts frame margins according to the entries in the dictionary argument.  
Note that this does not automatically adjust the clipping rectangle to the new frame.
If you want the clipping changed, call clip_to_frame after calling set_subframe.

Dictionary Entries
    'left_margin'      => a_float     # optional
    'right_margin'     => a_float     # optional
    'top_margin'       => a_float     # optional
    'bottom_margin'    => a_float     # optional
    'left'                            # alias for 'left_margin'
    'right'                           # alias for 'right_margin'
    'top'                             # alias for 'top_margin'
    'bottom'                          # alias for 'bottom_margin'

The following forms are also supported for calls to set_subframe:

    set_subframe('margins' => [ left, right, top, bottom ])
    set_subframe([ left, right, top, bottom ])

=end
    def set_subframe(dict=nil)
    end

=begin rdoc
Returns a dictionary with entries for 'left_margin' and 'right_margin' suitable for use with
set_subframe.  The margins are determined by the column specifications in the argument _dict_.
The leftmost column is number 1 and the number of columns equals the column number for the rightmost column.
The entries 'left_margin' and 'right_margin' determine the space outside the columns, and 'column_margin' is the space between
columns, all given as fractions of the frame width.  The space between the outer margins is divided to make room
for 'num_columns' of equally wide columns.  The returned margins bracket the requested 'column', or the
requested range of columns from 'first_column' to 'last_column', inclusive.  See also row_margins.

Dictionary Entries
    'left_margin'       => a_float      # default is 0
    'right_margin'      => a_float      # default is 0
    'column_margin'     => a_float      # default is 0
    'column'            => an_integer
    'first_column'      => an_integer   # default is 1
    'last_column'       => an_integer   # default is 'first_column'
    'num_columns'       => an_integer   # default is 'last_column'

Example

    def columns
        t.set_aspect_ratio(2)
        t.rescale(0.8)
        t.do_box_labels(
            'Blues, Reds, Greens',
            'Position',
            'Values for Colors')
        num_plots = 3
        t.subplot(t.column_margins('num_columns' => 3, 'column' => 1)) do
            t.right_edge_type = AXIS_HIDDEN
            blues
        end
        t.subplot(t.column_margins('num_columns' => 3, 'column' => 2)) do 
            t.yaxis_type = AXIS_WITH_TICKS_ONLY
            t.right_edge_type = AXIS_HIDDEN
            reds
        end
        t.subplot(t.column_margins('num_columns' => 3, 'column' => 3)) do
            t.yaxis_type = AXIS_WITH_TICKS_ONLY
            t.right_edge_type = AXIS_WITH_TICKS_ONLY
            greens
        end
    end
    
http://theory.kitp.ucsb.edu/~paxton/tioga_jpegs/Columns.jpg
=end
    def column_margins(dict)
    end
    
=begin rdoc
Returns a dictionary with entries for 'top_margin' and 'bottom_margin' suitable for use with
set_subframe.  The margins are determined by the row specifications in the argument _dict_.
The uppermost row is number 1 and the number of rows equals the row number for the bottom-most row.
The entries 'top_margin' and 'bottom_margin' determine the space above and below the rows, and 'row_margin' is the space between
rows, all given as fractions of the frame height.  The space between the outer margins is divided to make room
for 'num_rows' of equally tall rows.  The returned margins bracket the requested 'row', or the
requested range of rows from 'first_row' to 'last_row', inclusive.  See also column_margins.

Dictionary Entries
    'top_margin'        => a_float      # default is 0
    'bottom_margin'     => a_float      # default is 0
    'row_margin'        => a_float      # default is 0
    'row'               => an_integer
    'first_row'         => an_integer   # default is 1
    'last_row'          => an_integer   # default is 'first_row'
    'num_rows'          => an_integer   # default is 'last_row'

=end
    def row_margins(dict)
    end
        
=begin rdoc
Sets the bounds according to the entries in the dictionary argument (called by show_plot).

The values of the entries
are the figure coordinates for the edges of the frame.  The following attributes are changed:
bounds_left, bounds_right, bounds_bottom, bounds_top, xaxis_reversed, bounds_xmin, bounds_xmax,
bounds_width, yaxis_reversed, bounds_ymin, bounds_ymax, bounds_height,
default_text_height_dx, and default_text_height_dy.

Dictionary Entries
    'bounds_left' => a_float     # required
    'bounds_right' => a_float    # required
    'bounds_top' => a_float      # required
    'bounds_bottom' => a_float   # required
    'left_boundary'              # alias for 'bounds_left'
    'right_boundary'             # alias for 'bounds_right'
    'top_boundary'               # alias for 'bounds_top'
    'bottom_boundary'            # alias for 'bounds_bottom'

The following forms are also supported for calls to set_bounds:

    set_bounds('boundaries' => [ left, right, top, bottom ])
    set_bounds([ left, right, top, bottom ])
    
=end
    def set_bounds(dict=nil)
    end


    
# Convert the distance _d_ measured in output coordinates to millimeters.
    def convert_output_to_mm(d)
    end
    
# Convert the distance _d_ measured in millimeters to output coordinates.
    def convert_mm_to_output(d)
    end
    
# Convert the distance _d_ measured in output coordinates to inches.
    def convert_output_to_inches(d)
    end
    
# Convert the distance _d_ measured in inches to output coordinates.
    def convert_inches_to_output(d)
    end
    
    
# Convert the position _x_ measured in page x coordinates to the 
# position in output x coordinates.
    def convert_page_to_output_x(x)
    end

    
# Convert the position _y_ measured in page y coordinates to the 
# position in output y coordinates.
    def convert_page_to_output_y(y)
    end

# Convert the distance _dx_ measured in page x coordinates to the 
# distance in output x coordinates.
    def convert_page_to_output_dx(dx)
    end

# Convert the distance _dy_ measured in page y coordinates to the 
# distance in output y coordinates.
    def convert_page_to_output_dy(dy)
    end

# Convert the position _x_ measured in output x coordinates to the 
# position in page x coordinates.
    def convert_output_to_page_x(x)
    end

# Convert the position _y_ measured in output y coordinates to the 
# position in page y coordinates.
    def convert_output_to_page_y(y)
    end

# Convert the distance _dx_ measured in output x coordinates to the 
# distance in page x coordinates.
    def convert_output_to_page_dx(dx)
    end

# Convert the distance _dy_ measured in output y coordinates to the same
# distance in page y coordinates.
    def convert_output_to_page_dy(dy)
    end

# Convert the position _x_ measured in page x coordinates to the 
# position in frame x coordinates.
    def convert_page_to_frame_x(x)
    end

# Convert the position _y_ measured in page y coordinates to the 
# position in frame y coordinates.
    def convert_page_to_frame_y(y)
    end

# Convert the distance _dx_ measured in page x coordinates to the 
# distance in frame x coordinates.
    def convert_page_to_frame_dx(dx)
    end

# Convert the distance _dy_ measured in page y coordinates to the 
# distance in frame y coordinates.
    def convert_page_to_frame_dy(dy)
    end

# Convert the position _x_ measured in frame x coordinates to the 
# position in page x coordinates.
    def convert_frame_to_page_x(x)
    end

# Convert the position _y_ measured in frame y coordinates to the 
# position in page y coordinates.
    def convert_frame_to_page_y(y)
    end

# Convert the distance _dx_ measured in frame x coordinates to the 
# distance in page x coordinates.
    def convert_frame_to_page_dx(dx)
    end

# Convert the distance _dy_ measured in frame y coordinates to the 
# distance in page y coordinates.
    def convert_frame_to_page_dy(dy)
    end

# Convert the position _x_ measured in figure x coordinates to the 
# position in frame x coordinates.
    def convert_figure_to_frame_x(x)
    end

# Convert the position _y_ measured in figure y coordinates to the 
# position in frame y coordinates.
    def convert_figure_to_frame_y(y)
    end

# Convert the distance _dx_ measured in figure x coordinates to the 
# distance in frame x coordinates.
    def convert_figure_to_frame_dx(dx)
    end

# Convert the distance _dy_ measured in figure y coordinates to the 
# distance in frame y coordinates.
    def convert_figure_to_frame_dy(dy)
    end

# Convert the position _x_ measured in frame x coordinates to the 
# position in figure x coordinates.
    def convert_frame_to_figure_x(x)
    end

# Convert the position _y_ measured in frame y coordinates to the 
# position in figure y coordinates.
    def convert_frame_to_figure_y(y)
    end

# Convert the distance _dx_ measured in frame x coordinates to the 
# distance in figure x coordinates.
    def convert_frame_to_figure_dx(dx)
    end

# Convert the distance _dy_ measured in frame y coordinates to the 
# distance in figure y coordinates.
    def convert_frame_to_figure_dy(dy)
    end

# Convert the position _x_ measured in figure x coordinates to the 
# position in output x coordinates.
    def convert_figure_to_output_x(x)
    end

# Convert the position _y_ measured in figure y coordinates to the 
# position in output y coordinates.
    def convert_figure_to_output_y(y) 
    end

# Convert the distance _dx_ measured in figure x coordinates to the 
# distance in output x coordinates.
    def convert_figure_to_output_dx(dx)
    end

# Convert the distance _dy_ measured in figure y coordinates to the 
# distance in output y coordinates.
    def convert_figure_to_output_dy(dy)
    end

# Convert the position _x_ measured in output x coordinates to the 
# position in figure x coordinates.
    def convert_output_to_figure_x(x)
    end

# Convert the position _y_ measured in output y coordinates to the 
# position in figure y coordinates.
    def convert_output_to_figure_y(y)
    end

# Convert the distance _dx_ measured in output x coordinates to the 
# distance in figure x coordinates.
    def convert_output_to_figure_dx(dx)
    end

# Convert the distance _dy_ measured in output y coordinates to the 
# distance in figure y coordinates.
    def convert_output_to_figure_dy(dy)
    end

# Returns the angle measured in degrees clockwise from the horizontal for the
# slope specified by _dx_ and _dy_ given in figure coordinates.
    def convert_to_degrees(dx, dy)
    end







=begin rdoc
:call-seq:
               tex_preview_paper_width                                     
               tex_preview_paper_width = a_string

The string will be used as the paper width specification in the preview TeX file.  It will be inserted
into a TeX preamble 'setlength' command, so the string can be any valid length string.  For example, to set
the page width to 8.5 inches, set tex_preview_paper_width to '8.5in'; the preview preamble will then include
the following:

        \setlength{\paperwidth}{8.5in}
        
See also: tex_preview_paper_height, tex_preview_figure_width, and tex_preview_figure_height.
        
=end 
   def tex_preview_paper_width
   end


=begin rdoc
:call-seq:
               tex_preview_paper_height                                     
               tex_preview_paper_height = a_string

The string will be used as the paper height specification in the preview TeX file.  It will be inserted
into a TeX preamble 'setlength' command, so the string can be any valid length string.  For example, to set
the page height to 11 inches, set tex_preview_paper_height to '11in'; the preview preamble will then include
the following:

        \setlength{\paperheight}{11in}
        
See also: tex_preview_paper_width, tex_preview_figure_width, and tex_preview_figure_height.
        
=end 
   def tex_preview_paper_height
   end



=begin rdoc
:call-seq:
               tex_preview_figure_width                                     
               tex_preview_figure_width = a_string

The string will be used as the figure width specification in the call to the tex_preview_tiogafigure_command.
        
See also: tex_preview_hoffset, tex_preview_figure_height, tex_preview_paper_width, and tex_preview_paper_height.
        
=end 
   def tex_preview_figure_width
   end

=begin rdoc
:call-seq:
               tex_preview_figure_height                                     
               tex_preview_figure_height = a_string

The string will be used as the figure height specification in the call to the tex_preview_tiogafigure_command.
        
See also: tex_preview_voffset, tex_preview_figure_width, tex_preview_paper_width, and tex_preview_paper_height.
        
=end 
   def tex_preview_figure_height
   end


=begin rdoc
:call-seq:
               tex_preview_hoffset                                     
               tex_preview_hoffset = a_string

This string will be used as the horizontal offset specification in the preview TeX file.  It will be inserted
into a TeX preamble 'setlength' command, so the string can be any valid length string.  The figure will be
shifted horizontally by the offset distance (positive means to the right, negative to the left).
        
See also: tex_preview_voffset, tex_preview_paper_height, tex_preview_figure_width, and tex_preview_figure_height.
        
=end 
   def tex_preview_hoffset
   end

=begin rdoc
:call-seq:
               tex_preview_voffset                                     
               tex_preview_voffset = a_string

This string will be used as the vertical offset specification in the preview TeX file.  It will be inserted
into a TeX preamble 'setlength' command, so the string can be any valid length string.  The figure will be
shifted vertically by the offset distance (positive means down, negative up).
        
See also: tex_preview_hoffset, tex_preview_paper_height, tex_preview_figure_width, and tex_preview_figure_height.
        
=end 
   def tex_preview_voffset
   end






=begin rdoc

Sets tex_preview_paper_width and tex_preview_paper_height to the A4 standard values (297mm and 210mm). 
        
See also: set_A4_portrait, set_A5_landscape, set_A5_portrait, set_B5_landscape, and set_B5_portrait.
        
=end 
   def set_A4_landscape
   end


=begin rdoc

Sets tex_preview_paper_width and tex_preview_paper_height to the A5 standard values (210mm and 148mm).
        
See also: set_A5_portrait, set_A4_landscape, set_A4_portrait, set_B5_landscape, and set_B5_portrait.
        
=end 
   def set_A5_landscape
   end


=begin rdoc

Sets tex_preview_paper_width and tex_preview_paper_height to the B5 standard values (250mm and 176mm).
        
See also: set_B5_portrait, set_A5_landscape, set_A5_portrait, set_A4_landscape, and set_A4_portrait.
        
=end 
   def set_B5_landscape
   end




=begin rdoc

Sets tex_preview_paper_width and tex_preview_paper_height to the A4 standard values (210mm and 297mm).
        
See also: set_A4_landscape, set_A5_landscape, set_A5_portrait, set_B5_landscape, and set_B5_portrait.
        
=end 
   def set_A4_portrait
   end


=begin rdoc

Sets tex_preview_paper_width and tex_preview_paper_height to the A5 standard values (148mm and 210mm).
        
See also: set_A5_portrait, set_A4_landscape, set_A4_portrait, set_B5_landscape, and set_B5_portrait.
        
=end 
   def set_A5_portrait
   end

=begin rdoc

Sets tex_preview_paper_width and tex_preview_paper_height to the B5 standard values (176mm and 250mm).
        
See also: set_B5_landscape, set_A4_landscape, set_A4_portrait, set_A5_landscape, and set_A5_portrait.
        
=end 
   def set_B5_portrait
   end



=begin rdoc

Sets tex_preview_paper_width and tex_preview_paper_height to the JB5 standard values (182mm and 257mm).
        
See also: set_JB5_landscape, set_B5_landscape, and set_B5_portrait.
        
=end 
   def set_JB5_portrait
   end


=begin rdoc

Sets tex_preview_paper_width and tex_preview_paper_height to the JB5 standard values (257mm and 182mm).
        
See also: set_JB5_landscape, set_B5_landscape, and set_B5_portrait.
        
=end 
   def set_JB5_landscape
   end


=begin rdoc

Sets tex_preview_paper_width and tex_preview_paper_height to the US letter standard values (11in and 8.5in).
        
See also: set_USLetter_portrait, set_USLegal_portrait, and set_USLegal_landscape.
        
=end 
   def set_USLetter_landscape
   end

=begin rdoc

Sets tex_preview_paper_width and tex_preview_paper_height to the US letter standard values (8.5in and 11in).
        
See also: set_USLetter_landscape, set_USLegal_portrait, and set_USLegal_landscape.
        
=end 
   def set_USLetter_portrait
   end



=begin rdoc

Sets tex_preview_paper_width and tex_preview_paper_height to the US letter standard values (14in and 8.5in).
        
See also: set_USLegal_portrait, set_USLetter_portrait, and set_USLetter_landscape.
        
=end 
   def set_USLegal_landscape
   end

=begin rdoc

Sets tex_preview_paper_width and tex_preview_paper_height to the US letter standard values (8.5in and 14in).
        
See also: set_USLegal_landscape, set_USLetter_portrait, and set_USLetter_landscape.
        
=end 
   def set_USLegal_portrait
   end



# :call-seq:
#               tex_preview_tiogafigure_command                                     
#               tex_preview_tiogafigure_command = a_string
#
# This string will be used for showing the tioga figure in the sample TeX file (default is "tiogafigurescaledtofit").
   def tex_preview_tiogafigure_command
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
#               tex_preview_fullpage                                     
#               tex_preview_fullpage = true or false
#
# Defaults to +true+.
# If +true+, then preview tex file will use the tiogafigurefullpage command.
# If the tex_preview_minwhitespace has been set, it will be used in the call.
   def tex_preview_fullpage
   end


=begin rdoc
:call-seq:
               tex_preview_minwhitespace                                     
               tex_preview_minwhitespace = a_string # giving the length

This string will be used as the minwhitespace optional argument for the tiogafigurefullpage command.
        
See also: tex_preview_fullpage.
=end
   def tex_preview_minwhitespace
   end

=begin rdoc
:call-seq:
               tex_preview_fontsize                                     
               tex_preview_fontsize = a_string  # giving the size in units of "big points" (1bp = 1/72 inch)

This string will be used as the basic font size specification in the preview TeX file.  (Note that this is the
same font size specification as used in PostScript and PDF -- "10 scalefont" in PostScript gives a font that is
10 units in size where the unit is defined as 1/72 inch.  That unit has become known as a "big point" in comparison to
the printer point which is defined to be 1/72.27 inch, so there are 1.00375 points per big point.)
        
See also: tex_preview_fontfamily, tex_preview_fontseries, and tex_preview_fontshape.
=end
   def tex_preview_fontsize
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
