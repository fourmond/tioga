# visual_tests.rb
# This file is intended for many visual tests for the completion of
# Tioga. It is based on the plots.rb file. You may do whatever you think is
# suitable for this file.


require 'Tioga/FigureMaker'


class MyPlots

    include Math
    include Tioga
    include FigureConstants
    
    def t
        @figure_maker
    end

    def initialize
        @figure_maker = FigureMaker.default
        t.save_dir = 'visual_tests_out'
        t.def_eval_function { |str| eval(str) }
        
        t.def_figure("Arrows") { arrows }
        t.def_figure("Markers") { marker_position }
        t.def_figure("Lines") { lines_position }
        t.def_figure("Cap") { lines_cap }
        t.def_figure("show_marker_bbox") {show_marker_bbox}
        t.def_figure("marker_centered") {marker_centered}

        t.def_figure("Buggy ticks") { buggy_ticks }


        t.model_number = -1
        
        t.def_enter_page_function { enter_page }
            
    end
    
    def enter_page
        t.page_setup(11*72/2,8.5*72/2)
        t.set_frame_sides(0.15,0.85,0.85,0.15) # left, right, top, bottom in page coords        
    end
    
    def read_data
      @positions = Dvector.new(30) { |i| i}
      @blues = @positions*@positions
      @blues.sin!
      @blues = @blues*@blues
      
      @have_data = true
      t.need_to_reload_data = false
    end

    def plot_boundaries(xs,ys,margin,ymin=nil,ymax=nil)
        xmin = xs.min
        xmax = xs.max
        ymin = ys.min if ymin == nil
        ymax = ys.max if ymax == nil
        width = (xmax == xmin)? 1 : xmax - xmin
        height = (ymax == ymin)? 1 : ymax - ymin
        left_boundary = xmin - margin * width
        right_boundary = xmax + margin * width
        top_boundary = ymax + margin * height
        bottom_boundary = ymin - margin * height
        return [ left_boundary, right_boundary, top_boundary, bottom_boundary ]
    end
    
    def buggy_ticks
      read_data
      t.do_box_labels('Check the ticks are correct', 
                      'Position', 'Values')
      t.vincent_or_bill = true 
      xs = @positions
      ys = @blues * (6.82507277e-07- 3.66090205e-07) + 3.66090205e-07 
      t.show_plot(plot_boundaries(xs,ys,0)) { 
        t.show_polyline(xs,ys,Blue) 
      }
    end
    
    def reds
        read_data
        t.do_box_labels('Reds Plot', 'Position', '\textcolor{Crimson}{Reds}')
        show_model_number
        xs = @positions
        ys = @reds
        t.show_plot(plot_boundaries(xs,ys,@margin,-1,1)) { t.show_polyline(xs,ys,Red) }
    end
    
    def log_reds
        read_data
        t.do_box_labels('loglog Reds Plot', 'Position', '\textcolor[rgb]{1,0,0}{Reds}')
        show_model_number
        xs = @positions
        ys = @reds
        t.xaxis_log_values = true
        t.yaxis_log_values = true
        t.show_plot(plot_boundaries(xs,ys,@margin,-1,1)) { t.show_polyline(xs,ys,Red) }
    end
    
    def greens
        read_data
        show_model_number
        t.do_box_labels('Greens Plot', 'Position', '\textcolor[rgb]{0,0.5,0}{Greens}')
        xs = @positions
        ys = @greens
        greens_min = -4
        greens_max = -0.01
        boundaries = plot_boundaries(xs,ys,@margin,greens_min,greens_max)
        t.show_plot(boundaries) { t.show_polyline(xs,ys,Green) }
    end
    
    def reds_blues
        read_data
        t.set_aspect_ratio(1)
        t.do_box_labels("Reds and Blues", "Position", "Values")
        boundaries = setup_lines(@positions, [@blues, @reds], -1, 1)
        xs = @positions
        t.show_plot(boundaries) do
            t.show_polyline(xs,@blues,Blue,'Blues')
            t.show_polyline(xs,@reds,Red,'Reds')
        end
        show_model_number
    end
    
    def side_by_side
        read_data
        t.landscape
        t.do_box_labels('Side by Side', 'Position', nil)
        t.subplot('right_margin' => 0.5) { 
            t.yaxis_loc = t.ylabel_side = LEFT;
            t.right_edge_type = AXIS_LINE_ONLY; reds }
        t.subplot('left_margin' => 0.5) {
            t.yaxis_loc = t.ylabel_side = RIGHT;
            t.left_edge_type = AXIS_LINE_ONLY; greens }
        show_model_number
    end
    
    def two_yaxes
        read_data
        t.do_box_labels('Same X, Different Y\'s', 'Position', nil)
        t.subplot {
            t.yaxis_loc = t.ylabel_side = LEFT;
            t.right_edge_type = AXIS_HIDDEN; reds }
        t.subplot {
            t.yaxis_loc = t.ylabel_side = RIGHT;
            t.left_edge_type = AXIS_HIDDEN; greens }
        show_model_number
    end
    
    def legend_inside
        read_data
        show_model_number
        t.show_plot_with_legend(
            'legend_left_margin' => 0.65,
            'plot_scale' => 1,
            'legend_scale' => 1.3,
            'plot_right_margin' => 0) { reds_blues }
    end
    
    def legend_outside
        read_data
        show_model_number
        t.show_plot_with_legend('legend_scale' => 1.3) { reds_blues }
    end
    
    def legends
        read_data
        t.subfigure('top_margin' => 0.6) do
            t.rescale(0.5)
            t.subplot('right_margin' => 0.6) { legend_inside }
            t.subplot('left_margin' => 0.56) { legend_outside }
        end
    end
    
    def column_triplets
        read_data
        show_model_number
        column_margin = 0.15
        t.rescale(0.5)
        t.subplot(t.column_margins(
            'num_columns' => 3, 'column' => 1,
            'column_margin' => column_margin)) { blues }
        t.subplot(t.column_margins(
            'num_columns' => 3, 'column' => 2, 
            'column_margin' => column_margin)) { reds }
        t.subplot(t.column_margins(
            'num_columns' => 3, 'column' => 3, 
            'column_margin' => column_margin)) { greens }
    end
    
    def row_triplets
        read_data
        show_model_number
        row_margin = 0.15
        t.rescale(0.5)
        t.subplot(t.row_margins(
            'num_rows' => 3, 'row' => 1, 
            'row_margin' => row_margin)) { blues }
        t.subplot(t.row_margins(
            'num_rows' => 3, 'row' => 2, 
            'row_margin' => row_margin)) { reds }
        t.subplot(t.row_margins(
            'num_rows' => 3, 'row' => 3, 
            'row_margin' => row_margin)) { greens }
    end
    
    def rows
        read_data
        t.landscape
        show_model_number
        t.do_box_labels('Blues, Reds, Greens', 'Position', nil)
        t.rescale(0.8)
        num_plots = 3
        t.subplot(t.row_margins('num_rows' => num_plots, 'row' => 1)) do
            t.xaxis_type = AXIS_WITH_TICKS_ONLY
            blues
        end
        t.subplot(t.row_margins('num_rows' => num_plots, 'row' => 2)) do 
            t.xaxis_type = AXIS_WITH_TICKS_ONLY
            t.top_edge_type = AXIS_HIDDEN
            reds
        end
        t.subplot(t.row_margins('num_rows' => num_plots, 'row' => 3)) do 
            t.top_edge_type = AXIS_HIDDEN
            greens
        end
    end
    
    def columns
        read_data
        t.set_aspect_ratio(2)
        show_model_number
        t.rescale(0.8)
        t.do_box_labels('Blues, Reds, Greens', 'Position', 'Values for Colors')
        num_plots = 3
        t.subplot(t.column_margins('num_columns' => 3, 'column' => 1)) do
            t.right_edge_type = AXIS_HIDDEN
            blues
        end
        t.subplot(t.column_margins('num_columns' => 3, 'column' => 2)) do 
            t.yaxis_type = AXIS_WITH_TICKS_ONLY
            t.right_edge_type = AXIS_HIDDEN
            reds
        end
        t.subplot(t.column_margins('num_columns' => 3, 'column' => 3)) do
            t.yaxis_type = AXIS_WITH_TICKS_ONLY
            t.right_edge_type = AXIS_WITH_TICKS_ONLY
            greens
        end
    end
    
    def array
        read_data
        show_model_number
        t.do_box_labels("Array of Plots", "Position", "Values for Colors")
        row_margin = 0.0; num_rows = 2
        column_margin = 0.0; num_columns = 3
        t.subplot(t.row_margins('num_rows' => num_rows, 'row' => 1, 'row_margin' => row_margin)) do
            t.xaxis_type = AXIS_WITH_TICKS_ONLY
            margins = t.column_margins('num_columns' => num_columns, 
                'column' => 1, 'column_margin' => column_margin)
            t.subplot(margins) do
                t.right_edge_type = AXIS_HIDDEN
                blues
            end
            margins = t.column_margins('num_columns' => num_columns, 
                'column' => 2, 'column_margin' => column_margin)
            t.subplot(margins) do
                t.yaxis_type = AXIS_WITH_TICKS_ONLY
                t.right_edge_type = AXIS_HIDDEN
                reds
            end
            margins = t.column_margins('num_columns' => num_columns, 
                'column' => 3, 'column_margin' => column_margin)
            t.subplot(margins) do
                t.yaxis_type = AXIS_WITH_TICKS_ONLY
                t.right_edge_type = AXIS_WITH_TICKS_ONLY
                blues
            end
        end
        t.subplot(t.row_margins('num_rows' => num_rows, 'row' => 2, 'row_margin' => row_margin)) do 
            t.top_edge_type = AXIS_HIDDEN
            margins = t.column_margins('num_columns' => num_columns, 
                'column' => 1, 'column_margin' => column_margin)
            t.subplot(margins) do
                t.right_edge_type = AXIS_HIDDEN
                reds
            end
            margins = t.column_margins('num_columns' => num_columns, 
                'column' => 2, 'column_margin' => column_margin)
            t.subplot(margins) do
                t.yaxis_type = AXIS_WITH_TICKS_ONLY
                t.right_edge_type = AXIS_HIDDEN
                blues
            end
            margins = t.column_margins('num_columns' => num_columns, 
                'column' => 3, 'column_margin' => column_margin)
            t.subplot(margins) do
                t.yaxis_type = AXIS_WITH_TICKS_ONLY
                t.right_edge_type = AXIS_WITH_TICKS_ONLY
                reds
            end
        end
    end
    
    def setup_lines(xs, yarry, ymin, ymax)
        margin = 0.1
        num_lines = yarry.length
        return nil unless num_lines > 0
        xmin = xs.min
        xmax = xs.max
        width = (xmax == xmin)? 1 : xmax - xmin
        height = (ymax == ymin)? 1 : ymax - ymin
        return [ xmin - margin * width, xmax + margin * width, 
                 ymax + margin * height, ymin - margin * height ]
    end
    
    def trio
        read_data
        show_model_number
        t.rescale(0.6)
        t.subplot('bottom_margin' => 0.6, 'left_margin' => 0.15, 'right_margin' => 0.15) { rows }
        t.subplot('top_margin' => 0.60) { side_by_side }
    end
    
    def collage
        read_data
        show_model_number
        t.rescale(0.5)
        t.no_xlabel
        t.line_width = 0.5
        t.subplot(
            'left_margin' => 0.1, 'right_margin' => 0.5, 
            'bottom_margin' => 0.75) { t.ylabel_angle = -90; side_by_side }
        t.yaxis_loc = RIGHT
        t.subplot(
            'left_margin' => 0.75, 'right_margin' => 0.07, 
            'top_margin' => 0.2, 'bottom_margin' => 0.55) { reds }
        t.yaxis_loc = LEFT
        t.subplot(
            'right_margin' => 0.7, 'top_margin' => 0.4) { rows }
        t.subplot(
            'left_margin' => 0.5, 'top_margin' => 0.73) { legend_outside }
        t.subplot(
            'left_margin' => 0.4, 'right_margin' => 0.35,
            'top_margin' => 0.36, 'bottom_margin' => 0.4) { t.no_ylabel; blues }
    end
    
    def special_y
        read_data
        show_model_number
        t.ylabel_shift += 1.2
        t.do_box_labels('Special Y Axis', 'Position', "Y Values")
        t.yaxis_numeric_label_angle = -90
        t.yaxis_locations_for_major_ticks = [ -10.0, -6.0, -PI, 0.0, PI, 6.0, 10.0 ]
        t.yaxis_tick_labels = [
            "-IX", "-VI", "$-\\pi$", 
            "$\\mathcal{ZERO}$", "$\\pi$", "VI", "IX" ]
        t.yaxis_type = AXIS_WITH_MAJOR_TICKS_AND_NUMERIC_LABELS
        t.stroke_color = Blue
        t.yaxis_numeric_label_justification = RIGHT_JUSTIFIED
        t.right_edge_type = AXIS_WITH_MAJOR_TICKS_ONLY
        t.yaxis_numeric_label_shift = 0
        xs = @positions
        ys = @big_blues
        t.show_plot(plot_boundaries(xs,ys,@margin,-11, 11)) do
            t.append_points_to_path(xs,ys)
            t.stroke
        end
    end
    
    def do_labels_plot(title,&cmd)
        t.do_box_labels(title, "Position", "Blues")
        t.stroke_color = Blue
        read_data
        xs = @positions
        ys = @blues
        t.stroke_color = Blue
        num_pts = @positions.length
        @pt1 = (num_pts * 0.25 - t.model_number).round.mod(num_pts)
        @pt2 = (num_pts * 0.75 - t.model_number).round.mod(num_pts)
        t.show_plot(plot_boundaries(xs,ys,@margin,-1,1)) do
            t.append_points_to_path(xs,ys)
            t.stroke
            cmd.call
        end
        show_model_number
    end
    
    def do_label(xs, ys, pt, fmt, color, marker_scale, text_scale)
        x = xs[pt]; y = ys[pt]
        shift = text_scale * t.default_text_height_dy * 0.8
        shift = -shift if t.yaxis_reversed
        t.show_label('text' => sprintf(fmt, x, y), 'x' => x, 'y' => y + shift, 
            'justification' => CENTERED, 'color' => color, 'scale' => text_scale)
        t.show_marker('marker' => Bullet, 'point' => [x,y], 
            'scale' => marker_scale, 'color' => color)
    end
    
    def labels
        xs = @positions
        ys = @blues
        do_labels_plot("Labels on Points") do
            do_label(xs, ys, @pt1, "(%0.2f, %0.2f)", Red, 0.5, 0.6)
            do_label(xs, ys, @pt2, "(%0.2f, %0.2f)", Red, 0.5, 0.6)
        end
    end
    
    def error_bars
        xs = @positions
        ys = @blues
        do_labels_plot("Error Bars on Points") do
            t.show_error_bars('x' => xs[@pt2], 'y' => ys[@pt2],
                'dx' => 0.2, 'dy' => 0.12)
            t.show_error_bars('x' => xs[@pt1], 'y' => ys[@pt1],
                'dx' => 0.23, 'dy' => 0.15, 'color' => Crimson)
        end
    end
    
    def error_bars2
        t.do_box_labels("Another Example of Error Bars", "Number of Particles", "Crossing Times")
        xs = Dvector[3, 4, 5, 6, 7, 8, 9, 12]
        ys = Dvector[72, 10, 17, 20, 15.5, 11, 11.5, 11.5]
        deltaT = Dvector[13, 2.8, 3, 3.5, 2.7, 1.9, 2, 2]
        t.show_plot([0, 15, 100, 0]) do
            t.stroke_color = Blue
            t.show_polyline(xs, ys)
            xs.size.times { |i| t.show_error_bars('x' => xs[i], 'y' => ys[i], 'dx' => 0, 'dy' => deltaT[i]) }
            t.show_marker('Xs' => xs, 'Ys' => ys, 'marker' => Bullet, 
                'scale' => 0.3, 'color' => Red);
        end
    end
    
    def arrows
      x = 2.3
      y = 4.234
      #t.do_box_labels("Arrows tests", "Position", "Blues")
      t.show_plot([0,x + 1,0,y + 1]) do
        t.show_arrow('head' => [x,y], 
                     'tail' => [0, 0], 
                     'color' => Blue,
                     'line_width' => 0.05)
        t.show_arrow('head' => [0.5 * x, 0.5 * y], 
                     'tail' => [0, 0], 
                     'color' => Crimson,
                     'line_width' => 0.05)
        t.show_text('at' => [2,1], 
                    'text' => '\parbox{5cm}{Please check that the ' +
                    "symbols and the lines" + 
                    " are aligned together}")
      end
    end



    def marker_position
      x = Dvector[0,1,2,3,2,0,1,2,3,4,1,1]
      y = Dvector[2,0,3,5,4,0,1,2,3,4,4,-0.5]
      #t.do_box_labels("Arrows tests", "Position", "Blues")
      t.show_plot([-1,4,6,-1]) do
        t.show_marker('Xs' => x, 'Ys' => y,
                      'marker' => Circle,
                      'color' => Green)

        t.line_width = 0.3
        t.stroke_color = Red
        t.append_points_to_path(x, y)
        t.stroke

        t.show_text('at' => [1,5], 
                    'text' => '\parbox{5cm}{Please check that the ' +
                    "symbols are centered on the lines}")
      end
    end
    
    
    def marker_centered
      
      t.show_plot([-1,1,1,-1]) do
        angle = 90
        scale = 20
        mrkstr = "\251" # Diamond
        marker_info = t.marker_string_info(ZapfDingbats, mrkstr, scale)
        t.show_marker('x' => 0.0, 'y' => 0.0, 
                      'font' => ZapfDingbats,'string' => mrkstr, "scale" => scale, 'angle' => angle,
                      'color' => Green)
        t.stroke_width = 0.1
        t.stroke_line(0,-1,0,1)
        t.stroke_line(-1,0,1,0)
      end

    end
    
    
    def show_marker_bbox
      
      t.show_plot([-0.2,1.7,1.3,-0.2]) do
        angle = 0
        scale = 20
        mrkstr = "\251" # Diamond
        mrkstr = "\250" # Club
        mrkstr = "\252" # Heart
        mrkstr = "\072" # Cross
        mrkstr = "\053"
        marker_info = t.marker_string_info(ZapfDingbats, mrkstr, scale)
        t.show_marker('x' => 0.0, 'y' => 0.0, 
                      'justification' => LEFT, 'alignment' => ALIGNED_AT_BASELINE,
                      'font' => ZapfDingbats,'string' => mrkstr, "scale" => scale, 'angle' => angle,
                      'color' => Green)
        t.stroke_color = Crimson
        dx_left = marker_info[1]
        dy_down = marker_info[2]
        dx_right = marker_info[3]# - 0.115
        dy_up = marker_info[4]
        t.stroke_line(dx_left,dy_down,dx_left,dy_up)
        t.stroke_line(dx_right,dy_down,dx_right,dy_up)
        t.stroke_line(dx_left,dy_down,dx_right,dy_down)
        t.stroke_line(dx_left,dy_up,dx_right,dy_up)
      end
    
    end

    def lines_position
      x = Dvector[0,1,2,3,4]
      y = Dvector[0,1,2,3,4]
      #t.do_box_labels("Arrows tests", "Position", "Blues")
      t.show_plot([-1,5,8,-1]) do
        marker = Diamond
        angle = 0
        scale = 1
        t.show_marker('Xs' => x, 'Ys' => y,
                      'marker' => marker, "scale" => scale, 'angle' => angle,
                      'color' => Green)


        t.line_width = 3
        t.stroke_color = Red
        t.append_points_to_path(x, y)
        t.stroke


        y.add!(1)
        t.show_marker('Xs' => x, 'Ys' => y,
                      'marker' => marker, "scale" => 0.2, 'angle' => angle,
                      'color' => Green)

        t.line_width = 0.2
        t.stroke_color = Red
        t.append_points_to_path(x, y)
        t.stroke

        y.add!(1)
        y.reverse!
        x.reverse!
        t.show_marker('Xs' => x, 'Ys' => y,
                      'marker' => marker, "scale" => scale, 'angle' => angle,
                      'color' => Green)

        t.line_width = 3
        t.stroke_color = Red
        t.append_points_to_path(x, y)
        t.stroke

        t.line_width = 0.1
        t.stroke_color = Black
        for val in x
          t.append_points_to_path(Dvector[val,val], Dvector[0,6])
          t.stroke
        end

        t.show_text('at' => [1,7], 
                    'text' => '\parbox{5cm}{Please check that the ' +
                    "symbols are centered on the lines}")
      end
    end

    def lines_cap
      x = Dvector[0,0,1,1]
      y = Dvector[0,1,1,0]
      t.show_plot([-1,4,4,-1]) do

        t.line_width = 3
        t.line_cap = LINE_CAP_BUTT
        t.append_points_to_path(x, y)

        x.add!(2)
        t.move_to_point(2,0)
        x.shift
        y.shift

        t.stroke_color = Red
        t.append_points_to_path(x, y)
        t.stroke

        t.line_width = 0.1
        t.stroke_color = Green
        t.show_polyline([-1,4], [0,0])

        t.show_text('at' => [1,3], 
                    'text' => '\parbox{5cm}{Please check that the ' +
                    "red lines don't cross the green one}")
      end
    end
    
    def sampled_splines
        t.do_box_labels("Sampled Splines", "Position", "Average Count")
        xs = Dvector[ 1.0, 2.0, 5.0, 6.0, 7.0, 8.0, 10.0, 13.0, 17.0 ]
        ys = Dvector[ 3.0, 3.7, 3.9, 4.2, 5.7, 6.6,  7.1,  6.7,  4.5 ]
        data_pts = xs.size
        x_first = 0.0; x_last = 18.0; y_first = y_last = 2.5
        x_results = Dvector[]
        y_results = Dvector[]
        t.make_steps(
            'dest_xs' => x_results, 'dest_ys' => y_results, 
            'xs' => xs, 'ys' => ys,
            'x_first' => x_first, 'y_first' => y_first, 
            'x_last' => x_last, 'y_last' => y_last)
        t.show_plot([-1, 19, 8, 2]) do
            t.fill_color = FloralWhite
            t.fill_frame
            smooth_pts = 4*(data_pts-1) + 1
            dx = (xs[data_pts-1] - xs[0])/(smooth_pts-1)
            sample_xs = Dvector.new(smooth_pts) { |i| i*dx + xs[0] }
            result_ys = Dvector.new
            t.make_spline_interpolated_points(
                'sample_xs' => sample_xs, 'result_ys' => result_ys,
                'xs' => xs, 'ys' => ys,
                'start_slope' => 2.5*(ys[1]-ys[0])/(xs[1]-xs[0]))
            t.stroke_color = Blue
            t.append_points_to_path(sample_xs, result_ys)
            t.stroke
            t.show_marker('Xs' => sample_xs, 'Ys' => result_ys,
                'marker' => Bullet, 'scale' => 0.4, 'color' => Green);
            t.show_marker('Xs' => xs, 'Ys' => ys,
                'marker' => Bullet, 'scale' => 0.6, 'color' => Red);
        end
    end
    
    def steps
        t.do_box_labels("Steps", "Position", "Average Count")
        xs = Dvector[ 1.0, 2.0, 5.0, 6.0, 7.0, 8.0, 10.0, 13.0, 17.0 ]
        ys = Dvector[ 3.0, 3.7, 3.9, 4.2, 5.7, 6.6,  7.1,  6.7,  4.5 ]
        data_pts = xs.size
        x_first = 0.0; x_last = 18.0; y_first = y_last = 2.5
        x_results = Dvector[]
        y_results = Dvector[]
        t.make_steps(
            'dest_xs' => x_results, 'dest_ys' => y_results, 
            'xs' => xs, 'ys' => ys,
            'x_first' => x_first, 'y_first' => y_first, 
            'x_last' => x_last, 'y_last' => y_last)
        t.show_plot([-1, 19, 8, 2]) do
            t.fill_color = FloralWhite
            t.fill_frame
            t.stroke_color = Blue
            t.append_points_to_path(x_results, y_results)
            t.stroke
            t.show_marker('Xs' => xs, 'Ys' => ys, 'marker' => Bullet, 
                'scale' => 0.6, 'color' => Red);
        end
    end
    
    def splines # append bezier curves
        t.do_box_labels("Splines", "Position", "Average Count")
        xs = Dvector[ 1.0, 2.0, 5.0, 6.0, 7.0, 8.0, 10.0, 13.0, 17.0 ]
        ys = Dvector[ 3.0, 3.7, 3.9, 4.2, 5.7, 6.6,  7.1,  6.7,  4.5 ]
        t.show_plot([-1, 19, 8, 2]) do
            t.fill_color = FloralWhite
            t.fill_frame
            start_slope = 2.5*(ys[1]-ys[0])/(xs[1]-xs[0])
            interp = t.make_interpolant(
                'xs' => xs, 'ys' => ys, 
                'start_slope' => start_slope)
            t.append_interpolant_to_path(interp)
            t.stroke_color = Black
            t.stroke
            t.show_marker('Xs' => xs, 'Ys' => ys, 'marker' => Bullet, 
                'scale' => 0.6, 'color' => Red);
        end
    end
    
    def sampled_image(title, colormap = nil)
        t.do_box_labels(title, 'Log Density', 'Log Temperature')
        data = get_press_image
        xs = @eos_logRHOs
        ys = @eos_logTs
        colormap = t.mellow_colormap if colormap == nil
        t.show_plot([@eos_xmin, @eos_xmax, @eos_ymax, @eos_ymin]) do
            t.fill_color = Wheat
            t.fill_frame
            clip_press_image
            t.show_image(
                'll' => [xs.min, ys.min], 
                'lr' => [xs.max, ys.min], 
                'ul' => [xs.min, ys.max], 
                'color_space' => colormap, 
                'data' => data, 'value_mask' => 255,
                'w' => @eos_data_xlen, 'h' => @eos_data_ylen)
        end
    end
    
    def color_bar(ylabel, levels = nil)
        xmin = 0; xmax = 1; xmid = 0.5
        t.rescale(0.8)
        t.xaxis_type = AXIS_LINE_ONLY
        t.xaxis_loc = BOTTOM
        t.top_edge_type = AXIS_LINE_ONLY
        t.yaxis_loc = t.ylabel_side = RIGHT
        t.yaxis_type = AXIS_WITH_TICKS_AND_NUMERIC_LABELS
        t.left_edge_type = AXIS_WITH_TICKS_ONLY
        t.ylabel_shift += 0.5
        t.yaxis_major_tick_length *= 0.6
        t.yaxis_minor_tick_length *= 0.5
        t.show_ylabel(ylabel); t.no_ylabel
        t.show_plot('boundaries' => [xmin, xmax, @image_zmax, @image_zmin]) do
            t.axial_shading(
                'start_point' => [xmid, @image_zmin], 'end_point' => [xmid, @image_zmax], 
                'colormap' => t.mellow_colormap )
            if levels != nil
                t.stroke_color = Gray
                t.line_width = 1.5
                levels.each { |level| t.stroke_line(xmin, level, xmax, level) }
            end
        end
    end
    
    def sampled_data
        t.rescale(0.8)
        title = 'Log Opacity'
        t.subplot('right_margin' => @image_right_margin) { sampled_image(title) }
        t.subplot('left_margin' => 0.95, 
            'top_margin' => 0.05, 
            'bottom_margin' => 0.05) { color_bar(title) }
    end
    
    def read_press_image_data
        return unless @opacity_data == nil
        @eos_xmin = -8.5; @eos_xmax = 2.5
        @eos_ymin = 5.7; @eos_ymax = 7.0
        @image_zmin = -3
        @image_zmax = 6
        data = Dvector.read("data/logRHOs_for_EoS.data")
        @eos_logRHOs = data[0]
        @eos_data_xlen = @eos_logRHOs.size
        @eos_xmin = @eos_logRHOs.min; @eos_xmax = @eos_logRHOs.max
        data = Dvector.read("data/logTs_for_EoS.data")
        @eos_logTs = data[0]
        @eos_data_ylen = @eos_logTs.size
        @eos_ymin = @eos_logTs.min; @eos_ymax = @eos_logTs.max
        @opacity_data = Dtable.new(@eos_data_xlen, @eos_data_ylen)
        @opacity_data.read("data/Opacity_EoS.data")
    end
    
    def get_press_image
        read_press_image_data
        return t.create_image_data(
            @opacity_data,
            'min_value' => @image_zmin, 
            'max_value' => @image_zmax, 
            'masking' => true)
    end
    
    def clip_press_image
        t.move_to_point(t.bounds_left, t.bounds_bottom)
        t.append_point_to_path(t.bounds_left, 4.2)
        t.append_point_to_path(-3, t.bounds_top)
        t.append_point_to_path(t.bounds_right, t.bounds_top)
        t.append_point_to_path(t.bounds_right, t.bounds_bottom)
        t.close_path
        t.clip
    end
    
    def test_samples_with_contours
        t.rescale(0.8)
        title = 'Log Opacity'
        levels = Array.new
        (0..5).each { |i| levels << i + 0.4 }
        t.subplot('right_margin' => @image_right_margin) { sampled_image(title) }
        t.subplot('left_margin' => 0.95, 
            'top_margin' => 0.05, 
            'bottom_margin' => 0.05) { color_bar(title, levels) }
        t.subplot('right_margin' => @image_right_margin) do
            t.xaxis_type = t.yaxis_type = AXIS_WITH_TICKS_ONLY
            t.no_title; t.no_xlabel; t.no_ylabel
            bounds = [@eos_xmin, @eos_xmax, @eos_ymax, @eos_ymin]
            t.show_plot(bounds) do
                clip_press_image
                t.stroke_color = SlateGray
                t.line_width = 1
                dest_xs = Dvector.new; dest_ys = Dvector.new; gaps = Array.new
                dict = { 'dest_xs' => dest_xs, 
                        'dest_ys' => dest_ys, 
                        'gaps' => gaps,
                        'xs' => @eos_logRHOs, 
                        'ys' => @eos_logTs,
                        'data' => @opacity_data }
                levels.each do |level|
                    dict['level'] = level
                    num_xs = @eos_logRHOs.length
                    num_ys = @eos_logTs.length
                    legit = Dtable.new(num_xs, num_ys)
                    num_xs.times do |ix|
                      num_ys.times do |iy|
                        if @opacity_data[ix,iy] < -999
                          legit[ix,iy] = 0.0
                        else
                          legit[ix,iy] = 1.0
                        end
                      end
                    end
                    #dict['legit'] = legit
                    t.make_contour(dict)
                    if false
                      t.append_points_to_path(dest_xs, dest_ys)
                      t.fill_color = Green
                      t.fill
                    else
                      t.append_points_with_gaps_to_path(dest_xs, dest_ys, gaps, true)
                      t.stroke
                    end
                end
            end
        end
    end

    def samples_with_contours
        t.rescale(0.8)
        title = 'Log Opacity'
        levels = Array.new
        (0..5).each { |i| levels << i + 0.4 }
        t.subplot('right_margin' => @image_right_margin) { sampled_image(title) }
        t.subplot('left_margin' => 0.95, 
            'top_margin' => 0.05, 
            'bottom_margin' => 0.05) { color_bar(title, levels) }
        t.subplot('right_margin' => @image_right_margin) do
            t.xaxis_type = t.yaxis_type = AXIS_WITH_TICKS_ONLY
            t.no_title; t.no_xlabel; t.no_ylabel
            bounds = [@eos_xmin, @eos_xmax, @eos_ymax, @eos_ymin]
            t.show_plot(bounds) do
                clip_press_image
                t.stroke_color = SlateGray
                t.line_width = 1
                dest_xs = Dvector.new; dest_ys = Dvector.new; gaps = Array.new
                dict = { 'dest_xs' => dest_xs, 
                        'dest_ys' => dest_ys, 
                        'gaps' => gaps,
                        'xs' => @eos_logRHOs, 
                        'ys' => @eos_logTs,
                        'data' => @opacity_data }
                levels.each do |level|
                    dict['level'] = level
                    t.make_contour(dict)
                    t.append_points_with_gaps_to_path(dest_xs, dest_ys, gaps, true)
                    t.stroke
                end
            end
        end
    end


end

MyPlots.new


