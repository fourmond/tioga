
module Tioga
module Tutorial

=begin rdoc

= Using irb, "Interactive Ruby", as a tioga front-end

Ruby comes with a very useful interface called "irb" for "interactive Ruby".  If you'd like to
learn lots more details about it than I'll be giving here, check out Chapter 15 of the 2nd edition of
"Programming Ruby: The Pragmatic Programmer's Guide" by Dave Thomas.  

First, start irb by a command to the shell of the form:

    irb [ irb-options ] [ ruby_script ] [ program arguments ]

For most cases, a simple "irb" will suffice.
At the prompt from irb, load the tioga command line tool by typing:
    
    require 'Tioga/irb_tioga.rb'
    
Tell irb that you want to talk directly to IRB_Tioga by entering this:

    include Tioga::IRB_Tioga

Type "list_cmds" to get a list of the commands that irb_tioga has added.
You should see something like the following:

    >> list_cmds
    Command                short form  description
    load_figures 'filename'    ld      loads the figure definition file
    reload                     rl      reloads the most recently loaded file
    refresh                    rf      sets 'need_to_reload_data' and redoes make
    review                     rv      sets 'need_to_reload_data' and redoes preview
    list_figures               ls      lists the figures in the current file
    make_figure number         mk      makes the figure
    preview number             pv      makes the figure and opens the pdf file
    make_all                   ma      makes all of the figures in the current file
    refresh_period secs        rp      sets the seconds between auto refreshes
    auto_refresh cnt           ar      runs auto refresh cycle for cnt refreshes
    quiet                              turn off all but essential messages
    verbose                            undo the effect of the quiet command
    version                            prints the version string
    list_cmds                          prints this message

The commands correspond to methods that are defined in IRB_Tioga.  You can load (and reload) a
document with figure definitions, get a numbered list of the defined figures, make and optionally
preview figures, or make all of the figures at once.

In addition to these few commands, you are talking to the standard irb tool which will call the Ruby
interpreter for you to do other tasks as well.   This can be very useful when debugging a new
figure or plot.  When you are done, simply type "exit" to return to the shell.

If you're going to be doing a lot of work with IRB_Tioga, you can have it automatically loaded for you
by creating (or modifying) a file "~/.irbrc" that irb will automatically run at startup.  If you add the lines
to require and include IRB_Tioga, it will be there waiting for you.  (NOTE: you may need to put a copy of 
the .irbrc file in each folder where you run irb rather than just having a single copy in your home directory; this
seems to be the case on my Mac at least.)

There are lots of other nice features of irb too.  You can configure the prompt, add "Tab Completion",
or add a command history that extends across sessions.  Here's my own .irbrc file (which you
do NOT need to look at in detail):

    require 'irb/completion'
    
    IRB.conf[:PROMPT_MODE] = :SIMPLE
    IRB.conf[:USE_READLINE] = true
    
    HISTFILE = "~/.irb.hist"
    MAXHISTSIZE = 100
    
    begin
        if defined? Readline::HISTORY
            histfile = File::expand_path( HISTFILE )
            if File::exists?( histfile )
                lines = IO::readlines( histfile ).collect {|line| line.chomp}
                puts "Read %d saved history commands from %s." %
                    [ lines.nitems, histfile ] if $DEBUG || $VERBOSE
                Readline::HISTORY.push( *lines )
            else
                puts "History file '%s' was empty or non-existant." %
                    histfile if $DEBUG || $VERBOSE
            end
            
            Kernel::at_exit {
                lines = Readline::HISTORY.to_a.reverse.uniq.reverse
                lines = lines[ -MAXHISTSIZE, MAXHISTSIZE ] if lines.nitems > MAXHISTSIZE
                $stderr.puts "Saving %d history lines to %s." %
                    [ lines.length, histfile ] if $VERBOSE || $DEBUG
                File::open( histfile, File::WRONLY|File::CREAT|File::TRUNC ) {|ofh|
                    lines.each {|line| ofh.puts line }
                    }
            }
        end
    end
    
    require 'Tioga/irb_tioga.rb'
    include Tioga::IRB_Tioga
    
You can use this as a start for your own ~/.irbrc.  Either cut-and-paste this, or use the
copy of the file included as "irbrc" in the download.

---

Find the "samples" folder in the download and "cd" to the "figures" subfolder. 
Inside "figures" you should discover 2 folders and 2 files.  

* data
* figures_out
* figures.rb
* sample.rb 

The starting place is sample.rb, so open that in a text editor (if you have one that
understands Ruby syntax, that would be a good choice -- I use SubEthaEdit on the Mac).  
The "sample.rb" file contains the little program described in #DocStructure.
This might be a good time to reread that if you need a reminder of the general structure
of classes and methods we'll be using.

We'll run the programs using irb, the "Interactive Ruby" shell, with the
IRB_Tioga extensions which  give simple command line tools for driving
the operations.  Later on, the Mac-sters can abandon the command line tool for
a GUI interface if they'd like, but the tutorial will be mainly using irb.
If necessary, refresh your memory of the commands by a quick browse of the IRBTioga documentation.
If you haven't yet set up your "~/.irbrc" file, this would be the time to do it.
As a quick start, you might want to use the sample "irbrc" that comes with the download.
Its located in the top level installation folder.  Copy that to ~/.irbrc, and you're done.

Now, in a shell terminal, connect to the "samples/figures" folder and start irb.
Load the sample file by entering this (after the ">>" prompt from irb):

    >> ld 'sample.rb'
    => "sample.rb"

List the figures by entering the "ls" command.  You should see

    >> ls
      0 Blue
      1 Red
    => true

Those are the two figures defined in "sample.rb".  Make the first one by typing

    >> mk 0
    cd figures_out; pdflatex -interaction nonstopmode Blue.tex > pdflatex.log
    => "/Users/billpaxton/FigureMaker/samples/figures/figures_out/Blue.pdf"

The output gives the full pathname to the output PDF file.   You
should open that in another window just to confirm that it really is blue!


If you have can type "open Blue.pdf" to the shell and have it open in the pdf viewer, then
try this with the second figure:

    >> pv 1
    cd figures_out; pdflatex -interaction nonstopmode Red.tex > pdflatex.log
    => true

With any luck, your pdf viewer is now displaying Red.pdf for you.

* NOTE: If you are using Linux, you may type something like "xpdf Blue.pdf" instead of "open Blue.pdf".
  Then you need
  to tell tioga what to do when you ask to preview a file.  It uses a global variable holding
  a string specifying the viewer to use -- the default for this is 'open', but you can
  change it to something else.  For example, if you want to use xpdf, 
  then after loading irb_tioga, type

    >> $open_command = 'xpdf'

Close the window with Red.pdf in it, and go back to the window holding the text of sample.rb.
Find the definition of the "red" figure: it looks like this:

    def red
        t.fill_color = Red
        t.fill_frame
    end

Edit this and change "t.fill_color = Red" to "t.fill_color = Green" and save the file.
Now, back in irb again, reload the file and remake the figure as follows:  first enter
"rl", then reenter "pv 1" or backup two command lines and reuse the line you typed
previously.  Now the Red.pdf should be displayed in green (my apologies to  color blind
readers!).  

This sequence illustrates the basic development cycle for tioga:  run the program in Ruby,
look at the result in a pdf viewer, edit the source file, then reload and rerun to see the
changes.  It goes fairly smoothly even with a command line interface, and with a GUI,
it gets even better.

=end

module Sample
end # module Sample

end # module Tutorial
end # module Tioga

