
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
    
Tell irb that you want to talk directly to irb_tioga by entering this:

    include Tioga::IRB_Tioga

Type "cmds" to get a list of the commands that irb_tioga has defined.
You should see something like the following:

    >> cmds

    Command                   short form      description
    load_figures 'tiogafile'      ld          loads the tioga figure definition file into ruby
    preview number                pv          makes the figure and opens the pdf file in a viewer
              the pdf viewer is specified by the value of the $open_command variable in irb
    reload_and_review             rr          reloads the figures file and redoes the preview
    open_list 'outputfile'        ol          saves and opens a numbered list of figure names
              'outputfile' can be omitted -- it defaults to 'names.txt'
              the text editor is specified by the value of the $open_list_command variable in irb

    Those are the four essential commands.  Here are some others that are also available.
    list_figures                  ls          lists the figures by number for the current file
    save_list 'outputfile'        sl          saves list of figures to the named file
    make_figure number            mk          makes the pdf file for the figure
    make_all                      ma          makes all of the figures in the current file
    reload                        rl          reloads the most recently loaded file
    review                        rv          redoes preview for the most recently viewed figure
    eval_function string          ef          calls the current file's eval function
    quiet                                     turns off all but essential messages
    verbose                                   undoes the effect of the quiet command
    version                                   prints the Tioga FigureMaker version string
    cmds                                      prints this message

    => true

The commands correspond to methods that are defined in irb_tioga.  You can load (and reload) a
document with figure definitions, get a numbered list of the defined figures, make and optionally
preview figures, or make all of the figures at once.

In addition to these few commands, you are talking to the standard irb tool which will call the Ruby
interpreter for you to do other tasks as well.   This can be very useful when debugging a new
figure or plot.  When you are done, simply type "exit" to return to the shell.

If you're going to be doing a lot of work with irb_tioga, you can have it automatically loaded for you
by creating (or modifying) a file "~/.irbrc" that irb will automatically run at startup.  If you add the lines
to require and include irb_tioga, it will be there waiting for you.  (NOTE: you may need to put a copy of 
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
    $open_command = 'open'
    $open_list_command = 'open'
    
You can use this as a start for your own ~/.irbrc.  Either cut-and-paste this, or use the
copy of the file included as "irbrc" in the download.  Don't forget to edit the values of
the $open_command and $open_list_command to be your favorite pdf viewer and text editor
(on a Mac, you can of course just leave them as 'open').

---

Find the "samples" folder in the download and "cd" to the "figures" subfolder. 
Inside "figures" you should find the following folders and files.  

* data
* figures_out
* figures.rb
* sample.rb 

The starting place is sample.rb, so open that in a text editor (if you have one that
understands Ruby syntax, that would be a good choice -- I use TextMate on the Mac).  
The "sample.rb" file contains the little program described in #DocStructure.
This might be a good time to reread that if you need a reminder of the general structure
of classes and methods we'll be using.

We'll run the programs using irb, the "Interactive Ruby" shell, with the
irb_tioga extensions whichgive simple command line tools for driving
the operations.
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

Close the window with Red.pdf in it, and go back to the window holding the text of sample.rb.
Find the definition of the "red" figure: it looks like this:

    def red
        t.fill_color = Red
        t.fill_frame
    end

Edit this and change "t.fill_color = Red" to "t.fill_color = Green" and save the file.
Now, back in irb again, reload the file and remake the figure as follows:  first enter
"rl", then reenter "pv 1" or backup two command lines and reuse the line you typed
previously.  Now the Red.pdf should be displayed in green.  

This sequence illustrates the basic development cycle for a tioga plot: run the program in Ruby,
look at the result in a pdf viewer, edit the source file, then reload and rerun to see the
changes.  This is such a common sequence that there is a single command to do the irb part of it.  
After you've done the edits to the figure file and saved it, go back to irb and do "rr" to
have the new version of the figure definition file loaded and the most
recently viewed figure rebuilt and re-previewed.

I typically have 3 windows open corresponding to the 3 steps of edit-run-preview.  I have
my Tioga ruby source file open in a text editor window, irb open in a terminal
window, and the PDF file open in a previewer.  I edit and save the ruby program 
for my figure or plot, type "rr" to irb_tioga to get the file reloaded and 
the pdf recreated, then look at the result in the previewer.

Often my Tioga files define a long list of figures.  That's when I use the 'ol' command
that writes the list of figure names and numbers to a file
and then opens that file in a text editor.  So now in addition to the 3 windows mentioned above,
I also have another text editor window with a list of figures giving numbers and
names -- a quick glance at that is enough to tell me what number to give to the
"pv" command in irb_tioga to get the figure I want.

Finally, some previewers need a gentle kick in the pants to reload a PDF file
after it has been modified.  For example, Preview on the Mac discards requests to
open a PDF that it already has opened, even if the file has been written since
the previous open.  Happily, other people have solved this one for us.  Here's a 
simple way to get "repreview" to work on the Mac.

1) in Apple Menu, open System Preferences...

2) in the System section of preferences, click on "Universal Access"

3) At the bottom of Universal Access, check the box for "Enable access for assistive devices",
then close the window.

4) Download this {.scpt file}[http://theory.kitp.ucsb.edu/~paxton/tioga/Reload_Preview_Document.scpt] 
and copy it to /User/me/Library/Scripts (with "me" replaced appropriately):

5) Download this {shell script}[http://theory.kitp.ucsb.edu/~paxton/tioga/repreview] 
and copy it to /User/me/Library/bin  

6) Add /User/me/Library/bin to your $PATH if it isn't there already.

You should now have available a "repreview" command that takes the name of the pdf file as arg.
The new repreview command gets the document in the frontmost window of Preview and then does "Revert" to get it reloaded.

7) change your .irbrc to set repreview as your "open" command by adding this line:

  $open_command = 'repreview'

---

Now it's time to take a look at the details of a tioga figure definition -- next stop: SimpleFigure.

=end

module IRB_tioga
end # module IRB_tioga

end # module Tutorial
end # module Tioga

