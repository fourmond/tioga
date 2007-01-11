# sine.rb : minimal example of plotting with Tioga

# This file is copyright Vincent Fourmond, 2005
# You are free to use this file for whatever you find it fit to.

# this file demonstates the use of tioga for producing plots. 

require 'Tioga/FigureMaker'

class MyPlots
  
  include Math
  include Tioga
  include FigureConstants
  
  attr_reader :t

  
  def initialize (outdir = nil)
    @t = FigureMaker.default

    # we will store in @data both X and Y columns
    @data = []

    @margin = 0.05

    # we define the plot: 
    @plot = t.def_figure("sine") { exec_plot }
    
  end


  # in make_data, we create the data set to be plotted
  def make_data
    xAxis = Dvector.new(500) {|i| 2*PI*i/100.0 }
    
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

  # this is the function called when the block need to be executed
  def exec_plot
    make_data
    t.do_box_labels('Curve $y = \sin x$', '$x$', '$y$')
    xs = @data[0]
    ys = @data[1]
    t.show_plot(plot_boundaries(xs,ys,@margin)) {
      # plot lines
      t.show_polyline(xs,ys,Red)
      # plot markers
      t.show_marker('Xs' => xs, 'Ys' => ys,
                    'marker' => Asterisk, 'scale' => 0.5, 
                    'color' => Blue)
    }
  end

  def run
    t.make_preview_pdf("Plot")
  end
end

p = MyPlots.new($*[0])
p.run



