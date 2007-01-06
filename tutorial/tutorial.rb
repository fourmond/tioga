# tutorial.rb

module Tioga

=begin rdoc

= Tutorial

This tutorial gives a guided tour through the programs included in the "samples" subfolder
found in the installation folder.  The hope is that it will give you
an understanding of the samples that will let you pick and choose from them as a way of
getting started on your own applications.

The tutorial also contains material introducing the command
line interface and the graphical interface (for the Mac) along with hints and suggestions for using tioga
effectively.

The first step is to locate the samples folder and take a look at what's inside.  You should find a +README+ 
and several subfolders.

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

UsingRuby -- The bare essentials for using ruby with tioga

DocStructure -- The basic anatomy of a tioga document and "batch-mode" operation

IRB_tioga -- Intro to using interactive ruby as a tioga front-end

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

