#  Page_Frame_Bounds.rb

module Tioga

# These are the methods and attributes related to setting the coordinate systems and margins for plots.  See also Coordinate_Conversions and the Tutorial::DocStructure.

class Page_Frame_Bounds < Doc < FigureMaker

# :call-seq:
#               page_left                                     
#
# The position of the left of the page in the output coordinate system.
   def page_left
   end

# :call-seq:
#               page_right                                     
#
# The position of the right of the page in the output coordinate system.
   def page_right
   end

# :call-seq:
#               page_bottom                                     
#
# The position of the bottom of the page in the output coordinate system.
   def page_bottom
   end

# :call-seq:
#               page_top                                     
#
# The position of the top of the page in the output coordinate system.
   def page_top
   end

# :call-seq:
#               page_width                                     
#
# The width of the page in output coordinates.
   def page_width
   end

# :call-seq:
#               page_height                                     
#
# The height of the page in output coordinates.
   def page_height
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






end # class
end # module Tioga
