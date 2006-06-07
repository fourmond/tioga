
module Tioga
module Tutorial

=begin rdoc

= How to create a sequence of PDFs and convert it to a movie

If is often useful to create a series of plots showing the changes that occur as some
parameter changes.  For example, in my stellar evolution work, the parameter is time
and the plots are things like the profiles of element abundances from the center of the star
to the surface.  The stellar evolution simulation cranks out a series of models for
increasing ages, and for each model, or each 10th model perhaps, I may want to
save a plot showing the profiles at that time.  In addition to providing a record
that I can consult later, the series of PDFs can also be converted to a movie showing
the evolution.  And if, like me, you are running on a Mac, you can use the Tioga GUI
to watch the plots update in real-time as the simulation program produces results.
(It should be possible to work out a similar arrangement with a previewer on Linux.  If
you have a scheme that works, please let me know and I'll pass it on.)

This section of the documentation discusses the tools in Tioga for doing all of this and outlines
the method I use on the Mac for converting the series of PDFs into a movie.

The basic framework that I'm assuming is that there is a separate program, we'll call it the "Builder", that is running independently of Tioga to produce a series of "models".  The models are assigned sequence numbers that we'll use to 
label the different PDF files.  Tioga (either the GUI or the irb version) will be running at the
same time as the Builder to create PDFs from "status files" being output as the operation progresses.
So the Builder works away creating a series of models.  Periodically it writes information about
the current model to a status file.  That information is used by Tioga to produce a series of PDFs
corresponding to the series of models.  The PDF file names contain the model number which determines
the sequence.  (Alternatively, we may just be watching the plots change on the display as a way
of monitoring the Builder's progress.  In that case, we don't need to save separate PDFs for each
model, so that part of the operation can be omitted.)

To make all of this work requires cooperation between Tioga and your plotting routines.  At Tioga's end,
it needs to know a time period for doing a "refresh" of the plot, measured in seconds.  Your routine
will be called at that frequency, more or less, operating system willing.  That works, but in many
cases, we can skip the call if the status file has been updated since last time.  So Tioga also
optionally takes the name of the file and checks the modification time on it before asking for a refresh.
So, the standard operation is to call your plotting routine when (1) at least a specified number of seconds have passed
since the last call, and (2) a certain file has been modified since the last call.

If we want to save the PDFs with file names including the model numbers, then Tioga needs to know both
that we want that and what the model number is.  The FigureMaker has writable attributes for both of these things.
Set the add_model_number attribute to +true+ and set the model_number attribute to the model number.
It's up to your routines to find out the model number.  Typically, the Builder will include that information in the status file along with the data to be plotted.  (To ensure that the lexical ordering
of files will match the numerical order of the model numbers, Tioga pads the numbers with leading zeros to make them all 4 digits long.)

As an example of how this works, go to the "samples/plots" directory, load "plots.rb" in irb, and
make plot 0.  It should give you something like this:

    >> ld 'plots.rb'
    => "plots.rb"
    >> mk 0
    cd plots_out; ... Blues.tex > pdflatex.log
    => "plots_out/Blues.pdf"

Open "plots.rb" in a text editor and make this addition at the end of "initialize" (after the
line that sets model_number to -1) and save the file:

    t.add_model_number = true

Back in irb, reload and remake.  When I did this, the model number was 25 and I got the following output:

    >> rl
    => "plots.rb"
    >> mk 0
    cd plots_out; ... Blues_0025.tex > pdflatex.log
    => "plots_out/Blues_0025.pdf"

A check of the plots_out folder shows that there are now a bunch of files with names beginning "Blues_0025".
  
Now let's try it with the Builder running at the same time.  Connect to the "data" directory in "plots"
and do a make to construct a sample Builder. (This assumes you have a working C compiler!) 
Start it up by entering "./builder" to the shell.
It should start emitting a series of lines like this, with a new line every 10 seconds or so:

    Finished Model Number 1
    Finished Model Number 10
    Finished Model Number 20

Leave the Builder chugging away and start up irb again in a different shell.  Load the version of "plots.rb" that has add_model_number set to +true+ and remake plot 0.  Wait a few seconds and repeat.  Wait, reload, make, wait, reload, make -- you should get something like this:

    >> mk 0
    cd plots_out; ... Blues_0005.tex > pdflatex.log
    => "plots_out/Blues_0005.pdf"
    >> rl
    => "plots.rb"
    >> mk 0
    cd plots_out; ... Blues_0009.tex > pdflatex.log
    => "plots_out/Blues_0009.pdf"
    >> rl
    => "plots.rb"
    >> mk 0
    cd plots_out; ... Blues_0013.tex > pdflatex.log
    => "plots_out/Blues_0013.pdf"
    >> rl
    => "plots.rb"
    >> mk 0
    cd plots_out; ... Blues_0018.tex > pdflatex.log
    => "plots_out/Blues_0018.pdf"

The "reloads" are necessary here because we haven't done anything (yet) to let the
plotting routine know that it needs to reread the data file.  To fix that, we'll use
the FigureMaker attribute need_to_reload_data.  By setting that +true+ before calling the
plotting routine and checking that flag in the plotting routine, we can get the behavior we want.
Here's what the the "read_data" routine in "plots.rb" looks like (with line numbers added):

    def read_data
 1      if t.in_subplot or (@have_data and !t.need_to_reload_data)
            return
        end
 2      Dvector.read_row(@data_filename, 1, @header)
 3      model = @header[0].round
 4      if (model != t.model_number) or t.need_to_reload_data
 5          Dvector.read(@data_filename, @data_array, 2)
 6          t.model_number = model
 7          @big_blues = @blues.mul(@big_blues_scale)
        end
 8      @have_data = true
 9      t.need_to_reload_data = false
    end

All of the plot routines begin with a call on read_data.  The first thing read_data
does is to check if it really has to read anything or not.  If we're in a subplot,
then the data was already read for the top-level plot, so we can safely return.
Similarly, if we've already read the data for a previous plot, and we haven't been
asked to reread it, we can return.  All of this is going on in the tests on line 1.
The in_subplot attribute starts out +false+ and is set to +true+ by the #subplot routine.
We have our own attribute, @have_data, that is set to +false+ in our initialize method,
and is then set to +true+ on line 8 of read_data.  So once we take care of setting
need_to_reload_data to +true+, read_data will do the right thing for us.  It will read
the new version of the data file and then set need_to_reload_data to false to prevent
any unnecessary reads.  For a small data set, this is overkill.  But when the amount of
data is large, it can be a big help to avoid reading the same data multiple times.

The actual reading happens on lines 2 and 5.  The first simply reads 
line 1 of the file to get the new model number (which is passed to the FigureMaker
on line 6).  The second call on Dvector.read gets the columns of new data.
The assignment on line 7 is creating some new data derived from the file data.  The details
will be different for your applications, but the general structure of your read routine can be the same.

Now we just need to make sure that the need_to_reload_data flag gets set.  That job will be
done by whichever front-end we're using.  In irb, the Tioga front-end has two commands
that will set need_to_reload_data for us.  The first, "refresh" (alias "rf"), sets the flag and then
remakes the most recently made figure.  The other, "review" (alias "rv"), sets the flag and then calls
preview for the most recently made figure.  Here's a sample:

    >> ld 'plots.rb'
    => "plots.rb"
    >> pv 1
    cd plots_out; ... Reds_2475.tex > pdflatex.log
    => true
    >> rv  
    cd plots_out; ... Reds_2482.tex > pdflatex.log
    => true
    >> rv
    cd plots_out; ... Reds_2487.tex > pdflatex.log
    => true

With each call on "rv", I get a new version of the plot loaded in the previewer.  Try it (of course
the Builder must be running to give different models each time).  With the Mac GUI for Tioga, you
can do the same thing with the "Refresh" command under
the "File" menu. 

At this stage, we have the tools to manually drive the creation of a series of plots.  The next step is
to automate the process.  For this, the front-end needs to know the period to wait between refreshes
and, optionally, the file to check for modifications to determine the need for a refresh.
In irb, the period defaults to 1 second and can be changed using the "period_for_refresh"
command.  In the Mac GUI for Tioga, the default is again 1 second and can be changed in the "Preferences" panel,
accessed from the "Tioga" menu.  The file name to check is set in our plotting program in its
initialize routine.  In the case of "plots.rb", our initialize includes these lines:

    @data_filename = "data/datalog.data"
    t.auto_refresh_filename = @data_filename

To turn on the automatic refresh cycle in irb, enter the command "auto_refresh" (alias "ar") with a count
of how many refreshes you want it to do before stopping.  If it just sits there doing nothing,
check to see if the Builder is actually producing new models.  Here's some sample output:

    >> ld 'plots.rb'
    => "plots.rb"
    >> mk 2
    cd plots_out; ... Side_by_Side_0290.tex > pdflatex.log
    => "plots_out/Side_by_Side_0290.pdf"
    >> ar 4
    cd plots_out; ... Side_by_Side_0293.tex > pdflatex.log
    cd plots_out; ... Side_by_Side_0294.tex > pdflatex.log
    cd plots_out; ... Side_by_Side_0296.tex > pdflatex.log
    cd plots_out; ... Side_by_Side_0297.tex > pdflatex.log
    => nil

[Note that we've missed model number 295.  This simply reflects that fact that the Builder
and the viewer are running asynchronously and are not guaranteed to stay together.  If you
need to capture every model, you may need to take a different approach that forces the Builder
to stop while the viewer processes the data.  In my applications, I haven't need to do that.
I typically update the status file
for every 5th model and set a refresh period that is short enough that I rarely if ever miss one.
The short refresh period means that I'm often checking the status file before it has been updated, but that
operation carries a relatively low cost.]

In the Mac GUI, turn on timed refresh with a menu item (under "View") or by holding down
the right mouse button with the cursor over the figure to get a pop-up menu that let's
you get things started.  Turn the refreshing off similarly.  You'll see
the plot update on the screen each time a new version is created.

Now that we can automatically generate a series of plots, it's time to make a movie!
Tioga has done it's part.  It has made the PDFs.  Now other programs need to do the work
of converting the PDFs into some movie format.  I'll give the details of how I do this on the Mac
for MPG movies.
Once again, I'll leave the Linux case for someone else to solve. (Please let me know!)

Since I'll be limiting my discussion to the Mac, I'll describe the steps using the GUI, since
that's the way I actually work.  Open "plots.rb" by dragging it to the Tioga icon.  Go to the
"plots" folder and create a subfolder called "movie_out" (at the same level as "plots_out").
Open "plots.rb" in a text editor.  Change the "save_dir" from "plots_out" to "movie_out" in
the initialize routine.  (You can skip this and just use "plots_out" if you don't have things
in there that you need -- just clean it out before you start accumulating movie PDFs.)

        t.save_dir = 'plots_out'
            becomes
        t.save_dir = 'movie_out'

Make sure that initialize is setting auto_refresh_filename and add_model_number like this;

        @data_filename = "data/datalog.data"
        t.auto_refresh_filename = @data_filename
        t.add_model_number = true

Save "plots.rb" and start the Builder running in the "plots/data" directory.  Go back to Tioga,
reload "plots.rb",
and start the automatic refreshing by holding down the right mouse button with the cursor
over the figure and selecting the "Start Timed Refresh" command (or select the "Toggle Timed Refresh"
command under the "View" menu).    Check the "movie_out" folder to make sure that the output files
are appearing there.  Stop the refresh after you've created a few dozen versions.  In the shell, connect to the
"movie_out" folder and delete all the superfluous files so that only the plot PDFs are left.
(This would also be a good time to go back to "plots.rb" and restore the previous "save_dir" setting.)

    cd movie_out
    rm *.txt *.aux *.log *.tex *_figure.pdf

The next job is to convert all those PDFs into a movie.  I'm currently using GraphicConverter, Version 5.9.
(If you don't have it, or only have an earlier version, cough up the $30 and get it.  The site
is lemkesoft[http://www.lemkesoft.com/en/graphcon.htm].)
GraphicConverter has an operation that does just what we need.  It's called "Export Slide Show to Movie...", and it's found
under the "File" menu.

Before you create the movie, you can adjust the frame size by setting the Preferences for import resolution for 
PDFs.  Open the GraphicConverter Preferences and go to the panel for PDF under Open - Formats.  Make sure it is set to "Open with bit depth of 32 bit".  Change the "Open with resolution of 72 ppi" to "150 ppi".
The bigger the resolution number, the larger the movie frame.  For example, instead of 150, use 100 if you want a smaller movie frame.

Close the Preferences, open up the panel for the "Export Slide Show to Movie..." command (under File), and navigate to the "movie_out" folder.
Hit the "Choose" button and pick a name for the movie and a folder to hold it.  Click "Save".  When
the "Additional Movie Settings" panel appears, check the button that says "Use dimension of first image".
You might also want to adjust the "Delay between frames/images" -- I've used 0.1 seconds in the example.
Select the "Compression..." button to set the parameters for the MPG output.  Set "MPEG-4 Video" for compression type,
uncheck the "Limit data rate" choice, and set "Quality" to "Best".
Click "OK" for "Compression Settings", and then click "OK" again for "Additional Movie Settings".  That should do it.  Open your new movie and enjoy.

---

The folder 'samples/pythagorian_3_body' provides another example of producing a movie that might be useful as a source of ideas for doing your own.  In addition to the usual routines for making plots, I've added one called 'movie' that makes the sequence of pdfs.  The following MPG4 movie shows the results: {pythagorian 3-body}[http://theory.kitp.ucsb.edu/~paxton/mpgs/p3.mpg].  Perhaps you're saying to yourself, I've heard of the Pythagorian Theorem, but what's a "pythagorian 3-body"?  It's a simple test case for simulations of motion of particles under the effect of their mutual gravitational attraction.  In this case, we start with three bodies at rest at the corners of a 3-4-5 Pythagorian triangle.  The movie shows one simulation of what happens when the bodies are released.  The next step is to do 100,000 bodies in a simulation of a globular cluster of stars!  For example, see {MANYBODY.ORG}[http://www.manybody.org/manybody/].

=end

module Animation 
end # module Animation

end # module Tutorial
end # module Tioga

