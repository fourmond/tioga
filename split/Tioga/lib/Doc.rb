#  Doc.rb


=begin rdoc

:main: Tioga

===The tioga kernel

This is the reference document for using of the tioga kernel to create figures and plots using Ruby, PDF, and TeX.
Please understand that this is more like a dictionary than a textbook.  
It is written assuming that you've already familiarized yourself with how things work to the level covered in the Tutorial.  
Without that background, much of the following may be incomprehensible.



Page Layout and System Control

- Page_Frame_Bounds
- Figures_and_Plots
- Executive

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

- Dobjects::Dvector
- Dobjects::Dtable

For more information on PDF, consult the Adode PDF Reference document.  
It's a huge collection of details (over 1000 pages at last count), and I've borrowed 
extensively from it in some of the following descriptions of methods that map directly to PDF operations.


=end


module Tioga
    
class FigureMaker

# :call-seq:
#               FigureMaker.default                         
#               FigureMaker.default = a_FigureMaker
#
# The default FigureMaker is typically the one to use.  Most tioga figure class definitions begin their
# initialization by storing this in an instance variable for easy access in the figure methods.
    def FigureMaker.default
    end

    def FigureMaker.default=(fm)
    end
    
# :call-seq:
#               FigureMaker.exec do |t| ... end
#
# Executes the block with FigureMaker.default as argument.
    def FigureMaker.exec(&cmd)
    end
    
# :call-seq:
#               FigureMaker.make_pdf(name) do |t| ... end
#
# Executes the block to create a pdf with the given name.
# The argument for the block is FigureMaker.default.
    def FigureMaker.make_pdf(name,&cmd)
    end
    
# :call-seq:
#               FigureMaker.def_enter_page_function do |t| ... end
#
# Defines the block to be the enter_page_function for FigureMaker.default.
    def FigureMaker.def_enter_page_function(&cmd)
    end
    
# :call-seq:
#               FigureMaker.page_style do |t| ... end
#
# Alias for FigureMaker.def_enter_page_function.
# Defines the block to be the enter_page_function for FigureMaker.default.
    def FigureMaker.page_style(&cmd)
    end
    
# :call-seq:
#               FigureMaker.version
#
# Returns a string defining the current tioga version.
    def FigureMaker.version
    end

    
end # class FigureMaker

end # module Tioga

