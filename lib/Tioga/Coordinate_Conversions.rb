#  Coordinate_Conversions.rb

module Tioga

=begin rdoc

There are four different coordinate systems used in Tioga.  The contents of the figure or plot
are positioned using "figure" coordinates which correspond to the values along the x and y axes
as determined by the boundary attributes.  Things like the title and axis labels are positioned using
"frame" coordinates that run from (0, 0) at the lower left corner of the frame to (1, 1) at the upper right
corner.  The frame is positioned on the page using "page" coordinates that run from (0, 0)
at the lower left of the page to (1, 1) at the upper right.  The actual numbers used in PDF and TeX files
are given in "output" coordinates which are a by default tenth of a point (i.e., 720 in output coords equals 1 inch) (but you can redefined the scaling factor as the optional argument to FigureMaker.new).

The coordinate systems are naturally related in the following manner:

    figure <-> frame <-> page <-> output
    
There are conversion routines between each adjacent pair in this chain.  There are separate
routines for locations (x and y) and for distances (dx and dy).  And finally, the chain is closed
by conversion routines between figure and output coordinates.

=end

module Coordinate_Conversions

  # Returns the scaling factor from postscript points to output
  # coordinates (10 by default)
  def scaling_factor
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


end # module CoordinateConversions

end # module Tioga
