#  Arcs_and_Circles.rb

module Tioga

# These are the methods for creating and using arcs and circles in PDF graphics.

class Arcs_and_Circles < Doc < FigureMaker

# Like adding lines from _start_ to _corner_ to _end_, but with the corner rounded
# with a radius equal to the minimum of the actual output distances for _dx_ and _dy_.
#
# The illustration shows in dark blue the arc that is added for the control points given in red.
#
# link:images/append_arc.png
#
   def append_arc_to_path(x_start, y_start, x_corner, y_corner, x_end, y_end, dx, dy)
   end

# Create a circle with center at (_x_, _y_) having radius _r_ (given in x units).
   def append_circle_to_path(x, y, r)
   end

# Calls append_circle_to_path followed by #clip.
   def clip_circle(x, y, r)
   end
   
# Calls append_circle_to_path followed by #fill.
   def fill_circle(x, y, r)
   end

# Calls append_circle_to_path followed by #stroke.
   def stroke_circle(x, y, r)
   end

# Calls append_circle_to_path followed by fill_and_stroke.
   def fill_and_stroke_circle(x, y, r)
   end
   
# Equivalent to the following: create a unit circle with center at (_x_, _y_), scale it by _dx_ in the x direction,
# scale it by _dy_ in the y direction, then rotate it counter-clockwise by _angle_ degrees.
   def append_oval_to_path(x, y, dx, dy, angle)
   end

# Calls append_oval_to_path followed by #clip.
   def clip_oval(x, y, dx, dy, angle)
   end
   
# Calls append_oval_to_path followed by #fill.
   def fill_oval(x, y, dx, dy, angle)
   end
   
# Calls append_oval_to_path followed by #stroke.
   def stroke_oval(x, y, dx, dy, angle)
   end

# Calls append_oval_to_path followed by fill_and_stroke.
   def fill_and_stroke_oval(x, y, dx, dy, angle)
   end




end # class
end # module Tioga
