#  Transparency.rb

module Tioga

# These are the attributes for using transparency in PDF graphics.  The transparency is speficied by a fraction, with 1.0 for fully transparent and 0.0 for fully opaque.  Since transparency can also be thought of in terms of opacity, there are accessors defined in those terms also.  Thus, for example, setting fill_opacity to 0.2 is equivalent to setting fill_transparency to 0.8. 

class Transparency < Doc < FigureMaker

# :call-seq:
#               stroke_opacity                                     
#               stroke_opacity = a_float
#
# Defines the opacity to use for #stroke operations.
# Valid values are between 1.0 (fully opaque) to 0.0 (fully transparent).  Default is 1.0 (opaque).
   def stroke_opacity
   end
   
# :call-seq:
#               opacity_for_stroke                                     
#               opacity_for_stroke = a_float
#
# Alias for stroke_opacity.
   def opacity_for_stroke
   end
   
# :call-seq:
#               fill_opacity                                     
#               fill_opacity = a_float
#
# Defines the opacity to use for #fill and other non-stroke graphics operations including images.
# Valid values are between 1.0 (fully opaque) to 0.0 (fully transparent).  Default is 1.0 (opaque).
#
# Example: on left, fill_opacity = 1.0; on right fill_opacity = 0.6
#
# link:images/Squares.png
   def fill_opacity
   end
   
# :call-seq:
#               opacity_for_fill                                     
#               opacity_for_fill = a_float
#
# Alias for fill_opacity.
   def opacity_for_fill
   end
   
   
   
# :call-seq:
#               stroke_transparency                                     
#               stroke_transparency = a_float
#
# Defines the transparency to use for #stroke operations.
# Valid values are between 0.0 (fully opaque) to 1.0 (fully transparent).  Default is 0.0 (opaque).
   def stroke_transparency
   end
   
# :call-seq:
#               transparency_for_stroke                                     
#               transparency_for_stroke = a_float
#
# Alias for stroke_transparency.
   def transparency_for_stroke
   end
   
# :call-seq:
#               fill_transparency                                     
#               fill_transparency = a_float
#
# Defines the transparency to use for #fill and other non-stroke graphics operations including images.
# Valid values are between 0.0 (fully opaque) to 1.0 (fully transparent).  Default is 0.0 (opaque).
#
# Example: on left, fill_transparency = 0.0; on right fill_transparency = 0.4
#
# link:images/Squares.png
   def fill_transparency
   end
   
# :call-seq:
#               transparency_for_fill                                     
#               transparency_for_fill = a_float
#
# Alias for fill_transparency.
   def transparency_for_fill
   end
   

end # class
end # module Tioga
