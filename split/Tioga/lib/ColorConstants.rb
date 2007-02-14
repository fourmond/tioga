module Tioga

# = Color names
#
# The table at the end of this webpage has about 200 color names
# and the corresponding red-green-blue triples of intensities.  These color names can be used wherever a color is called for in tioga.
# To use the same colors in text, your TeX preamble should have the following:
#        \include{color_names}
# The file <tt>color_names.tex</tt> is included in the tioga download and defines the full set of
# color names given below.  To set some text in color, simply use something like the following:
#        \textcolor{Crimson}{This will appear in crimson.}
#
# By the way, if you want to use color_names.tex, but don't want copies of it scattered around, you can
# put it in a directory that is on the LaTeX search path.  Check your system for details.  On Linux, I believe
# you can "export" the path to TEXINPUTS.  On the Mac, you can try putting the file in ~/Library/texmf/tex/latex.
# Then add the needed line to your TeX files by doing this in your initialize method for the plots:
#       t.tex_preamble += "\n\\include{color_names}\n"
#
# In addition to the list of predefined color names, there is a table below giving a small sample of each color.
#
# ---
#
# = Concerning the use of colors in plots
#
# When you pick colors for your plots, keep in mind that color deficiency, commonly known as "color blindness",
# is a common genetic trait.  People with color deficiency are not blind to colors, they simply don't distinguish
# as many different colors as someone with normal color vision.  For example, reds and greens might be the same
# ("reens" or "geds"?) and only brightness would show up as different.  But the reds/greens would still appear to be
# different colors than the blues. 
#
# Two common forms of color deficiency are called "protanopia" and "deuteranopia".  Here's how they are
# described on the website {What Is Colorblindness}[http://colorvisiontesting.com/color2.htm].
#
# - For the protanope, the brightness of red, orange, and yellow is much reduced compared to normal. This dimming can be so pronounced that reds may be confused with black or dark gray, and red traffic lights may appear to be extinguished. They may learn to distinguish reds from yellows and from greens primarily on the basis of their apparent brightness or lightness, not on any perceptible hue difference. Violet, lavender, and purple are indistinguishable from various shades of blue because their reddish components are so dimmed as to be invisible. E.g. Pink flowers, reflecting both red light and blue light, may appear just blue to the protanope.
#
# link:images/normalcolor72gif.png
# link:images/protanope72gif.png
#
# - The deuteranope suffers the same hue discrimination problems as the protanope, but without the abnormal dimming. The names red, orange, yellow, and green really mean very little to him aside from being different names that every one else around him seems to be able to agree on. Similarly, violet, lavender, purple, and blue, seem to be too many names to use logically for hues that all look alike to him.
#
# link:images/normalcolor72gif.png
# link:images/deuteranope72gif.png
#
# ---
#
# To be considerate of your color deficient friends, pick color sets that are distinguishable for them too.
# Here's an example of a plot using six colors.
# In the first version on the left, the colors of the lines are BrightBlue, Goldenrod, Coral, Lilac, FireBrick, and RoyalPurple.
# The lower left version uses the often seen combination of red, green, blue, cyan, magenta, and yellow.
#
# On the right are the same plots as they might appear to someone with protanopia
# (made using VischeckURL[http://www.vischeck.com/vischeck/vischeckURL.php] -- a very useful site).
#
# link:images/color_choices.png
#
# By using the first set of colors rather than the second, you get a plot that looks good for
# those in your audience with normal color vision and is much easier on those with a color deficiency.
# Why not do it?
#
# ---
#
# ---
#
# link:images/colors1.png
#
# link:images/colors2.png
#
# link:images/colors3.png
#
# link:images/colors4.png
#

module ColorConstants
    AliceBlue = [ 0.94, 0.972, 1 ]
    AntiqueWhite = [ 0.98, 0.92, 0.844 ]
    Aqua = [ 0, 1, 1 ]
    Aquamarine = [ 0.498, 1, 0.83 ]
    Avocado = [ 0.6, 0.6, 0 ]
    Azure = [ 0.94, 1, 1 ]
    Beige = [ 0.96, 0.96, 0.864 ]
    Bisque = [ 1, 0.894, 0.77 ]
    Black = [ 0, 0, 0 ]
    BlanchedAlmond = [ 1, 0.92, 0.804 ]
    Blue = [ 0, 0, 1 ]
    BlueGreen = [ 0, 0.6, 0.4 ]
    BlueViolet = [ 0.54, 0.17, 0.888 ]
    BrickRed = [ 0.645, 0, 0.129 ]
    BrightBlue = [ 0, 0.4, 1.0 ]
    BrightPink = [ 1.0, 0.4, 0.8 ]
    Brown = [ 0.648, 0.165, 0.165 ]
    Burgundy = [ 0.6, 0, 0.2 ]
    BurlyWood = [ 0.87, 0.72, 0.53 ]
    CadetBlue = [ 0.372, 0.62, 0.628 ]
    Cement = [ 0.8, 0.8, 0.6 ]
    Chartreuse = [ 0.498, 1, 0 ]
    Chiffon = [ 0.98, 0.98, 0.824 ]
    Chocolate = [ 0.824, 0.41, 0.116 ]
    Coral = [ 1, 0.498, 0.312 ]
    CornflowerBlue = [ 0.392, 0.585, 0.93 ]
    Cornsilk = [ 1, 0.972, 0.864 ]
    Crimson = [ 0.8, 0, 0.2 ]
    Cyan = [ 0, 1, 1 ]
    DarkBlue = [ 0, 0, 0.545 ]
    DarkChocolate = [ 0.4, 0.2, 0 ]
    DarkCyan = [ 0, 0.545, 0.545 ]
    DarkGoldenrod = [ 0.72, 0.525, 0.044 ]
    DarkGray = [ 0.664, 0.664, 0.664 ]
    DarkGreen = [ 0, 0.392, 0 ]
    DarkGrey = [ 0.664, 0.664, 0.664 ]
    DarkKhaki = [ 0.74, 0.716, 0.42 ]
    DarkLavender = [ 0.4, 0.2, 0.6 ]
    DarkMagenta = [ 0.545, 0, 0.545 ]
    DarkOliveGreen = [ 0.332, 0.42, 0.185 ]
    DarkOrange = [ 1, 0.55, 0 ]
    DarkOrchid = [ 0.6, 0.196, 0.8 ]
    DarkPeriwinkle = [ 0.4, 0.4, 1.0 ]
    DarkPurpleBlue = [ 0.4, 0, 0.8 ]
    DarkRed = [ 0.545, 0, 0 ]
    DarkRoyalBlue = [ 0, 0.2, 0.8 ]
    DarkSalmon = [ 0.912, 0.59, 0.48 ]
    DarkSeaGreen = [ 0.56, 0.736, 0.56 ]
    DarkSlateBlue = [ 0.284, 0.24, 0.545 ]
    DarkSlateGray = [ 0.185, 0.31, 0.31 ]
    DarkSlateGrey = [ 0.185, 0.31, 0.31 ]
    DarkSmoke = [ 0.92, 0.92, 0.92 ]
    DarkTurquoise = [ 0, 0.808, 0.82 ]
    DarkViolet = [ 0.58, 0, 0.828 ]
    DeepPink = [ 1, 0.08, 0.576 ]
    DeepSkyBlue = [ 0, 0.75, 1 ]
    DimGray = [ 0.41, 0.41, 0.41 ]
    DimGrey = [ 0.41, 0.41, 0.41 ]
    DodgerBlue = [ 0.116, 0.565, 1 ]
    FireBrick = [ 0.698, 0.132, 0.132 ]
    FloralWhite = [ 1, 0.98, 0.94 ]
    ForestGreen = [ 0.132, 0.545, 0.132 ]
    Fuchsia = [ 1, 0, 1 ]
    Gainsboro = [ 0.864, 0.864, 0.864 ]
    GhostWhite = [ 0.972, 0.972, 1 ]
    Gold = [ 1, 0.844, 0 ]
    GoldenBrown = [ 0.6, 0.4, 0 ]
    Goldenrod = [ 0.855, 0.648, 0.125 ]
    GrassGreen = [ 0.2, 0.6, 0 ]
    Gray = [ 0.5, 0.5, 0.5 ]
    GrayBlue = [ 0, 0.4, 0.6 ]
    Green = [ 0, 0.5, 0 ]
    GreenYellow = [ 0.68, 1, 0.185 ]
    Grey = [ 0.5, 0.5, 0.5 ]
    Honeydew = [ 0.94, 1, 0.94 ]
    HotPink = [ 1, 0.41, 0.705 ]
    IndianRed = [ 0.804, 0.36, 0.36 ]
    Indigo = [ 0.294, 0, 0.51 ]
    Ivory = [ 1, 1, 0.94 ]
    Khaki = [ 0.94, 0.9, 0.55 ]
    Lavender = [ 0.9, 0.9, 0.98 ]
    LavenderBlue = [ 0.4, 0.2, 1.0 ]
    LavenderBlush = [ 1, 0.94, 0.96 ]
    LawnGreen = [ 0.488, 0.99, 0 ]
    LemonChiffon = [ 1, 0.98, 0.804 ]
    LightBlue = [ 0.68, 0.848, 0.9 ]
    LightBrightGreen = [ 0, 0.8, 0.2 ]
    LightCoral = [ 0.94, 0.5, 0.5 ]
    LightCrimson = [ 0.864, 0.08, 0.235 ]
    LightCyan = [ 0.88, 1, 1 ]
    LightDullGreen = [ 0.4, 1.0, 0.6 ]
    LightGold = [ 0.8, 0.8, 0.4 ]
    LightGrassGreen = [ 0.4, 1.0, 0.4 ]
    LightGray = [ 0.828, 0.828, 0.828 ]
    LightGreen = [ 0.565, 0.932, 0.565 ]
    LightGrey = [ 0.828, 0.828, 0.828 ]
    LightMustard = [ 1.0, 0.8, 0.4 ]
    LightOliveGreen = [ 0.6, 0.8, 0.6 ]
    LightOrchid = [ 0.6, 0.4, 0.8 ]
    LightPlum = [ 0.8, 0.6, 0.8 ]
    LightRose = [ 1.0, 0.6, 0.8 ]
    LightSalmon = [ 1, 0.628, 0.48 ]
    LightSandyBrown = [ 1.0, 0.8, 0.6 ]
    LightSeaGreen = [ 0.125, 0.698, 0.668 ]
    LightSienna = [ 0.8, 0.4, 0 ]
    LightSkyBlue = [ 0.53, 0.808, 0.98 ]
    LightSlateGray = [ 0.468, 0.532, 0.6 ]
    LightSlateGrey = [ 0.468, 0.532, 0.6 ]
    LightSteelBlue = [ 0.69, 0.77, 0.87 ]
    LightTurquoise = [ 0.2, 1.0, 0.8 ]
    LightYellow = [ 1, 1, 0.88 ]
    LightYellowGreen = [ 0.8, 0.8, 0.2 ]
    Lilac = [ 0.8, 0.6, 1.0 ]
    Lime = [ 0, 1, 0 ]
    LimeGreen = [ 0.196, 0.804, 0.196 ]
    Linen = [ 0.98, 0.94, 0.9 ]
    Magenta = [ 1, 0, 1 ]
    Maroon = [ 0.5, 0, 0 ]
    Mauve = [ 0.8, 0.2, 0.4 ]
    MediumAquamarine = [ 0.4, 0.804, 0.668 ]
    MediumBlue = [ 0, 0, 0.804 ]
    MediumGreen = [ 0, 0.6, 0 ]
    MediumOrange = [ 1.0, 0.4, 0 ]
    MediumOrchid = [ 0.73, 0.332, 0.828 ]
    MediumPurple = [ 0.576, 0.44, 0.86 ]
    MediumSeaGreen = [ 0.235, 0.7, 0.444 ]
    MediumSlateBlue = [ 0.484, 0.408, 0.932 ]
    MediumSpringGreen = [ 0, 0.98, 0.604 ]
    MediumTurquoise = [ 0.284, 0.82, 0.8 ]
    MediumVioletRed = [ 0.78, 0.084, 0.52 ]
    YellowGreen = [ 0.8, 0.8, 0 ]
    MidnightBlue = [ 0.098, 0.098, 0.44 ]
    MintCream = [ 0.96, 1, 0.98 ]
    MistyRose = [ 1, 0.894, 0.884 ]
    Moccasin = [ 1, 0.894, 0.71 ]
    MustardSeed = [ 0.8, 0.6, 0 ]
    NavajoWhite = [ 1, 0.87, 0.68 ]
    Navy = [ 0, 0, 0.5 ]
    OldLace = [ 0.992, 0.96, 0.9 ]
    Olive = [ 0.5, 0.5, 0 ]
    OliveDrab = [ 0.42, 0.556, 0.136 ]
    Orange = [ 1, 0.648, 0 ]
    OrangeRed = [ 1, 0.27, 0 ]
    Orchid = [ 0.855, 0.44, 0.84 ]
    PaleGoldenrod = [ 0.932, 0.91, 0.668 ]
    PaleGreen = [ 0.596, 0.985, 0.596 ]
    PaleTurquoise = [ 0.688, 0.932, 0.932 ]
    PaleVioletRed = [ 0.86, 0.44, 0.576 ]
    PapayaWhip = [ 1, 0.936, 0.835 ]
    PeachPuff = [ 1, 0.855, 0.725 ]
    Periwinkle = [ 0.6, 0, 1.0 ]
    Peru = [ 0.804, 0.52, 0.248 ]
    Pink = [ 1, 0.752, 0.796 ]
    Plum = [ 0.868, 0.628, 0.868 ]
    PowderBlue = [ 0.69, 0.88, 0.9 ]
    Pumpkin = [ 1.0, 0.6, 0.2 ]
    Purple = [ 0.5, 0, 0.5 ]
    PurpleBlue = [ 0.4, 0.2, 0.8 ]
    PurpleGray = [ 0.6, 0.6, 0.8 ]
    Red = [ 1, 0, 0 ]
    RedBrown = [ 0.8, 0.4, 0.2 ]
    RedOrange = [ 0.8, 0.2, 0 ]
    Rose = [ 1.0, 0.4, 0.6 ]
    RosyBrown = [ 0.736, 0.56, 0.56 ]
    RoyalBlue = [ 0.255, 0.41, 0.884 ]
    RoyalPurple = [ 0.4, 0, 0.6 ]
    SaddleBrown = [ 0.545, 0.27, 0.075 ]
    LightChartreuse = [ 0.8, 1.0, 0.4 ]
    Saffron = [ 1.0, 0.8, 0 ]
    Salmon = [ 0.98, 0.5, 0.448 ]
    SalmonRed = [ 1.0, 0.4, 0.4 ]
    SandyBrown = [ 0.956, 0.644, 0.376 ]
    SeaGreen = [ 0.18, 0.545, 0.34 ]
    Seashell = [ 1, 0.96, 0.932 ]
    Sienna = [ 0.628, 0.32, 0.176 ]
    Silver = [ 0.752, 0.752, 0.752 ]
    SkyBlue = [ 0.53, 0.808, 0.92 ]
    SlateBlue = [ 0.415, 0.352, 0.804 ]
    SlateGray = [ 0.44, 0.5, 0.565 ]
    SlateGrey = [ 0.44, 0.5, 0.565 ]
    Smoke = [ 0.95, 0.95, 0.95 ]
    Snow = [ 1, 0.98, 0.98 ]
    SoftYellow = [ 1.0, 1.0, 0.4 ]
    SpringGreen = [ 0, 1, 0.498 ]
    SteelBlue = [ 0.275, 0.51, 0.705 ]
    Tan = [ 0.824, 0.705, 0.55 ]
    Teal = [ 0, 0.5, 0.5 ]
    Thistle = [ 0.848, 0.75, 0.848 ]
    Tomato = [ 1, 0.39, 0.28 ]
    Turquoise = [ 0.25, 0.88, 0.815 ]
    Violet = [ 0.932, 0.51, 0.932 ]
    WarmGray = [ 0.678, 0.66, 0.562 ]
    Wheat = [ 0.96, 0.87, 0.7 ]
    White = [ 1, 1, 1 ]
    WhiteSmoke = [ 0.97, 0.97, 0.97 ]
    Yellow = [ 1, 1, 0 ]

end # module ColorConstants
end # module Tioga


