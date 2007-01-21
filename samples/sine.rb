# sine.rb : minimal example of plotting with Tioga

# Original of this file is copyright Vincent Fourmond, 2005
# Various edits made by Bill Paxton, 2007 for use in Tutorial.

# You are free to use this file for whatever you find it fit to.

# this file demonstates the use of tioga for producing plots. 

require 'Tioga/FigureMaker'

class MyPlots
  
  include Math
  include Tioga
  include FigureConstants
  
  attr_reader :t  # t is to talk to tioga

  # initialize is called when we do MyPlots.new
  def initialize
    @t = FigureMaker.default
    # we will store in @data both X and Y columns
    @data = []
    @margin = 0.05
    # we define the plot: 
    @plot = t.def_figure("sine") { exec_plot }    
  end


  # in make_data, we create the data set to be plotted
  def make_data
    num = 35
    xAxis = Dvector.new(num) {|i| 2*PI*i/(num-1) }    
    yAxis = xAxis.sin
    @data=[xAxis,yAxis]
  end


  # function for automatically computing graph boundaries
  def plot_boundaries(xs,ys,margin,xmin=nil,xmax=nil,ymin=nil,ymax=nil)
    xmin = xs.min if xmin == nil
    xmax = xs.max if xmax == nil
    ymin = ys.min if ymin == nil
    ymax = ys.max if ymax == nil

    width = (xmax == xmin) ? 1 : xmax - xmin
    height = (ymax == ymin) ? 1 : ymax - ymin

    left_boundary = xmin - margin * width
    right_boundary = xmax + margin * width

    top_boundary = ymax + margin * height
    bottom_boundary = ymin - margin * height

    return [ left_boundary, right_boundary, top_boundary, bottom_boundary ]
  end


  # this is called when the figure needs to be created
  def exec_plot
    make_data
    t.do_box_labels('Curve $y = \sin x$', '$x$', '$y$')
    xs = @data[0]; ys = @data[1]
    t.show_plot(plot_boundaries(xs,ys,@margin)) {
      t.show_polyline(xs,ys,Blue) # plot lines
      if false # plot markers
        t.show_marker('Xs' => xs, 'Ys' => ys,
                    'marker' => Asterisk,
                    'scale' => 1.2, 
                    'color' => Blue)
      end
    }
  end

  
end


MyPlots.new



