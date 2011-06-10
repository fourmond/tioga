
module Tioga
module Tutorial

=begin rdoc


= How To's, Part 1: EPS 

Some publishers insist on eps format rather than pdf.  Luckily 'pdftops' makes this easy 
(just add -eps to the command line).  For the Mac,
there is also a nice drag-and-drop converter called "Droppdftoeps" available at
http://insti.physics.sunysb.edu/~siegel.


= How To's, Part 2: Embedded Fonts

Some publishers insist on having ALL fonts embedded, even the standard 14
Adobe fonts that are required to be available in all PostScript/PDF implementations.
Roy Mayfield provided a simple "one-liner" to take care of this using GhostScript:

  gs -q -dNOPAUSE -dBATCH -dPDFSETTINGS=/prepress -sDEVICE=pdfwrite \
      -sOutputFile=output.pdf input.pdf

Vincent Fourmond also provided a similar solution that uses an eps file intermediate:

   pdftops -eps Plot.pdf
   epstopdf --nogs Plot.eps | gs -q -sDEVICE=pdfwrite -dAutoRotatePages=/None \
      -dPDFSETTINGS=/prepress -sOutputFile=Plot_new.pdf -

Don't forget the - at the end of the epstopdf command!
 

= What's in the other tioga/samples folders (and what's a "zams" anyway?)

We've now been through the "figures" folder and the "plots" folder in
the samples.  The remaining ones are included as examples of "real"
cases using tioga to make plots.  The plots and the data they
represent come from my stellar evolution program, "EZ[http://www.kitp.ucsb.edu/~paxton/#EZ]".
Here's the answer to what's a "zams":

[]     The "main sequence" is where stars spend most of their lives while they burn hydrogen in their cores.
       The zero-age main sequence, "ZAMS", corresponds to stars that are just settling into their careers as
       hydrogen burners after having formed from a collapsing gas cloud.

The name EZ stands for "Evolve ZAMS",
and the remaining samples are all taken from the general topic of stellar
evolution.  However, the point of the samples is not to teach astrophysics, but to provide a set of more
complex plots to illustrate tioga.  We'll take a quick look at what's there, and I'll point out a few highlights
along the way.

The starting place is the "zero age" stars, so go to the "zams" folder in "samples", and open "zams.rb" in your editor.

link:images/zams.png

Notice that we're plotting the same stuff for several different cases corresponding to
different "Z" values (astro jargon for the fraction of mass in things other than hydrogen
or helium).  Instead of 1 data set, we have 4 sets with the same structure.  This is
reflected in the code by a new class definition, "ZAMS_Data".  The ZAMS_Data class
knows how to read a set of data given a file name.  The code makes 4 instances of this
class, one for each of the data sets.  If we decide to add a 5th data set, it simply means creating a fifth instance of the class.  The plotting routines have iterators for doing each of the data objects.  For example, here are the routines for plotting the center temperatures and the surface temperatures:

    def temp_surface_plot
        zams_plot('log $T_{surface}$', 5.1, 3.2) do
            @zams_data_array.each_index do |i|
                zams = @zams_data_array[i]
                t.show_polyline(zams.log_Ms, zams.log_Tsurfs,
                    @colors[i], @legends[i])
            end
        end
    end
    
    def temp_center_plot
        zams_plot('log $T_{center}$', 7.83, 6.45) do
            @zams_data_array.each_index do |i|
                zams = @zams_data_array[i]
                t.show_polyline(zams.log_Ms, zams.log_Tcents,
                    @colors[i], @legends[i])
            end
        end
    end
    
They both call the "zams_plot" routine in "zams.rb" with a title and y boundaries as arguments followed by a block of code that loops through each of the data sets calling show_polyline.

    def zams_plot(ylabel, ytop, ybottom, &cmd)
        t.xlabel = 'Mass ($M_{\odot}$)'
        t.ylabel = ylabel
        xleft =log10(0.09)
        xright = log10(101.0)
        t.xaxis_log_values = true
        t.xaxis_use_fixed_pt = true
        t.show_plot(
            'left_boundary' => xleft,
            'right_boundary' => xright,
            'top_boundary' => ytop,
            'bottom_boundary' => ybottom) { background; cmd.call }
    end
    
The final item in the definition of the arguments for zams_plot is a new one for us.  The "&cmd" refers to the block of code in the caller.  The command block is called on the last line as part of a show_plot.

link:images/zams_fancy.png

The "ZAMS fancy" plot is mainly for fun.  But it gives examples of setting up special tick labels as well as putting different information on each of the four sides of the frame.  The background coloring is done with a call on axial_shading using a specially built color map.  The gray dashed line shows the main sequence line where the stars burn their hydrogen.  The stars off the line are at later stages of development, such as the red giant Betelgeuse and the white dwarf Procyon B.  

---

Let's move on to the next folder in samples, "star_history".  Load "history.rb" in IRB_tioga and take a look at the plots.  Here's the first one:

link:images/H_R_T_RHO.png

These plots show the simulated history of our own sun, from its creation to its very late stages as it starts down the path to becoming a white dwarf.  The upper plot shows the tracks for the things we can observe at the surface: the luminosity and the temperature.  The lower plot shows the tracks predicted by theory for the center temperature and density.  Both tracks are labelled with numbers at a few key points during the evolution.  To the right of the plots is a list of these points in terms of age (in units of 10^9 years!) and mass (in units of the mass of the sun at the present).  The point labelled "He Flash" marks the time of a major upheaval in the star and a gap in the simulation.  The simulation starts up again at the point labelled "After He Flash".  The dashed line through the point labelled "0" in each plot shows the zero-age main sequence.  On the lower plot, the other dashed lines are labelled with values of a parameter, "Psi", that measures changes in the stellar material as it becomes so compressed that it begins to behave more like a liquid than a gas.

Open "history.rb" in a text editor and notice that it starts out by loading several other files.  Check out the "lib" folder in "star_history" to see what's there.  This is our first example of splitting a large plotting program into several files that work closely together.  Each file has the same general form:

    class StarHistory
    
        include Math
        include FigureConstants
        
        < define some things >
    
    end

It looks like each file is redefining the StarHistory class, but actually they are just adding new methods to the same class.  Ruby is happy for you to add on to a class, and it even lets you add to the built in classes.  For example, elsewhere we've added to the Array class to provide a method, to_dvector, that converts an array to a Dvector.  Very cool.

Also notice that the other files are loaded using "load" rather than "require".  The difference between these two is that "load" always reads the file, whereas "require" only reads it if it hasn't already been loaded.
For our edit-reload-repeat cycle, we need to make sure the subfiles are reloaded too in case they're the ones we've changed -- so we need to "load" them rather than "require" them.

---

The final folder in "samples" is "star_profile".  The plots here capture the state of the star at a certain time during its evolution.  The "profile" from the center to the surface of different properties reveals the details of the internal stellar structure.  I use a summary plot to capture both lots of graphs and a listing of values for lots of significant properties.

link:images/full_profile.png

The "profile_data.rb" file takes care of reading in the rather large "status.data" describing the state of the star.  It is an example of dealing with lots of structured data, and it also shows one way of dealing with the issues that come up when you're reading a file that might not be complete.  (You might expect that the file system wouldn't let you open a file to read that another task has open to write, but that seems to happen.)

    def read_profile(filename)
        keep_trying = 5
        while keep_trying > 0
            begin
                Dvector.read_rows(filename, @dest)
                unpack_rows(filename)
                puts "model number #{@model_Number.round}"
                keep_trying = 0
            rescue
                if keep_trying == 0
                    raise "Unable to read profile from #{filename}"
                end
                sleep 0.5
            end
            keep_trying -= 1
        end
    end

Ruby has a nice set of features for dealing with exceptions.  The begin-rescue-end construct lets us try to read the data and recover if something goes wrong.  If an exception is raised during the call on Dvector.read_rows or in unpack_rows, our "rescue" code will be invoked.  If we've tried 5 times without any luck, it gives up and raises another exception.  Otherwise, it sleeps a little and then tries again in hopes that the data file will be complete the next time we read it.

Sometimes a colleague (initials LB) will e-mail me with a request for a specific set of data from the full data set.  A recent example involved getting the abundances of carbon and oxygen in the inner regions of the star at a very late stage of evolution.  The read_profile routine is an easy place to add some code to extract the stuff we want.  Here's an example where I've added two line to read_profile and a new method that will "save_stuff" for us:

    def read_profile(filename)
 >>     saving_stuff = false # change to true when saving
        keep_trying = 5
        while keep_trying > 0
            begin
                Dvector.read_rows(filename, @dest)
                unpack_rows(filename)
                puts "model number #{@model_Number.round}"
                keep_trying = 0
 >>             save_stuff if saving_stuff
            rescue
                if keep_trying == 0
                    raise "Unable to read profile from #{filename}"
                end
                sleep 0.5
            end
            keep_trying -= 1
        end
    end

    def save_stuff
        puts "write_C_O_profile"
        file = File.open('C_O_profile.data', 'w')
        file.printf(
            'Columns contain: Mass XC XO XNE XN.  ' +
            'Initial mass = %0.2g  Current mass = %0.4g   ' +
            'Age = %0.4g   Initial Z = %g' + "\n",
            @initial_Mass, @star_Mass, @star_Age, @initial_Z)
        @sx_M.each_index do |i|
            if @sx_XC[i] > @sx_XH[i]
                file.printf("%12.5g %12.5g %12.5g %12.5g %12.5g\n",
                    @sx_M[i], @sx_XC[i], @sx_XO[i],
                    @sx_XNE[i], @sx_XN[i])
            end
        end
        file.close
    end

Once we've succeeded in reading the profile status data, we open a file, "C_O_profile.data", 
and output the selected information in a form that will be easy for our colleague to use.  
The next step is to convince said colleague to learn how to do this on his own -- wish me luck on that one!
  
---

We've reached the end of the tutorial.  I hope you've enjoyed it, and I hope you will enjoy using tioga.
You should sign up for the two mailing lists, {tioga-news}[http://rubyforge.org/mailman/listinfo/tioga-news] 
and {tioga-users}[http://rubyforge.org/mailman/listinfo/tioga-users].  The first is for
general information from the tioga developers about new releases and such.  The second is for comments,
questions, and answers concerning tioga.  

Have fun and happy plotting!


=end

module OtherStuff
end # module OtherStuff

end # module Tutorial
end # module Tioga

