# sample.rb

require 'Tioga/FigureMaker'

class MyFigures

    include Tioga
    include FigureConstants

    def t
        @figure_maker
    end

    def initialize
        @figure_maker = FigureMaker.default
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

end

MyFigures.new
