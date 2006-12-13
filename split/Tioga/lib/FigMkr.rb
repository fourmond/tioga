#  FigMkr.rb

=begin
   Copyright (C) 2005, 2006  Bill Paxton

   This file is part of Tioga.

   Tioga is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Library Public License as published
   by the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.

   Tioga is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with Tioga; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
=end

require 'Tioga/FigureConstants.rb'

module Tioga
class FigureMaker
    
    include FigureConstants
    
    @@default_figure_maker = nil
    @@initialized = false  # set true by the C code when first make a figure

    # The tag used for cvs export 
    CVS_TAG = "$Name$"
    
    # Version now uses the CVS_TAG to create the version number. CVS_TAG should
    # look like 'rel_1_1_0' for the 1.1.0 release. 
    def FigureMaker.version
      CVS_TAG =~ /\D+(.*?)\s*\$?$/
      version = $1.tr("-_", "..")
      if version.length > 0
        return version
      else
        return "CVS_version"    # Can't do better than that.
      end
    end

    def FigureMaker.default
        @@default_figure_maker = FigureMaker.new if @@default_figure_maker == nil
        @@default_figure_maker
    end

    def FigureMaker.default=(fm)
        @@default_figure_maker = fm
    end
    
    def FigureMaker.make_name_lookup_hash(ary)
        dict = Hash.new
        ary.each { |name| dict[name] = true }
        return dict
    end

    attr_accessor :legend_info
    
    attr_reader   :num_figures
    
    attr_reader   :figure_names

    attr_reader   :run_dir
    
    attr_accessor :save_dir
    
    attr_accessor :which_pdflatex
    
    attr_accessor :quiet_mode
    
    attr_accessor :model_number
    
    attr_accessor :add_model_number
    
    attr_accessor :need_to_reload_data
    
    attr_accessor :auto_refresh_filename
    
    attr_accessor :legend_defaults
    
    attr_accessor :marker_defaults
    
    attr_accessor :tex_preamble
    
    attr_accessor :tex_fontsize    
    attr_accessor :tex_fontfamily    
    attr_accessor :tex_fontseries    
    attr_accessor :tex_fontshape
    
    attr_accessor :num_error_lines
    
    # old preview attributes -- to be removed later
    
    #attr_accessor :tex_preview_documentclass   
    #attr_accessor :tex_preview_pagestyle
    #attr_accessor :tex_preview_tiogafigure_command  
    attr_accessor :tex_xoffset
    attr_accessor :tex_yoffset
    
    #attr_accessor :tex_preview_paper_width
    #attr_accessor :tex_preview_paper_height
    #attr_accessor :tex_preview_hoffset
    #attr_accessor :tex_preview_voffset
    #attr_accessor :tex_preview_figure_width
    #attr_accessor :tex_preview_figure_height
    #attr_accessor :tex_preview_minwhitespace
    #attr_accessor :tex_preview_fullpage

    # Whether or not to create +save_dir+ if it doesn't exist
    attr_accessor :create_save_dir

    # Whether or not do do automatic cleanup of the files
    attr_accessor :autocleanup

    
    def reset_figures # set the state to default values
        
        @figure_commands = []
        @num_figures = 0
        @create_save_dir = true # creates +save_dir+ by default

        @name = nil
        @auto_refresh_filename = nil
        @figure_names = [ ]
        @legend_info = [ ]
        @run_dir = Dir.getwd
        @save_dir = nil
        @which_pdflatex = 'pdflatex'
        @quiet_mode = false
        @model_number = -1
        @need_to_reload_data = true
        @add_model_number = false
        @tex_preview_documentclass = 'article'
        @tex_preamble = '% start of preamble.  
    \usepackage[dvipsnames,usenames]{color} % need this for text colors
'
        #     \usepackage[pdftex]{geometry} % need this for setting page size for preview
        # This has been commented out as it's place lie in the texout.c, for
        # it's parameters to be set properly...
        @tex_preview_pagestyle = 'empty'
        
        @tex_preview_fullpage = true
        @tex_preview_minwhitespace = nil # use default
        
        @tex_preview_tiogafigure_command = 'tiogafigurescaledtofit'
        @tex_preview_figure_width = '\paperwidth - 2in'
        @tex_preview_figure_height = '\paperheight - 2in'
        
        @num_error_lines = 6
        
        @tex_xoffset = 0
        @tex_yoffset = 0
        
        @tex_preview_hoffset = '1in'
        @tex_preview_voffset = '1in'
        
        @tex_fontsize = '10.0'  
        @tex_fontfamily = 'rmdefault'
        @tex_fontseries = 'mddefault'
        @tex_fontshape = 'updefault'

        @legend_defaults = { 
            'legend_top_margin' => 0.03,
            'legend_bottom_margin' => 0.03,
            'legend_left_margin' => 0.83,
            'legend_right_margin' => 0.0,
            'plot_top_margin' => 0.0,
            'plot_bottom_margin' => 0.0,
            'plot_left_margin' => 0.0,
            'plot_right_margin' => 0.18,
            'plot_scale' => 1,
            'legend_scale' => 1 }
        
        @marker_defaults = { 
            'fill_color' => Black,
            'stroke_color' => Black,
            'scale' => 1,
            'angle' => 0,
            'justification' => CENTERED,
            'alignment' => ALIGNED_AT_MIDHEIGHT,
            'horizontal_scale' => 1.0,
            'vertical_scale' => 1.0,
            'italic_angle' => 0.0,
            'ascent_angle' => 0.0 }
                
        @eval_command = nil

        @enter_show_plot_function = nil
        @exit_show_plot_function = nil

        @enter_subfigure_function = nil
        @exit_subfigure_function = nil

        @enter_subplot_function = nil
        @exit_subplot_function = nil

        @enter_context_function = nil
        @exit_context_function = nil

        @enter_page_function = nil
        @exit_page_function = nil
                
        @tex_preview_paper_width = '297mm'
        @tex_preview_paper_height = '210mm'

        @plot_box_command = lambda { show_plot_box }

        # Automatic cleanup of by default
        @autocleanup = true
        
    end

    def initialize        
        reset_figures
    end

    def reset_state        
        reset_figures
    end

    def set_default_font_size(size, update_preview_size_string = true)
        private_set_default_font_size(size)
        return unless update_preview_size_string == true
        @tex_preview_fontsize = sprintf("%0.2fbp", size)
    end

    def page_setup(width,height) # in big-points (1/72 inch)
        set_device_pagesize(width*10-1, height*10-1)
        @tex_preview_figure_width = width.to_s + 'bp'
        @tex_preview_figure_height = height.to_s + 'bp'
        @tex_preview_paper_height = "#{height}bp"
        @tex_preview_paper_width = "#{width}bp"
        @tex_preview_tiogafigure_command = 'tiogafigureshow'
        @tex_preview_fullpage = false
        @tex_xoffset = 0
        @tex_yoffset = 0
        @tex_preview_hoffset = '0in'
        @tex_preview_voffset = '0in'
        set_frame_sides(0,1,1,0)
        set_bounds(
            'left_boundary' => 0, 'right_boundary' => 1, 
            'top_boundary' => 1, 'bottom_boundary' => 0)
        self.update_bbox(0,0)
        self.update_bbox(0,1)
        self.update_bbox(1,0)
        self.update_bbox(1,1)
    end
    
    def tex_preview_preamble # for backward compatibility
        self.tex_preamble
    end
    
    def tex_preview_preamble=(str)
        self.tex_preamble = str
    end

    def figure_name(num)
        @figure_names[num]
    end
    
    def line_color
        self.stroke_color
    end
    
    def line_color=(color)
        self.stroke_color=(color)
    end
    
    def stroke_width
        self.line_width
    end
    
    def stroke_width=(width)
        self.line_width=(width)
    end
    
    
    def opacity_for_stroke
        self.stroke_opacity
    end
    
    def opacity_for_stroke=(frac)
        self.stroke_opacity=(frac)
    end    
    
    def opacity_for_fill
        self.fill_opacity
    end
    
    def opacity_for_fill=(frac)
        self.fill_opacity=(frac)
    end
    
    
    def transparency_for_stroke
        1.0 - self.stroke_opacity
    end
    
    def transparency_for_stroke=(frac)
        self.stroke_opacity=(1.0 - frac)
    end    
    
    def transparency_for_fill
        1.0 - self.fill_opacity
    end
    
    def transparency_for_fill=(frac)
        self.fill_opacity=(1.0 - frac)
    end
    
    
    def fill_transparency
        self.transparency_for_fill
    end
    
    def fill_transparency=(frac)
        self.transparency_for_fill = frac
    end
    
    def stroke_transparency
        self.transparency_for_stroke
    end
    
    def stroke_transparency=(frac)
        self.transparency_for_stroke = frac
    end
    
    
    def title_visible=(bool)
        if bool != false
            raise "Sorry: can only set 'title_visible' to false."
        end
        self.no_title
    end
    
    def xlabel_visible=(bool)
        if bool != false
            raise "Sorry: can only set 'xlabel_visible' to false."
        end
        self.no_xlabel
    end
    
    def ylabel_visible=(bool)
        if bool != false
            raise "Sorry: can only set 'ylabel_visible' to false."
        end
        self.no_ylabel
    end
    
    def xaxis_visible=(bool)
        if bool != false
            raise "Sorry: can only set 'xaxis_visible' to false."
        end
        self.no_xaxis
    end
    
    def yaxis_visible=(bool)
        if bool != false
            raise "Sorry: can only set 'yaxis_visible' to false."
        end
        self.no_yaxis
    end
    
    def left_edge_visible=(bool)
        if bool != false
            raise "Sorry: can only set 'left_edge_visible' to false."
        end
        self.no_left_edge
    end
    
    def right_edge_visible=(bool)
        if bool != false
            raise "Sorry: can only set 'right_edge_visible' to false."
        end
        self.no_right_edge
    end
    
    def top_edge_visible=(bool)
        if bool != false
            raise "Sorry: can only set 'top_edge_visible' to false."
        end
        self.no_top_edge
    end
    
    def bottom_edge_visible=(bool)
        if bool != false
            raise "Sorry: can only set 'bottom_edge_visible' to false."
        end
        self.no_bottom_edge
    end
    
    @@keys_for_save_legend_info = FigureMaker.make_name_lookup_hash(['text', 'dy',
        'line_color', 'stroke_color', 'line_width', 'stroke_width', 
        'line_cap', 'line_type', 'marker', 'marker_color', 'marker_scale', 'marker_dict' ])
    def save_legend_info(arg)
        if arg.kind_of?String
            dict = { 'text' => arg }
        elsif arg.kind_of?Hash
            dict = arg
            check_dict(dict, @@keys_for_save_legend_info, 'save_legend_info')
        else
            raise "Sorry: argument for save_legend_info must be text or a dictionary"
        end
        dict['line_color'] = self.line_color if dict['line_color'] == nil
        dict['line_width'] = self.line_width if dict['line_width'] == nil
        dict['line_cap'] = self.line_cap if dict['line_cap'] == nil
        dict['line_type'] = self.line_type if dict['line_type'] == nil
        dict['dy'] = self.legend_text_dy if dict['dy'] == nil
        if dict['marker_dict'] == nil
            dict['marker'] = false if dict['marker'] == nil
            dict['marker_color'] = self.line_color if dict['marker_color'] == nil
            dict['marker_scale'] = 0.5 if dict['marker_scale'] == nil
        end
        @legend_info << dict
    end

    def show_legend
        char_dx = self.default_text_height_dx
        char_dy = self.default_text_height_dy
        line_ht_x = char_dx * self.legend_scale
        line_ht_y = char_dy * self.legend_scale
        x = self.legend_text_xstart*line_ht_x
        ltw = self.legend_text_width
        if ltw < 0
            if @pr_margin == 0
                ltw = 1
            else
                ltw = 7
            end
        end
        xright = x + ltw*line_ht_x
        y = 1.0 - self.legend_text_ystart*line_ht_y
        update_bbox(xright, y)
        dy = -self.legend_text_dy*line_ht_y
        line_x0 = self.legend_line_x0*line_ht_x
        line_x1 = self.legend_line_x1*line_ht_x
        line_dy = self.legend_line_dy*line_ht_y
        self.label_left_margin = self.label_right_margin = self.label_top_margin = self.label_bottom_margin = -1e99
        @legend_info.each do |dict|
            text = dict['text']
            if text != nil
                show_text('text' => text,
                    'x' => x, 'y' => y, 'scale' => self.legend_scale,
                    'justification' => self.legend_justification,
                    'alignment' => self.legend_alignment)
            end
            line_width = dict['line_width']
            if line_width >= 0
                self.line_color = dict['line_color']
                self.line_width = dict['line_width']
                self.line_cap = dict['line_cap']
                self.line_type = dict['line_type']
                stroke_line(line_x0, y+line_dy, line_x1, y+line_dy)
            end
            # place any marker right in the middle of the line
            if dict['marker_dict'] != nil
                marker_dict = dict['marker_dict']
                marker_dict['x'] = 0.5*(line_x0 + line_x1)
                marker_dict['y'] = y+line_dy
                show_marker(marker_dict)
            elsif dict['marker']
                show_marker( 'x' => 0.5*(line_x0 + line_x1),
                        'y' => (y+line_dy),
                        'marker' => dict['marker'],
                        'color' => dict['marker_color'],
                        'scale' => dict['marker_scale'])
            end
            dy = dict['dy']; dy = 1 if dy == nil
            y -= line_ht_y * dy
        end
    end

    def legend_height
        height = 0.0
        @legend_info.each { |dict| height += dict['dy'] }
        return height
    end
   
    def reset_legend_info
        @legend_info = [ ]
    end
   
    def save_legend_separator(dy)
        save_legend_info('dy' => dy, 'line_width' => -1)
    end
   
    def do_box_labels(title, xlabel, ylabel)
        if title != nil
            show_title(title); no_title
        end
        if xlabel != nil
            show_xlabel(xlabel); no_xlabel
        end
        if ylabel != nil
            show_ylabel(ylabel); no_ylabel
        end
    end
    
    def set_aspect_ratio_relative_to_frame(width_to_height)
        if width_to_height >= 1
            margin = (1 - 1.0/width_to_height) * 0.5
            set_subframe('top_margin' => margin, 'bottom_margin' => margin)
        else
            margin = (1 - width_to_height) * 0.5
            set_subframe('left_margin' => margin, 'right_margin' => margin)
        end
    end
    
    def set_physical_aspect_ratio(width_to_height)
        wd = convert_frame_to_page_dx(1)
        ht = convert_frame_to_page_dy(1)
        set_aspect_ratio_relative_to_frame(width_to_height * ht / wd)
    end
    
    def set_aspect_ratio(width_to_height)
        set_physical_aspect_ratio(width_to_height)
    end

    def set_landscape
        set_aspect_ratio(11.0/8.5)
    end
    
    def landscape
        set_landscape
    end
    
    def set_portrait
        set_aspect_ratio(8.5/11.0)
    end
    
    def portrait
        set_portrait
    end

    @@keys_for_set_subframe = FigureMaker.make_name_lookup_hash(['margins',
        'left_margin', 'right_margin', 'top_margin', 'bottom_margin',
        'left', 'right', 'plot_left_margin', 'top', 'bottom' ])
    def set_subframe(dict=nil)
        return if dict == nil
        if dict.kind_of?Array and dict.size == 4
            left = dict[0]
            right = dict[1]
            top = dict[2]
            bottom = dict[3]
        else
            check_dict(dict, @@keys_for_set_subframe, 'set_subframe')
            if (margins = dict['margins']) != nil and margins.kind_of?Array and margins.size == 4
                left = margins[0]
                right = margins[1]
                top = margins[2]
                bottom = margins[3]
            else
                left = alt_names(dict, 'left_margin', 'left')
                right = alt_names(dict, 'right_margin', 'right')
                top = alt_names(dict, 'top_margin', 'top')
                bottom = alt_names(dict, 'bottom_margin', 'bottom')
            end
        end
        left = 0 if left == nil
        right = 0 if right == nil
        top = 0 if top == nil
        bottom = 0 if bottom == nil
        private_set_subframe(left, right, top, bottom)
    end
    
    @@keys_for_set_bounds = FigureMaker.make_name_lookup_hash([
        'bounds_left', 'bounds_right', 'bounds_top', 'bounds_bottom',
        'left_boundary', 'right_boundary', 'top_boundary', 'bottom_boundary', 'boundaries' ])
    def set_bounds(dict=nil)
        return if dict == nil
        if dict.kind_of?Array and dict.size == 4
            left = dict[0]
            right = dict[1]
            top = dict[2]
            bottom = dict[3]
        else
            check_dict(dict, @@keys_for_set_bounds, 'set_bounds')
            if (boundaries = dict['boundaries']) != nil and boundaries.kind_of?Array and boundaries.size == 4
                left = boundaries[0]
                right = boundaries[1]
                top = boundaries[2]
                bottom = boundaries[3]
            else
                left = complain_if_missing_numeric_arg(dict,'bounds_left','left_boundary','set_bounds')
                right = complain_if_missing_numeric_arg(dict,'bounds_right','right_boundary','set_bounds')
                top = complain_if_missing_numeric_arg(dict,'bounds_top','top_boundary','set_bounds')
                bottom = complain_if_missing_numeric_arg(dict,'bounds_bottom','bottom_boundary','set_bounds')
            end
        end
        private_set_bounds(left, right, top, bottom)
    end
    
    
    def trace_cmd_no_arg(entry_function, exit_function, &cmd)
        
        unless entry_function == nil 
            begin
                entry_function.call
            rescue Exception => er
                report_error(er, nil)
            end
        end
        
        result = cmd.call
        
        unless exit_function == nil 
            begin
                exit_function.call
            rescue Exception => er
                report_error(er, nil)
            end
        end
        
        return result
        
    end
    
    
    def trace_cmd_one_arg(entry_function, exit_function, arg, &cmd)
        
        unless entry_function == nil 
            begin
                entry_function.call(arg)
            rescue Exception => er
                report_error(er, nil)
            end
        end
        
        result = cmd.call
        
        unless exit_function == nil 
            begin
                exit_function.call(arg)
            rescue Exception => er
                report_error(er, nil)
            end
        end
        
        return result
        
    end
    
    
    def show_plot(bounds=nil,&cmd)       
        trace_cmd_one_arg(@enter_show_plot_function, @exit_show_plot_function, bounds) {        
            set_bounds(bounds)
            context { clip_to_frame; cmd.call }
            show_plot_box
        }      
    end
    
    def subfigure(margins=nil,&cmd)
        trace_cmd_one_arg(@enter_subfigure_function, @exit_subfigure_function, margins) {        
            context { doing_subfigure; set_subframe(margins); cmd.call }
        }      
    end

    def subplot(margins=nil,&cmd)
        trace_cmd_one_arg(@enter_subplot_function, @exit_subplot_function, margins) {        
            reset_legend_info
            context { doing_subplot; set_subframe(margins); cmd.call }
        }      
    end
    
    
    @@keys_for_column_margins = FigureMaker.make_name_lookup_hash([
        'left_margin', 'right_margin', 'column_margin', 'column',
        'first_column', 'last_column', 'num_columns' ])
    def column_margins(dict)
        check_dict(dict, @@keys_for_column_margins, 'column_margins')
        left_margin = get_if_given_else_default(dict, 'left_margin', 0)
        right_margin = get_if_given_else_default(dict, 'right_margin', 0)
        num_columns = dict['num_columns']
        first_column = dict['first_column']
        last_column = dict['last_column']
        column_margin = dict['column_margin']
        column = dict['column']
        if column != nil
            first_column = last_column = column
        end
        first_column = 1 if first_column == nil
        last_column = first_column if last_column == nil
        first_column -= 1; last_column -= 1 # switch to 0 for left column instead of 1
        num_columns = last_column + 1 if num_columns == nil
        column_margin = 0 if column_margin == nil
        columns_after = num_columns - last_column - 1
        columns_before = first_column
        column_width = (1.0 - (left_margin + right_margin + column_margin * (num_columns-1))) / num_columns
        left_margin = left_margin + columns_before * (column_width + column_margin)
        right_margin = right_margin + columns_after * (column_width + column_margin)
        return { 'left_margin' => left_margin, 'right_margin' => right_margin }
    end
    
    @@keys_for_row_margins = FigureMaker.make_name_lookup_hash([
        'top_margin', 'bottom_margin', 'row_margin', 'row',
        'first_row', 'last_row', 'num_rows' ])
    def row_margins(dict)
        check_dict(dict, @@keys_for_row_margins, 'row_margins')
        top_margin = get_if_given_else_default(dict, 'top_margin', 0)
        bottom_margin = get_if_given_else_default(dict, 'bottom_margin', 0)
        num_rows = dict['num_rows']
        first_row = dict['first_row']
        last_row = dict['last_row']
        row_margin = dict['row_margin']
        row = dict['row']
        if row != nil
            first_row = last_row = row
        end
        first_row = 1 if first_row == nil
        last_row = first_row if last_row == nil
        first_row -= 1; last_row -= 1 # switch to 0 for top row instead of 1
        num_rows = last_row + 1 if num_rows == nil
        row_margin = 0 if row_margin == nil
        rows_below = num_rows - last_row - 1
        rows_above = first_row
        row_width = (1.0 - (top_margin + bottom_margin + row_margin * (num_rows-1))) / num_rows
        top_margin = top_margin + rows_above * (row_width + row_margin)
        bottom_margin = bottom_margin + rows_below * (row_width + row_margin)
        return { 'top_margin' => top_margin, 'bottom_margin' => bottom_margin }
    end


    def context(&cmd)
        trace_cmd_no_arg(@enter_context_function, @exit_context_function) {        
            private_context(cmd) }      
    end

    
    def rescale(factor)
        rescale_text(factor)
        rescale_lines(factor)
    end

    def show_plot_box
        show_xaxis
        show_yaxis
        show_edges
        show_title
        show_xlabel
        show_ylabel
    end
    
    def show_xaxis
        show_axis(self.xaxis_loc) if self.xaxis_visible
    end
    
    def show_yaxis
        show_axis(self.yaxis_loc) if self.yaxis_visible
    end
    
    def show_edges
        show_top_edge
        show_bottom_edge
        show_left_edge
        show_right_edge
    end
    
    def show_top_edge
        show_edge(TOP) if self.top_edge_visible && self.xaxis_loc != TOP
    end
    
    def show_bottom_edge
        show_edge(BOTTOM) if self.bottom_edge_visible && self.xaxis_loc != BOTTOM
    end
    
    def show_left_edge
        show_edge(LEFT) if self.left_edge_visible && self.yaxis_loc != LEFT
    end
    
    def show_right_edge
        show_edge(RIGHT) if self.right_edge_visible && self.yaxis_loc != RIGHT
    end
    
    def show_title(text = nil)
        text = self.title if text == nil
        if (self.title_visible && text != nil)
            show_text('text' => text,
                'side' => self.title_side, 'position' => self.title_position,
                'scale' => self.title_scale, 'shift' => self.title_shift,
                'angle' => self.title_angle, 'alignment' => self.title_alignment,
                'justification' => self.title_justification,
                'color' => self.title_color)
        end
    end
    
    def show_xlabel(text = nil)
        text = self.xlabel if text == nil
        if (self.xlabel_visible && text != nil)
            angle = self.xlabel_angle
            side = self.xlabel_side
            shift = self.xlabel_shift
            shift -= 0.5 if (side == TOP && angle == 0)
            show_text('text' => text,
                'side' => side, 'position' => self.xlabel_position,
                'scale' => self.xlabel_scale, 'shift' => shift,
                'angle' => angle, 'alignment' => self.xlabel_alignment,
                'justification' => self.xlabel_justification,
                'color' => self.xlabel_color)
        end
    end
    
    def show_ylabel(text = nil)
        text = self.ylabel if text == nil
        if (self.ylabel_visible && text != nil)
            angle = self.ylabel_angle
            side = self.ylabel_side
            shift = self.ylabel_shift
            shift += 0.5 if (side == RIGHT && angle == 0)
            show_text('text' => text,
                'side' => side, 'position' => self.ylabel_position,
                'scale' => self.ylabel_scale, 'shift' => shift,
                'angle' => angle, 'alignment' => self.ylabel_alignment,
                'justification' => self.ylabel_justification,
                'color' => self.ylabel_color)
        end
    end
    
    @@keys_for_show_plot_with_legend = FigureMaker.make_name_lookup_hash([
        'legend_top_margin', 'legend_bottom_margin', 'legend_left_margin', 'legend_right_margin',
        'plot_top_margin', 'plot_bottom_margin', 'plot_left_margin', 'plot_right_margin',
        'plot_scale', 'legend_scale' ])
    def show_plot_with_legend(dict=nil, &cmd)
        check_dict(dict, @@keys_for_show_plot_with_legend, 'show_plot_with_legend') if dict != nil
        legend_top_margin = get_if_given_else_use_default_dict(dict, 'legend_top_margin', @legend_defaults)
        legend_bottom_margin = get_if_given_else_use_default_dict(dict, 'legend_bottom_margin', @legend_defaults)
        legend_left_margin = get_if_given_else_use_default_dict(dict, 'legend_left_margin', @legend_defaults)
        legend_right_margin = get_if_given_else_use_default_dict(dict, 'legend_right_margin', @legend_defaults)
        legend_scale = get_if_given_else_use_default_dict(dict, 'legend_scale', @legend_defaults)
        plot_top_margin = get_if_given_else_use_default_dict(dict, 'plot_top_margin', @legend_defaults)
        plot_bottom_margin = get_if_given_else_use_default_dict(dict, 'plot_bottom_margin', @legend_defaults)
        plot_left_margin = get_if_given_else_use_default_dict(dict, 'plot_left_margin', @legend_defaults)
        plot_right_margin = get_if_given_else_use_default_dict(dict, 'plot_right_margin', @legend_defaults)
        plot_scale = get_if_given_else_use_default_dict(dict, 'plot_scale', @legend_defaults)
        reset_legend_info
        rescale(plot_scale)
        subplot([plot_left_margin, plot_right_margin, plot_top_margin, plot_bottom_margin]) { cmd.call }
        set_subframe([legend_left_margin, legend_right_margin, legend_top_margin, legend_bottom_margin])
        rescale(legend_scale) # note that legend_scale is an addition to the plot_scale, not a replacement
        @pr_margin = plot_right_margin
        show_legend
    end
    
    def append_points_with_gaps_to_path(xs, ys, gaps, close_subpaths = false)
        private_append_points_with_gaps_to_path(xs, ys, gaps, close_subpaths)
    end

    def show_polyline(xs, ys, color = nil, legend = nil, type = nil, gaps = nil, close_subpaths = nil)
        context do
            self.line_type = type if type != nil
            self.stroke_color = color if color != nil
            append_points_with_gaps_to_path(xs, ys, gaps, close_subpaths)
            stroke
            save_legend_info(legend) if legend != nil
        end
    end
    
    def stroke_polyline(xs, ys, color = nil, legend = nil, type = nil, gaps = nil, close_subpaths = nil)
        show_polyline(xs, ys, color, legend, type, gaps, close_subpaths)
    end
        
    def show_contour(xs, ys, gaps = nil, color = nil, type = nil, legend = nil)
        show_polyline(xs, ys, color, legend, type, gaps, false)
    end
    
    @@keys_for_make_contour = FigureMaker.make_name_lookup_hash([
        'dest_xs', 'dest_ys', 'z_level', 'z', 'level', 'xs', 'ys', 'gaps', 'zs', 'data', 'legit', 'method'])
    def make_contour(dict)
        check_dict(dict, @@keys_for_make_contour, 'make_contour')
        z_level = dict['z_level']
        if z_level == nil
            z_level = complain_if_missing_numeric_arg(dict, 'z', 'level', 'make_contour')
        end
        dest_xs = get_dvec(dict, 'dest_xs', 'make_contour')
        dest_ys = get_dvec(dict, 'dest_ys', 'make_contour')
        xs = get_dvec(dict, 'xs', 'make_contour')
        ys = get_dvec(dict, 'ys', 'make_contour')
        gaps = dict['gaps']
        if (!(gaps.kind_of? Array))
            raise "Sorry: 'gaps' for 'make_contour' must be an Array"
        end
        zs = alt_names(dict, 'zs', 'data')
        if (!(zs.kind_of? Dtable))
            raise "Sorry: 'zs' for 'make_contour' must be a Dtable"
        end
        dest_xs.clear; dest_ys.clear; gaps.clear
        
        legit = dict['legit']
        if legit == nil
          legit = Dtable.new(xs.length,ys.length).set(1.0)
        elsif (!(legit.kind_of? Dtable))
            raise "Sorry: 'legit' for 'make_contour' must be a Dtable -- nonzero means legitimate value in corresponding entry in zs"
        end
        
        method = dict['method']
        use_conrec = (method == 'conrec' or method == 'CONREC')? 1 : 0
        private_make_contour(dest_xs, dest_ys, gaps, xs, ys, zs, z_level, legit, use_conrec)
            
    end
    
    @@keys_for_make_steps = FigureMaker.make_name_lookup_hash([
        'xfirst', 'x_first', 'yfirst', 'y_first', 'xlast', 'x_last', 'ylast', 'y_last',
        'xs', 'ys', 'dest_xs', 'dest_ys'])
    def make_steps(dict)
        check_dict(dict, @@keys_for_make_steps, 'make_steps')
        xfirst = complain_if_missing_numeric_arg(dict, 'xfirst', 'x_first', 'make_steps')
        yfirst = complain_if_missing_numeric_arg(dict, 'yfirst', 'y_first', 'make_steps')
        xlast = complain_if_missing_numeric_arg(dict, 'xlast', 'x_last', 'make_steps')
        ylast = complain_if_missing_numeric_arg(dict, 'ylast', 'y_last', 'make_steps')
        dest_xs = get_dvec(dict, 'dest_xs', 'make_steps')
        dest_ys = get_dvec(dict, 'dest_ys', 'make_steps')
        xs = get_dvec(dict, 'xs', 'make_steps')
        ys = get_dvec(dict, 'ys', 'make_steps')
        private_make_steps(dest_xs, dest_ys, xs, ys, xfirst, yfirst, xlast, ylast)
    end
    
    @@keys_for_make_curves = FigureMaker.make_name_lookup_hash([
        'start_slope', 'end_slope', 'xs', 'ys', 'sample_xs', 'result_ys'])
    def make_spline_interpolated_points(dict)
        check_dict(dict, @@keys_for_make_curves, 'make_spline_interpolated_points')
        start_slope = dict['start_slope']
        end_slope = dict['end_slope']
        sample_xs = get_dvec(dict, 'sample_xs', 'make_spline_interpolated_points')
        result_ys = get_dvec(dict, 'result_ys', 'make_spline_interpolated_points')
        xs = get_dvec(dict, 'xs', 'make_spline_interpolated_points')
        ys = get_dvec(dict, 'ys', 'make_spline_interpolated_points')
        private_make_spline_interpolated_points(sample_xs, result_ys, xs, ys, start_slope, end_slope)
    end
    
    @@keys_for_make_interpolant = FigureMaker.make_name_lookup_hash([
        'start_slope', 'end_slope', 'xs', 'ys'])
    def make_interpolant(dict)
        check_dict(dict, @@keys_for_make_interpolant, 'make_smoothed')
        start_slope = dict['start_slope']
        end_slope = dict['end_slope']
        xs = get_dvec(dict, 'xs', 'make_interpolant')
        ys = get_dvec(dict, 'ys', 'make_interpolant')
        return Dvector.create_spline_interpolant(xs, ys,
                (start_slope != nil), start_slope, (end_slope != nil), end_slope)
    end

    def append_interpolant_to_path(interp)
        x_s = interp[0]; y_s = interp[1]; a_s = interp[2]; b_s = interp[3]; c_s = interp[4]
        ctrl = Dvector.new
        move_to_point(x_s[0], y_s[0]);
        (x_s.size - 1).times do |i|
            x0 = x_s[i]; delta_x = x_s[i+1] - x0
            ctrl.make_bezier_control_points_for_cubic_in_x(x0, y_s[i], delta_x, a_s[i], b_s[i], c_s[i])
            append_curve_to_path(ctrl[0], ctrl[1], ctrl[2], ctrl[3], ctrl[4], ctrl[5])
        end
    end
    
    @@keys_for_show_error_bars = FigureMaker.make_name_lookup_hash(['x', 'y', 'dx', 'dy',
        'dx_plus', 'dx_minus', 'dy_plus', 'dy_minus', 'color', 'end_cap', 'line_width'])
    def show_error_bars(dict)
        check_dict(dict, @@keys_for_show_error_bars, 'show_error_bars')
        x = dict['x']
        y = dict['y']
        if (x == nil || y == nil)
            raise "Sorry: Must give both 'x' and 'y' for show_error_bar."
        end
        dx = dict['dx']
        dx_plus = get_if_given_else_default(dict, 'dx_plus', dx)
        dx_minus = get_if_given_else_default(dict, 'dx_minus', dx)
        dy = dict['dy']
        dy_plus = get_if_given_else_default(dict, 'dy_plus', dy)
        dy_minus = get_if_given_else_default(dict, 'dy_minus', dy)
        if (dx_plus == nil || dx_minus == nil || dy_plus == nil || dy_minus == nil)
            raise "Sorry: Must give both 'dx' and 'dy' error ranges for show_error_bar."
        end
        end_cap = get_if_given_else_default(dict, 'end_cap', 0.15) # end_cap length in default text heights
        x_end_cap = end_cap * self.default_text_height_dy
        y_end_cap = end_cap * self.default_text_height_dx
        line_width = get_if_given_else_default(dict, 'line_width', 1)
        prev_line_width = self.line_width
        self.line_width = line_width if prev_line_width != line_width
        color = get_if_given_else_default(dict, 'color', Black)
        prev_color = self.stroke_color
        self.stroke_color = color if prev_color != color
        # vertical error (dy)
        if (dy_plus != 0 || dy_minus != 0) 
            stroke_line(x, y+dy_plus, x, y-dy_minus)
            stroke_line(x-y_end_cap, y+dy_plus, x+y_end_cap, y+dy_plus)
            stroke_line(x-y_end_cap, y-dy_minus, x+y_end_cap, y-dy_minus)
        end
        # horizontal error (dx)
        if (dx_plus != 0 || dx_minus != 0) 
            stroke_line(x+dx_plus, y, x-dx_minus, y)
            stroke_line(x+dx_plus, y-x_end_cap, x+dx_plus, y+x_end_cap)
            stroke_line(x-dx_minus, y-x_end_cap, x-dx_minus, y+x_end_cap)
        end
        self.line_width = prev_line_width if prev_line_width != line_width
        self.stroke_color = prev_color if prev_color != color
    end
    
    @@keys_for_show_arrow = FigureMaker.make_name_lookup_hash([
        'x_head', 'y_head', 'x_tail', 'y_tail', 'head', 'tail',
        'color', 'head_color', 'tail_color', 'line_color',
        'head_marker', 'tail_marker', 'line_width', 'head_scale', 'tail_scale'])
    def show_arrow(dict)
        check_dict(dict, @@keys_for_show_arrow, 'show_arrow')
        if check_pair(head = dict['head'], 'head', 'show_arrow')
            x_head = head[0]
            y_head = head[1]
        else
            x_head = dict['x_head']
            y_head = dict['y_head']
        end
        if (x_head == nil || y_head == nil)
            raise "Sorry: Must give both 'x_head' and 'y_head' for show_arrow."
        end
        if check_pair(tail = dict['tail'], 'tail', 'show_arrow')
            x_tail = tail[0]
            y_tail = tail[1]
        else
            x_tail = dict['x_tail']
            y_tail = dict['y_tail']
        end
        if (x_tail == nil || y_tail == nil)
            raise "Sorry: Must give both 'x_tail' and 'y_tail' for show_arrow."
        end
        color = get_if_given_else_default(dict, 'color', Black)
        head_color = get_if_given_else_default(dict, 'head_color', color)
        tail_color = get_if_given_else_default(dict, 'tail_color', color)
        line_color = get_if_given_else_default(dict, 'line_color', color)
        head_marker = get_if_given_else_default(dict, 'head_marker', Arrowhead)
        tail_marker = get_if_given_else_default(dict, 'tail_marker', BarThin)
        line_width = get_if_given_else_default(dict, 'line_width', 1)
        head_scale = dict['head_scale']
        head_scale = @marker_defaults['scale'] if head_scale == nil
        tail_scale = dict['tail_scale']
        prev_line_cap = self.line_cap
        self.line_cap = LINE_CAP_BUTT if prev_line_cap != LINE_CAP_BUTT
        prev_line_width = self.line_width
        self.line_width = line_width if prev_line_width != line_width
        prev_stroke_color = self.stroke_color
        self.stroke_color = line_color if line_color != prev_stroke_color
        dx = x_head - x_tail
        dy = y_head - y_tail
        pg_dx = convert_figure_to_output_dx(dx)
        pg_dy = convert_figure_to_output_dy(dy)
        len = (pg_dx*pg_dx + pg_dy*pg_dy).sqrt
        chsz = convert_figure_to_output_dx(default_text_height_dx)
        chsz = -chsz if chsz < 0
        newlen = len - 0.5*chsz
        newlen = 0 if newlen < 0
        frac = newlen / len
        stroke_line(x_tail + frac*dx, y_tail + frac*dy, x_tail, y_tail)
        angle = convert_to_degrees(dx, dy)
        if head_marker != 'None'
            if head_marker == Arrowhead || head_marker == ArrowheadOpen
                just = RIGHT_JUSTIFIED
            else
                just = CENTERED
            end
            show_marker('marker' => head_marker, 'point' => [x_head, y_head], 'color' => head_color,
                'justification' => just, 'angle'=> angle, 'scale' => head_scale)
        end
        if tail_marker != 'None'
            show_marker('marker' => tail_marker, 'point' => [x_tail, y_tail], 'color' => tail_color,
                'angle'=> angle, 'scale' => tail_scale)
        end
        self.line_cap = prev_line_cap if prev_line_cap != LINE_CAP_BUTT
        self.line_width = prev_line_width if prev_line_width != line_width
        self.stroke_color = prev_stroke_color if prev_stroke_color != line_color
    end
    
    @@keys_for_axial_shading = FigureMaker.make_name_lookup_hash(['extend_start', 'extend_end',
        'x_start', 'y_start', 'start', 'start_point', 'x_end', 'y_end', 'end', 'end_point', 'colormap', 'color_map'])
    def axial_shading(dict)
        check_dict(dict, @@keys_for_axial_shading, 'axial_shading')
        x_start = y_start = nil
        start = alt_names(dict, 'start', 'start_point')
        if check_pair(start, 'start', 'axial_shading')
            x_start = start[0]; y_start = start[1];
        end
        x_start = dict['x_start'] if x_start == nil
        y_start = dict['y_start'] if y_start == nil
        if (x_start == nil || y_start == nil)
            raise "Sorry: Must give both 'x_start' and 'y_start' for axial_shading."
        end
        x_end = y_end = nil
        endpt = alt_names(dict, 'end', 'end_point')
        if check_pair(endpt, 'end', 'axial_shading')
            x_end = endpt[0]; y_end= endpt[1];
        end
        x_end = dict['x_end'] if x_end == nil
        y_end = dict['y_end'] if y_end == nil
        if (x_end == nil || y_end == nil)
            raise "Sorry: Must give both 'x_end' and 'y_end' for axial_shading."
        end
        colormap = alt_names(dict, 'colormap', 'color_map')
        extend_start = dict['extend_start']
        extend_end = dict['extend_end']
        private_axial_shading(x_start, y_start, x_end, y_end, colormap, extend_start, extend_end)
    end
    
    @@keys_for_radial_shading = FigureMaker.make_name_lookup_hash(['extend_start', 'extend_end',
        'x_start', 'y_start', 'radius_start', 'start_radius', 'start', 'start_circle',
        'x_end', 'y_end', 'radius_start', 'end_radius', 'end', 'end_circle',
        'colormap', 'color_map', 'x_hat', 'y_hat', 'xhat', 'yhat'])
    def radial_shading(dict)
        check_dict(dict, @@keys_for_radial_shading, 'radial_shading')
        start_circle = alt_names(dict, 'start_circle', 'start')
        end_circle = alt_names(dict, 'end_circle', 'end')
        colormap = alt_names(dict, 'colormap', 'color_map')
        x_hat = alt_names(dict, 'x_hat', 'xhat')
        x_hat = [1, 0] if x_hat == nil
        y_hat = alt_names(dict, 'y_hat', 'yhat')
        y_hat = [0, 1] if y_hat == nil
        origin = dict['origin']
        origin = [0, 0] if origin == nil
        extend_start = dict['extend_start']
        extend_end = dict['extend_end']
        private_radial_shading(start_circle[0], start_circle[1], start_circle[2],
            end_circle[0], end_circle[1], end_circle[2], colormap,
            x_hat[0], y_hat[0], x_hat[1], y_hat[1],
            extend_start, extend_end)
    end
    
    @@keys_for_show_image = FigureMaker.make_name_lookup_hash([
        'll', 'lr', 'ul', 'w', 'width', 'height', 'h',
        'opacity_mask', 'stencil_mask', 
        'jpg', 'JPG', 'jpeg', 'JPEG', 'interpolate', 'data', 'value_mask',
        'color_space', 'color_map', 'colormap'])
    def show_image(dict)
        internal_show_image(dict, false)
    end
    
    @@keys_for_create_colormap = FigureMaker.make_name_lookup_hash(['length', 'points', 'Rs', 'Gs', 'Bs', 'Hs', 'Ls', 'Ss'])
    def create_colormap(dict)
        check_dict(dict, @@keys_for_create_colormap, 'create_colormap')
        length = dict['length']
        length = 256 if length == nil
        if !(length >= 2 && length <= 256)
            raise "Sorry: dictionary for create color map must have 'length' between 2 and 256"
        end
        points = dict['points']
        if points == nil
            raise "Sorry: dictionary for create color map must have 'points' set to a vector of increasing locations from 0.0 to 1.0"
        end
        c1s = dict['Rs']
        c2s = dict['Gs']
        c3s = dict['Bs']
        if c1s != nil && c2s != nil && c3s != nil
            rgb_flag = true
        else
            c1s = dict['Hs']
            c2s = dict['Ls']
            c3s = dict['Ss']
            if c1s == nil || c2s == nil || c3s == nil
                raise "Sorry: dictionary for create color map must have 'Rs', 'Gs', and 'Bs', or 'Hs', 'Ls', and 'Ss'."
            end
            rgb_flag = false
        end
        private_create_colormap(rgb_flag, length, points, c1s, c2s, c3s)
    end
    
    def intense_colormap
        if @intense_colormap == nil
            @intense_colormap = create_colormap(
                'points' => [0.0, 0.44, 0.50, 0.50, 0.56, 1.0],
                'Hs' =>     [240,  240,  240,    0,    0,   0],
                'Ls' =>     [0.5, 0.90, 0.99, 0.99, 0.90, 0.5],
                'Ss' =>     [1.0,  1.0,  1.0,  1.0,  1.0, 1.0]
                )
        end
        @intense_colormap
    end
    
    def mellow_colormap 
        if @mellow_colormap == nil
            @mellow_colormap = create_colormap(
                'points' => [0.0, 0.44, 0.50, 0.50, 0.56, 1.0],
                'Hs' =>     [240,  240,  240,    0,    0,   0],
                'Ls' =>     [0.5, 0.90, 0.99, 0.99, 0.90, 0.5],
                'Ss' =>     [0.5,  1.0,  1.0,  1.0,  1.0, 0.5]
                )   
        end
        @mellow_colormap
    end
    
    def rainbow_colormap
        if @rainbow_colormap == nil
            @rainbow_colormap = create_colormap(
                'points' => [0.00, 0.27, 0.46, 0.73, 1.00],  # distorted to reduce size of green
  # modified for debugging purposes...
                'Hs' => (Dvector[0.0, 1.0, 2.0, 3.0, 4.0]*90.0 - 20.0)*(340.0/360.0) + 10.0,
                'Ls' =>     [  0.5,    0.5,    0.5,    0.5,    0.5],
                'Ss' =>     [  1.0,    1.0,    1.0,    1.0,    1.0]
                )  
        end
        @rainbow_colormap
    end
    
    @@keys_for_create_gradient_colormap = FigureMaker.make_name_lookup_hash([
        'hue', 'starting_H', 'ending_H',
        'lightness', 'starting_L', 'ending_L', 'saturation', 'starting_S', 'ending_S'])
    def create_gradient_colormap(dict)
        check_dict(dict, @@keys_for_create_gradient_colormap, 'create_gradient_colormap')
        hue = get_if_given_else_default(dict, 'hue', 0)
        starting_H = get_if_given_else_default(dict, 'starting_H', hue)
        ending_H = get_if_given_else_default(dict, 'ending_H', hue)
        lightness = get_if_given_else_default(dict, 'lightness', 0.5)
        starting_L = get_if_given_else_default(dict, 'starting_L', lightness)
        ending_L = get_if_given_else_default(dict, 'ending_L', lightness)
        saturation = get_if_given_else_default(dict, 'saturation', 0.5)
        starting_S = get_if_given_else_default(dict, 'starting_S', saturation)
        ending_S = get_if_given_else_default(dict, 'ending_S', saturation)
        create_colormap(
                'points' => [0.00, 1.00],
                'Hs' =>     [starting_H, ending_H],
                'Ls' =>     [starting_L, ending_L],
                'Ss' =>     [starting_S, ending_S]
                )  
    end
    
    @@keys_for_create_image_data = FigureMaker.make_name_lookup_hash([
        'first_row', 'last_row', 'first_column', 'last_column',
        'min_value', 'max_value', 'max_code', 'if_below_range', 'if_above_range', 'masking'])
    def create_image_data(data, dict)
        check_dict(dict, @@keys_for_create_image_data, 'create_image_data')
        first_row = dict['first_row']
        last_row = dict['last_row']
        first_column = dict['first_column']
        last_column = dict['last_column']
        min_value = dict['min_value']
        max_value = dict['max_value']
        max_code = dict['max_code']
        if_below_range = dict['if_below_range']
        if_above_range = dict['if_above_range']
        first_row = 0 if first_row == nil
        last_row = -1 if last_row == nil
        first_column = 0 if first_column == nil
        last_column = -1 if last_column == nil
        min_value = data.min if min_value == nil
        max_value = data.max if max_value == nil
        max_code = 255 if max_code == nil
        if_below_range = 0 if if_below_range == nil
        if_above_range = max_code if if_above_range == nil
        if dict['masking'] == true
            max_code = 254; if_below_range = if_above_range = 255
        end
        return private_create_image_data(data, first_row, last_row, first_column, last_column,
            min_value, max_value, max_code, if_below_range, if_above_range);
    end
    
    @@keys_for_create_monochrome_image_data = FigureMaker.make_name_lookup_hash([
        'first_row', 'last_row', 'first_column', 'last_column', 'boundary', 'reverse'])
    def create_monochrome_image_data(data, dict)
        check_dict(dict, @@keys_for_create_monochrome_image_data, 'create_monochrome_image_data')
        first_row = dict['first_row']
        last_row = dict['last_row']
        first_column = dict['first_column']
        last_column = dict['last_column']
        boundary = dict['boundary']
        reverse = dict['reverse']
        first_row = 0 if first_row == nil
        last_row = -1 if last_row == nil
        first_column = 0 if first_column == nil
        last_column = -1 if last_column == nil
        boundary = 0 if boundary == nil
        reverse = false if reverse == nil
        return private_create_monochrome_image_data(data, first_row, last_row, first_column, last_column,
            boundary, reverse);
    end
    
    @@keys_for_show_marker = FigureMaker.make_name_lookup_hash([
        'marker', 'x', 'y', 'at', 'point', 'Xs', 'Ys', 'xs', 'ys', 'mode', 'rendering_mode',
        'angle', 'scale', 'font', 'string', 'text', 'color', 'fill_color', 'stroke_color', 'stroke_width',
        'horizontal_scale', 'vertical_scale', 'italic_angle', 'ascent_angle', 'alignment', 'justification'])
    def show_marker(dict)
        check_dict(dict, @@keys_for_show_marker, 'show_marker')
        marker = dict['marker']
        x = y = nil
        at = alt_names(dict, 'at', 'point')
        if check_pair(at, 'at', 'show_text')
            x = at[0]; y = at[1];
        end
        x = dict['x'] if x == nil
        y = dict['y'] if y == nil
        xs = alt_names(dict, 'Xs', 'xs')
        ys = alt_names(dict, 'Ys', 'ys')
        if ((xs != nil && ys == nil) || (ys != nil && xs == nil))
            raise "Sorry: Must supply both xs and ys for show_marker"
        end
        if (xs != nil && ys != nil && xs.size != ys.size)
            raise "Sorry: Must equal length xs and ys for show_marker"
        end
        color = dict['color']
        if color == nil
            fill_color = get_if_given_else_use_default_dict(dict, 'fill_color', @marker_defaults)
            stroke_color = get_if_given_else_use_default_dict(dict, 'stroke_color', @marker_defaults)
        else
            fill_color = stroke_color = color
        end
        font = dict['font']
        mode = alt_names(dict, 'mode', 'rendering_mode')
        string = alt_names(dict, 'string', 'text')
        if (marker == nil && string == nil)
            raise "Sorry: Must give either 'marker' or 'string' for show_marker"
        end
        if (marker != nil && string != nil)
            raise "Sorry: Must give either 'marker' or 'string' for show_marker, but not both"
        end
        if (marker == nil)
            glyph = stroke_width = nil
        else
            if !(marker.kind_of?Array) && marker.size >= 2 && marker.size <= 3
                raise "Sorry: 'marker' for show_marker must be array of [font_number, char_code] or [font_number, char_code, rendering_mode]"
            end
            font = marker[0]
            glyph = marker[1]
            if marker.size == 3
                mode = STROKE if mode == nil
                stroke_width = marker[2]
            else
                mode = FILL if mode == nil
                stroke_width = nil
            end
        end
        mode = FILL if mode == nil
        font = Times_Roman if (font == nil && string != nil)
        if (font != nil && !(font.kind_of? Integer))
            raise "Sorry: 'font' for show_marker must be an integer font number (see FigureConstants list)"
        end
        stroke_width = get_if_given_else_default(dict, 'stroke_width', stroke_width)
        angle = get_if_given_else_use_default_dict(dict, 'angle', @marker_defaults)
        scale = get_if_given_else_use_default_dict(dict, 'scale', @marker_defaults)
        just = get_if_given_else_use_default_dict(dict, 'justification', @marker_defaults)
        align = get_if_given_else_use_default_dict(dict, 'alignment', @marker_defaults)
        h_scale = get_if_given_else_use_default_dict(dict, 'horizontal_scale', @marker_defaults)
        v_scale = get_if_given_else_use_default_dict(dict, 'vertical_scale', @marker_defaults)
        it_angle = get_if_given_else_use_default_dict(dict, 'italic_angle', @marker_defaults)
        ascent_angle = get_if_given_else_use_default_dict(dict, 'ascent_angle', @marker_defaults)
        glyph = 0 if glyph == nil
        int_args = glyph*100000 + font*1000 + mode*100 + align*10 + just
            # Ruby limits us to 15 args, so pack some small integers together
        private_show_marker(int_args, stroke_width, string, x, y, xs, ys,
            h_scale, v_scale, scale, it_angle, ascent_angle, angle, fill_color, stroke_color)
    end
    
    def show_label(dict)
        at = alt_names(dict, 'at', 'point')
        if check_pair(at, 'at', 'show_text')
            xloc = at[0]
            yloc = at[1]
        else
            xloc = dict['x']
            yloc = dict['y']
        end
        if (xloc == nil || yloc == nil)
            raise "Sorry: Must supply 'at', 'point',  or 'x' and 'y' for show_label"
        end
        return if !check_label_clip(xloc, yloc)
        show_text(dict)
    end
   
    @@keys_for_show_text = FigureMaker.make_name_lookup_hash([
        'text', 'side', 'loc', 'position', 'pos', 'x', 'y',
        'shift', 'scale', 'color', 'angle', 'alignment', 'justification', 'at', 'point'])
    def show_text(dict)
        check_dict(dict, @@keys_for_show_text, 'show_text')
        text = dict['text']
        if text == nil
            raise "Sorry: Must supply 'text' entry in dictionary for show_text"
        end
        scale = get_if_given_else_default(dict, 'scale', 1)
        color = dict['color'] # color is [r,g,b] array.  this adds \textcolor[rgb]{r,g,b}{...}
        if color != nil
            if !color.kind_of?Array
                raise "Sorry: 'color' must be array of [r,g,b] intensities for show_text (#{color})"
            end
            r = color[0]; g = color[1]; b = color[2];
            if (!(r.kind_of? Numeric) || !(g.kind_of? Numeric) || !(b.kind_of? Numeric) )
                raise "Sorry: 'color' must be array of [r,g,b] intensities for show_text"
            end 
            text = sprintf("\\textcolor[rgb]{%0.2f,%0.2f,%0.2f}{%s}", r, g, b, text)
        end
        just = get_if_given_else_default(dict, 'justification', self.justification)
        align = get_if_given_else_default(dict, 'alignment', self.alignment)
        angle = get_if_given_else_default(dict, 'angle', 0)
        loc = alt_names(dict, 'loc', 'side')
        if (loc == nil)
            at = alt_names(dict, 'at', 'point')
            if check_pair(at, 'at', 'show_text')
                xloc = at[0]
                yloc = at[1]
            else
                xloc = dict['x']
                yloc = dict['y']
            end
            if (xloc == nil || yloc == nil)
                raise "Sorry: Must supply a location for show_text"
            end
            show_rotated_label(text, xloc, yloc, scale, angle, just, align)
            return
        end
        position = alt_names(dict, 'position', 'pos')
        position = 0.5 if position == nil
        shift = dict['shift']
        if loc == LEFT
            shift = self.text_shift_on_left if shift == nil
        elsif loc == RIGHT
            shift = self.text_shift_on_right if shift == nil
        elsif loc == TOP
            shift = self.text_shift_on_top if shift == nil
        elsif loc == BOTTOM
            shift = self.text_shift_on_bottom if shift == nil
        else
            if (loc == AT_X_ORIGIN)
                shift = self.text_shift_from_x_origin if shift == nil
                xloc = shift*self.char_height_dx
                yloc = convert_frame_to_figure_y(position)
                return
            elsif (loc == AT_Y_ORIGIN)
                shift = self.text_shift_from_y_origin if shift == nil
                yloc = shift*self.char_height_dy
                xloc = convert_frame_to_figure_x(position)
            else
                raise "Sorry: 'loc' must be LEFT, RIGHT, TOP, BOTTOM, AT_X_ORIGIN, or AT_Y_ORIGIN for show_text"
            end
            show_rotated_label(text, xloc, yloc, scale, angle, just, align)
            return
        end
        show_rotated_text(text, loc, shift, position, scale, angle, just, align)
    end
    
    
    def reset_eval_function
        @eval_command = nil
    end
    
    def def_eval_function(&cmd)
        if cmd == nil
            raise "Sorry: must provide a command block for def_eval"
        end
        @eval_command = cmd
    end
    
    
    
    def def_enter_page_function(&cmd)
        if cmd == nil
            raise "Sorry: must provide a command block for def_enter_page_function"
        end
        @enter_page_function = cmd
    end
    
    def reset_enter_page_function
        @enter_page_function = nil
    end   
    
    def def_exit_page_function(&cmd)
        if cmd == nil
            raise "Sorry: must provide a command block for def_exit_page_function"
        end
        @exit_page_function = cmd
    end
    
    def reset_exit_page_function
        @exit_page_function = nil
    end
    
    
    
    def def_enter_show_plot_function(&cmd)
        if cmd == nil
            raise "Sorry: must provide a command block for def_enter_show_plot_function"
        end
        @enter_show_plot_function = cmd
    end
    
    def reset_enter_show_plot_function
        @enter_show_plot_function = nil
    end   
    
    def def_exit_show_plot_function(&cmd)
        if cmd == nil
            raise "Sorry: must provide a command block for def_exit_show_plot_function"
        end
        @exit_show_plot_function = cmd
    end
    
    def reset_exit_show_plot_function
        @exit_show_plot_function = nil
    end

    
    
    def def_enter_subfigure_function(&cmd)
        if cmd == nil
            raise "Sorry: must provide a command block for def_enter_subfigure_function"
        end
        @enter_subfigure_function = cmd
    end
    
    def reset_enter_subfigure_function
        @enter_subfigure_function = nil
    end
     
    def def_exit_subfigure_function(&cmd)
        if cmd == nil
            raise "Sorry: must provide a command block for def_exit_subfigure_function"
        end
        @exit_subfigure_function = cmd
    end
    
    def reset_exit_subfigure_function
        @exit_subfigure_function = nil
    end
    
    
    def def_enter_subplot_function(&cmd)
        if cmd == nil
            raise "Sorry: must provide a command block for def_enter_subplot_function"
        end
        @enter_subplot_function = cmd
    end
    
    def reset_enter_subplot_function
        @enter_subplot_function = nil
    end     
    
    def def_exit_subplot_function(&cmd)
        if cmd == nil
            raise "Sorry: must provide a command block for def_exit_subplot_function"
        end
        @exit_subplot_function = cmd
    end
    
    def reset_exit_subplot_function
        @exit_subplot_function = nil
    end
    
    
    def def_enter_context_function(&cmd)
        if cmd == nil
            raise "Sorry: must provide a command block for def_enter_context_function"
        end
        @enter_context_function = cmd
    end
    
    def reset_enter_context_function
        @enter_subplot_context = nil
    end
    
    def def_exit_context_function(&cmd)
        if cmd == nil
            raise "Sorry: must provide a command block for def_exit_context_function"
        end
        @exit_context_function = cmd
    end
    
    def reset_exit_context_function
        @exit_subplot_context = nil
    end
    
    
    def eval_function(string)
        result = string
        if @eval_command == nil
            begin
                result = eval(string)
            rescue Exception => er
                report_error(er, nil)
            end
        else
            begin
                result = @eval_command.call(string)
            rescue Exception => er
                report_error(er, nil)
            end
        end
        return result
    end
    
    def def_figure(name, &cmd)
        name = name.split(' ').join('_') # replace blanks by underscores since names must be okay for shell
        if cmd == nil
            raise "Sorry: must provide a command block for def_figure"
        end
        if (num = @figure_names.index(name)) == nil
            @figure_names << name
            num = @figure_names.index(name)
            @num_figures = @figure_names.length
        end
        @figure_commands[num] = cmd
        return cmd
    end
    
    def make_portfolio_pdf(name=nil)
        ensure_safe_save_dir
        if !(name.kind_of?String)
            puts "Sorry: arg for make_portfolio must be a filename string"
            return
        end
        pdflatex = @which_pdflatex
        quiet=@quiet_mode
        begin
            make_portfolio(name)
            if (@save_dir == nil)
                syscmd = "#{pdflatex} -interaction nonstopmode #{name} > pdflatex.log"
            else
                syscmd = "cd #{@save_dir}; #{pdflatex} -interaction nonstopmode #{name} > pdflatex.log"
            end
            puts "#{syscmd}" unless quiet
            begin
                result = system(syscmd)
            rescue Exception => er
                report_error(er, "")
                result = false
            end
        rescue Exception => er
            puts "#{syscmd}" if quiet
            report_error(er, "ERROR: make_portfolio failed for #{name}")
            return false
        end
        return true
    end

    def figure_index(name)
        return @figure_names.index(name)
    end
    
    def create_figure(name)
        if name.kind_of?(Integer)
            num = name
            name = @figure_names[num]
        else
            num = @figure_names.index(name)
        end
        return false if num == nil
        cmd = @figure_commands[num]
        return false unless cmd.kind_of?(Proc)
        begin
            reset_legend_info
            result = private_make(name, cmd)
            return result
        rescue Exception => er
            report_error(er, "ERROR while executing command: #{cmd}")
        end
        return false
    end

    def make_figure(num)
        make_pdf(num)
    end

    def make_preview_pdf(num) # old name
        make_pdf(num)
    end
    
    def make_pdf(num)
        num = @figure_names.index(num) unless num.kind_of?(Integer)
        ensure_safe_save_dir
        run_directory = @run_dir; pdflatex = @which_pdflatex; quiet = @quiet_mode
        num = num.to_i
        num_figures = @figure_names.size
        num += num_figures if num < 0
        if ((num < 0) or (num >= num_figures))
            puts "Sorry: number must be between 0 and #{num_figures-1}"
            result = false
        else
            name = @figure_names[num]
            begin
                result = create_figure(num)
                name = get_save_filename(name) # must be done after making the figure to get correct model_number
            rescue Exception => er
                report_error(er, "ERROR: make failed for #{name}")
                result = false
            end
        end
        if result
            if (@save_dir == nil)
                syscmd = "#{pdflatex} -interaction nonstopmode #{name}.tex > pdflatex.log"
            else
                syscmd = "cd #{@save_dir}; #{pdflatex} -interaction nonstopmode #{name}.tex > pdflatex.log"
            end
            puts "#{syscmd}" unless (quiet)
            begin
                result = system(syscmd)
            rescue Exception => er
                report_error(er, "")
                result = false
            end
            if !result
                if (@save_dir == nil)
                    logname = "pdflatex.log"
                else
                    logname = "#{@save_dir}/pdflatex.log"
                end
                puts "ERROR: #{pdflatex} failed.  see #{logname} for details."
                file = File.open(logname)
                if file == nil
                    puts "cannot open #{logname}"
                else
                    reporting = false; linecount = 0
                    file.each_line do |line|
                        firstchar = line[0..0]
                        comparison = (firstchar <=> '!')
                        reporting = true if comparison == 0
                        if reporting
                            puts line
                            linecount = linecount + 1
                            break if linecount == @num_error_lines
                        end
                    end
                    file.close
                end
            end
        end
        if result
            pdfname = "#{name}"
            logname = "pdflatex.log"
            files = %w(.tex .out .aux .log _figure.pdf _figure.txt).map do |suffix|
              pdfname + suffix
            end
            files << logname
            files << "color_names.aux"
            if @save_dir # prepend directory specification
              files.map! do |f|
                "#{@save_dir}/#{f}"
              end
            end
            begin
              if @autocleanup
                files.each do |f|
                  begin
                    File.delete(f)
                  rescue
                  end
                end
              end
            end
            pdfname = "#{@save_dir}/#{pdfname}" if @save_dir != nil
            pdfname = "#{run_directory}/#{pdfname}" if run_directory != nil && pdfname[0..0] != '/'
            return pdfname + ".pdf"
        end
        return false
    end
    
    def make_portfolio(name)
        ensure_safe_save_dir
        if @save_dir != nil
            if @save_dir[-1..-1] != '/'
                fullname = @save_dir + '/' + name
            else
                fullname = @save_dir + name
            end
        else
                fullname = name
        end
        private_make_portfolio(name, fullname, @figure_names)
    end
    
    private

    # This function is de facto overwritten by the one after, I comment it
    # out.
    
#     def report_error(er, msg)
#         puts msg
#         puts ""
#         puts "    " + "#{er.message}"
#         line_count = 0
#         er.backtrace.each do |line|
#             if line_count < @num_error_lines
#                 puts "    " + line
#             end
#             line_count = line_count + 1
#         end
#         puts "ERROR"  # GUI uses this
#     end

    def internal_show_image(dict, is_mask)
        check_dict(dict, @@keys_for_show_image, 'show_image')
        ll = dict['ll']; lr = dict['lr']; ul = dict['ul']
        w = alt_names(dict, 'w', 'width')
        h = alt_names(dict, 'h', 'height')
        opacity_mask = alt_names(dict, 'opacity_mask', 'stencil_mask')
        if opacity_mask != nil
            mask_xo_num = internal_show_image(opacity_mask, true)
        else
            mask_xo_num = 0
        end
        filename = alt_names(dict, 'jpg', 'JPG')
        filename = dict['jpeg'] if filename == nil
        filename = dict['JPEG'] if filename == nil
        if filename != nil
            return private_show_jpg(filename, w, h, [ll[0], ll[1], lr[0], lr[1], ul[0], ul[1]], mask_xo_num)
        end
        interpolate = get_if_given_else_default(dict, 'interpolate', true)
        data = dict['data']
        value_mask = dict['value_mask']
        color_space = alt_names(dict, 'color_space', 'colormap')
        color_space = dict['color_map'] if color_space == nil
        if color_space == nil
            raise "Sorry: must specify 'color_space' for the image"
        end
        if color_space == 'MONO' || color_space == 'mono'
            raise "Sorry: monochrome image must not itself have a mask" unless mask_xo_num == 0
            reversed = get_if_given_else_default(dict, 'reversed', false)
            xo_obj = private_show_monochrome_image(ll[0], ll[1], lr[0], lr[1], ul[0], ul[1], 
                            interpolate, reversed, w, h, data, ((is_mask)? -1 : 0))
            return is_mask ? xo_obj : self
        end
        if color_space == 'GRAY' || color_space == 'gray' || color_space == 'GREY' || color_space == 'grey'
            if is_mask
                raise("Sorry: mask must not itself have a mask") unless mask_xo_num == 0
                mask_xo_num = -1
            end
            xo_obj = private_show_grayscale_image(ll[0], ll[1], lr[0], lr[1], ul[0], ul[1],
                            interpolate, w, h, data, mask_xo_num)
            return is_mask ? xo_obj : self
        end
        if is_mask
            raise "Sorry: mask image must have 'color_space' set to 'gray' or 'mono'"
        end
        if color_space == 'RGB' || color_space == 'rgb'
            private_show_rgb_image(ll[0], ll[1], lr[0], lr[1], ul[0], ul[1], interpolate, w, h, data, mask_xo_num)
            return self
        end
        if color_space == 'CMYK' || color_space == 'cmyk'
            private_show_cmyk_image(ll[0], ll[1], lr[0], lr[1], ul[0], ul[1], interpolate, w, h, data, mask_xo_num)
            return self
        end
        if value_mask == nil
            value_mask_min = value_mask_max = 256
        elsif value_mask.kind_of?Integer
            value_mask_min = value_mask_max = value_mask
        else
            value_mask_min = value_mask[0];
            value_mask_max = value_mask[1];
        end
        private_show_image(
                    ll[0], ll[1], lr[0], lr[1], ul[0], ul[1], interpolate, 
                    w, h, data, value_mask_min, value_mask_max, color_space[0], color_space[1], mask_xo_num)
        return self
    end
    
    def report_error(er, msg)
        if msg != nil
            puts msg
            puts ""
        end
        puts "    " + "#{er.message}" + "  [version: " + FigureMaker.version + "]"
        line_count = 0
        er.backtrace.each do |line|
            if line_count < @num_error_lines
                puts "    " + line
            end
            line_count = line_count + 1
        end
    end
    

    def make_page(cmd)  # the C implementation uses this to call figure command
        entry_function = @enter_page_function
        exit_function = @exit_page_function
        unless entry_function == nil 
            begin
                entry_result = entry_function.call
            rescue Exception => er
                report_error(er, nil)
            end
        end       
        result = do_cmd(cmd)       
        unless result == false or exit_function == nil 
            begin
                exit_result = exit_function.call
            rescue Exception => er
                report_error(er, nil)
            end
        end       
        return result
    end


    def do_cmd(cmd)  # the C implementation uses this to call Ruby commands
        begin
            cmd.call
            return true
        rescue Exception => er
            report_error(er, nil)
        end
        return false
    end
    
    def check_dict(dict,names,str)
        dict.each_key do |name|
            if names[name] == nil
                raise "Sorry: Invalid dictionary key for #{str} (#{name})."
            end
        end
    end
    
    def set_if_given(name, dict)
        val = dict[name]
        return if val == nil
        eval "self." + name + " = val"
    end
        
    def alt_names(dict, name1, name2)
        val = dict[name1]
        val = dict[name2] if val == nil
        return val
    end
        
    def get_if_given_else_use_default_dict(dict, name, default_dict)
        if dict != nil
            val = dict[name]
            return val if val != nil
        end
        val = default_dict[name]
        if val == nil
            raise "Sorry: failed to find value for '#{name}' in the defaults dictionary."
        end
        return val
    end
    
    def get_if_given_else_default(dict, name, default)
        return default if dict == nil
        val = dict[name]
        return val if val != nil
        return default
    end
    
    def complain_if_missing_numeric_arg(dict, name, alt_name, who_called)
        val = dict[name]
        val = dict[alt_name] if val == nil
        if val == nil
            raise "Sorry: Must supply '#{name}' in call to '#{who_called}'"
        end
        if !(val.kind_of?Numeric)
            raise "Sorry: Must supply numeric value for '#{name}' in call to '#{who_called}'"
        end
        return val
    end
    
    def check_pair(ary, name, who_called)
        return false if ary == nil
        if !(ary.kind_of?Array) and ary.size == 2
            raise "Sorry: '#{name}' must be array [x,y] for #{who_called}."
        end
        return true
    end
    
    def get_dvec(dict, name, who_called)
        val = dict[name]
        if val == nil || !(val.kind_of? Dvector)
            raise "Sorry: '#{name}' must be a Dvector for '#{who_called}'"
        end
        return val
    end

    # We make sure that save_dir exists and is a directory, creating it
    # if necessary.
    def ensure_safe_save_dir
      if @save_dir
        if File.exists?(@save_dir) 
          raise "save_dir (#{@save_dir}) exists and is not a directory" unless File.directory?(@save_dir)
        else
          # we create the directory if possible
          if @create_save_dir
            Dir.mkdir @save_dir
          else
            raise "save_dir (#{@save_dir}) doesn't exist " +
              " and I was told not to create it"
          end
        end
      end
    end
    
end # class FigureMaker
end # module Tioga
