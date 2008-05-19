#  Colormaps.rb

module Tioga

# These are the methods for creating and using colormaps and color spaces in PDF graphics.

class Colormaps < Doc < FigureMaker

=begin rdoc
Returns a color map as specified by the dictionary argument _dict_.  The 'length' parameter determines the
number of entries in the map and must be an integer between 2 and 256.  The 'points' vector gives positions
between 0.0 and 1.0 where the colors are explicitly specified for the map.  The first color map entry corresponds to
position 0.0, the last map entry is for position 1.0, and intermediate map entries are spaced evenly in between.
The color specifications are given either as RGB triples (in the vectors 'Rs', 'Gs', and 'Bs') or as
HLS triples (in the vectors 'Hs', 'Ls', and 'Ss').  In either case, there must be the same number of triples as
there are positions in the 'points' vector.  The color position points[i] is assigned the color from the
corresponding Rs[i], Gs[i], and Bs[i] for the RGB case.  In the HLS case, the triple Hs[i], Ls[i], and Ss[i]
is converted to a triple of RGB intensities, and that is assigned to the color position given by points[i].  Color map
entries for other positions are determined by linear interpolation from the neighboring assigned colors.

See also the prebuilt colormaps: intense_colormap, mellow_colormap, and rainbow_colormap.

Dictionary Entries
    'length'   => an_integer      # integer between 2 and 256.  default is 256.
    'points'   => a_dvector       # increasing locations starting at 0.0 and ending at 1.0
    'Rs'       => a_dvector       # red intensities (between 0.0 and 1.0)
    'Gs'       => a_dvector       # green intensities (between 0.0 and 1.0)
    'Bs'       => a_dvector       # blue intensities (between 0.0 and 1.0)
    'Hs'       => a_dvector       # hue angles in degrees (between 0 and 360)
    'Ls'       => a_dvector       # lightness (between 0.0 and 1.0)
    'Ss'       => a_dvector       # saturation (between 0.0 and 1.0)

=end
    def create_colormap(dict)
    end

# :call-seq:
#  intense_colormap
#
# Returns a colormap with fully saturated reds and blues.
    def intense_colormap
    end

# :call-seq:
#  mellow_colormap
#
# Returns a colormap with reds and blues in which the saturation drops for high and low color positions,
    def mellow_colormap 
    end

# :call-seq:
#  rainbow_colormap
#
# Returns a colormap covering the spectrum from red, orange, yellow, green, blue, indigo, to violet
# (special thanks to Roy G. Biv for providing this).
    def rainbow_colormap
    end

=begin rdoc
Returns a color map for a gradient of HLS colors as specified by the dictionary argument _dict_.
The hue is either fixed ('hue') or varies linearly (from 'starting_H' to 'ending_H').
The lightness is either fixed ('lightness') or varies linearly (from 'starting_L' to 'ending_L').
The saturation is either fixed ('saturation') or varies linearly (from 'starting_S' to 'ending_S').

Dictionary Entries
    'hue'             => a_float   # hue angle in degrees (default 0)
    'starting_H'      => a_float   # the starting hue angle
    'ending_H'        => a_float   # the ending hue angle
    'lightness'       => a_float   # lightness (default 0.5)
    'starting_L'      => a_float   # the starting lightness
    'ending_L'        => a_float   # the ending lightness
    'saturation'      => a_float   # saturation (default 0.5)
    'starting_S'      => a_float   # the starting saturation
    'ending_S'        => a_float   # the ending saturation

=end
    def create_gradient_colormap(dict)
    end

# Returns a colormap corresponding to the given vectors of intensities.
# The intensities must be numbers between 0 and 1, and the vectors must
# be of equal lengths.
   def convert_to_colormap(reds, greens, blues)
   end

# Returns the triple [ red, green, blue ] for the intensities of the color
# at the given <i>color_position</i> in _colormap_.  Recall that a color position
# is a number between 0 and 1.  See create_colormap.
   def get_color_from_colormap(colormap, color_position)
   end

# Returns a vector of [ red, green, blue ] intensities corresponding to the
# <i>hls_vec</i> color given as [ hue, lightness, saturation ].  See also rgb_to_hls.
    def hls_to_rgb(hls_vec)
    end
    
# Returns a vector of [ hue, lightness, saturation ] corresponding to the
# <i>rgb_vec</i> color given as [ red, green, blue ] intensities.  See also hls_to_rgb.
    def rgb_to_hls(rgb_vec)
    end


# Returns a new string of [ red, green, blue ] byte triples corresponding to the
# colors given in <i>str</i> as triples of [ hue, lightness, saturation ].
# For representation as a byte, intensities in range 0.0 to 1.0 are multiplied by 255 and rounded;
# hue angles in the range 0.0 to 360.0 are divided by 360, multiplied by 255, and rounded.
# See also string_hls_to_rgb!.
    def string_hls_to_rgb(str)
    end

# Modifies in-place the string of [ red, green, blue ] byte triples to give the corresponding
# colors as triples of [ hue, lightness, saturation ].  
# See also string_hls_to_rgb.
    def string_hls_to_rgb!(str)
    end
    
# Returns a new string of [ hue, lightness, saturation ] byte triples corresponding to the
# colors given in <i>str</i> as triples of [ red, green, blue ].  
# For representation as a byte, intensities in range 0.0 to 1.0 are multiplied by 255 and rounded;
# hue angles in the range 0.0 to 360.0 are divided by 360, multiplied by 255, and rounded.
# See also string_rgb_to_hls!.
    def string_rgb_to_hls(str)
    end
    
# Modifies in-place the string of [ hue, lightness, saturation ] byte triples to give the corresponding
# colors as triples of [ red, green, blue ].  
# See also string_rgb_to_hls.
    def string_rgb_to_hls!(str)
    end





end # class
end # module Tioga
