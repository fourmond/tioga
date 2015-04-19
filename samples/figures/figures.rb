#  figures.rb

require 'Tioga/FigureMaker'

require './figure_styles.rb'

class MyFigures

    include Tioga
    include MyFigureStyles
    include FigureConstants
    
    def t
        @figure_maker
    end

    def initialize
        @figure_maker = FigureMaker.default
                
        t.save_dir = 'figures_out'
        # t.autocleanup = false
        t.def_eval_function { |str| eval(str) }

        t.def_figure("Icon") { icon }
        t.def_figure("Rounded_Rect") { rounded_rect }
        t.def_figure("Curve") { curve }
        t.def_figure("Arc") { arc }
        t.def_figure("Fill_Rules") { fill_rules }
        t.def_figure("Line_Types") { line_types }
        t.def_figure("Caps_and_Joins") { caps_and_joins }
        t.def_figure("Rendering_Modes") { rendering_modes }
        t.def_figure("Marker_Names") { marker_names }
        t.def_figure("Text J_and_A") { justification_and_alignment }
        #t.def_figure("Test_Pattern") { test_pattern }
        t.def_figure("Strings") { strings }
        t.def_figure("Arrows") { show_arrows }
        t.def_figure("Arrows_second_take") { show_arrows_second_take }
        t.def_figure("Marker_Horizontal_Scaling") { marker_horizontal_scaling }
        t.def_figure("Marker_Vertical_Scaling") { marker_vertical_scaling }
        t.def_figure("Marker_Italic_Angle") { marker_italic_angle }
        t.def_figure("Marker_Ascent_Angle") { marker_ascent_angle }
        t.def_figure("Marker_Shadow_Effect") { marker_shadow_effect }
        t.def_figure("Framebox") { framebox }
        t.def_figure("Parbox") { parbox }
        t.def_figure("Minipages") { minipages }
        t.def_figure("Minipage_List") { minipage_list }
        t.def_figure("Minipage_Table") { minipage_table }
        t.def_figure("Math_Typesetting") { math_typesetting }
        t.def_figure("Squares") { both_squares }
        t.def_figure("Sample_Jpegs") { sample_jpegs }
        t.def_figure("Many_Jpegs") { many_jpegs }
        t.def_figure("Monochrome_Image") { mono_image }
        t.def_figure("RGB_Image") { rgb_image }
        t.def_figure("HLS_Image") { hls_image }
        t.def_figure("Axial_Shading") { axial_shading }
        t.def_figure("Radial_Shading") { radial_shading }
        t.def_figure("Dingbats") { dingbats }
        t.def_figure("PDF_Fonts") { pdf_fonts }
        t.def_figure("Subfigures") { subfigures }
        t.def_figure("Colors1") { colors1 }
        t.def_figure("Colors2") { colors2 }
        t.def_figure("Colors3") { colors3 }
        t.def_figure("Colors4") { colors4 }


        t.def_figure("Tex_Fonts") { tex_fonts }

        t.def_figure("Text_size") { text_size }
        t.def_figure("Text_size_with_rotation") { text_size_with_rotation }
        t.def_figure("Text_size_with_rotation_and_subframes") { 
        text_size_with_rotation_and_subframes }
        

        hues
        
        t.def_enter_page_function { enter_page }
            
    end
    
    def enter_page
        set_default_style # defined in figure_styles.rb
        # Set a reasonable size for
        t.default_page_width = 72*5 # in big-points (1/72 inch)
        t.default_page_height = t.default_page_width
        t.default_enter_page_function
    end
    
    def hues
        grays = []
        arys = Array.new(12) { Array.new }
        ColorConstants.constants.each do |name|
            rgb = ColorConstants::const_get(name)
            if (rgb[0] == rgb[1] and rgb[1] == rgb[2])
                grays << name
            else
                hls = t.rgb_to_hls(rgb)
                i = hls[0].floor/30
                arys[i] << name
            end
        end
        grays.sort! { |a,b| 
          as = t.rgb_to_hls(ColorConstants::const_get(a))[1]; 
          bs = t.rgb_to_hls(ColorConstants::const_get(b))[1]; 
          as <=> bs 
        }
        arys.each { |ary| ary.sort! { |a,b| 
          as = t.rgb_to_hls(ColorConstants::const_get(a))[1]; 
          bs = t.rgb_to_hls(ColorConstants::const_get(b))[1]; 
          as <=> bs } }
        colors = grays
        arys.each { |ary| colors.concat(ary) }
        @color_list = colors
        @color_list.reverse!
    end
    
    def color_swatch(name, x, y, width, height)
        yfrac = 0.7
        dy = 0.75
        t.fill_color = White
        t.fill_rect(x, y + yfrac * height, width, (1-yfrac) * height)
        t.fill_color = ColorConstants::const_get(name)
        t.fill_rect(x, y, width, yfrac * height)
        t.show_text('text' => name, 'x' => x + 0.5 * width, 'y' => y + dy * height,
            'justification' => CENTERED, 'scale' => 0.55)
        t.line_color = Black
        t.stroke_width = 0.5
        t.stroke_rect(x, y, width, height)
    end
    
    def color_row(names, y, x, dx, width, height)
        names.size.times { |i| color_swatch(names[i], x, y, width, height); x+= dx }
    end
    
    def color_page(first)
        sans_serif_style
        width = 0.13; height = 0.094
        x = x0 = 0.02; y = y0 = 0.895
        dx = 0.14; dy = 0.11
        t.rescale(0.5)
        t.fill_color = Smoke; t.fill_rect(0, y0-6.2*dy, 7.15*dx, 7.3*dy)
        num_rows = 7
        num_cols = 7
        clr_num = first
        num_colors = @color_list.size
        num_rows.times do
            next_row = clr_num+num_cols
            next_row = num_colors if num_colors < next_row
            color_row(@color_list[clr_num...next_row],
                y, x, dx, width, height)
            y -= dy; x = x0; clr_num = next_row
        end
    end
    
    def colors1
        color_page(0)
    end
    
    def colors2
        color_page(49)
    end
     
    def colors3
        color_page(98)
    end
    
    def colors4
        color_page(147)
    end
    
    def dingcolumn(numbers, x, dx, y, dy)
        first = numbers[0]
        ddy = t.default_text_height_dy * 0.26
        numbers.each do |num|
            i = num - first
            break if num >= 255
            t.show_text('text' => sprintf("%3i", num), 'at' => [x, y-ddy],
                'scale' => 0.5)
            t.show_marker('marker' => [ZapfDingbats, num],
                'at' => [x+dx, y], 'scale' => 0.8)
            y -= dy
        end
    end
    
    def dingbats
        t.landscape
        t.rescale(0.8)
        t.show_text('text' => 'ZapfDingbats Set and Encoding', 'side' => TOP, 'shift' => 0.6,
            'justification' => CENTERED, 'scale' => 0.9, 'position' => 0.5)
        background
        first = 33
        x = 0.05
        dx = 0.048
        y = 0.95
        dy = 0.045
        percolumn = 21
        spacing = 2.2
        dingcolumn((first...first+percolumn).to_a, x, dx, y, dy)
        first += percolumn; x += spacing*dx
        dingcolumn((first...first+percolumn).to_a, x, dx, y, dy)
        first += percolumn; x += spacing*dx
        dingcolumn((first...first+percolumn).to_a, x, dx, y, dy)
        first += percolumn; x += spacing*dx
        dingcolumn((first...first+percolumn).to_a, x, dx, y, dy)
        first += percolumn; x += spacing*dx
        numbers = (121..126).to_a
        first = 161; last = first + percolumn - 7
        numbers.concat((first..last).to_a)
        dingcolumn(numbers, x, dx, y, dy)
        first = last + 1; x += spacing*dx
        dingcolumn((first...first+percolumn).to_a, x, dx, y, dy)
        first += percolumn; x += spacing*dx
        dingcolumn((first...first+percolumn).to_a, x, dx, y, dy)
        first += percolumn; x += spacing*dx
        dingcolumn((first...first+percolumn).to_a, x, dx, y, dy)
        first += percolumn; x += spacing*dx
        dingcolumn((first...first+percolumn).to_a, x, dx, y, dy)
        first += percolumn; x += spacing*dx
        dingcolumn((first...first+percolumn).to_a, x, dx, y, dy)
    end
    
    def pdf_fonts
        background
        14.times do |i|
            dy = 0.35 * cos(i*PI/7)
            dx = 0.35 * sin(i*PI/7)
            t.show_marker('font' => i+1, 'text' => 'abc', 'at' => [0.5+dx, 0.5+dy], 'scale' => 1.25)
        end
    end


    def tex_fonts
      background
      t.show_text('text' => 'Various fonts (currently not working)', 
                  'side' => TOP, 'shift' => 0.6,
                  'justification' => CENTERED, 
                  'scale' => 0.9, 'position' => 0.5)
      
      t.show_text('text' => 'Default font',
                  'x' => 0.02, 'y' => 0.93,
                  'justification' => LEFT_JUSTIFIED )
      t.tex_fontfamily = "sfdefault"
      t.show_text('text' => 'Default sans serif',
                  'x' => 0.02, 'y' => 0.86,
                  'justification' => LEFT_JUSTIFIED )
    end
    
    def rounded_rect
        t.show_text('text' => 'append\_rounded\_rect\_to\_path', 'side' => TOP, 'shift' => 0.6,
            'justification' => CENTERED, 'scale' => 0.9, 'position' => 0.5)
        background
        x = y = 0.1
        width = height = 0.8
        dx = dy = 0.25
        t.append_rounded_rect_to_path(x, y, width, height, dx, dy)
        t.stroke_color = SlateGray
        t.stroke_width = 10
        t.stroke
        t.append_rounded_rect_to_path(x, y, width, height, dx, dy)
        t.clip
        jpg_image_transparent
    end
    
    def arc
        t.landscape
        t.show_text('text' => 'append\_arc\_to\_path', 'side' => TOP, 'shift' => 0.6,
            'justification' => CENTERED, 'scale' => 0.9, 'position' => 0.5)
        background
        x_start = 0.75; y_start = 0.1
        x_corner = 0.9; y_corner = 0.9
        x_end = 0.1; y_end = 0.6
        dx = 0.3; dy = 0.3
        t.move_to_point(x_start, y_start)
        t.append_point_to_path(x_corner, y_corner)
        t.append_point_to_path(x_end, y_end)
        t.line_width = 1
        t.line_color = LightBlue
        t.stroke
        t.append_arc_to_path(x_start, y_start, x_corner, y_corner, x_end, y_end, dx, dy)
        t.line_width = 2.5
        t.line_color = DarkBlue
        t.stroke
        t.show_marker('marker' => Bullet, 
            'xs' => [x_start, x_corner,  x_end],
            'ys' => [y_start, y_corner,  y_end],
            'scale' => 0.6, 'color' => Crimson);
    end
    
    def curve
        t.landscape
        t.show_text('text' => 'append\_curve\_to\_path', 'side' => TOP, 'shift' => 0.6,
            'justification' => CENTERED, 'scale' => 0.9, 'position' => 0.5)
        background
        t.clip_to_frame
        x0 = 0.75; y0 = 0.9; x1 = 0.9; y1 = 0.3
        x2 = 0.4; y2 = 0.1; x3 = 0.1; y3 = 0.8
        xs = [ x0, x1, x2, x3 ]
        ys = [ y0, y1, y2, y3 ]
        t.move_to_point(x0, y0)
        t.append_curve_to_path(x1, y1, x2, y2, x3, y3)
        t.line_width = 2.5; t.line_color = DarkBlue
        t.stroke
        t.show_marker('xs' => xs, 'ys' => ys,
            'marker' => Bullet, 'scale' => 0.6, 'color' => Red);
        dx = t.default_text_height_dx * 1.4
        t.show_label('x' => x0+dx, 'y' => y0, 'text' => "start", 'scale' => 0.9);
        scale = 1.2; dy = t.default_text_height_dy * 0.8
        t.show_marker('at' => [x1, y1+dy], 'marker' => Circled1, 'scale' => scale);
        t.show_marker('at' => [x2, y2+dy], 'marker' => Circled2, 'scale' => scale);
        t.show_marker('at' => [x3, y3+dy], 'marker' => Circled3, 'scale' => scale);
    end
    
    def caps_and_joins
        t.rescale(0.55)
        t.subfigure('right_margin' => 0.5, 'top_margin' => 0.5) { line_caps }
        t.subfigure('left_margin' => 0.5, 'top_margin' => 0.5) { line_joins }
    end
    
    def justification_and_alignment
        t.rescale(0.55)
        t.subfigure('right_margin' => 0.5, 'top_margin' => 0.5) { text_justification }
        t.subfigure('left_margin' => 0.5, 'top_margin' => 0.5) { text_alignment }
    end
    
    def both_squares
        t.subfigure('right_margin' => 0.5, 'top_margin' => 0.5) { opaque_squares }
        t.subfigure('left_margin' => 0.5, 'top_margin' => 0.5) { transparent_squares }
    end
    
    def sample_jpegs
        t.subfigure('top_margin' => 0.67) do
            t.subfigure(t.column_margins('num_columns' => 3, 'column' => 1, 'column_margin' => 0.01)) { jpg_image }
            t.subfigure(t.column_margins('num_columns' => 3, 'column' => 2, 'column_margin' => 0.01)) { jpg_image_transparent }
            t.subfigure(t.column_margins('num_columns' => 3, 'column' => 3, 'column_margin' => 0.01)) { stencil_mask }
        end
    end

    def many_jpegs
      ref = t.register_image('jpg' => "data/cassini.jpg", 'width' => 999, 'height' => 959) 

      dx = 0.25
      dy = 0.25
      4.times do |i|
        x = i * dx
        4.times do |j|
          y = j * dy

          ul = [x, y]
          ll = [x, y + dy * (0.8 + 0.2 * sin(y * 3.14 * 2))]
          lr = [x+dx *(0.8 + 0.2 * sin(x * 3.14*2)), y + dy * (0.8 + 0.2 * sin(x * 3.14 * 2))]
          
          t.show_image('ll' => ll, 'lr' => lr, 'ul' => ul, 'ref' => ref)
        end
      end
    end

    
    def strings
        t.stroke_rect(0,0,1,1)
        center_x = 0.5; center_y = 0.5; len = 0.125
        hls = t.rgb_to_hls(Red)
        angles = 10
        delta = 360.0/angles
        equation = '\int_{-\infty}^{\infty} \! e^{-x^{2}}\, \! dx = \sqrt{\pi}'
        text = '\parbox{15em}{\begin{displaymath}' + equation + '\end{displaymath}}'
        angles.times do |angle|
            angle *= delta
            dx = len*cos(angle*RADIANS_PER_DEGREE)
            dy = len*sin(angle*RADIANS_PER_DEGREE)
            x = center_x + 2*dx; y = center_y + 2*dy;
            text_color = t.hls_to_rgb([angle/1.8 + 200, hls[1], hls[2]])
            t.show_text('text' => text, 'color' => text_color, 'x' => x, 'y' => y,
                'alignment' => ALIGNED_AT_MIDHEIGHT,
                'scale' => 0.7, 'angle' => t.convert_to_degrees(dx,dy)) 
        end
    end

    # In this example, we show how we can use measures of the text
    # transparently with Tioga:
    def text_size
      t.stroke_rect(0,0,1,1)
      t.rescale(0.5)
      
      tries = [
               {
                 'text' => 'Using side/position instead of X and Y', 
                 'side' => TOP, 'shift' => 0.6,
                 'justification' => CENTERED, 
                 'scale' => 2, 'position' => 0.5
               },
               {
                 'x' => 0.2, 'y' => 0.8,
                 'justification' => LEFT_JUSTIFIED,
                 'alignment' => ALIGNED_AT_TOP,
                 'scale' => 4,
                 'text' => 'top left'
               },
               {
                 'x' => 0.3, 'y' => 0.5,
                 'justification' => RIGHT_JUSTIFIED,
                 'alignment' => ALIGNED_AT_BOTTOM,
                 'scale' => 2,
                 'text' => 'bottom right'
               },
               {
                 'x' => 0.3, 'y' => 0.2,
                 'justification' => CENTERED,
                 'alignment' => ALIGNED_AT_BASELINE,
                 'scale' => 2.3,
                 'text' => 'centered \raise-.5ex\hbox{b}aseline'
               },
               {
                 'x' => 0.7, 'y' => 0.6,
                 'justification' => LEFT_JUSTIFIED,
                 'alignment' => ALIGNED_AT_MIDHEIGHT,
                 'scale' => 2,
                 'text' => 'left mid-height'
               },
               {
                 'x' => 0.7, 'y' => 0.9,
                 'justification' => CENTERED,
                 'alignment' => ALIGNED_AT_MIDHEIGHT,
                 'scale' => 2,
                 'text' => 'rotated',
                 'angle' => -30
               },
               {
                 'x' => 0.5, 'y' => 0.4,
                 'justification' => RIGHT_JUSTIFIED,
                 'alignment' => ALIGNED_AT_BASELINE,
                 'scale' => 2,
                 'text' => 'a right-justified rotated text',
                 'angle' => 12
               },
               {
                 'x' => 1, 'y' => 0.4,
                 'justification' => LEFT_JUSTIFIED,
                 'alignment' => ALIGNED_AT_TOP,
                 'scale' => 2,
                 'text' => 'a left-justified rotated text',
                 'angle' => 240
               },
               
              ]
      i = 0
      for try in tries
        a = { 'scale' => 1, 
          'measure' => "box#{i}"
        }
        a.update(try)
        t.show_text(a)
        size = t.get_text_size("box#{i}")
        i += 1

        # We draw a box around the text:
        if size.key? 'width'
          xs = Dvector.new
          ys = Dvector.new
          for x,y in size['points']
            xs << t.convert_output_to_figure_x(t.scaling_factor*x)
            ys << t.convert_output_to_figure_y(t.scaling_factor*y)
          end
          xs << xs[0]
          ys << ys[0]
          t.show_polyline(xs,ys, Red, nil, Line_Type_Dot)
        end
        t.show_marker({
                        'x' => try['x'],
                        'y' => try['y'],
                        'marker' => MarkerConstants::Plus,
                        'scale' => 0.6,
                        'color' => Green,
                        'angle' => try['angle'] || 0
                      })
      end
    end

    def text_size_with_rotation_and_subframes
      t.context do 
        t.subfigure([0.1,0.4,0.3,0.3]) do 
          text_size_with_rotation
        end
      end
    end


    # In this example, we show how we can use measures of the text
    # transparently with Tioga:
    def text_size_with_rotation
      t.stroke_rect(0,0,1,1)
      t.rescale(0.5)

      equation = '\int_{-\infty}^{\infty} \! e^{-x^{2}}\, \! dx = \sqrt{\pi}'
      text = "\\fbox{$\\displaystyle #{equation}$}"

      nb = 5
      nb.times do |i|
        scale = 0.5 + i * 0.2
        angle = i * 37
        x = (i+1)/(nb+1.0)
        y = x
        color = [1.0 - i * 0.2, i*0.2, 0]
        t.show_text('text' => text, 'color' => color, 'x' => x, 
                    'y' => x,
                    'alignment' => ALIGNED_AT_MIDHEIGHT,
                    'scale' => scale , 'measure' => "box#{i}", 
                    'angle' => angle )
        size = t.get_text_size("box#{i}")
        if size.key? 'points'
          xs = Dvector.new
          ys = Dvector.new
          for x,y in size['points']
            xs << t.convert_output_to_figure_x(t.scaling_factor*x)
            ys << t.convert_output_to_figure_y(t.scaling_factor*y)
          end
          t.stroke_color = color
          t.line_type = Line_Type_Dashes
          t.stroke_rect(xs.min, ys.min,
                        (xs.max - xs.min),(ys.max - ys.min))
        end
      end
    end
    
    def gradient_colormap
        t.landscape
        t.clip_to_frame
        t.axial_shading(
            'start_point' => [0, 0],
            'end_point' => [0, 1], 
            'colormap' => t.mellow_colormap
        )
    end
    
    def text_justification
        background
        centerx = t.bounds_xmin + 0.5 * t.bounds_width
        t.show_text('text' => '\texttt Text attributes: justification', 'x' => centerx, 'y' => 0.9)
        dx = 0.05; y = 0.66; dy = -0.2; t.line_width = 0.7; t.stroke_color = Blue
        scale = 1.2
        t.stroke_line(centerx, 0.13, centerx, 0.8)
        t.show_text('text' => 'Right Justified', 'at' => [centerx, y],
            'justification' => RIGHT_JUSTIFIED, 'scale' => scale)
        y += dy
        t.show_text('text' => 'Centered', 'at' => [centerx, y],
            'justification' => CENTERED, 'scale' => scale)
        y += dy
        t.show_text('text' => 'Left Justified', 'at' => [centerx, y],
            'justification' => LEFT_JUSTIFIED, 'scale' => scale)
    end
        
    def text_alignment
        background
        centerx = t.bounds_xmin + 0.5 * t.bounds_width
        t.show_text('text' => '\texttt Text attributes: alignment', 'x' => centerx, 'y' => 0.9)
        dx = 0.05; y = 0.7; dy = -0.17; t.line_width = 0.7; t.stroke_color = Blue
        scale = 1.2
        t.stroke_line(t.bounds_xmin+dx, y, t.bounds_xmax-dx, y)
        t.show_text('text' => 'Aligned at bottom', 'at' => [centerx, y],
                'alignment' => ALIGNED_AT_BOTTOM, 'scale' => scale)
        y += dy
        t.stroke_line(t.bounds_xmin+dx, y, t.bounds_xmax-dx, y)
        t.show_text('text' => 'Aligned at baseline', 'at' => [centerx, y],
                'alignment' => ALIGNED_AT_BASELINE, 'scale' => scale)
        y += dy
        t.stroke_line(t.bounds_xmin+dx, y, t.bounds_xmax-dx, y)
        t.show_text('text' => 'Aligned at mid-height', 'at' => [centerx, y],
                'alignment' => ALIGNED_AT_MIDHEIGHT, 'scale' => scale)
        y += dy
        t.stroke_line(t.bounds_xmin+dx, y, t.bounds_xmax-dx, y)
        t.show_text('text' => 'Aligned at top', 'at' => [centerx, y],
                'alignment' => ALIGNED_AT_TOP, 'scale' => scale)
    end
         
    def framebox
        t.landscape
        background
        centerx = t.bounds_xmin + 0.5 * t.bounds_width
        t.justification = CENTERED
        t.show_text('text' => 'Examples using \textbackslash framebox', 'x' => centerx, 'y' =>0.8)
        dx = 0.05; y = 0.6; dy = -0.15; t.line_width = 0.7; t.stroke_color = Blue
        t.rescale_text(0.75)
        t.show_text('text' => '\framebox[20em][c]{\textbackslash framebox[20em][c]\{ a, b, c \}}',
            'at' => [centerx, y])
        y += dy
        t.show_text('text' => '\framebox[20em][l]{\textbackslash framebox[20em][l]\{ a, b, c \}}',
            'at' => [centerx, y])
        y += dy 
        t.show_text('text' => '\framebox[20em][r]{\textbackslash framebox[20em][r]\{ a, b, c \}}',
            'at' => [centerx, y])
        y += dy
        t.show_text('text' => '\framebox[20em][s]{\textbackslash framebox[20em][s]\{ a, b, c \}}',
            'at' => [centerx, y])
    end
         
    def show_arrows
        t.stroke_rect(0,0,1,1)
        center_x = 0.5; center_y = 0.5; len = 0.45
        hls = t.rgb_to_hls(Red)
        angles = 36
        delta = 360.0/angles
        angles.times do |angle|
            angle *= delta
            dx = len*cos(angle*RADIANS_PER_DEGREE)
            dy = len*sin(angle*RADIANS_PER_DEGREE)
            x = center_x + dx; y = center_y + dy;
            t.show_arrow('head' => [x,y], 'tail'=> [center_x, center_y], 'head_scale' => 1.5,
                'tail_marker' => 'None', 'head_color' => t.hls_to_rgb([angle, hls[1], hls[2]]))
        end
    end

    def show_arrows_second_take
        t.stroke_rect(0,0,1,1)
        center_x = 0.5; center_y = 0.5; len = 0.45
        hls = t.rgb_to_hls(Red)
        angles = 36
        delta = 360.0/angles
        angles.times do |angle|
            angle *= delta
            dx = len*cos(angle*RADIANS_PER_DEGREE)
            dy = len*sin(angle*RADIANS_PER_DEGREE)
            x = center_x + dx; y = center_y + dy;
            t.show_arrow('head' => [x,y], 
                         'tail'=> [center_x + 0.5 * dx, 
                                   center_y + 0.5 * dy], 
                         'head_scale' => 1.5,
                         'tail_marker' => Semicircle,
                         'tail_just' => RIGHT_JUSTIFIED,
                         'head_color' => t.hls_to_rgb([angle, hls[1], hls[2]]),
                         'tail_color' => t.hls_to_rgb([- angle, hls[1], hls[2]]),
                         'line_style' => Line_Type_Dash)
        end
    end
    
    def show_test_line(x0, y0, x1, y1)
        t.context do
            t.line_width = 3
            t.stroke_color = Red
            t.stroke_line(x0, y0, x1, y1)
        end
    end
    
    def icon
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

    def marker_horizontal_scaling
        t.landscape
        background
        t.show_text('text' => "Marker Horizontal Scaling", 'side' => TOP, 'shift' => 0.6,
            'justification' => CENTERED, 'position' => 0.5, 'scale' => 0.9)
        scale = 1.3
        x = t.bounds_xmin + 0.5 * t.bounds_width
        dy = -0.2; y = 1 + dy; dx = 0.0
        t.show_marker('string' => 'horizontal_scale = 1.0', 'horizontal_scale' => 1,
            'color' => Blue, 'scale' => scale, 'point' => [x,y])
        x += dx; y += dy
        t.show_marker('string' => 'horizontal_scale = 1.2', 'horizontal_scale' => 1.2,
            'color' => Blue, 'scale' => scale, 'point' => [x,y])
        x += dx; y += dy
        t.show_marker('string' => 'horizontal_scale = 0.8', 'horizontal_scale' => 0.8,
            'color' => Blue, 'scale' => scale, 'point' => [x,y])
        x += dx; y += dy
        t.show_marker('string' => 'horizontal_scale = -1.2', 'horizontal_scale' => -1.2,
            'color' => Blue, 'scale' => scale, 'point' => [x,y])
    end
    
    def marker_vertical_scaling
        t.landscape
        background
        t.show_text('text' => "Marker Vertical Scaling", 'side' => TOP, 'shift' => 0.6,
            'justification' => CENTERED, 'position' => 0.5, 'scale' => 0.9)
        scale = 1.4
        x = t.bounds_xmin + 0.5 * t.bounds_width
        dy = -0.2; y = 1 + dy; dx = 0.0
        t.show_marker('string' => 'vertical_scale = 1.0', 'vertical_scale' => 1.0,
            'color' => Blue, 'scale' => scale, 'point' => [x,y])
        x += dx; y += dy
        t.show_marker('string' => 'vertical_scale = 1.2', 'vertical_scale' => 1.2,
            'color' => Blue, 'scale' => scale, 'point' => [x,y])
        x += dx; y += dy
        t.show_marker('string' => 'vertical_scale = 0.8', 'vertical_scale' => 0.8,
            'color' => Blue, 'scale' => scale, 'point' => [x,y])
        x += dx; y += dy
        t.show_marker('string' => 'vertical_scale = -1.0', 'vertical_scale' => -1.0,
            'color' => Blue, 'scale' => scale, 'point' => [x,y])
    end
    
    def marker_italic_angle
        t.landscape
        background
        t.show_text('text' => "Marker Italic Angle", 'side' => TOP, 'shift' => 0.6,
            'justification' => CENTERED, 'position' => 0.5, 'scale' => 0.9)
        scale = 1.4
        x = t.bounds_xmin + 0.5 * t.bounds_width
        dy = -0.25; y = 1 + dy; dx = 0.0
        t.show_marker('string' => 'italic_angle = 0', 'italic_angle' => 0,
            'color' => Blue, 'scale' => scale, 'point' => [x,y])
        x += dx; y += dy
        t.show_marker('string' => 'italic_angle = -12', 'italic_angle' => -12,
            'color' => Blue, 'scale' => scale, 'point' => [x,y])
        x += dx; y += dy
        t.show_marker('string' => 'italic_angle = 12', 'italic_angle' => 12,
            'color' => Blue, 'scale' => scale, 'point' => [x,y])
    end
    
    def marker_ascent_angle
        t.landscape
        background
        t.show_text('text' => "Marker Ascent Angle", 'side' => TOP, 'shift' => 0.6,
            'justification' => CENTERED, 'position' => 0.5, 'scale' => 0.9)
        scale = 1.4
        x = t.bounds_xmin + 0.5 * t.bounds_width
        dy = -0.25; y = 1 + dy; dx = 0.0
        t.show_marker('string' => 'ascent_angle = 0', 'ascent_angle' => 0,
            'color' => Blue, 'scale' => scale, 'point' => [x,y])
        x += dx; y += dy
        t.show_marker('string' => 'ascent_angle = -6', 'ascent_angle' => -6,
            'color' => Blue, 'scale' => scale, 'point' => [x,y])
        x += dx; y += dy
        t.show_marker('string' => 'ascent_angle = 6', 'ascent_angle' => 6,
            'color' => Blue, 'scale' => scale, 'point' => [x,y])
    end
    
    def marker_shadow_effect
        t.landscape
        background
        scale = 2.6
        t.line_width = 0.6
        x = t.bounds_xmin + 0.5 * t.bounds_width
        dy = -0.5; y = 1 + dy;
        t.show_marker('string' => 'Shadow Effect', 'scale' => scale, 'point' => [x,y], 'mode' => FILL_AND_STROKE,
            'alignment' => ALIGNED_AT_BASELINE, 'fill_color' => Blue, 'stroke_color' => Black)
        t.fill_opacity = 0.8
        t.show_marker('string' => 'Shadow Effect', 'vertical_scale' => -0.4, 'fill_color' => Grey,
            'italic_angle' => -30, 'scale' => scale, 'point' => [x+0.022,y-0.03])
    end
    
    def test_pattern # to check alignment of pdf and tex
        t.line_width = 1
        t.stroke_frame
        margin = 0.1
        t.set_subframe(
            'left' => margin, 'right' => margin,
            'top' => margin, 'bottom' => margin)
        centerx = t.bounds_xmin + 0.5 * t.bounds_width
        chr_ht = t.default_text_height_dy
        t.stroke_color = Red
        t.line_width = 1
        t.stroke_line(t.bounds_xmax, t.bounds_ymin, t.bounds_xmax, t.bounds_ymax)
        t.stroke_line(centerx, t.bounds_ymin, centerx, t.bounds_ymax)
        t.stroke_line(t.bounds_xmin, t.bounds_ymin, t.bounds_xmin, t.bounds_ymax)
        y = t.bounds_ymin; dy = t.bounds_height / 10
        scale = 0.85;
        10.times do
            t.stroke_line(t.bounds_xmin, y, t.bounds_xmax, y)
            t.show_text('text'=>"Left", 'x'=> t.bounds_xmin, 'y' => y,
                'scale'=>0.5+scale, 'justification' => LEFT_JUSTIFIED)
            t.show_text('text'=>"Center", 'x'=> centerx, 'y' => y,
                'scale'=>1.8-scale, 'justification' => CENTERED)
            t.show_text('text'=>"Right", 'x'=> t.bounds_xmax, 'y' => y,
                'scale'=>0.5+scale, 'justification' => RIGHT_JUSTIFIED)
            scale = scale - 0.1
            y = y + dy
        end 
        t.stroke_line(t.bounds_xmin, t.bounds_ymax, t.bounds_xmax, t.bounds_ymax)
        t.justification = CENTERED
        t.show_text('text'=>"TeX and PDF Alignment Test Pattern",
            'side'=> TOP, 'pos' => 0.5, 'shift'=>0.3)
        t.show_text('text'=>"Check for Proper Registration of Text and Graphics",
            'side'=>BOTTOM, 'pos' => 0.5, 'shift'=> 1.6, 'scale'=>0.4)
        t.show_text(
            'text'=>'If needed, adjust by changing {\sffamily FigureMaker.tex\_xoffset} ' +
                'and {\sffamily FigureMaker.tex\_yoffset}.',
            'side'=>BOTTOM, 'pos' => 0.5, 'shift'=> 3.9, 'scale'=>0.3)
    end
    
    def math_typesetting
        t.landscape
        background
        centerx = t.bounds_xmin + 0.5 * t.bounds_width
        equation = 
            '\int_{-\infty}^{\infty} e^{\color{Red}-x^{2}}\, \! dx = ' +
            '\color{Green}\sqrt{\pi}'
        t.justification = CENTERED
        t.rescale_text(0.8)
        t.show_text('text' => 'Inline Math Mode',
            'x'=>centerx, 'y' => t.bounds_ymin + 0.88 * t.bounds_height)
        t.show_text('text' => 'Display Math Mode',
            'x'=> centerx, 'y' => t.bounds_ymin + 0.46 * t.bounds_height)
        t.rescale_text(0.8)
        t.show_text('text' => '$' + equation + '$',
            'x'=> centerx, 'y' => t.bounds_ymin + 0.78 * t.bounds_height, 'scale'=>1.3)
        t.show_text('text'=> '$' + equation + '$',
            'x'=> centerx, 'y' => t.bounds_ymin + 0.64 * t.bounds_height, 'angle' => 10, 'scale'=>1.3)
        equation = '\begin{displaymath}' + equation + '\end{displaymath}'
        equation = '\parbox{15em}{' + equation + '}'
        t.show_text('text' => equation, 'scale'=>1.3,
            'x'=> centerx, 'y' => t.bounds_ymin + 0.33 * t.bounds_height)
        t.show_text('text' => equation, 'scale'=>1.3, 'angle' => 10,
            'x' => centerx, 'y' => t.bounds_ymin + 0.16 * t.bounds_height)
    end
    
    def parbox
        t.portrait
        background
        centerx = t.bounds_xmin + 0.5 * t.bounds_width
        t.justification = CENTERED
        t.show_text('text' => 'Examples using \textbackslash parbox', 'x' => centerx, 'y' => 0.9)
        t.rescale_text(0.5)
        dx = 0.05; y = 0.78; dy = -0.16; t.line_width = 0.7; t.stroke_color = Blue
        t.show_text('text' => '\framebox{\parbox{20em}{\begin{enumerate}\item \textbackslash parbox\{20em\}\{...\} \item default height and position for list \end{enumerate}}}', 'at' => [centerx, y])
        y += dy
        t.show_text('text' => '\framebox{\parbox[c][6em][t]{20em}{\begin{enumerate}\item \textbackslash parbox[c][6em][t]\{20em\}\{...\} \item position list at top of box \end{enumerate}}}', 'at' => [centerx, y])
        y += dy
        t.show_text('text' => '\framebox{\parbox[c][6em][c]{20em}{\begin{enumerate}\item \textbackslash parbox[c][6em][c]\{20em\}\{...\} \item position list at center of box \end{enumerate}}}', 'at' => [centerx, y])
        y += dy 
        t.show_text('text' => '\framebox{\parbox[c][6em][b]{20em}{\begin{enumerate}\item \textbackslash parbox[c][6em][b]\{20em\}\{...\} \item position list at bottom of box \end{enumerate}}}', 'at' => [centerx, y])
        y += dy 
        t.show_text('text' => '\framebox{\parbox[c][6em][s]{20em}{\begin{enumerate}\item \textbackslash parbox[c][6em][s]\{20em\}\{...\} \item stretch list vertically to fill box \end{enumerate}}}', 'at' => [centerx, y])
    end
         
    def minipages
        t.landscape
        background
        centerx = t.bounds_xmin + 0.5 * t.bounds_width
        t.show_text('text' => 'Examples using paragraph boxes', 'x' => centerx, 'y' => 0.9)
        t.rescale_text(0.5)
        t.justification = CENTERED
        str2 = 'The \textcolor{Red}{minipage} is a vertical alignment environment with a \textcolor{Red}{specified width}.  It can contain paragraphs, lists, tables, and equations.  Hyphenation and formatting is automatic.'
        str2 = '\parbox{15em}{' + str2 + '}'
        t.show_text('text' => str2, 'x'=> centerx, 'y' => t.bounds_ymin + 0.68 * t.bounds_height)
        t.show_text('text' => str2, 'x'=> centerx, 'y' => t.bounds_ymin + 0.30 * t.bounds_height, 'angle' => 20)
    end
    
    def minipage_list
        t.landscape
        background
        x = t.bounds_xmin + 0.25 * t.bounds_width
        t.justification = LEFT_JUSTIFIED
        t.show_text('text'=>'\texttt{\textbackslash begin\{itemize\}...}',
            'x'=> x+0.05, 'y' => t.bounds_ymin + 0.925 * t.bounds_height)
        t.show_text('text'=>'\texttt{...\textbackslash end\{itemize\}}',
            'x'=> x+0.05, 'y' => t.bounds_ymin + 0.07 * t.bounds_height)
        t.rescale(0.6)
        str2 = '
            \begin{itemize}
            \item This is the \textcolor{Red}{first} entry of an itemized list.
            \item This is the \textcolor{Red}{second}.
            \item And this is the \textcolor{Red}{third}.
            \end{itemize}
            '
        str2 = '\parbox{15em}{' + str2 + '}'
        t.show_text('text'=>str2,
            'x'=> x, 'y' => t.bounds_ymin + 0.7 * t.bounds_height)
        t.show_text('text'=>str2,
            'x'=> x, 'y' => t.bounds_ymin + 0.27 * t.bounds_height, 'angle' => 14)
    end
    
    def minipage_table
        t.landscape
        background
        x = t.bounds_xmin + 0.35 * t.bounds_width
        t.justification = LEFT_JUSTIFIED
        t.show_text('text'=>'\texttt{\textbackslash begin\{tabular\}...}',
            'x'=> x-0.05, 'y' => t.bounds_ymin + 0.925 * t.bounds_height)
        t.show_text('text'=>'\texttt{...\textbackslash end\{tabular\}}',
            'x'=> x-0.05, 'y' => t.bounds_ymin + 0.07 * t.bounds_height)
        t.rescale_text(0.6)
        str2 = '
            \renewcommand{\arraystretch}{1.5}
            \begin{tabular}{|r|l|c|} \hline  
               & \itshape Name                      & \itshape Code \\\  \hline
            1  & \textcolor{Red}{Alice}    & 7    \\\  \hline
            2  & \textcolor{Green}{Bob}    & 3    \\\  \hline
            3  & \textcolor{Blue}{Charlie} & 5    \\\  \hline
            \end{tabular}'
        str2 = '\parbox{15em}{' + str2 + '}'
        t.show_text('text'=>str2,
            'x'=> x, 'y' => t.bounds_ymin + 0.7 * t.bounds_height)
        t.show_text('text'=>str2,
            'x'=> x, 'y' => t.bounds_ymin + 0.29 * t.bounds_height, 'angle' => 14)
    end
    
    def squares(opacity)
        t.stroke_color = SlateGray
        t.fill_opacity = opacity
        t.line_width = 1.5
        t.fill_color = LemonChiffon
        t.fill_and_stroke_rect(0.05, 0.05, 0.9, 0.9)
        t.fill_color = DarkSlateGray
        width = height = 0.5
        x = y = 0.1
        t.fill_and_stroke_rect(x, y, width, height)
        t.fill_color = CadetBlue
        width *= 0.9; height *= 0.9; x += 0.1; y+= 0.1
        t.fill_and_stroke_rect(x, y, width, height)
        t.fill_color = OrangeRed
        width *= 0.9; height *= 0.9; x += 0.1; y+= 0.1
        t.fill_and_stroke_rect(x, y, width, height)
        t.fill_color = Aquamarine
        width *= 0.9; height *= 0.9; x += 0.1; y+= 0.1
        t.fill_and_stroke_rect(x, y, width, height)
        t.fill_color = Lavender
        width *= 0.9; height *= 0.9; x += 0.1; y+= 0.1
        t.fill_and_stroke_rect(x, y, width, height)
        t.fill_color = NavajoWhite
        width *= 0.9; height *= 0.9; x += 0.1; y+= 0.1
        t.fill_and_stroke_rect(x, y, width, height)
    end
    
    def opaque_squares
        squares(1.0)
    end
    
    def transparent_squares
        squares(0.6)
    end
    
    def jpg_image
        t.show_image( # Cassini image of Jupiter with Io in foreground
            'jpg' => "data/cassini.jpg", 'width' => 999, 'height' => 959,
            'll' => [0.01, 0.01], 'lr' => [0.99, 0.01], 'ul' => [0.01, 0.99]) 
    end
    
    def jpg_image_transparent
        t.show_image(
            'jpg' => "data/cassini.jpg", 'width' => 999, 'height' => 959,
            'll' => [0.01, 0.01], 'lr' => [0.99, 0.01], 'ul' => [0.01, 0.99])
        t.fill_opacity = 0.6
        t.show_image( # Lucy amazed by Io
            'jpg' => "data/lucy.jpg", 'width' => 148, 'height' => 164,
            'll' => [0.52, 0.97], 'lr' => [0.41, 0.52], 'ul' => [0.12, 0.97])
    end
    
    def rgb_image
        background
        samples = "\277" "\000" "\000" "\000" "\277" "\000" "\000" "\000" "\277" "\237" "\237" "\237"
        t.show_image(
            'll' => [0.1, 0.1], 'lr' => [0.9, 0.1], 'ul' => [0.1, 0.9], 
            'color_space' => 'RGB', 'w' => 2, 'h' => 2, 'data' => samples)
    end
    
    def hls_image
        background
        samples = t.string_rgb_to_hls("\277" "\000" "\000" "\000" "\277" "\000" "\000" "\000" "\277" "\237" "\237" "\237")
        t.show_image(
            'll' => [0.1, 0.1], 'lr' => [0.9, 0.1], 'ul' => [0.1, 0.9], 
            'color_space' => 'HLS', 'w' => 2, 'h' => 2, 'data' => samples)
    end

    def mono_image_table(light, dark)
        num_cols = 6
        num_rows = 6
        table = Dtable.new(num_cols, num_rows)
        row0 = Dvector[light, dark, light, dark, light, dark]
        row1 = Dvector[dark, light, dark, light, dark, light]
        table.set_row(0,row0)
        table.set_row(1,row1)
        table.set_row(2,row0)
        table.set_row(3,row1)
        table.set_row(4,row0)
        table.set_row(5,row1)
        return table
    end
    
    def mono_image
        samples = t.create_monochrome_image_data(mono_image_table(0.1, 0.9), 'boundary' => 0.5)
        background
        t.fill_color = Black
        t.show_image(
            'width' => 6, 'height' => 6, 'color_space' => 'mono', 'data' => samples,
            'interpolate' => false,
            'll' => [0.1, 0.1], 'lr' => [0.9, 0.1], 'ul' => [0.1, 0.9]) 
    end

    def build_table(light, dark)
        num_cols = 8
        num_rows = 8
        table = Dtable.new(num_cols, num_rows)
        row0 = Dvector[light, dark, light, dark, light, dark, light, dark]
        row1 = Dvector[dark, light, dark, light, dark, light, dark, light]
        table.set_row(0,row0)
        table.set_row(1,row1)
        table.set_row(2,row0)
        table.set_row(3,row1)
        table.set_row(4,row0)
        table.set_row(5,row1)
        table.set_row(6,row0)
        table.set_row(7,row1)
        return table
    end
 
    def stencil_mask
        t.show_image(
            'jpg' => "data/lucy.jpg", 'width' => 148, 'height' => 164,
            'll' => [0.99, 0.01], 'lr' => [0.01, 0.01], 'ul' => [0.99, 0.99])
        samples = t.create_monochrome_image_data(build_table(0.1, 0.9), 'boundary' => 0.5)        
        dict = Hash.new
        dict['interpolate'] = false
        dict['ll'] = [0.1, 0.1]
        dict['lr'] = [0.9, 0.1]
        dict['ul'] = [0.1, 0.9]
        dict['color_space'] = 'mono'
        dict['w'] = 8
        dict['h'] = 8
        dict['data'] = samples
        t.show_image(
            'jpg' => "data/cassini.jpg", 'width' => 999, 'height' => 959, 'stencil_mask' => dict,
            'll' => [0.01, 0.01], 'lr' => [0.99, 0.01], 'ul' => [0.01, 0.99]) 
    end
    
    def axial_shading
        t.landscape
        background
        t.clip_rect(0, 0, 1, 1)
        t.axial_shading(
            'start_point' => [0, 0],
            'end_point' => [1, 1], 
            'colormap' => t.mellow_colormap
        )
    end
    
    def radial_shading
        t.landscape
        background
        t.clip_rect(0, 0, 1, 1)
        t.radial_shading(
            'x_hat' => [0.5, 0.2], 'y_hat' => [0.0, 0.75],
            'start_circle' => [0.75, 0.65, 0.9], 'end_circle' => [0.75, 0.65, 0.0], 
            'colormap' => t.intense_colormap, 'extend_start' => true)
    end
    
    def subfigures
        t.rescale_text(0.45)
        t.subfigure('right' => 0.5, 'bottom' => 0.5) { icon }
        t.subfigure('right' => 0.5, 'top' => 0.5) { t.stroke_color = White; math_typesetting }
        t.subfigure('left' => 0.5, 'bottom' => 0.5) { transparent_squares }
        t.subfigure('left' => 0.5, 'top' => 0.5) { jpg_image_transparent }
    end
    
    def star(eo_flag)
        t.fill_color = LightBlue
        t.line_width = 1
        t.stroke_color = Black
        c = cos(4*PI/5)
        s = sin(4*PI/5)
        dx = 0; dy = 0.15
        x = 0.25; y = 0.7
        t.move_to_point(x+dx, y+dy)
        tmp = dx*c - dy*s; dy = dx*s + dy*c; dx = tmp
        t.append_point_to_path(x+dx, y+dy)
        tmp = dx*c - dy*s; dy = dx*s + dy*c; dx = tmp
        t.append_point_to_path(x+dx, y+dy)
        tmp = dx*c - dy*s; dy = dx*s + dy*c; dx = tmp
        t.append_point_to_path(x+dx, y+dy)
        tmp = dx*c - dy*s; dy = dx*s + dy*c; dx = tmp
        t.append_point_to_path(x+dx, y+dy)
        t.close_path
        if eo_flag
            t.eofill
        else
            t.fill
        end      
        
        t.fill_color = Black
        dx = 0; dy = 0.15
        x = 0.25; y = 0.7
        tail = [x+dx, y+dy]
        tmp = dx*c - dy*s; dy = dx*s + dy*c; dx = tmp
        head = [x+dx, y+dy]
        t.show_arrow('tail' => tail, 'head' => head, 'tail_marker' => 'None')
        tail = head
        tmp = dx*c - dy*s; dy = dx*s + dy*c; dx = tmp
        head = [x+dx, y+dy]
        t.show_arrow('tail' => tail, 'head' => head, 'tail_marker' => 'None')
        tail = head
        tmp = dx*c - dy*s; dy = dx*s + dy*c; dx = tmp
        head = [x+dx, y+dy]
        t.show_arrow('tail' => tail, 'head' => head, 'tail_marker' => 'None')
        tail = head
        tmp = dx*c - dy*s; dy = dx*s + dy*c; dx = tmp
        head = [x+dx, y+dy]
        t.show_arrow('tail' => tail, 'head' => head, 'tail_marker' => 'None')
        tail = head
        tmp = dx*c - dy*s; dy = dx*s + dy*c; dx = tmp
        head = [x+dx, y+dy]
        t.show_arrow('tail' => tail, 'head' => head, 'tail_marker' => 'None')
        
    end
    
    def square1(eo_flag)
        x = 0.5; y = 0.3; dx = 0; dy = 0.15
        t.move_to_point(x+dx, y+dy)
        tmp = dx; dx = dy; dy = -tmp
        t.append_point_to_path(x+dx, y+dy)
        tmp = dx; dx = dy; dy = -tmp
        t.append_point_to_path(x+dx, y+dy)
        tmp = dx; dx = dy; dy = -tmp
        t.append_point_to_path(x+dx, y+dy)
        t.close_path
        
        x = 0.5; y = 0.3; dx = 0; dy = 0.1
        t.move_to_point(x+dx, y+dy)
        tmp = dx; dx = dy; dy = -tmp
        t.append_point_to_path(x+dx, y+dy)
        tmp = dx; dx = dy; dy = -tmp
        t.append_point_to_path(x+dx, y+dy)
        tmp = dx; dx = dy; dy = -tmp
        t.append_point_to_path(x+dx, y+dy)
        t.close_path
        t.fill_color = LightBlue
        if eo_flag
            t.eofill
        else
            t.fill
        end      
        
        t.fill_color = Black
        x = 0.5; y = 0.3; dx = 0; dy = 0.15
        tail = [x+dx, y+dy]
        tmp = dx; dx = dy; dy = -tmp
        head = [x+dx, y+dy]
        t.show_arrow('tail' => tail, 'head' => head, 'tail_marker' => 'None')
        tail = head
        tmp = dx; dx = dy; dy = -tmp
        head = [x+dx, y+dy]
        t.show_arrow('tail' => tail, 'head' => head, 'tail_marker' => 'None')
        tail = head
        tmp = dx; dx = dy; dy = -tmp
        head = [x+dx, y+dy]
        t.show_arrow('tail' => tail, 'head' => head, 'tail_marker' => 'None')
        tail = head
        tmp = dx; dx = dy; dy = -tmp
        head = [x+dx, y+dy]
        t.show_arrow('tail' => tail, 'head' => head, 'tail_marker' => 'None')
        
        x = 0.5; y = 0.3; dx = 0; dy = 0.1
        tail = [x+dx, y+dy]
        tmp = dx; dx = dy; dy = -tmp
        head = [x+dx, y+dy]
        t.show_arrow('tail' => tail, 'head' => head, 'tail_marker' => 'None')
        tail = head
        tmp = dx; dx = dy; dy = -tmp
        head = [x+dx, y+dy]
        t.show_arrow('tail' => tail, 'head' => head, 'tail_marker' => 'None')
        tail = head
        tmp = dx; dx = dy; dy = -tmp
        head = [x+dx, y+dy]
        t.show_arrow('tail' => tail, 'head' => head, 'tail_marker' => 'None')
        tail = head
        tmp = dx; dx = dy; dy = -tmp
        head = [x+dx, y+dy]
        t.show_arrow('tail' => tail, 'head' => head, 'tail_marker' => 'None')
        
    end
    
    def square2(eo_flag)
        x = 0.75; y = 0.705; dx = 0; dy = 0.15
        t.move_to_point(x+dx, y+dy)
        tmp = dx; dx = dy; dy = -tmp
        t.append_point_to_path(x+dx, y+dy)
        tmp = dx; dx = dy; dy = -tmp
        t.append_point_to_path(x+dx, y+dy)
        tmp = dx; dx = dy; dy = -tmp
        t.append_point_to_path(x+dx, y+dy)
        t.close_path

        x = 0.75; y = 0.705; dx = 0; dy = -0.1
        t.move_to_point(x+dx, y+dy)
        tmp = dx; dx = -dy; dy = tmp
        t.append_point_to_path(x+dx, y+dy)
        tmp = dx; dx = -dy; dy = tmp
        t.append_point_to_path(x+dx, y+dy)
        tmp = dx; dx = -dy; dy = tmp
        t.append_point_to_path(x+dx, y+dy)
        t.close_path
        t.fill_color = LightBlue
        if eo_flag
            t.eofill
        else
            t.fill
        end      
        
        t.fill_color = Black
        x = 0.75; y = 0.705; dx = 0; dy = 0.15
        tail = [x+dx, y+dy]
        tmp = dx; dx = dy; dy = -tmp
        head = [x+dx, y+dy]
        t.show_arrow('tail' => tail, 'head' => head, 'tail_marker' => 'None')
        tail = head
        tmp = dx; dx = dy; dy = -tmp
        head = [x+dx, y+dy]
        t.show_arrow('tail' => tail, 'head' => head, 'tail_marker' => 'None')
        tail = head
        tmp = dx; dx = dy; dy = -tmp
        head = [x+dx, y+dy]
        t.show_arrow('tail' => tail, 'head' => head, 'tail_marker' => 'None')
        tail = head
        tmp = dx; dx = dy; dy = -tmp
        head = [x+dx, y+dy]
        t.show_arrow('tail' => tail, 'head' => head, 'tail_marker' => 'None')
        tail = head

        x = 0.75; y = 0.705; dx = 0; dy = -0.1
        tail = [x+dx, y+dy]
        tmp = dx; dx = -dy; dy = tmp
        head = [x+dx, y+dy]
        t.show_arrow('tail' => tail, 'head' => head, 'tail_marker' => 'None')
        tail = head
        tmp = dx; dx = -dy; dy = tmp
        head = [x+dx, y+dy]
        t.show_arrow('tail' => tail, 'head' => head, 'tail_marker' => 'None')
        tail = head
        tmp = dx; dx = -dy; dy = tmp
        head = [x+dx, y+dy]
        t.show_arrow('tail' => tail, 'head' => head, 'tail_marker' => 'None')
        tail = head
        tmp = dx; dx = -dy; dy = tmp
        head = [x+dx, y+dy]
        t.show_arrow('tail' => tail, 'head' => head, 'tail_marker' => 'None')
        tail = head
        
    end
    
    def winding_number_rule
        background
        t.show_text('text' => "Winding Number Fill Rule", 'side' => TOP, 'shift' => 0.6,
            'justification' => CENTERED, 'scale' => 0.7, 'position' => 0.5)
        t.rescale(0.3)
        star(false)
        square1(false)
        square2(false)
    end
    
    def even_odd_rule
        background
        t.show_text('text' => "Even-Odd Fill Rule", 'side' => TOP, 'shift' => 0.6,
            'justification' => CENTERED, 'scale' => 0.7, 'position' => 0.5)
        t.rescale(0.3)
        star(true)
        square1(true)
        square2(true)
    end
    
    def fill_rules
        t.subfigure('right_margin' => 0.5, 'top_margin' => 0.5) { winding_number_rule }
        t.subfigure('left_margin' => 0.5, 'top_margin' => 0.5) { even_odd_rule }
    end
    
    def line_type_example(y, type, type_name, txt)
        t.context do
            x_type_name = 0.6; x0 = 0.05; x1 = 1 - x0; x_description = x_type_name
            dy = -t.default_text_height_dy
            t.show_text('text' => type_name, 'x' => x_type_name, 'y' => y, 'justification' => RIGHT_JUSTIFIED, 'scale' => 0.8)
            t.show_text('text' => ' =   ' + txt, 'x' => x_description, 'y' => y,
                'justification' => LEFT_JUSTIFIED, 'scale' => 0.8)
            t.stroke_color = DarkBlue
            t.line_cap = LINE_CAP_BUTT
            t.line_type = type
            t.line_width = 2
            y += 0.5 * dy
            t.stroke_line(x0, y, x1, y)
        end
    end
    
    def line_types
        t.landscape
        background
        y = 0.9; dy = -0.135
        t.rescale(0.8)
        line_type_example(y, Line_Type_Solid, 
            'Line\_Type\_Solid', '[[ ], 0]')
        y += dy
        line_type_example(y, Line_Type_Dot, 
            'Line\_Type\_Dot', '[[1, 2], 0]')
        y += dy
        line_type_example(y, Line_Type_Dash, 
            'Line\_Type\_Dash', '[[4, 2], 0]')
        y += dy
        line_type_example(y, Line_Type_Long_Dash, 
            'Line\_Type\_Long\_Dash', '[[6, 2], 0]')
        y += dy
        line_type_example(y, Line_Type_Dot_Dash, 
            'Line\_Type\_Dot\_Dash', '[[1, 2, 4, 2], 0]')
        y += dy
        line_type_example(y, Line_Type_Dot_Long_Dash,
            'Line\_Type\_Dot\_Long\_Dash', '[[1, 2, 6, 2], 0]')
        y += dy
        line_type_example(y, Line_Type_Short_Dash_Long_Dash,
            'Line\_Type\_Short\_Dash\_Long\_Dash', '[[4, 2, 6, 2], 0]')
    end 
    
    def line_cap_example(y, cap, cap_name)
        t.context do
            x_cap_name = 0.6; x0 = 0.3; x1 = 0.85; x_description = 0.15
            dy = t.default_text_height_dy
            t.show_text('text' => cap_name, 
                'x' => x_cap_name, 'y' => y, 'justification' => RIGHT_JUSTIFIED)
            t.stroke_color = DarkBlue
            t.line_cap = cap
            t.line_width = 12
            y -= 1.6*dy
            t.stroke_line(x0, y, x1, y)
            t.stroke_color = White
            t.line_cap = LINE_CAP_BUTT
            t.line_width = 1
            t.stroke_line(x0, y, x1, y)
            t.show_marker('marker' => Bullet, 'scale' => 0.4, 'x' => x0, 'y' => y, 'color' => White)
            t.show_marker('marker' => Bullet, 'scale' => 0.4, 'x' => x1, 'y' => y, 'color' => White)
        end
    end
    
    def line_caps
        t.landscape
        background
        y = 0.8; dy = -0.27
        t.rescale(0.9)
        line_cap_example(y, LINE_CAP_BUTT, 'LINE\_CAP\_BUTT')
        y += dy
        line_cap_example(y, LINE_CAP_ROUND, 'LINE\_CAP\_ROUND')
        y += dy
        line_cap_example(y, LINE_CAP_SQUARE, 'LINE\_CAP\_SQUARE')
    end
    
    def line_join_example(y, join, join_name)
        t.context do
            line_dy = 0.17
            x_join_name = 0.65; x0 = 0.3; x2 = 0.75; x1 = 0.8; x_description = 0.15
            dy = -t.default_text_height_dy
            t.show_text('text' => join_name, 'x' => x_join_name, 'y' => y+1.3*dy, 'justification' => RIGHT_JUSTIFIED)
            t.stroke_color = DarkBlue
            t.line_cap = LINE_CAP_BUTT
            t.line_join = join
            t.line_width = 12
            y -= 0.5*dy
            t.move_to_point(x0, y)
            t.append_point_to_path(x1, y)
            t.append_point_to_path(x2, y-line_dy)
            t.stroke
            t.stroke_color = White
            t.line_width = 1
            t.move_to_point(x0, y)
            t.append_point_to_path(x1, y)
            t.append_point_to_path(x2, y-line_dy)
            t.stroke
            t.show_marker('marker' => Bullet, 'scale' => 0.4, 'x' => x0, 'y' => y, 'color' => White)
            t.show_marker('marker' => Bullet, 'scale' => 0.4, 'x' => x1, 'y' => y, 'color' => White)
            t.show_marker('marker' => Bullet, 'scale' => 0.4, 'x' => x2, 'y' => y-line_dy, 'color' => White)
        end
    end
    
    def line_joins
        t.landscape
        background
        y = 0.8; dy = -0.27
        t.rescale(0.9)
        line_join_example(y, LINE_JOIN_MITER, 'LINE\_JOIN\_MITER')
        y += dy
        line_join_example(y, LINE_JOIN_ROUND, 'LINE\_JOIN\_ROUND')
        y += dy
        line_join_example(y, LINE_JOIN_BEVEL, 'LINE\_JOIN\_BEVEL')
    end
    
    def rendering_mode_example(y, mode, mode_name, clipping, description)
        t.context do
            x_mode_name = 0.65; x_example = 0.8
            t.show_text('text' => mode_name, 'x' => x_mode_name, 'y' => y-0.01,
                'justification' => RIGHT_JUSTIFIED, 'scale' => 1.4)
            t.show_marker('marker' => HeartWide, 'scale' => 3.2, 'fill_color' => Blue, 'stroke_color' => Black,
                'mode' => mode, 'x' => x_example, 'y' => y+0.01)
            return unless clipping
            t.context do
                t.line_width = 2
                t.stroke_color = Red
                y -= 0.02
                7.times { |i| t.stroke_line(x_mode_name, y+i*0.01, 1, y+i*0.01) }
            end
        end
    end
    
    def rendering_modes
        t.landscape
        background
        t.show_text('text' => "Marker Rendering Modes", 'side' => TOP, 'shift' => 0.6,
            'justification' => CENTERED, 'position' => 0.5, 'scale' => 0.9)
        y = 0.89; dy = -0.11
        t.rescale(0.6)
        t.stroke_color = White
        rendering_mode_example(y, FILL, 'FILL', false, 'Fill path with current fill\_color')
        y += dy
        rendering_mode_example(y, STROKE, 'STROKE', false, 'Stroke path with current stroke\_color')
        y += dy
        rendering_mode_example(y, FILL_AND_STROKE, 'FILL\_AND\_STROKE', false, 'Fill path, then stroke')
        y += dy
        rendering_mode_example(y, DISCARD, 'DISCARD', false, 'Neither fill nor stroke')
        y += dy
        rendering_mode_example(y, FILL_AND_CLIP, 'FILL\_AND\_CLIP', true, 'Fill and add to path for clipping')
        y += dy
        rendering_mode_example(y, STROKE_AND_CLIP, 'STROKE\_AND\_CLIP', true, 'Stroke and add to path for clipping')
        y += dy
        rendering_mode_example(y, FILL_STROKE_AND_CLIP, 'FILL\_STROKE\_AND\_CLIP', true,
            'Fill, stroke, and add for clipping')
        y += dy
        rendering_mode_example(y, CLIP, 'CLIP', true, 'Add to path for clipping')
    end
    
    def background
        clr = t.rgb_to_hls(Tan)
        clr[1] = 0.97
        t.fill_color = t.hls_to_rgb(clr)
        t.line_width = 2
        t.fill_and_stroke_frame
    end
        
    def show_marker(name, x, y)
        x_name = x - 0.02
        x_marker = x + 0.02
        dy = t.default_text_height_dy * 0.4
        t.show_text('text' => name, 'x' => x_name, 'y' => y, 'justification' => RIGHT_JUSTIFIED)
        t.show_marker('x' => x_marker, 'y' => y+dy, 'marker' => eval(name),
            'scale' => 1.8, 'justification' => CENTERED);
    end

    def marker_names
        t.set_portrait
        background
        t.rescale(0.45)
        dy = -t.default_text_height_dy * 1.95
        t.fill_color = Black
        x = 0.24; y = y_init = 0.94
        list = [ 'Arrow', 'Arrowhead', 'Asterisk', 'Bar', 'BarThick', 'BarThin', 'Box', 'Bullet', 
            'Check', 'Circle', 'Club', 'Diamond', 'Semicircle', 'Heart', 'HeartWide',
            'Spade', 'Square', 'Star', 'TriangleUp', 'TriangleDown', 'Plus', 'Times']
        list.each { |name| show_marker(name, x, y); y += dy }
        x = 0.61; y = y_init
        list = [ 'ArrowOpen', 'ArrowheadOpen', 'AsteriskOpen', 'BarOpen', 'BarThickOpen', 'BarThinOpen',
            'BoxOpen', 'BulletOpen', 'CheckOpen', 'CircleOpen', 'ClubOpen',
            'DiamondOpen', 'SemicircleOpen', 'HeartOpen', 'HeartWideOpen', 'SpadeOpen',
            'SquareOpen', 'StarOpen', 'TriangleUpOpen', 'TriangleDownOpen', 'PlusOpen', 'TimesOpen']
        list.each { |name| show_marker(name, x, y); y += dy }
        x = 0.89; y = y_init
        list = [ 'Circled1', 'Circled2', 'Circled3', 'Circled4', 'Circled5', 'Circled6',
            'Circled7', 'Circled8', 'Circled9', 'Circled10', 'SolidBall1', 'SolidBall2', 
            'SolidBall3', 'SolidBall4', 'SolidBall5', 'SolidBall6', 'SolidBall7', 'SolidBall8',
            'SolidBall9', 'SolidBall10', 'Hand', 'OtherHand' ]
        list.each { |name| show_marker(name, x, y); y += dy }
    end


    
end

MyFigures.new









