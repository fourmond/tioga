
module Tioga
module Tutorial

=begin rdoc

= Tioga's graphical user interface for the Mac

Tioga comes with a GUI for the Mac that makes use of the fact that OS/X "talks PDF" as a native
language.  (Anyone who wants to do a front-end for Linux has my blessings and is welcome
to take anything of use from the Mac code.)  The rest of this
section is for those Mac users who'd like to start using the GUI rather than the command line
interface in irb.  

The Mac front-end is simply called 'Tioga'.  Installation should be a trivial drag-and-drop
of the icon from the disk image to the folder where you keep applications.  Double-click
the icon now to launch Tioga.  We'll use the "figures.rb" file to illustrate, so open it
using the "Open" command in the "File" menu (recall that the file we want is located in
the samples/figures subfolder of the FigureMaker folder).  You should get a window that
looks similar to this:

http://theory.kitp.ucsb.edu/~paxton/tioga_jpegs/startup.jpg

If instead you got something like the following alert, you'll need to tinker with the
Ruby installation or the Tioga preferences so that Tioga can find the right
Ruby to launch.  (When it first starts up, Tioga checks to see if there is a file
named "/usr/local/bin/ruby".  If so, that is used as the default.  Otherwise,
it uses "/usr/bin/ruby" as the default.)

http://theory.kitp.ucsb.edu/~paxton/tioga_jpegs/ruby_alert.jpg

If the window comes up blank, check the log at the bottom.  It may have a message
like this, indicating that Tioga was able to launch Ruby but was unable to
load the FigureMaker extension.

http://theory.kitp.ucsb.edu/~paxton/tioga_jpegs/tioga_trouble.jpg

A blank window might also mean that Tioga couldn't find pdflatex.  In that case the message will look something like this.

http://theory.kitp.ucsb.edu/~paxton/tioga_jpegs/pdflatexerror.jpg

In this case, open the Tioga Preferences panel and fill in the correct path to the pdflatex you want to use.

http://theory.kitp.ucsb.edu/~paxton/tioga_jpegs/preferences.jpg

= Eval is more than 2+2

At the bottom of the window is a text field and a button labeled "Eval".  Type an expression in the field
and hit Eval to get the value printed in the Log drawer below the window.  You type 2+2, and it prints 4.

The string in the text field is being sent to Ruby for evaluation, so you can enter anything Ruby understands.
That includes math (type sin(log(PI)), get 0.910598499212615), system commands (type system('date'), get the
current date and time), and printing (type puts 'hello', get a greeting).  

But Eval is more than 2+2.  You can arrange to have the string sent to your code rather than going
directly to the Ruby eval method (see def_eval_function in FigureMaker).  That opens up the means
to interact with your code in ways that go beyond the standard set of front-end commands.  Just add the
following to the initialize routine:

    t.def_eval_function { |str| eval(str) }

Here's an example.  In my "star history" plotting code, there are parameters for deciding which section of history to show
(basically, starting and ending ages for the plots).   The code makes default decisions for these, but
there are times when I'd like to zoom in on a particular era.  I can do that by using Eval with a
private eval_function in my program.  Let's say I want to change the starting age for the plots.
Since I wrote the code, I know that the start age is determined by an attribute called "@track_start_param".
I'm looking at the plot in Tioga, and now I want to change the start.  First, I put

    @track_start_param

in the Eval text field and hit Return (which has the same effect as clicking the Eval button).
I get back that the current value is 0.1, and I'd like to make it bigger.  So, I eval this:

    @track_start_param = 0.8; puts "track_start_param is #{@track_start_param}"

which prints out "track_start_param is 0.8" in the log.  Now a Refresh command will use the new setting.
Recall that Refresh empties the front-end's cache so that the plots will be remade, but it doesn't cause the
program to be reloaded in Ruby the way the Reload command does.  That's important here, since a
Reload would restore the default setting for the starting age, while Refresh will preserve our change.

= Hints and Details

That's probably all of this section that you'll bother to read if you are a typical Mac user.
Things should pretty much work the way you'd expect, and what's not immediately obvious
you can quickly learn by poking buttons to see what happens.  However, here are a few items
that might take you a bit longer to discover on your own (as a reward to those of you
who are still reading this!).

-- You can drag-and-drop a Tioga file onto the icon to open it.   

-- Use the up and down arrow keys to move up and down in the list of figures.

-- Use the left and right arrow keys to move back and forward in the history list.

-- When you are scaled up so that scroll bars are showing, you can scroll by dragging
with the grab-hand cursor.

-- The alternative to the grab-hand is a cross-hair cursor.  Click on the cross-hair at the
bottom of the window to change the cursor mode.  The horizontal and vertical lines that
track the mouse-down location are especially useful with eyeballing values on plots.
The lines stay in place even when you rescale the image or scroll it.  When you are in
this mouse mode, you can still use grab-hand dragging by holding down the command
key along with the mouse down to drag.  The cross-lines are removed when you change
back to drag-mode for the mouse by clicking on the grab-hand button at the bottom of the
window.

-- Rescaling can be done conveniently using command-1 though command-5.

-- During the look-edit-reload-repeat development cycle, use command-r to reload.  It automatically refreshes the current figure.

-- You can change the font size for the log in the Preferences.  You can also select and copy
text from the log, but it isn't editable.

-- The command buttons have pop-up help that appears when the cursor is over them for more than a few seconds.
Many of the menu items behave similarly.

-- You can have several different documents open simultaneously.  Each one has its own Ruby subtask,
so they don't interfere with each other.

-- The Print command prints what's currently showing in the window -- clipping, 
scaling, cross-hair lines,
and all.  If you just want to print the pdf file for the figure, you don't need Tioga to do it
for you!

-- Hold down the right mouse button with the cursor over the figure to bring up a menu for controlling
slide shows and auto refresh.  Change the slide show timing in Preferences.  Use auto-refresh
with dynamically updating figures (more on that in the following discussion of Animation).

=end

module MacGUI
end # module MacGUI

end # module Tutorial
end # module Tioga

