#  Rectangles.rb

module Tioga

# These are the methods for creating and using rectangular paths for PDF graphics operations.

class Rectangles < Doc < FigureMaker

# Add a rectangle with corners at (_x_, _y_), (_x_ + _width_, _y_),
# (_x_ + _width_, _y_ + _height_), and (_x_, _y_ + _height_).
   def append_rect_to_path(x, y, width, height)
   end
   
# Calls append_rect_to_path followed by #fill.
   def fill_rect(x, y, width, height)
   end

# Calls append_rect_to_path followed by #stroke.
   def stroke_rect(x, y, width, height)
   end

# Calls append_rect_to_path followed by fill_and_stroke.
   def fill_and_stroke_rect(x, y, width, height)
   end
   
# Calls append_rect_to_path followed by #clip.
   def clip_rect(x, y, width, height)
   end
   
# :call-seq:
#               append_frame_to_path                         
#                   
# Calls append_rect_to_path with the current frame rectangle.
   def append_frame_to_path
   end

# :call-seq:
#               stroke_frame                         
#                   
# Calls append_frame_to_path followed by #stroke.
   def stroke_frame
   end

# :call-seq:
#               fill_frame                         
#                   
# Calls append_frame_to_path followed by #fill.
   def fill_frame
   end

# :call-seq:
#               fill_and_stroke_frame                         
#                   
# Calls append_frame_to_path followed by fill_and_stroke.
   def fill_and_stroke_frame
   end

# :call-seq:
#               clip_to_frame                         
#                   
# Calls append_frame_to_path followed by #clip.
   def clip_to_frame
   end
   
# Like append_rect_to_path, but with corners rounded with
# curvatures given by _dx_ and _dy_.
#
# The illustration shows a rounded rectangle stroked and used as a clipping path
# for showing the image.
#
# link:images/append_rounded_rect.png
#
   def append_rounded_rect_to_path(x, y, width, height, dx, dy)
   end
   
# Calls append_rounded_rect_to_path followed by #clip.
   def clip_rounded_rect(x, y, width, height, dx, dy)
   end
   
# Calls append_rounded_rect_to_path followed by #fill.
   def fill_rounded_rect(x, y, width, height, dx, dy)
   end
   
# Calls append_rounded_rect_to_path followed by #stroke.
   def stroke_rounded_rect(x, y, width, height, dx, dy)
   end

# Calls append_rounded_rect_to_path followed by fill_and_stroke.
   def fill_and_stroke_rounded_rect(x, y, width, height, dx, dy)
   end


end # class
end # module Tioga
