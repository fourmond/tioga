#  Doc.rb

=begin rdoc

This is the reference document for the FigureMaker class which implements the Tioga kernel for creating figures and plots using Ruby, PDF, and TeX.

Understand that this reference material is more like a dictionary than a textbook.  It is written assuming that you've already familiarized yourself with how things work to the level covered in the Tutorial.  Without that background, much of the following will be incomprehensible.



Page Layout and System Control

- Coordinate_Conversions
- Figures_and_Plots
- Page_Frame_Bounds
- Executive
- Flate (Lossless Compression Scheme)

Graphics

- ColorConstants
- Creating_Paths
- Using_Paths
- Strokes (Lines and Curves)
- Rectangles
- Arcs_and_Circles
- Special_Paths (Contours, Splines, Steps, Arrows, Error Bars)
- Colormaps
- Images (JPEGs and Sampled Data)
- Shading (Radial and Axial Blends)
- Transparency
- FigureConstants

Text

- MarkerConstants
- Markers
- TeX_Text

Extras for Plots

- Titles_and_Labels
- X_and_Y_Axes
- Legends
- Colorbars

Facilities for Storing Numeric Data

- Dvector
- Dtable

For more information on PDF, consult the Adode PDF Reference document.  It's a huge collection of details (1172 pages at last count), and I've borrowed extensively from it in some of the following descriptions of methods that map directly to PDF operations.


=end

module Tioga

class FigureMaker

# :call-seq:
#               FigureMaker.default                         
#               FigureMaker.default = a_FigureMaker
#
# The default FigureMaker is typically the one to use.  Most Tioga figure class definitions begin their
# initialization by storing this in an instance variable for easy access in the figure methods.
    def FigureMaker.default
    end

    def FigureMaker.default=(fm)
    end
    
# :call-seq:
#               FigureMaker.version
#
# Returns a string defining the current Tioga version.
    def FigureMaker.version
    end

    
end # class FigureMaker

end # module Tioga

