module Tioga

# = PDF Fonts
#
# Text is normally sent to TeX, allowing you to use any font that can be accessed from your TeX documents.  
# However, in certain special cases you may need to use text in a "graphical" manner and have it processed directly in the 
# PDF output.  In that case, tioga needs to have metric information for the font (in the form of an "afm" file), and any 
# output device needs to have access to the font definitions.  Providing a general solution to this is still a bit messy, and for the 
# expected use in tioga, I decided that it wasn't worth it.  All PDF devices are guaranteed to have the 14 standard Adobe 
# fonts, so they are easy to provide -- and that's what I've done.
#
# Here are the abc's of the PDF fonts, clockwise from the top: Times_Roman, Times_Italic, Times_Bold, Times_BoldItalic, Helvetica, Helvetica_Oblique, Helvetica_Bold, Helvetica_BoldOblique, Courier, Courier_Oblique, Courier_Bold, Courier_BoldOblique, Symbol, and ZapfDingbats.
#
# link:images/pdf_fonts.png
#
# ---
#
# = Markers
#
# Any character from any of the PDF fonts can be used as a marker.
# The predefined marker names are provided as a convenience, not as an exhaustive list.   Markers are specified by 
# either a font number and character code (for filled characters), or by a font number, character code, and line 
# width (for stroked characters).  In the following figure many of the markers appear in both a filled form, such 
# as 'Arrow', and a stroked form, such as 'ArrowOpen'.   The color of filled markers is determined by the current 
# setting of fill_color, and, similarly, stroke_color determines the color of stroked markers.
#
# link:images/Marker_Names.png
#
# Here is the full set of options from the ZapfDingbats font.  If you would like to use the jet plane as a marker,
# just write [ ZapfDingbats, 40 ] where you would have used one of the predefined marker names.
#
# link:images/dingbats.png
#

module MarkerConstants

    Times_Roman = 1
    Times_Italic = 2
    Times_Bold = 3
    Times_BoldItalic = 4
    Helvetica = 5
    Helvetica_Oblique = 6
    Helvetica_Bold = 7
    Helvetica_BoldOblique = 8
    Courier = 9
    Courier_Oblique = 10
    Courier_Bold = 11
    Courier_BoldOblique = 12
    Symbol = 13
    ZapfDingbats = 14
    
    Arrow           = [ZapfDingbats, 0334]
    Arrowhead       = [ZapfDingbats, 0344]
    Asterisk        = [ZapfDingbats, 0135]
    Bar             = [ZapfDingbats, 0171]
    BarThick        = [ZapfDingbats, 0172]
    BarThin         = [ZapfDingbats, 0170]
    Box             = [ZapfDingbats, 0156]
    Bullet          = [ZapfDingbats, 0154]
    Check           = [ZapfDingbats, 0064]
    Circle          = [ZapfDingbats, 0154]
    Club            = [ZapfDingbats, 0250]
    Cross           = [ZapfDingbats, 0072]
    Diamond         = [ZapfDingbats, 0251]
    Semicircle      = [ZapfDingbats, 0167]
    Heart           = [ZapfDingbats, 0252]
    HeartWide       = [ZapfDingbats, 0244]
    Spade           = [ZapfDingbats, 0253]
    Square          = [ZapfDingbats, 0156]
    Star            = [ZapfDingbats, 0110]
    TriangleDown    = [ZapfDingbats, 0164]
    TriangleUp      = [ZapfDingbats, 0163]
    Plus            = [ZapfDingbats, 0072]
    Times           = [ZapfDingbats, 0066]
    
    ArrowOpen           = [ZapfDingbats, 0334, 0.5]
    ArrowheadOpen       = [ZapfDingbats, 0344, 0.5]
    AsteriskOpen        = [ZapfDingbats, 0135, 0.5]
    BarOpen             = [ZapfDingbats, 0172, 0.5]
    BarThickOpen        = [ZapfDingbats, 0172, 0.5]
    BarThinOpen         = [ZapfDingbats, 0170, 0.5]
    BoxOpen             = [ZapfDingbats, 0156, 0.5]
    BulletOpen          = [ZapfDingbats, 0154, 0.5]
    CheckOpen           = [ZapfDingbats, 0064, 0.5]
    CircleOpen          = [ZapfDingbats, 0154, 0.5]
    ClubOpen            = [ZapfDingbats, 0250, 0.5]
    DiamondOpen         = [ZapfDingbats, 0251, 0.5]
    SemicircleOpen      = [ZapfDingbats, 0167, 0.5]
    HeartOpen           = [ZapfDingbats, 0252, 0.5]
    HeartWideOpen       = [ZapfDingbats, 0244, 0.5]
    SpadeOpen           = [ZapfDingbats, 0253, 0.5]
    SquareOpen          = [ZapfDingbats, 0156, 0.5]
    StarOpen            = [ZapfDingbats, 0110, 0.5]
    TriangleDownOpen    = [ZapfDingbats, 0164, 0.5]
    TriangleUpOpen      = [ZapfDingbats, 0163, 0.5]
    PlusOpen            = [ZapfDingbats, 0072, 0.5]
    TimesOpen           = [ZapfDingbats, 0066, 0.5]
    Circled1       = [ZapfDingbats, 0300]
    Circled2       = [ZapfDingbats, 0301]
    Circled3       = [ZapfDingbats, 0302]
    Circled4       = [ZapfDingbats, 0303]
    Circled5       = [ZapfDingbats, 0304]
    Circled6       = [ZapfDingbats, 0305]
    Circled7       = [ZapfDingbats, 0306]
    Circled8       = [ZapfDingbats, 0307]
    Circled9       = [ZapfDingbats, 0310]
    Circled10      = [ZapfDingbats, 0311]
    SolidBall1    = [ZapfDingbats, 0312]
    SolidBall2    = [ZapfDingbats, 0313]
    SolidBall3    = [ZapfDingbats, 0314]
    SolidBall4    = [ZapfDingbats, 0315]
    SolidBall5    = [ZapfDingbats, 0316]
    SolidBall6    = [ZapfDingbats, 0317]
    SolidBall7    = [ZapfDingbats, 0320]
    SolidBall8    = [ZapfDingbats, 0321]
    SolidBall9    = [ZapfDingbats, 0322]
    SolidBall10   = [ZapfDingbats, 0323]
    Hand          = [ZapfDingbats, 0052]
    OtherHand     = [ZapfDingbats, 0053]

end
end # module Tioga
