
module Tioga
module Tutorial

=begin rdoc

= Other ways to run tioga

If you have looked at the tioga reference material, you might have noticed that
it talks about the "tioga kernel" and describes it as a tool for creating PDFs.  There's
no mention of a user interface as an integral part of tioga, and the interface we've described here
is an optional "add-on" provided as part of the tioga download.  There are several other options
available -- here's the current list.

Batch mode -- you can write your tioga file in a way that can be directly run by ruby to create
the pdfs.  Take a look at the file samples/immediate.rb for an example of how to do this.

{ctioga2}[http://ctioga2.rubyforge.org] by Vincent Fourmond is a
command-line plotting system along the lines of gnuplot that uses the
tioga kernel.  Basically, +ctioga2+ writes the tioga ruby files for
you based on a powerful set of command line options.

{Vtioga}[http://astro.u-strasbg.fr/~jfleck/] by Jean-Julien Fleck is a browser-based viewer that uses
tioga to create the pdfs, converts them to png's, and them displays them in a browser page 
created using Ruby on Rails.  The browser page has thumbnails for all the figures in the file -- click 
them to see a larger version.

{Tioga Droplet}[http://www.kitp.ucsb.edu/~paxton/tioga.html] by me (Bill Paxton) is a tiny Mac application
whose only function is to do the equivalent of 'tioga filename -p' for any files dropped on it.

There is also an irb version of the tioga command line interface that is part of the standard
tioga download.  To try it, start up irb, then enter:

  require 'Tioga/irb_tioga.rb' ; include Tioga::IRB_Tioga ; h

That should output a short description of the available commands.  If you'd like irb to load these
tioga commands automatically, add the +require+ and +include+ commands to your ~/.irbrc file.
If you don't have one, you can copy tioga/irbrc that is part of the download.

If you create another user interface for tioga that you'd like to share, please let me know 
and I'll add it to this list.

---

Now it is time to start digging into the details of using Ruby to define figures and
plots in tioga -- next stop, UsingRuby.

=end

module OtherWaysToRunTioga
end # module 

end # module Tutorial
end # module Tioga

