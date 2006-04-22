# burn_conv.rb

class StarHistory

    include Math
    include Tioga
    include FigureConstants
    
    def draw_conv_info(xs, s1, e1, s2, e2,s3, e3)
        null_zone = -20
        t.stroke_color = Blue
        t.stroke_width = 2
        xs.each_with_index do |x,i|
            y = e1[i]; t.stroke_line(x, s1[i], x, y) if y > null_zone
            y = e2[i]; t.stroke_line(x, s2[i], x, y) if y > null_zone
            y = e3[i]; t.stroke_line(x, s3[i], x, y) if y > null_zone
        end
    end
    
    def plot_conv_by_mass(xs)
        t.show_ylabel('convection $\mathrm{M/M_{\\odot}}$'); t.no_ylabel
        conv_M_s1 = d.conv_M_s1[@track_first .. @track_last]
        conv_M_e1 = d.conv_M_e1[@track_first .. @track_last]
        conv_M_s2 = d.conv_M_s2[@track_first .. @track_last]
        conv_M_e2 = d.conv_M_e2[@track_first .. @track_last]
        conv_M_s3 = d.conv_M_s3[@track_first .. @track_last]
        conv_M_e3 = d.conv_M_e3[@track_first .. @track_last]
        ary = [ conv_M_s1, conv_M_e1, conv_M_s2, conv_M_e2, conv_M_s3, conv_M_e3 ]
        ymax = max_of_many(ary) * 1.05
        t.show_plot('boundaries' => [ @start_age, @end_age, ymax, 0.001 ]) do
            background
            mark_profiles_on_x
            draw_conv_info(xs, conv_M_s1, conv_M_e1, conv_M_s2, conv_M_e2, conv_M_s3, conv_M_e3)
            t.show_xlabel(@age_xlabel)
        end
    end
    
    def plot_conv_by_radius(xs)
        t.show_ylabel('convection $\mathrm{R/R_{\\odot}}$'); t.no_ylabel
        conv_R_s1 = d.conv_R_s1[@track_first .. @track_last]
        conv_R_e1 = d.conv_R_e1[@track_first .. @track_last]
        conv_R_s2 = d.conv_R_s2[@track_first .. @track_last]
        conv_R_e2 = d.conv_R_e2[@track_first .. @track_last]
        conv_R_s3 = d.conv_R_s3[@track_first .. @track_last]
        conv_R_e3 = d.conv_R_e3[@track_first .. @track_last]
        ary = [ conv_R_s1, conv_R_e1, conv_R_s2, conv_R_e2, conv_R_s3, conv_R_e3 ]
        ymax = max_of_many(ary) * 1.05
        t.show_plot('boundaries' => [ @start_age, @end_age, ymax, 0.001 ]) do
            background
            mark_profiles_on_x
            draw_conv_info(xs, conv_R_s1, conv_R_e1, conv_R_s2, conv_R_e2, conv_R_s3, conv_R_e3)
            t.show_xlabel(@age_xlabel)
        end
    end
    
    def draw_burn_info(xs, burn1, burn2, burn3, burn4)
        null_zone = -20
        t.stroke_color = Blue
        t.stroke_width = 2
        xs.each_with_index do |x,i|
            z1 = burn1[i]; z2 = burn2[i]; z3 = burn3[i]; z4 = burn4[i]
            t.stroke_line(x, z1, x, z2) if z2 > null_zone
            t.stroke_line(x, z3, x, z4) if z4 > null_zone
            if z3 > null_zone
                t.stroke_color = Red
                t.stroke_line(x, z2, x, z3)
                t.stroke_color = Blue
            end
        end
    end
    
    def plot_burn_by_mass(xs)
        t.show_ylabel('burn zone $\mathrm{M/M_{\\odot}}$'); t.no_ylabel
        burn_H_M_1 = d.burn_H_M_1[@track_first .. @track_last]
        burn_H_M_2 = d.burn_H_M_2[@track_first .. @track_last]
        burn_H_M_3 = d.burn_H_M_3[@track_first .. @track_last]
        burn_H_M_4 = d.burn_H_M_4[@track_first .. @track_last]
        burn_He_M_1 = d.burn_He_M_1[@track_first .. @track_last]
        burn_He_M_2 = d.burn_He_M_2[@track_first .. @track_last]
        burn_He_M_3 = d.burn_He_M_3[@track_first .. @track_last]
        burn_He_M_4 = d.burn_He_M_4[@track_first .. @track_last]
        ary = [ burn_H_M_1, burn_H_M_2, burn_H_M_3, burn_H_M_4, burn_He_M_1, burn_He_M_2, burn_He_M_3, burn_He_M_4 ]
        ymax = max_of_many(ary) * 1.05
        t.show_plot('boundaries' => [ @start_age, @end_age, ymax, 0.001 ]) do
            background
            mark_profiles_on_x
            draw_burn_info(xs, burn_H_M_1, burn_H_M_2, burn_H_M_3, burn_H_M_4)
            draw_burn_info(xs, burn_He_M_1, burn_He_M_2, burn_He_M_3, burn_He_M_4)
            t.show_xlabel(@age_xlabel)
        end
    end
    
    def plot_burn_by_radius(xs)
        t.show_ylabel('burn zone $\mathrm{R/R_{\\odot}}$'); t.no_ylabel
        burn_H_R_1 = d.burn_H_R_1[@track_first .. @track_last]
        burn_H_R_2 = d.burn_H_R_2[@track_first .. @track_last]
        burn_H_R_3 = d.burn_H_R_3[@track_first .. @track_last]
        burn_H_R_4 = d.burn_H_R_4[@track_first .. @track_last]
        burn_He_R_1 = d.burn_He_R_1[@track_first .. @track_last]
        burn_He_R_2 = d.burn_He_R_2[@track_first .. @track_last]
        burn_He_R_3 = d.burn_He_R_3[@track_first .. @track_last]
        burn_He_R_4 = d.burn_He_R_4[@track_first .. @track_last]
        ary = [ burn_H_R_1, burn_H_R_2, burn_H_R_3, burn_H_R_4, burn_He_R_1, burn_He_R_2, burn_He_R_3, burn_He_R_4 ]
        ymax = max_of_many(ary) * 1.05
        t.show_plot('boundaries' => [ @start_age, @end_age, ymax, 0.001 ]) do
            background
            mark_profiles_on_x
            draw_burn_info(xs, burn_H_R_1, burn_H_R_2, burn_H_R_3, burn_H_R_4)
            draw_burn_info(xs, burn_He_R_1, burn_He_R_2, burn_He_R_3, burn_He_R_4)
            t.show_xlabel(@age_xlabel)
        end
    end
    
    def plot_burning_convection
        setup_data
        set_track_start_for_plots
        t.rescale(0.7)
        t.yaxis_numeric_label_scale = 0.55
        t.ylabel_scale = 0.66
        num_plots = 4; row = 1
        xs = d.star_Age[@track_first .. @track_last]
        t.subplot(t.row_margins('num_rows' => num_plots, 'row' => 1)) do # first
            t.xaxis_type = AXIS_WITH_TICKS_ONLY
            t.no_xlabel
            plot_conv_by_mass(xs)
        end
        row += 1
        t.subplot(t.row_margins('num_rows' => num_plots, 'row' => row)) do 
            t.xaxis_type = AXIS_WITH_TICKS_ONLY
            t.top_edge_type = AXIS_HIDDEN
            t.no_xlabel
            plot_conv_by_radius(xs)
        end
        row += 1
        t.subplot(t.row_margins('num_rows' => num_plots, 'row' => row)) do 
            t.xaxis_type = AXIS_WITH_TICKS_ONLY
            t.top_edge_type = AXIS_HIDDEN
            t.no_xlabel
            plot_burn_by_mass(xs)
        end
        row += 1
        t.subplot(t.row_margins('num_rows' => num_plots, 'row' => num_plots)) do # last
            t.top_edge_type = AXIS_HIDDEN
            plot_burn_by_radius(xs)
        end
    end
    
end
