
module Tioga
module Tutorial

=begin rdoc

= Tools for reading and manipulating the data for plots

Ruby has good tools for reading data and working with it, but I decided to add a few more
just to boost the efficiency.  In some of my astrophysical simulations, I'm reading fairly large
data sets and doing some operations on them before creating the plots.  The implementation of
arrays in Ruby is wonderfully general, allowing arbitrary collections of things to be stored.
That's cool, but my data is just floating point numbers, and I'd like a special kind of array
that will take advantage of that.  

The result of my obsession with speed is a pair of classes: Dvector for 1 dimensional vectors
of floats, and Dtable for 2 dimensional tables of floats.  In addition to storing the data
in the standard C manner, the classes implement a variety of methods of operating on it
(similar to those for standard Array objects), and provide methods to simplify reading
rows and columns of tablular data from files.

For details, visit the class definitions: Dvector and Dtable.

---

Have you ever opened a PDF file in a text editor just to see what's there?  If so, you've
probably been disappointed to encounter something like this:

    2 0 obj <<
    /Type /Pages
    /Kids [4 0 R]
    /Count 1
    >> endobj
    3 0 obj <<	/Filter /FlateDecode   /Length 1894         
    >>
    stream
    xúçYAé‰6º˜+ÊŸñe˘‰êÏ1»)AΩá‰≤ﬂOãU¢ÈmRkpfƒbU€Æj[ÈsIÚØ˚
    ...
    
The contents of a PDF file are typically compressed using a lossless scheme such as Flate.
(Images are often compressed using a lossy scheme such as JPEG.)

The code for Flate is available as part of zlib[http://www.gzip.org/zlib/],
and I've included it as a module in tioga.  Methods for both compression and decompression are included.
You may never have an occasion to call Flate yourself, but it's working for you every time you 
call on tioga to make a PDF file.

And if you're still wondering what's inside that PDF file, you might be able to use the Flate decompression method
to find out!


---

Now let's see how to make plots with a false-color image of sampled data -- next stop: SampledData.


=end

module Data
end # module Data

end # module Tutorial
end # module Tioga

