# default_plot.rb

class Default_Plot

    include Math
    include Tioga
    include FigureConstants
    
    def t
        @figure_maker
    end

    def initialize
        $data = 'forward.out' # better use a default file that exists !
        $y_column = 2
        $first_line = 1
        @figure_maker = FigureMaker.default
        t.def_eval_function { |str| eval(str) }
        t.save_dir = 'default_plot_out'
        t.def_figure("Column2") { plot 2 }
        t.def_figure("Column3") { plot 3 }
        t.def_figure("Column4") { plot 4 }
        t.def_figure("Column5") { plot 5 }
        t.def_figure("Column6") { plot 6 }
        t.def_figure("Column7") { plot 7 }
        t.def_figure("Column8") { plot 8 }
        t.def_figure("Column9") { plot 9 }
    end
    
    def set_style
        @line_color = Blue
        @scale = 1.0
        @weight = 1.0
        @margin = 0.15
        @title = 'test'
        @xlabel = 'x'
        @ylabel = 'y'
        t.landscape
        t.rescale(@scale)
        t.line_width = @weight
    end
    
    def read_data
        row = Dvector.read_row($data, $first_line)
        numcols = row.size
        result = true
        if ($y_column > numcols)
            puts "ALERT! There are only #{numcols} columns of data in the file #{$data}"
            $y_column = numcols
            result = false
        end
        # uses first column for x
        readvec = [ @xs = Dvector.new ]
        ($y_column - 1).times { |i| readvec << Dvector.new }
        Dvector.read($data, readvec, $first_line)
        @ys = readvec[$y_column-1]
        @grid = Dvector.new(@xs.size) { |i| i+1 }
        
        @xs = @grid
        
        result
    end
    
    def set_bounds
        xmin = @xs.min; xmax = @xs.max; ymin = @ys.min; ymax = @ys.max
        dx = xmax - xmin; dx = 1 if dx == 0; dx *= @margin
        dy = ymax - ymin; dy = 1 if dy == 0; dy *= @margin
        @plot_bounds = [ xmin - dx, xmax + dx, ymax + dy, ymin - dy ]
    end
    
    def plot(num)
        $y_column = num if num >= 2
        result = read_data
        set_style
        set_bounds
        t.do_box_labels(@title, @xlabel, @ylabel)
        t.show_plot(@plot_bounds) do
            if result
                t.show_polyline(@xs, @ys, @line_color)
            else
                t.show_text('text' => "ALERT! There are only #{$y_column} columns of data in the file #{$data}",
                    'side' => TOP, 'position' => 0.5, 'shift' => -10, 'justification' => CENTERED, 'scale' => 0.5)
            end
        end
    end
    
end

Default_Plot.new
