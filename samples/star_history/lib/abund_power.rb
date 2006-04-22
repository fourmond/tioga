# abund_power.rb

class StarHistory

    include Math
    include Tioga
    include FigureConstants
    
    def get_track_log(xs)
        return xs[@track_first .. @track_last].safe_log10
    end
    
    def get_boundaries(ary, y_limit = nil, ymax = nil)
        ymin = min_of_many(ary, y_limit)
        ymax = max_of_many(ary) if ymax == nil
        dy = ymax - ymin
        ymax += 0.05 * dy
        ymin -= 0.05 * dy
        return [ @start_age, @end_age, ymax, ymin ]
    end
    
    def plot_abund(xs)
        t.show_ylabel('$\log \mathrm{M_{center}}$ Fraction'); t.no_ylabel
        he = get_track_log(d.center_He)
        c = get_track_log(d.center_C)
        o = get_track_log(d.center_O)
        n = get_track_log(d.center_N)
        ne = get_track_log(d.center_Ne)
        t.show_plot_with_legend do
            t.show_plot('boundaries' => get_boundaries([ he, c, o, n, ne ], -5, 0)) do
                background
                mark_profiles_on_x
                stroke_track(xs, he, BrightBlue, 'Helium')
                stroke_track(xs, c, Goldenrod, 'Carbon')
                stroke_track(xs, o, Coral, 'Oxygen')
                stroke_track(xs, n, Lilac, 'Nitrogen')
                stroke_track(xs, ne, FireBrick, 'Neon', Line_Type_Dot)
                t.show_xlabel(@age_xlabel)
            end
        end
    end
    
    def plot_power(xs)
        t.show_ylabel('$\log$ Power $\mathrm{L_{\odot}}$'); t.no_ylabel
        pp = get_track_log(d.power_PP)
        cno = get_track_log(d.power_CNO)
        three_alpha = get_track_log(d.power_3_alpha)
        alpha_Z = get_track_log(d.power_He_burn - d.power_3_alpha)
        other = get_track_log(d.power_Metal_burn)
        total = get_track_log(d.power_H_burn + d.power_He_burn + d.power_Metal_burn)
        t.show_plot_with_legend do
            t.show_plot('boundaries' => get_boundaries([ pp, cno, three_alpha, alpha_Z, other, total ], -5)) do
                background
                mark_profiles_on_x
                stroke_track(xs, pp, BrightBlue, 'PP')
                stroke_track(xs, cno, Goldenrod, 'CNO')
                stroke_track(xs, three_alpha, Coral, '3$\alpha$')
                stroke_track(xs, alpha_Z, Lilac, '$\alpha$+metal')
                stroke_track(xs, other, FireBrick, 'Other')
                stroke_track(xs, total, RoyalPurple, 'Total Burning', Line_Type_Dot)
                t.show_xlabel(@age_xlabel)
            end
        end
    end
    
    def plot_neutrinos(xs)
        t.show_ylabel('$\log \nu$ Loss $\mathrm{L_{\odot}}$'); t.no_ylabel
        plasmon = get_track_log(d.power_plasmon_neutrinos)
        brems = get_track_log(d.power_brem_neutrinos)
        photo = get_track_log(d.power_photo_neutrinos)
        pair = get_track_log(d.power_pair_neutrinos)
        total = get_track_log(d.power_Neutrinos)
        t.show_plot_with_legend do
            t.show_plot('boundaries' => get_boundaries([ plasmon, brems, photo, pair, total ], -5)) do
                background
                mark_profiles_on_x
                stroke_track(xs, plasmon, BrightBlue, 'Plasmon')
                stroke_track(xs, brems, Goldenrod, 'Brems')
                stroke_track(xs, photo, Coral, 'Photo')
                stroke_track(xs, pair, Lilac, 'Pair')
                stroke_track(xs, total, FireBrick, 'Total Loss', Line_Type_Dot)
                t.show_xlabel(@age_xlabel)
            end
        end
    end
    
    def plot_pressure(xs)
        t.show_ylabel('$\log \mathrm{P_{\!center}}$'); t.no_ylabel
        pcorr = get_track_log(d.center_PCORR.abs)
        pel = get_track_log(d.center_PEL)
        pion = get_track_log(d.center_PION)
        prad = get_track_log(d.center_PRAD)
        total = d.log_center_Pressure[@track_first .. @track_last]
        t.show_plot_with_legend do
            t.show_plot('boundaries' => get_boundaries([ pcorr, pel, pion, prad, total ], 10)) do
                background
                mark_profiles_on_x
                stroke_track(xs, pcorr, BrightBlue, '$\log$ $\vert$$P_c$ CORR$\vert$')
                stroke_track(xs, pel, Goldenrod, '$\log$ $P_c$ ELEC')
                stroke_track(xs, pion, Coral, '$\log$ $P_c$ ION')
                stroke_track(xs, prad, Lilac, '$\log$ $P_c$ RAD')
                stroke_track(xs, total, FireBrick, '$\log$ $P_c$ TOTAL', Line_Type_Dot)
                t.show_xlabel(@age_xlabel)
            end
        end
    end
    
    def plot_abund_power_nu_press
        setup_data
        set_track_start_for_plots
        t.legend_text_dy = 1.3
        t.rescale(0.8)
        t.yaxis_numeric_label_scale = 0.6
        t.ylabel_scale = 0.6
        num_plots = 4; row = 1
        xs = d.star_Age[@track_first .. @track_last]
        t.subplot(t.row_margins('num_rows' => num_plots, 'row' => 1)) do # first
            t.xaxis_type = AXIS_WITH_TICKS_ONLY
            t.no_xlabel
            plot_abund(xs)
        end
        row += 1
        t.subplot(t.row_margins('num_rows' => num_plots, 'row' => row)) do 
            t.xaxis_type = AXIS_WITH_TICKS_ONLY
            t.top_edge_type = AXIS_HIDDEN
            t.no_xlabel
            plot_power(xs)
        end
        row += 1
        t.subplot(t.row_margins('num_rows' => num_plots, 'row' => row)) do 
            t.xaxis_type = AXIS_WITH_TICKS_ONLY
            t.top_edge_type = AXIS_HIDDEN
            t.no_xlabel
            plot_neutrinos(xs)
        end
        row += 1
        t.subplot(t.row_margins('num_rows' => num_plots, 'row' => num_plots)) do # last
            t.top_edge_type = AXIS_HIDDEN
            plot_pressure(xs)
        end
    end
    
end
