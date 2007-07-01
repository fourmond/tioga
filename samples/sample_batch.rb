# sample.rb

require 'Tioga/FigureMaker'
include Tioga

class MyFigures

    include Tioga
    include FigureConstants

    def t
        @figure_maker
    end

    def initialize
        @figure_maker = FigureMaker.default
        t.def_eval_function { |str| eval(str) }
        t.def_figure("Blue") { blue }
        t.def_figure("Red") { red }
        t.save_dir = 'figures_out'
    end

    def blue
        t.fill_color = Blue
        t.fill_frame
    end

    def red
        t.fill_color = Red
        t.fill_frame
    end
    
   def do_one(figure_name, pdffile)
      t.make_pdf(figure_name)
      syscmd = 'mv ' + t.save_dir + '/' + figure_name + '.pdf ' + pdffile
      puts syscmd
      system(syscmd)
   end

end

MyFigures.new
FigureMaker.default.eval_function("do_one('Blue','Blue.pdf')")
