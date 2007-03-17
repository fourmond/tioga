# zams.rb

require 'Tioga/FigureMaker'

include Tioga
    
class ZAMS_Data

    attr_accessor :num_pts, :masses, :log_Ls, :log_TNs, :log_Tsurfs, :log_RHOs, :center_Ts
    attr_accessor :log_Rs, :top_CNVs, :bottom_CNVs, :power_PPs, :power_CNOs, :opacities
    attr_accessor :log_Ms, :log_Tcents, :log_KAPPAs, :fraction_CNO

    def initialize(filename)
        Dvector.read(filename, [ # the columns of data in the file
            nil,
            @masses = Dvector.new,
            @log_Ls = Dvector.new,
            @log_TNs = Dvector.new,
            @log_Tsurfs = Dvector.new,
            @log_RHOs = Dvector.new,
            @center_Ts = Dvector.new,
            @log_Rs = Dvector.new,
            @top_CNVs = Dvector.new,
            @bottom_CNVs = Dvector.new,
            @power_PPs = Dvector.new,
            @power_CNOs = Dvector.new,
            @opacities = Dvector.new ])
        @num_pts = @masses.length
        # some derived quantities
        @log_Ms = @masses.safe_log10
        @log_Tcents = @center_Ts.safe_log10 + 7
        @log_KAPPAs = @opacities.safe_log10
        @fraction_CNO = @power_CNOs / (@power_CNOs + @power_PPs)
    end
    
end

class ZAMSPlots

    include Math
    include Tioga
    include FigureConstants
    
    def t
        @figure_maker
    end

    def initialize
        @figure_maker = FigureMaker.default
        t.def_eval_function { |str| eval(str) }
        t.save_dir = 'zams_out'
        @zams_data_array = [
            @z0001_data = ZAMS_Data.new("zams_data/z0001.log"),
            @z001_data = ZAMS_Data.new("zams_data/z001.log"),
            @z004_data = ZAMS_Data.new("zams_data/z004.log"),
            @z02_data = ZAMS_Data.new("zams_data/z02.log") ]
        @num_zams = @zams_data_array.size
        @num_masses = @z02_data.masses.size
        @legends = [ 'Z = 0.0001', 'Z = 0.001', 'Z = 0.004', 'Z = 0.02' ]
        @colors = [ Purple, Crimson, Green, Blue ]

        t.def_figure("ZAMS_H_R") { zams_H_R }
        t.def_figure("ZAMS_T_RHO") { zams_T_RHO }
        t.def_figure("ZAMS_H_R_and_T_RHO") { zams_BOTH }
        t.def_figure("ZAMS_convection") { zams_convection }
        t.def_figure("ZAMS_plots1") { zams_plots1 }
        t.def_figure("ZAMS_plots2") { zams_plots2 }
        t.def_figure("ZAMS_fancy") { zams_fancy }
    
        @legend_scale = 0.95
        @subplot_scale = 0.8
        @plot_with_legend_right_margin = 0.18
        @legend_left_margin = 0.83
        @legend_top_margin = 0.05
        @side_by_side_margin = 0.5
        @mass_points = [ 0.1, 0.3, 1.0, 3.0, 10.0, 100.0 ]
        @labels = [ "0.1", "0.3", "1", "3", "10", "100" ]
        t.def_enter_page_function { enter_page }    
    end
    
    def enter_page
        t.page_setup(11*72/2,8.5*72/2)
        t.set_frame_sides(0.15,0.85,0.85,0.15) # left, right, top, bottom in page coords        
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
    
    def zams_fancy
        star_types = [ # spectral types as [ typename, effective T, T string ]
            [ 'O5', 40e3, '40' ],
            [ 'B0', 28e3, '28' ],
            [ 'B5', 15.5e3, '15.5' ],
            [ 'A0', 9.9e3, '9.9' ],
            [ 'F0', 7e3, '7' ],
            [ 'G2', 5.8e3, '5.8' ],
            [ 'K0', 4.1e3, '4.1' ],
            [ 'M5', 2.8e3, '2.8' ] ]
        stars = [ # star info as [ name, L, Teff ]
            [ '\textbf{Zeta Ophiuchi}', 68e3, 32.5e3 ],
            [ '\textbf{Canopus}', 15e3, 7.8e3 ],
            [ '\textbf{Deneb}', 2e5, 14e3 ],
            [ '\textbf{Adhara}', 15e3, 20e3 ],
            [ '\textbf{Antares}', 9e3, 3.4e3 ],
            [ '\textbf{Betelgeuse}', 60e3, 3.1e3 ],
            [ '\textbf{Sun}', 1.0, 5.8e3 ],
            [ '\textbf{Proxima}', 1.38e-4, 3.04e3 ],
            [ '\textbf{Naos}', 1e6, 42e3 ],
            [ '\textbf{Rigel}', 66e3, 11e3 ],
            [ '\textbf{Hadar}', 112, 25.5e3 ],
            [ '\textbf{Sirius A}', 26.0, 11e3 ],
            [ '\textbf{Regulus}', 240, 12e3 ],
            [ '\textbf{Arcturus}', 180.0, 4.3e3 ],
            [ '\textbf{Procyon A}', 7.6, 7.2e3 ],
            [ '\textbf{Procyon B}', 5e-4, 9.5e3 ],
            [ '\textbf{Sirius B}', 2e-3, 26e3 ],
            [ '\textbf{Kr\"{u}ger 60 A}', 1.6e-3, 4e3 ],
            [ '\textbf{61 Cygni A}', 6e-2, 4.5e3 ] ]
        t.yaxis_type = t.xaxis_type = AXIS_WITH_MAJOR_TICKS_AND_NUMERIC_LABELS
        min_T_eff = 2.2e3; max_T_eff = 5e4; min_L = 5e-5; max_L = 7e6
        xleft = max_T_eff.log10
        xright = min_T_eff.log10
        ytop = max_L.log10
        ybottom = min_L.log10
        bounds = { 'left_boundary' => xleft, 'right_boundary' => xright,
            'top_boundary' => ytop, 'bottom_boundary' => ybottom }
        t.xaxis_loc = t.xlabel_side = BOTTOM; t.yaxis_loc = t.ylabel_side = LEFT
        t.top_edge_type = t.right_edge_type = AXIS_HIDDEN
        xmajors = []
        xlabels = []
        star_types.each { |type| xmajors << type[1].log10; xlabels << type[0] }
        t.xaxis_locations_for_major_ticks = xmajors
        t.xaxis_tick_labels = xlabels
        t.xlabel = 'Spectral Type'
        t.ylabel = 'Luminosity ($L_{\odot}$)'
        t.yaxis_tick_interval = 2
        t.rescale(0.9)
        t.show_plot(bounds) do
            t.axial_shading('start_point' => [xright, ytop], 'end_point' => [xleft, ytop], 
                'colormap' => t.create_colormap(
                    'points' => [0.0, 0.12, 0.28, 0.37, 0.47, 0.59, 0.8, 1.0],
                    'Hs' =>     [0, 0, 60, 60, 60, 240, 240, 240],
                    'Ls' =>     [0.5, 0.6, 0.65, 0.65, 1, 1, 0.65, 0.65],
                    'Ss' =>     [1, 1, 0.9, 0.9, 0.5, 0.5, 0.9, 1] ))
            t.show_polyline(@z02_data.log_Tsurfs, @z02_data.log_Ls, Gray, nil, Line_Type_Dash)
            dy = t.default_text_height_dy * 0.4
            t.rescale(0.8)
            stars.each do |info|
                name = info[0]; luminosity = info[1]; temp = info[2]
                log_L = luminosity.log10; log_T = temp.log10
                if temp < 4e3
                    dx = -0.02 * t.bounds_width
                elsif temp > 15e3
                    dx = 0.02 * t.bounds_width
                else
                    dx = 0
                end
                t.show_label('text' => name, 'x' => log_T - dx, 'y' => log_L + dy, 'justification' => CENTERED)
                t.show_marker('marker' => Bullet, 'point' => [log_T, log_L], 'scale' => 0.6, 'color' => Black)
            end
        end
        t.xaxis_loc = t.xlabel_side = TOP; t.yaxis_loc = t.ylabel_side = RIGHT
        t.bottom_edge_type = t.left_edge_type = AXIS_HIDDEN
        t.xlabel = 'Surface Temperature (x 1000K)'
        t.ylabel = 'Absolute Magnitude'
        t.yaxis_locations_for_major_ticks = [ -4.0, -2.0, 0.0, 2.0, 4.0, 6.0 ]
        t.yaxis_tick_labels = [ '+15', '+10', '+5', '0', '-5', '-10' ]
        xlabels = []
        star_types.each { |type| xlabels << type[2] }
        t.xaxis_tick_labels = xlabels
        t.show_plot_box
    end
    
    def show_zams_line(xs, ys, masses, line_number)
        t.show_polyline(xs, ys, @colors[line_number], @legends[line_number])
        dx = 0.5 * t.default_text_height_dx
        if t.xaxis_reversed
            just_first = RIGHT_JUSTIFIED; just_last = LEFT_JUSTIFIED
        else
            just_first = LEFT_JUSTIFIED; just_last = RIGHT_JUSTIFIED; dx = -dx
        end
        @mass_points.each_index do | i |
            mass = @mass_points[i]; label = @labels[i]
            k = masses.where_closest(mass); x = xs[k]; y = ys[k]
            t.show_marker('marker' => Bullet, 'point' => [x,y], 'scale' => 0.4, 'color' => Black)
            s = 0.9
            if line_number == 0
                t.show_label('text' => label, 'x' => x-dx, 'y' => y,
                    'alignment' => ALIGNED_AT_MIDHEIGHT,
                    'justification' => just_first, 'scale' => s)
            elsif line_number == @num_zams-1
                t.show_label('text' => label, 'x' => x+dx, 'y' => y,
                    'alignment' => ALIGNED_AT_MIDHEIGHT,
                    'justification' => just_last, 'scale' => s)
            end
        end
    end
    
    def background
        t.fill_color = FloralWhite
        t.fill_frame
    end
    
    def show_zams(bounds, h_r_flag)
        t.show_plot(bounds) do
            background
            @zams_data_array.each_index do |i|
                zams = @zams_data_array[i]
                if h_r_flag
                    show_zams_line(zams.log_Tsurfs, zams.log_Ls, zams.masses, i)
                else
                    show_zams_line(zams.log_RHOs, zams.log_Tcents, zams.masses, i)
                end
            end
        end
    end
    
    def plot_H_R
        t.rescale(0.9)
        t.title = 'Zero-Age Main Sequence'
        t.xlabel = 'log Surface Temp'
        t.ylabel = 'log Luminosity'
        min_T_eff = 1.7e3; max_T_eff = 1.2e5; min_L = 5e-5; max_L = 7e6
        xleft = max_T_eff.log10
        xright = min_T_eff.log10
        ytop = max_L.log10
        ybottom = min_L.log10
        bounds = { 'left_boundary' => xleft, 'right_boundary' => xright,
            'top_boundary' => ytop, 'bottom_boundary' => ybottom }
        show_zams(bounds, true)
        t.show_text('text' => 'Mass in $M_\odot$', 'side' => RIGHT, 'pos' => 0.05, 'shift' => -0.9,
            'scale' => 0.9, 'justification' => RIGHT_JUSTIFIED, 'angle' => -90)
    end
    
    def zams_H_R
        t.show_plot_with_legend { plot_H_R }
    end
    
    def plot_T_RHO
        t.rescale(0.9)
        t.title = 'Zero-Age Main Sequence'
        t.xlabel = 'log Center Density'
        t.ylabel = 'log Center Temp'
        xleft = -0.3
        xright = 3.1
        ytop = 7.83
        ybottom = 6.45
        bounds = { 'left_boundary' => xleft, 'right_boundary' => xright,
            'top_boundary' => ytop, 'bottom_boundary' => ybottom }
        show_zams(bounds, false)
        t.show_text('text' => 'Mass in $M_\odot$', 'side' => RIGHT, 'pos' => 0.05, 'shift' => -1.7,
            'scale' => 0.9, 'justification' => RIGHT_JUSTIFIED, 'angle' => -90)
    end
    
    def zams_T_RHO
        t.show_plot_with_legend { plot_T_RHO }
    end
    
    def zams_BOTH
        t.rescale(0.7)
        bottom_margin = 0.1
        t.context do
            t.set_subframe('right_margin' => @plot_with_legend_right_margin)
            t.show_title('Zero-Age Main Sequence')
            t.no_title
            t.subplot('right_margin' => @side_by_side_margin, 'bottom_margin' => bottom_margin) do
                t.yaxis_loc = t.ylabel_side = LEFT; t.right_edge_type = AXIS_LINE_ONLY
                plot_H_R
            end
            t.reset_legend_info
            t.subplot('left_margin' => @side_by_side_margin, 'bottom_margin' => bottom_margin) do
                t.yaxis_loc = t.ylabel_side = RIGHT; t.left_edge_type = AXIS_HIDDEN
                plot_T_RHO
            end
        end
        t.set_subframe('left_margin' => 0.85, 'bottom_margin' => bottom_margin)
        t.rescale(@legend_scale)
        t.legend_text_ystart = 0.9
        t.legend_text_dy = 1.5
        t.show_legend
    end
    
    def plot_convection
        t.rescale(0.9)
        t.title = 'Main Convection Zone'
        t.xlabel = 'Mass ($M_{\odot}$)'
        t.ylabel = 'Location (by fraction of total mass)'
        xleft =log10(0.09)
        xright = log10(101.0)
        ytop = 1.0
        ybottom = 0.0
        t.xaxis_log_values = true
        t.xaxis_use_fixed_pt = true
        t.set_landscape
        t.show_plot('left_boundary' => xleft, 'right_boundary' => xright,
            'top_boundary' => ytop, 'bottom_boundary' => ybottom) do
            dx = 0.2 * t.bounds_width / @num_masses
            t.line_width = 0.5
            background
            @zams_data_array.each_index do |i|
                zams = @zams_data_array[i]
                xs = zams.log_Ms; tops = zams.top_CNVs; bottoms = zams.bottom_CNVs
                xoffset = dx * (i - 1)
                t.stroke_color = @colors[i]
                t.save_legend_info(@legends[i])
                xs.each_index do |j|
                    x = xs[j]+xoffset
                    t.stroke_line(x, bottoms[j], x, tops[j])
                end
            end
        end
    end
    
    def zams_convection
        t.show_plot_with_legend('plot_top_margin' => 0.01, 'legend_top_margin' => 0.15) { plot_convection }
    end
    
    def zams_plot(ylabel, ytop, ybottom, &cmd)
        t.xlabel = 'Mass ($M_{\odot}$)'
        t.ylabel = ylabel
        xleft =log10(0.09)
        xright = log10(101.0)
        t.xaxis_log_values = true
        t.xaxis_use_fixed_pt = true
        t.show_plot('left_boundary' => xleft, 'right_boundary' => xright,
            'top_boundary' => ytop, 'bottom_boundary' => ybottom) { background; cmd.call }
    end
    
    def luminosity_plot
        zams_plot('log L ($L_{\odot}$)', 6.8, -4.2) do
            @zams_data_array.each_index do |i|
                zams = @zams_data_array[i]
                t.show_polyline(zams.log_Ms, zams.log_Ls, @colors[i], @legends[i])
            end
        end
    end
    
    def temp_surface_plot
        zams_plot('log $T_{surface}$', 5.1, 3.2) do
            @zams_data_array.each_index do |i|
                zams = @zams_data_array[i]
                t.show_polyline(zams.log_Ms, zams.log_Tsurfs, @colors[i], @legends[i])
            end
        end
    end
    
    def temp_center_plot
        zams_plot('log $T_{center}$', 7.83, 6.45) do
            @zams_data_array.each_index do |i|
                zams = @zams_data_array[i]
                t.show_polyline(zams.log_Ms, zams.log_Tcents, @colors[i], @legends[i])
            end
        end
    end
    
    def density_plot
        zams_plot('log $\rho_{center}$', 3.1, -0.3) do
            @zams_data_array.each_index do |i|
                zams = @zams_data_array[i]
                t.show_polyline(zams.log_Ms, zams.log_RHOs, @colors[i], @legends[i])
            end
        end
    end
    
    def plots1
        t.rescale(0.9)
        t.show_title('Luminosity, Temperature, Density')
        t.no_title
        num_plots = 4; row = 1
        t.subplot(t.row_margins('num_rows' => num_plots, 'row' => row)) do
            t.xaxis_type = AXIS_WITH_TICKS_ONLY; t.no_xlabel
            luminosity_plot
            row += 1
        end
        t.top_edge_type = AXIS_HIDDEN
        t.subplot(t.row_margins('num_rows' => num_plots, 'row' => row)) do 
            t.xaxis_type = AXIS_WITH_TICKS_ONLY; t.no_xlabel
            temp_surface_plot
            row += 1
        end
        t.subplot(t.row_margins('num_rows' => num_plots, 'row' => row)) do 
            t.xaxis_type = AXIS_WITH_TICKS_ONLY; t.no_xlabel
            temp_center_plot
            row += 1
        end
        t.reset_legend_info
        t.subplot(t.row_margins('num_rows' => num_plots, 'row' => row)) do 
            density_plot
            row += 1
        end
    end
    
    def zams_plots1
        t.show_plot_with_legend('plot_top_margin' => 0.01, 'legend_top_margin' => 0.01) { plots1 }
    end
    
    def radius_plot
        zams_plot('log R ($R_{\odot}$)', 1.3, -1) do
            @zams_data_array.each_index do |i|
                zams = @zams_data_array[i]
                t.show_polyline(zams.log_Ms, zams.log_Rs, @colors[i], @legends[i])
            end
        end
    end
    
    def cno_fraction_plot
        zams_plot('\small{CNO/(PP+CNO)}', 1.08, -0.08) do
            @zams_data_array.each_index do |i|
                zams = @zams_data_array[i]
                t.show_polyline(zams.log_Ms, zams.fraction_CNO, @colors[i], @legends[i])
            end
        end
    end
    
    def opacity_plot
        zams_plot('$\\log \kappa_{center}$', 1.4, -0.7) do
            @zams_data_array.each_index do |i|
                zams = @zams_data_array[i]
                t.show_polyline(zams.log_Ms, zams.log_KAPPAs, @colors[i], @legends[i])
            end
        end
    end
    
    def nuclear_timescale_plot
        zams_plot('log $t_{nuclear}$ \small (yrs)', 12.7, 5.5) do
            @zams_data_array.each_index do |i|
                zams = @zams_data_array[i]
                t.show_polyline(zams.log_Ms, zams.log_TNs, @colors[i], @legends[i])
            end
        end
    end
    
    def plots2
        t.rescale(0.9)
        t.show_title('Radius, Power, Opacity, Nuclear Timescale')
        t.no_title
        num_plots = 4; row = 1
        t.subplot(t.row_margins('num_rows' => num_plots, 'row' => row)) do
            t.xaxis_type = AXIS_WITH_TICKS_ONLY; t.no_xlabel
            radius_plot
            row += 1
        end
        t.top_edge_type = AXIS_HIDDEN
        t.subplot(t.row_margins('num_rows' => num_plots, 'row' => row)) do 
            t.xaxis_type = AXIS_WITH_TICKS_ONLY; t.no_xlabel
            cno_fraction_plot
            row += 1
        end
        t.subplot(t.row_margins('num_rows' => num_plots, 'row' => row)) do 
            t.xaxis_type = AXIS_WITH_TICKS_ONLY; t.no_xlabel
            opacity_plot
            row += 1
        end
        t.reset_legend_info
        t.subplot(t.row_margins('num_rows' => num_plots, 'row' => row)) do 
            nuclear_timescale_plot
            row += 1
        end
    end
    
    def zams_plots2
        t.show_plot_with_legend('plot_top_margin' => 0.01, 'legend_top_margin' => 0.01) { plots2 }
    end
    
end

ZAMSPlots.new

