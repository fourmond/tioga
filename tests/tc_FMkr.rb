#  tc_FMkr.rb

require 'Tioga/tioga'

require 'test/unit'

class MyPlots
  
    include Math
    include Tioga
    include FigureConstants
    
    attr_reader :t
  
    def initialize
        @t = FigureMaker.default
        t.def_figure("Icon_Test") { icon_test }
    end

    def icon_test
        t.fill_color = SlateGray
        t.fill_frame
        margin = 0.02 
        t.set_subframe('left' => margin, 'right' => margin, 'top' => margin, 'bottom' => margin)
        t.clip_to_frame
        t.axial_shading(
            'start_point' => [0, 0],
            'end_point' => [0, 1], 
            'colormap' => t.create_gradient_colormap('hue' => t.rgb_to_hls(Linen)[0],
                            'saturation' => 0.3, 'starting_L' => 0.6, 'ending_L' => 0.99))
        t.stroke_color = Black
        t.line_width = 8 
        t.stroke_frame
        angle = 60; size = 3; shift = -1.3
        t.show_text('text' => '\sffamily\textbf{Ruby}', 'side' => BOTTOM, 'pos' => 0.27, 'shift' => shift,
            'scale' => size, 'angle' => angle)
        t.show_text('text' => '\sffamily\textbf{PDF}', 'side' => BOTTOM, 'pos' => 0.58, 'shift' => shift,
            'scale' => size, 'angle' => angle)
        t.show_text('text' => '\textbf{\TeX}', 'side' => BOTTOM, 'pos' => 0.86, 'shift' => shift,
            'scale' => size, 'angle' => angle)
        x = t.bounds_xmin + 0.5 * t.bounds_width
        y = 0.68; scale = 7
        t.line_width = 2
        t.fill_opacity = 0.6
        t.show_marker('font' => Helvetica, 'string' => 'Tioga', 'scale' => scale, 'point' => [x+0.03,y-0.28],
            'color' => Grey,
            'mode' => FILL, 'horizontal_scale' => 0.9, 'vertical_scale' => -0.4, 'italic_angle' => -7)
        t.context do
            t.fill_opacity = 1.0
            t.show_marker('font' => Helvetica, 'string' => 'Tioga', 'scale' => scale, 'point' => [x+0.01,y],
                'mode' => STROKE_AND_CLIP, 'horizontal_scale' => 0.9)
            t.axial_shading( # this fills the 'Tioga' string
                'start_point' => [0, -0.06],
                'end_point' => [0, 1.4], 
                'colormap' => t.rainbow_colormap)
        end
        t.show_marker('font' => Times_Roman, 'string' => 'Compare this to the file samples/Icon.pdf', 
            'scale' => 0.85, 'point' => [x,0.93], 'color' => Crimson)
        t.fill_opacity = 1.0
    end

end

class TestFMkr < Test::Unit::TestCase

    def test_fmkr
        MyPlots.new.t.make_preview_pdf("Icon")
        puts " "
        puts " "
        puts " "
        puts ">>> NOTE: please look at tests/Icon_Test.pdf and compare it to samples/Icon.pdf"
    end
    
    def do_one_string(tst)
        compressed = Flate.compress(tst)
        puts "compressed from #{tst.size} to #{compressed.size}"
        expanded = Flate.expand(compressed)
        assert_equal(tst, expanded)
    end

    def test_flate
        puts ''
        do_one_string('')
        do_one_string('A test string')
        do_one_string('A longer string might actually become a little smaller when compressed')
        file = File.open('dvector_test.data')
        str = file.read
        do_one_string(str)
    end
    
end



















