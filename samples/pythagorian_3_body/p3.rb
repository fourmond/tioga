# p3.rb  -- for Pythagorian 3 body plots

require 'Tioga/FigureMaker'

class Pythagorian_3_body

    include Math
    include Tioga
    include FigureConstants
    
    def t
        @figure_maker
    end

    def initialize(filename)
        @data_filename = filename
        @figure_maker = FigureMaker.default
        t.def_eval_function { |str| eval(str) }
        t.save_dir = 'p3_out'
        t.def_figure("All") { all }
        t.def_figure("Reds") { reds }
        t.def_figure("Blues") { blues }
        t.def_figure("Greens") { greens }
        t.def_figure("Section") { section }
        Dvector.read(filename, [ # don't bother with the z coords since are all 0
            @time = Dvector.new,
            @xred = Dvector.new,
            @yred = Dvector.new,
            nil, # z1
            @xgreen = Dvector.new,
            @ygreen = Dvector.new,
            nil, # z2
            @xblue = Dvector.new,
            @yblue = Dvector.new,
            nil] # z3
            )
        @plot_bounds = [ -5, 5, 5, -5 ]
        @section_start = 0 # time
        @section_length = 1 # in time units
        @weight = 0.5 # default line weight for non-movie plots
        @scale = 0.8
        t.def_enter_page_function { enter_page }    
    end
    
    def enter_page
        t.page_setup(11*72/2,8.5*72/2)
        t.set_frame_sides(0.15,0.85,0.85,0.15) # left, right, top, bottom in page coords        
    end
    
    def set_style
        t.rescale(@scale)
        t.line_width = @weight
    end
    
    def blues
        set_style
        t.do_box_labels('Blue Star: Mass 5 $\mathrm{M_{\odot}}$', 'x', 'y')
        t.show_plot(@plot_bounds) { t.show_polyline(@xblue, @yblue, Blue) }
    end
    
    def reds
        set_style
        t.do_box_labels('Red Star: Mass 3 $\mathrm{M_{\odot}}$', 'x', 'y')
        t.show_plot(@plot_bounds) { t.show_polyline(@xred, @yred, Red) }
    end
    
    def greens
        set_style
        t.do_box_labels('Green Star: Mass 4 $\mathrm{M_{\odot}}$', 'x', 'y')
        t.show_plot(@plot_bounds) { t.show_polyline(@xgreen, @ygreen, Green) }
    end
    
    def piece_of_line(xs, ys, loc, color)
        t.stroke_color = color
        t.stroke_line(xs[loc], ys[loc], xs[loc+1], ys[loc+1])
    end
    
    def show_section(start_time, duration)
        set_style
        t.do_box_labels("Time from #{start_time} to #{start_time + duration}", "x", "y")
        end_time = start_time + duration
        start_loc = @time.where_closest(start_time)
        loc = start_loc
        t.show_plot(@plot_bounds) do
            while (loc+1 < @time.size and @time[loc] <= end_time) do
                piece_of_line(@xblue, @yblue, loc, Blue)
                piece_of_line(@xred, @yred, loc, Red)
                piece_of_line(@xgreen, @ygreen, loc, Green)
                loc = loc + 1
            end
        end
    end
    
    def section
        show_section(@section_start, @section_length)
        @section_start = @section_start + 1
        @section_start = 0 if @section_start+@section_length >= @time[-1]
        # each time section is called it moves to the next section of the data
    end
    
    def all
        show_section(0, @time[-1])
    end
    
    def movie(num_frames = 200, line_width = 2, bounds = [ -7, 7, 7, -7 ])
        puts "make movie"
        t.save_dir = 'movie_out'
        system('rm movie_out/*')
        t.add_model_number = true
        section_plot_number = t.figure_names.index('Section')
        lw = @weight
        @weight = line_width
        bnds = @plot_bounds
        @plot_bounds = bounds
        @section_start = @time[0]
        frame_size = (@time[-1] - @time[0]) / num_frames
        @section_length = frame_size * 1.5
        num_frames.times do |i|
            t.model_number = i
            @section_start = frame_size * i
            t.make_preview_pdf(section_plot_number)
        end
        @weight = lw
        @plot_bounds = bnds
        system('cd movie_out; rm *.txt *.aux *.log *.tex *_figure.pdf')
        t.save_dir = 'p3_out'
        t.add_model_number = false
        return 'finished'
    end
    
end

$p3 = Pythagorian_3_body.new('p3.data')


