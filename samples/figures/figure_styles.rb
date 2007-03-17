# figure_styles.rb

require 'Tioga/FigureMaker'

module MyFigureStyles
  
  include FigureConstants
  
  
  def sans_serif_style(t = FigureMaker.default)
    set_default_style
    t.tex_fontfamily = 'sfdefault'
  end
  
  
  def set_default_style(t = FigureMaker.default)
    
  # Page size and margins
    # these default values are used by the default_enter_page_function

    t.default_page_width = 72*4.25 # in big-points (1/72 inch)
    t.default_page_height = 72*4.25 # in big-points (1/72 inch)

    t.default_frame_left = 0.15 # as fraction of width from left edge
    t.default_frame_right = 0.85 # as fraction of width from left edge
    t.default_frame_top = 0.85 # as fraction of width from bottom edge
    t.default_frame_bottom = 0.15 # as fraction of width from bottom edge

  # Graphics
    t.default_line_scale = 1
    t.line_cap = LINE_CAP_ROUND
    t.line_join = LINE_JOIN_ROUND
    t.line_type = LINE_TYPE_SOLID
    t.line_width = 1.2
    t.miter_limit = 2
    t.stroke_opacity = 1
    t.fill_opacity = 1

  # Markers   
    t.marker_defaults = { 
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

  # TeX text
    t.tex_preamble = '% start of preamble.  
        \usepackage[dvipsnames,usenames]{color} % need this for text colors
    '

    t.tex_fontsize = '10.0'  
    t.tex_fontfamily = 'rmdefault'
    t.tex_fontseries = 'mddefault'
    t.tex_fontshape = 'updefault'

    t.alignment = ALIGNED_AT_BASELINE
    t.justification = CENTERED

    t.label_bottom_margin = 0
    t.label_left_margin = 0
    t.label_right_margin = 0
    t.label_top_margin = 0

    t.text_shift_from_x_origin = 1.8
    t.text_shift_from_y_origin = 2.0
    t.text_shift_on_bottom = 2.0
    t.text_shift_on_left = 1.8
    t.text_shift_on_right = 2.5
    t.text_shift_on_top = 0.7
    
  end
  
end
