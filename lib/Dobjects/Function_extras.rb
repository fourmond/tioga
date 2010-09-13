#    Function_extras.rb
# 
#    Some extra functions that are much more compact and hardly any slower in
#    pure Ruby
# 
#    Copyright (C) 2006  Vincent Fourmond
# 
#    This program is free software; you can redistribute it and/or modify
#    it under the terms of the GNU General Library Public License as published
#    by the Free Software Foundation; either version 2 of the License, or
#    (at your option) any later version.
# 
#    This program is distributed in the hope that it will be useful,
#    but WITHOUT ANY WARRANTY; without even the implied warranty of
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#    GNU Library General Public License for more details.
# 
#    You should have received a copy of the GNU Library General Public License
#    along with this program; if not, write to the Free Software
#    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
   

module Dobjects
  class Function

    # Returns [xmin, ymin, xmax, ymax]
    def bounds
      xmin,xmax = x.bounds
      ymin,ymax = y.bounds
      return [xmin, ymin, xmax, ymax]
    end

    # Returns the point where Y is the minimum
    def min
      return point(y.where_min)
    end

    # Returns the point where Y is the maximum
    def max
      return point(y.where_max)
    end

  end
end
