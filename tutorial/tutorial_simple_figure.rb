
module Tioga
module Tutorial

=begin rdoc

= A first look at a nontrivial tioga figure definition

In the samples/figures directory, enter this to the shell to make a pdf 
show it in your viewer.

  tioga figures -s 'Curve'
  
This is what you should see.

link:images/append_curve.png

In the source file, the definition looks like this (with line numbers added for this tutorial only):

  1  def curve
  2      t.landscape
  3      t.show_text('text' => 'append\_curve\_to\_path',
            'side' => TOP, 'shift' => 0.6,
            'justification' => CENTERED,
            'scale' => 0.9, 'position' => 0.5)
  4      background
  5      x0 = 0.75; y0 = 0.9
  6      x1 = 0.9; y1 = 0.3
  7      x2 = 0.4; y2 = 0.1
  8      x3 = 0.1; y3 = 0.8
  9      t.move_to_point(x0, y0)
 10      t.append_curve_to_path(x1, y1, x2, y2, x3, y3)
 11      t.line_width = 2.5
 12      t.line_color = DarkBlue
 13      t.stroke
 14      t.show_marker(
             'xs' => [ x0, x1, x2, x3 ],
             'ys' => [ y0, y1, y2, y3 ],
             'marker' => Bullet,
             'scale' => 0.6,
             'color' => Red);
 15      dx = t.default_text_height_dx * 1.4
 16      t.show_label('x' => x0+dx, 'y' => y0,
             'text' => "start", 'scale' => 0.9);
 17      scale = 1.2; dy = t.default_text_height_dy * 0.8
 18      t.show_marker(
             'at' => [x1, y1+dy],
             'marker' => Circled1, 'scale' => scale);
 19      t.show_marker(
             'at' => [x2, y2+dy],
             'marker' => Circled2, 'scale' => scale);
 20      t.show_marker(
             'at' => [x3, y3+dy],
             'marker' => Circled3, 'scale' => scale);
 21  end

Everything that starts with "t." is directed at the FigureMaker.  The other things
are either local methods or local variables.  The code starts by changing the aspect
ratio to landscape.  Then it puts the text at the top using "show_text".  Notice the
backslashes before the underscores.  They are needed to keep TeX happy since the text
will be sent to TeX for typesetting.  The call on "background" invokes one of our own
methods to fill in the background and put a line around the frame.  The control points
for the Bezier curve are stored in local variables in lines 5 to 8, and the curve is
added to the graphics path in lines 9 and 10.  After setting the line width and color,
it is painted by "t.stroke" in line 13.  The first call to "show_marker" (line 14) takes care of
the four red bullets at the control points.  The text label is placed at point (x0,y0)
by the call on "t.show_label" in line 16.  Finally, the numbers inside circles are painted
by the calls on "t.show_marker" to finish things off (lines 18-20).

[What's a "marker"?  Visit MarkerConstants to find out and to see lots of examples.]

---

Let's change a few things and look at the results.  Start off by changing line 6 so that
y1 is 1.3 instead of 0.3 and remake the figure.   Point 1 jumps clean out of the frame!
Here's what I get:

link:images/bad_append_curve.png

Why didn't that point get clipped?  Because, unlike plots, figures don't automatically
clip to the current frame.  If we want clipping in a figure, we need to turn it on
explicitly.  Let's add "t.clip_to_frame" after the call on "background",
and try it again.  Now the "out-of-frame" stuff is gone.

link:images/append_curve_clipped.png

---

Change line 6 back so that y1 is 0.3 again.  Let's add lines connecting the control points.
Insert the following before the move_to_point call on line 9.

        t.append_points_to_path([ x0, x1, x2, x3 ], [ y0, y1, y2, y3 ])
        t.line_width = 1; t.line_color = LightBlue
        t.stroke


Redo the figure by doing this command again.

  tioga figures -s 'Curve'

Does the result look like this?

link:images/append_curve2.png

Okay, that's progress but the line now goes through the numbers
for points 1 and 2, so we'll need to move them a little.  Try adjusting the "at" positions in
the calls to show_marker for Circled1 and Circled2 until you get something that looks ok.
Here's what I ended up with after changing the positions this way:

        t.show_marker(
            'at' => [x1, y1-dy],        # instead of [x1, y1+dy]
            'marker' => Circled1, 'scale' => scale);
        t.show_marker(
            'at' => [x2-0.5*dx, y2],    # instead of [x2, y2+dy]
            'marker' => Circled2, 'scale' => scale);

link:images/append_curve3.png

---

Here's our little routine for doing the background that we're calling on line 4
of the curve method.

    def background
        clr = t.rgb_to_hls(Tan)
        clr[1] = 0.97
        t.fill_color = t.hls_to_rgb(clr)
        t.line_width = 2
        t.fill_and_stroke_frame
    end
    
To find out what rgb_to_hls and hls_to_rgb do, we can look at the reference material for the
FigureMaker class, or if you are viewing this page using frames and have lists of "Files",
"Classes", and "Methods" at the top of the window, you can search the list of methods until
you find what you are after.  We find from this that rgb_to_hls is converting a triple of
Red-Green-Blue intensities to a triple of Hue-Lightness-Saturation.  The argument we're
passing to rgb_to_hls, Tan, is a predefined color constant.  The full list of color names is
given in ColorConstants.  Searching there for "Tan" reveals that it is defined as [ 0.824, 0.705, 0.55 ].

To see the color names as colors rather than as vectors of numbers, visit the website
mentioned in the introduction to the ColorConstants.  Here it is again: 
{SVG samples website}[http://www.december.com/html/spec/colorsvgsvg.html].
If your browser has a SVG viewer (free from Adobe), you'll see color samples for all of the color names.
Let's pick a new color for the background.  We could use fuchsia.  That would wake people up.
Back in ColorConstants we find the definition Fuchsia = [ 1, 0, 1 ], so fuchsia is just full red and blue
with no green -- i.e., it is a synonym for Magenta.  Let's edit background by replacing "Tan" with
"Fuchsia".

link:images/background.png

Maybe that wasn't such a great idea after all.

As a last comment, notice that in defining the background color, I haven't just used "Tan" as it
comes from the factory.  I've made a new color that might be called "ReallyLightTan" by first
converting standard "Tan" from RGB to HLS, changing the lightness to a number close to 1,
and then converting back to RGB.  You can of course play similar games with the saturation.
If you create a color you like, just save the RGB triple for later use.  


---

Now let's look at how tioga uses TeX to do text -- next stop: TextForTeX.




=end

module SimpleFigure
end # module SimpleFigure

end # module Tutorial
end # module Tioga

