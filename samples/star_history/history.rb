# history.rb

require 'Tioga/FigureMaker'

# use load rather than require for the other files so will reload properly in Tioga
load 'lib/history_logs.rb'
load 'lib/tracks.rb'
load 'lib/plots.rb'
load 'lib/burn_conv.rb'
load 'lib/abund_power.rb'

class StarHistory

    include Math
    include Tioga
    include FigureConstants
        
    def t
        @figure_maker
    end

    def d
        @log
    end

    def initialize
        @figure_maker = FigureMaker.default
        t.def_eval_function { |str| eval(str) }
        t.save_dir = 'history_out'
        t.def_figure("H_R_T_RHO") { plot_H_R_T_RHO }
        t.def_figure("R_L_Ts_Tc_Rho_Psi") { plot_R_L_Ts_Tc_Rho_Psi }
        t.def_figure("Z_3a_aZ_nu_M_Core") { plot_Z_3a_aZ_nu_M_Core }
        t.def_figure("burning_convection") { plot_burning_convection }
        t.def_figure("abund_power_nu_press") { plot_abund_power_nu_press }
        @have_setup_data = false
        @margin = 0.1
        @track_start_param =  0.1 # fractional way from 1st to 2nd profiles
        @track_end_param = 0.0 # fractional way from last to next to last profiles
        t.def_eval_function { |str| eval(str) }
        t.def_enter_page_function { enter_page }    
    end
    
    def enter_page
        t.page_setup(11*72/2,8.5*72/2)
        t.set_frame_sides(0.15,0.85,0.85,0.15) # left, right, top, bottom in page coords        
    end
    
    def read_PSIs(path = nil)
        path = '' if path == nil
        path = path + '/' if path.length > 0 && path[-1..-1] != '/'
        Dvector.read(path + 'psi0.data', @psi0 = [Dvector.new, Dvector.new], 2)
        Dvector.read(path + 'psi5.data', @psi5 = [Dvector.new, Dvector.new], 2)
        Dvector.read(path + 'psi100.data', @psi100 = [Dvector.new, Dvector.new], 2)
    end
    
    def read_ZAMS(path = nil)
        path = '' if path == nil
        path = path + '/' if path.length > 0 && path[-1..-1] != '/'
        @zams = [
            @zams_log_Center_Density = Dvector.new,
            @zams_log_Center_Temp = Dvector.new,
            @zams_log_Luminosity = Dvector.new,
            @zams_log_Surface_Temp = Dvector.new,
            @zams_Ms = Dvector.new ]
        Dvector.read(path + 'ZAMS.data', @zams, 2)
    end
    
    def set_track_start_for_plots
        if d.num_profiles == 0
            @track_first = 0
        elsif d.num_profiles == 1
            @track_first = d.star_Age.where_closest(d.profile_Ages[0]*0.95)
        else
            start_age = (1-@track_start_param)*d.profile_Ages[0]+@track_start_param*d.profile_Ages[1]
            @track_first = d.star_Age.where_closest(start_age)
            end_age = (1-@track_end_param)*d.profile_Ages[-1]+@track_end_param*d.profile_Ages[-2]
            @track_last = d.star_Age.where_closest(end_age)
            @track_last -= 1 if @track_last+1 == d.star_Age[-1] # don't use the very last model
        end
        @start_age = d.star_Age[@track_first]
        @start_age = d.star_Age[@track_first]
        @end_age = d.star_Age[@track_last]
        age_range = @end_age - @start_age
        @end_age += 0.01 * age_range
    end
    
    def setup_data
        return if @have_setup_data
        @log = HistoryLogs.new('star_logs')
        read_PSIs('star_data')
        read_ZAMS('star_data')
        adjust_ages
        find_He_gap
        @have_setup_data = true
    end
    
    def adjust_ages
        age = d.star_Age[-1]
        if age >= 1e9
            d.star_Age.times!(1e-9)
            d.profile_Ages.times!(1e-9)
            @age_Units = "Gyrs"
            @age_Scale = 1e9
            @age_xlabel = 'Age (Gyrs)'
        elsif age >= 1e6
            d.star_Age.times!(1e-6)
            d.profile_Ages.times!(1e-6)
            @age_Units = "Myrs"
            @age_Scale = 1e6
            @age_xlabel = 'Age (Myrs)'
        else
            @age_Units = "yrs"
            @age_Scale = 1
            @age_xlabel = 'Age (yrs)'
        end
      @age_format = (d.star_Age[-1] > 9.99)? '\sffamily %0.2f' : '\sffamily %0.3f'
      @mass_format = (d.star_Mass[0] > 9.99)? '\sffamily %0.2f' : '\sffamily %0.3f'
    end
    
    def find_He_gap # may have a gap in tracks if had a helium flash
        @he_burn_start = d.net_He_power.where_gt(0)
        if @he_burn_start == nil || @he_burn_start >= d.num_models-1
            @gap_in_tracks = false
        else
            density_jumps = d.log_center_Density[@he_burn_start .. -2] - d.log_center_Density[@he_burn_start+1 .. -1]
            jumps_at = density_jumps.where_gt(0.9) # large drop in density signals gap in tracks
            if jumps_at == nil
                @gap_in_tracks = false
            else
                @helium_gap_start = @he_burn_start + 1 + jumps_at
                @gap_in_tracks = (@helium_gap_start != nil && @helium_gap_start < d.num_models)
            end
        end
    end
    
    def stroke_HR_T_RHO_track(xs, ys, color)
        stroke_track(xs, ys, color, nil, Line_Type_Solid, 0)
    end
    
    def stroke_track(xs, ys, color=Black, legend=nil, type=Line_Type_Solid, offset=@track_first)
        t.stroke_color = color
        t.line_type = type
        if @gap_in_tracks
            t.append_points_with_gaps_to_path(xs,ys,[@helium_gap_start-offset],false)
        else
            t.append_points_to_path(xs,ys)
        end
        t.save_legend_info(legend) if legend != nil
        t.stroke
    end
    
    def mark_profiles_on_x
        base = t.bounds_bottom
        height = 0.8 * t.default_text_height_dy
        if t.yaxis_reversed
            top = base - height
        else
            top = base + height
        end
        t.line_type = Line_Type_Solid
        t.stroke_color = Green
        d.profile_Ages.each { |age| t.stroke_line(age, base, age, top) }
    end
    
    def min_of_many(ary, y_limit = nil)
        return nil if ary == nil || ary.size == 0
        ymin = Dvector.min_of_many(ary)
        ymin = y_limit if y_limit != nil && ymin < y_limit
        return ymin
    end
    
    def max_of_many(ary, y_limit = nil)
        return nil if ary == nil || ary.size == 0
        ymax = Dvector.max_of_many(ary)
        ymax = y_limit if y_limit != nil && ymax > y_limit
        return ymax
    end
    
    def show_box_labels(title, xlabel=nil, ylabel=nil)
        if title != nil
            t.show_title(title); t.no_title
        end
        if xlabel != nil
            t.show_xlabel(xlabel); t.no_xlabel
        end
        if ylabel != nil
            t.show_ylabel(ylabel); t.no_ylabel
        end
    end
        
    def background
        t.fill_color = FloralWhite
        t.fill_frame
    end
    
    def mark_spot(x, y)
        t.show_marker(
          'x' => x, 
          'y' => y, 
          'marker' => Bullet, 
          'scale' => 0.5, 
          'color' => Red);
    end
    
end

StarHistory.new
