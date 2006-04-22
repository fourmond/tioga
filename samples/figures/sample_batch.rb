require 'Tioga/FigureMaker'
require 'sample.rb'

class MyFig < MyFigures

    def run
        t.figure_names.each do |name| 
            puts "\nMake #{name}.pdf"
            t.make_preview_pdf(name)
        end
    end
 
end

MyFig.new.run
