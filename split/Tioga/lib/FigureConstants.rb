=begin
   Copyright (C) 2005  Bill Paxton

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

# FigureConstants.rb

module Tioga

require 'Tioga/MarkerConstants.rb'
require 'Tioga/ColorConstants.rb'

# These constants are for use in making figures and plots with tioga.
#
# They cover the choices for justification, vertical alignment, frame sides,
# axis types, line types, stroke caps, stroke joins, and rendering modes.

module FigureConstants

    include Math
    include MarkerConstants
    include ColorConstants

    LEFT_JUSTIFIED = -1
    CENTERED = 0
    RIGHT_JUSTIFIED = 1
    
    ALIGNED_AT_TOP = 0
    ALIGNED_AT_MIDHEIGHT = 1
    ALIGNED_AT_BASELINE = 2
    ALIGNED_AT_BOTTOM = 3

    LEFT = LEFT_JUSTIFIED # LEFT == LEFT_JUSTIFIED so Bill doesn't get confused!
    RIGHT = RIGHT_JUSTIFIED # similarly, make RIGHT == RIGHT_JUSTIFIED
    TOP = 2
    BOTTOM = 3

    AT_X_ORIGIN = 4
    AT_Y_ORIGIN = 5
    
    AXIS_HIDDEN = 0
    AXIS_LINE_ONLY = 1
    AXIS_WITH_MAJOR_TICKS_ONLY = 2
    AXIS_WITH_TICKS_ONLY = 3
    AXIS_WITH_MAJOR_TICKS_AND_NUMERIC_LABELS = 4
    AXIS_WITH_TICKS_AND_NUMERIC_LABELS = 5


    EDGE_HIDDEN = 0
    EDGE_LINE_ONLY = 1
    EDGE_WITH_MAJOR_TICKS_ONLY = 2
    EDGE_WITH_TICKS = 3
    

    Line_Type_Solid = [[], 0]
    Line_Type_Dot = [[1, 2], 0]
    Line_Type_Dots = Line_Type_Dot
    Line_Type_Dash =  [[4, 2], 0]
    Line_Type_Short_Dash = Line_Type_Dash
    Line_Type_Dashes = Line_Type_Dash
    Line_Type_Short_Dashes = Line_Type_Dash
    
    LINE_TYPE_SOLID = Line_Type_Solid
    LINE_TYPE_DOT = Line_Type_Dot
    LINE_TYPE_DOTS = Line_Type_Dots
    LINE_TYPE_DASH = Line_Type_Dash
    LINE_TYPE_SHORT_DASH = Line_Type_Short_Dash
    LINE_TYPE_DASHES = Line_Type_Dashes
    LINE_TYPE_SHORT_DASHES = Line_Type_Short_Dashes
    
    
    Line_Type_Long_Dash = [[6, 2], 0]
    Line_Type_Long_Dashes = Line_Type_Long_Dash
    Line_Type_Dot_Dash = [[1, 2, 4, 2], 0]
    Line_Type_Dot_Short_Dash = Line_Type_Dot_Dash
    Line_Type_Dot_Long_Dash = [[1, 2, 6, 2], 0]
    Line_Type_Short_Dash_Long_Dash = [[4, 2, 6, 2], 0]
    
    LINE_TYPE_LONG_DASH = Line_Type_Long_Dash
    LINE_TYPE_LONG_DASHES = Line_Type_Long_Dashes
    LINE_TYPE_DOT_DASH = Line_Type_Dot_Dash
    LINE_TYPE_DOT_SHORT_DASH = Line_Type_Dot_Short_Dash
    LINE_TYPE_DOT_LONG_DASH = Line_Type_Dot_Long_Dash
    LINE_TYPE_SHORT_DASH_LONG_DASH = Line_Type_Short_Dash_Long_Dash

    LINE_CAP_BUTT = 0
    LINE_CAP_ROUND = 1
    LINE_CAP_SQUARE = 2

    LINE_JOIN_MITER = 0
    LINE_JOIN_ROUND = 1
    LINE_JOIN_BEVEL = 2

    FILL = 0
    STROKE = 1
    FILL_AND_STROKE = 2
    DISCARD = 3
    FILL_AND_CLIP = 4
    STROKE_AND_CLIP = 5
    FILL_STROKE_AND_CLIP = 6
    CLIP = 7

    DEGREES_PER_RADIAN = 180.0/PI
    RADIANS_PER_DEGREE = PI/180.0

end # module FigureConstants

end # module Tioga


