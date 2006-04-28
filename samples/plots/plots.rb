# plots.rb

class MyPlots

    include Math
    include Tioga
    include FigureConstants
    
    def t
        @figure_maker
    end

    def initialize
        @figure_maker = FigureMaker.default
        t.save_dir = 'plots_out'
        @data_filename = "data/datalog.data"
        t.auto_refresh_filename = @data_filename
        t.add_model_number = true

        @image_right_margin = 0.07
        @margin = 0.1
        @header = Dvector.new
        @positions = Dvector.new
        @blues = Dvector.new
        @reds = Dvector.new
        @greens = Dvector.new
        @data_array = [@positions, @blues, @reds, @greens]
        @have_data = false
        @big_blues = Dvector.new
        @big_blues_scale = 11.0
        @blues_plot = t.def_figure("Blues") { blues }
        @reds_plot = t.def_figure("Greens") { greens }
        @reds_plot = t.def_figure("Reds") { reds }
        t.def_figure("Log_Reds") { log_reds }
        @side_by_side_plot = t.def_figure("Side_by_Side") { side_by_side }
        @two_yaxes_plot = t.def_figure("Two_Ys") { two_yaxes }
        @legend_inside_plot = t.def_figure("Legends") { legends }
        @legend_inside_plot = t.def_figure("Legend_Inside") { legend_inside }
        @legend_outside_plot = t.def_figure("Legend_Outside") { legend_outside }
        @column_triplets_plot = t.def_figure("Column_Triplets") { column_triplets }
        @row_triplets_plot = t.def_figure("Row_Triplets") { row_triplets }
        @rows_plot = t.def_figure("Rows") { rows }
        @columns_plot = t.def_figure("Columns") { columns }
        @array_plot = t.def_figure("Array") { array }
        @trio_plot = t.def_figure("Trio") { trio }
        @collage_plot = t.def_figure("Collage") { collage }
        @labels_plot = t.def_figure("Labels") { labels }
        @error_bars_plot = t.def_figure("Error_Bars") { error_bars }
        @error_bars_plot = t.def_figure("Error_Bars2") { error_bars2 }
        @arrows_plot = t.def_figure("Arrows") { arrows }
        @special_y_plot = t.def_figure("Special_Y") { special_y }
        @bins_plot = t.def_figure("Sampled_Splines") { sampled_splines }
        @bins2_plot = t.def_figure("Steps") { steps }
        @bins2_plot = t.def_figure("Splines") { splines }
        @sampled_data_plot = t.def_figure("Sampled_Data") { sampled_data }
        @samples_with_contours_plot = t.def_figure("Contours") { samples_with_contours }
        t.model_number = -1
            
    end
    
    def read_data
        if t.in_subplot || (@have_data && !t.need_to_reload_data)
            return
        end
        Dvector.read_row(@data_filename, 1, @header)
        model = @header[0].round
        if (model != t.model_number) || t.need_to_reload_data
            Dvector.read(@data_filename, @data_array, 2)
            t.model_number = model
            @big_blues = @blues.mul(@big_blues_scale)
        end
        @have_data = true
        t.need_to_reload_data = false
    end

    def show_model_number(pos = 1, shift = 4)
        if !(t.in_subplot) && t.model_number > 0
            t.show_text('text' => t.model_number.to_s,
                'side' => TOP, 'pos' => pos,
                'shift' => shift, 'scale' => 0.8,
                'justification' => RIGHT_JUSTIFIED)
        end
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
    
    def blues
        read_data
        t.do_box_labels('Blues Plot', 'Position', '\textcolor[rgb]{0,0,1}{Blues}')
        show_model_number
        xs = @positions
        ys = @blues
        t.show_plot(plot_boundaries(xs,ys,@margin,-1,1)) { t.show_polyline(xs,ys,Blue) }
    end
    
    def reds
        read_data
        t.do_box_labels('Reds Plot', 'Position', '\textcolor[rgb]{1,0,0}{Reds}')
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
        t.xaxis_number_of_minor_intervals = 1
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
            'legend_left_margin' => 0.7,
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
        t.subplot('bottom_margin' => 0.55, 'left_margin' => 0.15, 'right_margin' => 0.15) { rows }
        t.subplot('top_margin' => 0.55) { side_by_side }
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
        t.ylabel_shift += 2
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
        xs = @positions
        ys = @blues
        do_labels_plot("Arrows") do
            pt3 = (3*@pt1+@pt2)/4
            pt4 = (@pt1+3*@pt2)/4
            pt5 = (@pt1+6*@pt2)/7
            mid_pt = (@pt1+@pt2)/2
            t.show_arrow('head' => [xs[@pt1], ys[@pt1]], 
                'tail' => [xs[pt3], ys[pt3]], 'color' => Crimson)
            t.show_arrow('head' => [xs[mid_pt], ys[mid_pt]], 
                'tail' => [xs[pt4], ys[pt4]], 'color' => Lime)
            t.show_arrow('head' => [xs[@pt2], ys[@pt2]], 
                'tail' => [xs[pt5], ys[pt5]], 'color' => Magenta)
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
    
    def sampled_image(title)
        t.do_box_labels(title, 'Log Density', 'Log Temperature')
        data = get_press_image
        xs = @eos_logRHOs
        ys = @eos_logTs
        t.show_plot([@eos_xmin, @eos_xmax, @eos_ymax, @eos_ymin]) do
            t.fill_color = Wheat
            t.fill_frame
            clip_press_image
            t.show_image(
                'll' => [xs.min, ys.min], 
                'lr' => [xs.max, ys.min], 
                'ul' => [xs.min, ys.max], 
                'color_space' => t.mellow_colormap, 
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
    
    def get_press_image
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
    
    def samples_with_contours
        t.rescale(0.8)
        title = 'Log Opacity'
        levels = Array.new
        (-3..5).each { |i| levels << i + 0.4 }
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


