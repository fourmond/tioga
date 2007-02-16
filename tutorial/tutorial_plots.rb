
module Tioga
module Tutorial

=begin rdoc

= The extras for making plots

As usual, I'll be assuming you've been working your way through the sections of the tutorial in order,
so by the time you get here, you're familiar with the basic structure of a tioga document and the
fundamentals of making figures with FigureMaker.  This section builds on that knowledge to add the tools needed for
making figures that are plots.  We'll use "plots.rb" as a source of examples. 

You'll find "plots.rb" file in the "samples/plots" folder.  Make a portfolio by entering

  tioga plots -p

Take a look to see what's there.

As you can see, the examples range from the simple ("Blues") to the complex ("Contours").  Along the way,
they illustrate ways of combining plots ("Side_by_Side", "Two_Ys", "Rows", "Columns", etc.), and
provide samples of various techniques for adding information to plots ("Legend_Inside", "Legend_Outside",
"Labels", "Error_Bars", "Arrows", and "Special_Y").  Finally, there are several cases where tioga includes
tools for creating things to be shown in the plots ("Sampled_Splines", "Steps", "Splines", and
"Sampled_Data").  

We'll go through some of the cases in some detail, and then let you pick and choose from the rest.  Rather than
attempting to wade through it all now, you might prefer to skim quickly to see what's there,
and come back later if you want to look at a particular piece of sample code for ideas.

---

link:images/blues.png

Let's start by loading "plots.rb" into a text editor and finding the definition for "Blues".  It looks
like this:

    def blues
        read_data
        t.do_box_labels('Blues Plot', 'Position',
            '\textcolor[rgb]{0,0,1}{Blues}')
        show_model_number
        xs = @positions
        ys = @blues
        t.show_plot(plot_boundaries(xs,ys,@margin,-1,1)) { 
            t.show_polyline(xs,ys,Blue) }
    end

The read_data routine is part of "plots.rb" and reads data -- big surprise -- and is discussed in a later section (Animation).
The do_box_labels method from the FigureMaker puts up the title, the xlabel, and the ylabel. Our own show_model_number
routine puts the model number in the upper right corner of the plot.   The read_data routine put the current Dvector values for the x positions and the corresponding y values for "blues" in local instance variables for us, and we're just copying them to local variables, "xs" and "ys", for convenience.  The actual creation of the
contents of the plot happens inside the call to show_plot.  The show_plot routine takes one argument, the array of
boundaries, and a block of code to do the work of creating the contents of the plot frame.

We'll go through show_plot in detail, but first a few words about frames and boundaries and coordinate systems.  Here's 
what it says in the introduction to the CoordinateConversions module.

[]  There are four different coordinate systems used in tioga.  The contents of the figure or plot
    are positioned using "figure" coordinates which correspond to the values along the x and y axes
    as determined by the boundary attributes.  Things like the title and axis labels are positioned using
    "frame" coordinates that run from (0, 0) at the lower left corner of the frame to (1, 1) at the upper right
    corner.  The frame is positioned on the page using "page" coordinates that run from (0, 0)
    at the lower left of the page to (1, 1) at the upper right.  The actual numbers used in PDF and TeX files
    are given in "output" coordinates which are simply scaled-up page coordinates (so that locations
    can be represented using small integers rather than floats).

In our case, the figure coordinates come from the "xs" and "ys" that were read from the data source.
The values of the coordinates at the edges of the frame are referred to as the "boundaries" of the plot,
and they get passed to show_plot in an array that is created by the plot_boundaries routine defined in "plots.rb".
Here's what it does.

     def plot_boundaries(xs, ys, margin, ymin=nil, ymax=nil)
  1      xmin = xs.min
  2      xmax = xs.max
  3      ymin = ys.min if ymin == nil
  4      ymax = ys.max if ymax == nil
  5      width = (xmax == xmin)? 1 : xmax - xmin
  6      height = (ymax == ymin)? 1 : ymax - ymin
  7      left_boundary = xmin - margin * width
  8      right_boundary = xmax + margin * width
  9      top_boundary = ymax + margin * height
 10      bottom_boundary = ymin - margin * height
 11      return [ left_boundary, right_boundary,
                  top_boundary, bottom_boundary ]
     end
    
The "xs" and "ys" given as arguments are Dvectors, and in lines 1 and 2 the Dvector min and max
methods are used to find the range of values covered by the current xs.  Lines 3 and 4 do the
same for the ys, but only if the values for ymin and ymax were not provided by the caller.
The "if ymin == nil" at the end of line 3 is an example of using a conditional qualifier at 
the end of statement.  In addition to "if <test>", you can say "unless <some other test>".
The notation "ymin=nil" in the argument list specifies that "ymin" is optional and "nil" is
its default value when the caller omits it.
In our case, we specified -1 for ymin and +1 for ymax, so those values will be used.
Lines 5 and 6 calculate the width and height ranges from the min and max values, and check
for the nasty case of min equal max.  The "?" notation here comes from C.  You can also write this as

    if (xmax == xmin)
        width = 1
    else
        width = xmax - xmin
    end

The "margin" argument to plot_boundaries lets the caller specify how much
room to leave on the edges around the data.  In our case, we passed the local attribute "@margin"
which our initialization routine set to 0.1.  The given margin and the calculated width and height
are used to determine the boundaries for the plot frame on lines 7 to 10.  These are packed into an array
and returned on line 11.

Now that we have the boundaries, let's look at what show_plot does with them and with the block of
plot commands that follows the argument list.  Here's our call on show_plot from blues:

        t.show_plot(plot_boundaries(xs,ys,@margin,-1,1)) { 
            t.show_polyline(xs,ys,Blue) }

And here's what it would look like if we skipped show_plot and called the next level of
routines instead (this is all that show_plot does after all):

  1      t.set_bounds(plot_boundaries(xs,ys,@margin,-1,1))
  2      t.context do
  3          t.clip_to_frame
  4          t.show_polyline(xs,ys,Blue)
  5      end
  6      t.show_plot_box

The set_bounds routine takes the boundary values we just calculated and sets a bunch of figure attributes
accordingly, things like xaxis_reversed, yaxis_reversed, default_text_height_dx, and default_text_height_dy.
Next the #context routine establishes a save/restore context for the code that will do the actual creation
of the plot contents.  It saves the current values of lots of figure attributes, yields
control to the block of code, and then restores values when the code finishes.  The first thing that
happens in the code is a call on clip_to_frame that adjusts the clipping region by intersecting the
current clipping region with the new frame.  Then our plotting code happens (line 4).
Because we're inside a "context", we can set the stroke_color to Blue and not have to
worry about a possible side-effect on a caller who had previously set the stroke_color to something else.
When we are done, #context will restore the old stroke_color for us.
Finally, after #context finishes, the show_plot_box routine is called to add the axes, labels, and such.
The call on show_plot simply packages all of that in a neat bundle for us.  (And it is a lovely thing that
Ruby makes it all possible with code blocks!)

Our simple little "blue" example shows the basic form that's used even in complex plots: get the data,
figure out the boundaries, invoke show_plot, and do the plotting calls inside the code block that follows
the show_plot argument list.  

---

Now we"ll take a quick look at some of the ways to combine subplots.  The key here is to be able to
take existing plot definitions that were written as complete, stand-alone plots, and embed them
without modification as subplots in a more complex configuration.  Here's an example.  We have
plots for "Reds" and "Greens" separately.  They look like this:

link:images/reds.png

link:images/greens.png

We might want to combine them "side-by-side" like this:

link:images/red_green_side_side.png

Here's the side-by-side code:

      def side_by_side
  1       read_data
  2       t.landscape
  3       t.do_box_labels('Side by Side', 'Position', nil)
  4       t.subplot('right_margin' => 0.5) { 
  5           t.yaxis_loc = t.ylabel_side = LEFT;
  6           t.right_edge_type = AXIS_LINE_ONLY; reds }
  7       t.subplot('left_margin' => 0.5) {
  8           t.yaxis_loc = t.ylabel_side = RIGHT;
  9           t.left_edge_type = AXIS_LINE_ONLY; greens }
 10       show_model_number
      end
    
We start on line 1 with the usual call to read the data.  On line 2, we change the aspect ratio
to landscape, and on line 3 we specify a title and an xlabel, leaving the ylabel "nil" for now.
Then come the #subplot calls. 
The first call to #subplot sets a right margin of 0.5 which means that the "reds" plot will go
in the left 50% of our frame.  Similarly, the second call sets a left margin of 0.5 meaning that
the "greens" plot will go in the right 50%.  The code block following the argument list
for each #subplot contains the commands to be executed.  These commands are carried out inside a call
on #context, so any changes they make to the state will be restored before subplot returns.
In the first, on lines 5 and 6, we specify that the y axis and ylabel will be on the left, and the
right edge will be displayed as a line only.  Then we simply call the "reds" plot routine we
defined previously, the same one that makes the "Reds" plot.  On lines 8 and 9, we move the
y axis to the other side and call the same "greens" routine that makes the "Greens" plot for us.
Finally, when the subplots are done, we call our show_model_number routine to put the model number
in the upper right corner.

---

Since the "Reds" and the "Greens" have the same x axis, we might also combine them in a single plot
with different y axes on the left and right -- like this:

link:images/two_ys_plot.png

And here's the corresponding code from "plots.rb":

    def two_yaxes
  1      read_data
  2      t.landscape
  3      t.do_box_labels('Same X, Different Y\'s', 'Position', nil)
  6      t.subplot { 
  7          t.yaxis_loc = t.ylabel_side = LEFT;
  8          t.right_edge_type = AXIS_HIDDEN; reds }
  9      t.subplot {
 10          t.yaxis_loc = t.ylabel_side = RIGHT;
 11          t.left_edge_type = AXIS_HIDDEN; greens }
 12      show_model_number
    end

The only changes from side-by-side are to omit the "margin" arguments to subplot
so that the plots will use the same frame, and "AXIS_HIDDEN" has replaced "AXIS_LINE_ONLY".

---

When there are several plots with the same x axis, a stack of rows is a common choice:

link:images/rgb_stack.png

This is the "rows" routine from "plots.rb" that does this one.

    def rows
  1      read_data
  2      t.landscape
  3      show_model_number
  4      t.do_box_labels('Blues, Reds, Greens', 'Position', nil)
  5      t.rescale(0.8)
  6      num_plots = 3
  7      t.subplot(t.row_margins('num_rows' => num_plots, 'row' => 1)) do
  8          t.xaxis_type = AXIS_WITH_TICKS_ONLY
  9          blues
 10      end
 11      t.subplot(t.row_margins('num_rows' => num_plots, 'row' => 2)) do 
 12          t.xaxis_type = AXIS_WITH_TICKS_ONLY
 13          t.top_edge_type = AXIS_HIDDEN
 14          reds
 15      end
 16      t.subplot(t.row_margins('num_rows' => num_plots, 'row' => 3)) do 
 17          t.top_edge_type = AXIS_HIDDEN
 18          greens
 19      end
    end
    
There are a few new features here.  On line 5, we do a #rescale to 80%. This changes the
scale of text and lines to 80% of their previous height and width.  Text and line widths
don't scale 1:1 with graphics in tioga.  When you shrink a plot to use it as a subplot, the
graphics might get smaller by 50%, but the text might only get smaller by 80%.  Another new
feature is the use of the row_margins routine, on lines 7, 11, and 16, to calculate the subplot margins for us.
In the various code blocks, we adjust the appearance of the horizontal axes so that the
numbers only show up on the bottom (lines 8, 12, 13, and 17).   After making those arrangements,
we can just call the existing routines to do the "blues", "reds", and "greens" plots (lines 9, 14, and 18).

---

In the previous cases, we've combined plots that each show a single set of data.  For the case of
several lines of data in a single plot, we'll want to add a legend saying what data goes with what
line style.  There are built-in commands to collect the information, and other commands for adding it to
the plot.  Here's a simple example, one version with the legend inside the frame, and another with
the legend on the right.

link:images/Legends.png

The routines for the two versions are naturally quite similar:

    def legend_outside
        read_data
        show_model_number
        t.show_plot_with_legend('legend_scale' => 1.3) { reds_blues }
    end
    
    def legend_inside
        read_data
        show_model_number
        t.show_plot_with_legend(
            'legend_scale' => 1.3,
            'legend_left_margin' => 0.7,
            'plot_right_margin' => 0) { reds_blues }
    end
    
The show_plot_with_legend routine does the work for us.
It takes a dictionary argument specifying the layout for the main plot and the
legend.  After reseting the legend information, it calls #subplot for the main
plot, then does a set_subframe for the legend, and calls show_legend to do the
actually plotting of the legend text and lines.  Defaults for the layout are stored
in the FigureMaker's dictionary, legend_defaults, that you can change as desired.  For our "legend_outside"
example, we just use the defaults.  For the "legend_inside" case, we override the
default for the left margin of the legend to place it 70% of the distance along the frame
width.  In both cases, we're calling the "reds_blues" routine to make the plot.  Here's what it does.

    def reds_blues
        read_data
        t.do_box_labels("Reds and Blues", "Position", "Values")
        boundaries = setup_lines(@positions, [@blues, @reds], -1, 1)
        xs = @positions
        t.show_plot('boundaries' => boundaries) do
            t.show_polyline(xs,@blues,Blue,'Blues')
            t.show_polyline(xs,@reds,Red,'Reds')
        end
        show_model_number
    end
    
It is written like a standard plotting routine, and in fact could be used for that if we didn't want a legend.
The main new items are the calls on "save_legend_info" which tell the system
to save the given text along with the current line formating attributes for later
use by show_legend.  The show_legend routine uses various attributes defining the
layout of the legend information to show the saved text and make the sample lines.


== Plot Styles

You may have noticed the file plot_styles.rb in the samples/plots directory.  It has a couple of routines for setting
the tioga attributes that determine plot formats.  In plots.rb itself, at the top of the file, you'll find a line that does

  require 'plot_styles'
  
Open plot_styles.rb in your text edit and let's take a look inside.  It defines a module called MyPlotStyles, and if we
check the plots.rb file again, we'll find that it does 'include MyPlotStyles'.  That means that within the MyPlots class
in plots.rb we can directly use any methods defined in MyPlotStyles.  Turns out that there are currently two such methods
(although you may want to add more later).  The first is called sans_serif_style, and as the name suggests it changes
a few attributes so that TeX will use sans serif fonts.  The second method in MyPlotStyles is set_default_plot_style,
and it gives values to all of the tioga attributes relevant to figures and plots.

You may want to add special styles of your own -- if you make something that might be of interest to others, 
let us know.

There are several options for how style methods are used.  One would be to call a style method from the
initialization routine for your plot class (such as at the start of MyPlots initialize, just after setting
@figure_maker).  If you take that approach, you don't need to have your own 'enter_page' function since the
default will do just fine.  An alternative that I often use is to have an enter_page function and call the
style method from there.  This is the method implemented in plots.rb currently.  At the end of the initialization
method, there's a line that defines our enter_page_function:

        t.def_enter_page_function { enter_page }

And then we define out enter_page as follows:

    def enter_page
        set_default_plot_style
        t.default_enter_page_function
    end

A final way to use the style methods is to call them directly from a plot definition.  This is
illustrated by the method reds.  It begins with this line:

    sans_serif_style unless t.in_subplot

This is doing a conditional style change.  If we use the Reds plot by itself, it switches to
sans serif, but if we combine it as part of "super-plot", then it uses whatever has already
been set up.  I don't mean to suggest that you start having lots of conditional style changes;
the point here is simply that styles can be set in various ways at various stages of creating a plot.

In all of this it is important to be clear about the order in which things happen.  First is the initialization method
that is called when the `new' method is called for the class you've defined in your file.  Next is the enter_page
function that is called whenever a figure definition is about to be called to make a pdf.  Last of all is the actual
figure definition itself.  Style parameters can be changed at each of these stages -- for example, most figures may use
the style set during initialization, but some of them may make changes.  NOTE: any changes made by a figure routine
go away when the figure is done; all the settings revert back to the values they were given at initialization.
In our example above where the 'Reds' plot used sans serif, that choice was in effect only during the remainder of
the creation of that plot -- no "side-effects" of building the Reds plot will carry over to the next plot to be made.

---

Now let's review the tools for reading and manipulating the data for plots -- next stop: Data.


=end

module Plots
end # module Plots

end # module Tutorial
end # module Tioga

