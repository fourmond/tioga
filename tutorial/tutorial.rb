# tutorial.rb

module Tioga

=begin rdoc

= Tutorial

This tutorial gives a guided tour through the programs included in the "samples" subfolder
found in the installation folder.  It also contains material introducing the command
line interface along with hints and suggestions for using tioga effectively.

By the way, the tutorial assumes you want to use Ruby directly to write the programs that 
describe the figures and plots. If instead of that you'd prefer a command-line interface 
along the lines of gnuplot, then be sure to read all about {Ctioga}[http://sciyag.rubyforge.org/ctioga/index.html]. 
Basically, Ctioga writes the ruby program for you, making it easy to do easy plots while still having 
the ability to do complex ones by adding options to the command line.
  
---

The first step in the tutorial is to locate the samples folder and take a look at what's inside.  
You should find a +README+ and several subfolders including:

* figures
* plots
* star_history
* star_profile
* zams

The +README+ file basically just directs readers to this documentation.  The "figures" subfolder has the most basic stuff.
Next comes "plots" which extends the discussion to the methods
needed to make plots. The final three folders, "star_history", "star_profile", and "zams",
come from my own stellar evolution work.  They give "real life"
examples of non-trivial applications. 

Here are the sections of the tutorial, in the order they are intended to be read.

DocStructure -- The basic anatomy of a tioga document

CommandLine -- How to run and debug your tioga programs

OtherWaysToRunTioga -- Batch mode, Ctioga, Vtioga, etc.

UsingRuby -- The bare essentials for using ruby for tioga

SimpleFigure -- A first look at a nontrivial tioga figure definition

TextForTeX -- How to Add Packages and How (and How Not) to Enter Text for TeX

Plots -- The extras for making plots

Data -- Tools for reading and manipulating the data for plots

SampledData -- Making plots with a false-color image of sampled data

Animation -- How to create a sequence of PDFs and convert it to a movie

OtherStuff -- A few "real life" examples

Epilog -- "Do it yourself" using the tioga kernel for making figures


=end
module Tutorial
end # module Tutorial

end # module Tioga

