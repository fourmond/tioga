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
        t.def_enter_page_function { enter_page }
    end
    
    def enter_page
        sz = 8.5
        t.page_setup(sz*72/2,sz*72/2)
        t.set_frame_sides(0.15,0.85,0.85,0.15) # left, right, top, bottom in page coords        
    end

    def icon_test
        tioga_cool = true
        # set aspect ratio and font scale depending on whether root figure or not
        unless t.in_subfigure
          sz = 4.25; scale = 8
          t.page_setup(sz*72,sz*72)
          t.set_frame_sides(0.05,0.95,0.91,0.09) # left, right, top, bottom in page coords
        else
          scale = 7   
        end
        t.fill_color = SlateGray
        t.fill_frame
        margin = 0.02 
        t.set_subframe('left' => margin, 'right' => margin, 'top' => margin, 'bottom' => margin)
        t.clip_to_frame
        saturation = (tioga_cool)? 0.4 : 1
        starting_L = (tioga_cool)? 0.6 : 0.2
        ending_L = (tioga_cool)? 0.9 : 1
        hue = (tioga_cool)? (t.rgb_to_hls(Linen)[0]) : (t.rgb_to_hls(SlateGray)[0])
        t.axial_shading(
              'start_point' => [0, 0],
              'end_point' => [0, 1], 
              'colormap' => t.create_gradient_colormap(
                              'hue' => hue,
                              'saturation' => saturation, 
                              'starting_L' => starting_L, 
                              'ending_L' => ending_L)
              )
        t.stroke_color = Black
        t.line_width = 8 
        t.stroke_frame
        angle = 60; size = 3.5; shift = -1.0
        t.show_text('text' => '\sffamily\textbf{Ruby}', 'side' => BOTTOM, 'pos' => 0.27, 'shift' => shift,
            'scale' => size, 'angle' => angle)
        t.show_text('text' => '\sffamily\textbf{PDF}', 'side' => BOTTOM, 'pos' => 0.58, 'shift' => shift,
            'scale' => size, 'angle' => angle)
        t.show_text('text' => '\textbf{\TeX}', 'side' => BOTTOM, 'pos' => 0.86, 'shift' => shift,
            'scale' => size, 'angle' => angle)
        x = t.bounds_xmin + 0.5 * t.bounds_width
        y = 0.68
        t.line_width = 2
        t.fill_opacity = 0.6
        t.show_marker('font' => Helvetica, 'string' => 'Tioga', 'scale' => scale, 'point' => [x+0.03,y-0.28],
            'color' => Grey,
            'mode' => FILL, 'horizontal_scale' => 0.9, 'vertical_scale' => -0.4, 'italic_angle' => -7)
        t.show_marker('font' => Times_Roman, 'string' => 'Compare this to the file samples/Icon.pdf', 
            'scale' => 0.7, 'point' => [x,0.95], 'color' => Crimson)
        t.fill_opacity = 1.0
        t.show_marker(
              'font' => Helvetica_BoldOblique, 'string' => 'Tioga', 
              'scale' => scale, 'point' => [x+0.02,y],
              'mode' => STROKE_AND_CLIP, 'horizontal_scale' => 0.9)
        if tioga_cool
          t.axial_shading( # this fills the 'Tioga' string
              'start_point' => [0, -0.06],
              'end_point' => [0, 1.4], 
              'colormap' => t.rainbow_colormap)
        else
          t.axial_shading( # this fills the 'Tioga' string
              'start_point' => [0, 0.4],
              'end_point' => [0, 2.3], 
              'colormap' => t.rainbow_colormap)
        end
    end

end

class TestFMkr < Test::Unit::TestCase

    def test_fmkr
        MyPlots.new.t.make_pdf("Icon")
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

    # Returns the real file name of a given file.
    def real_file_name(file_name)
      dir = File.dirname(__FILE__)
      if dir.empty?
        return file_name
      else
        return "#{dir}/#{file_name}"
      end
    end

    def test_flate
        puts ''
        do_one_string('')
        do_one_string('A test string')
        do_one_string('A longer string might actually become a little smaller when compressed')
        file = File.open(real_file_name('dvector_test.data'))
        str = file.read
        do_one_string(str)
    end

    # A test function for the new scheme of Hash specifications
    def test_arguments
      a = { 'biniou' => 0,
        'stuff' => '->biniou'.to_sym,
      }
      b = MyPlots.new.t.prepare_argument_hash(a, {})
      assert_equal(b['stuff'], 0)
      assert_equal(b['biniou'], 0)
    end

    def test_hls_to_rgb
      t = Tioga::FigureMaker.default
      rgb_old = [0.1, 0.1, 1.0]
      rgb_new = t.hls_to_rgb(t.rgb_to_hls(rgb_old))
      3.times do |i|
        # Exact conversion is not expected, but that shouldn't be so
        # bad.
        assert((rgb_old[i] - rgb_new[i]).abs < 1e-5)
      end
    end


end



















