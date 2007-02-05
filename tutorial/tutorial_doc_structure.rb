
module Tioga
module Tutorial

=begin rdoc

= The basic anatomy of a tioga document: "sample.rb"

Source files for tioga are Ruby programs of a particular form and content.  
The overall form is described in this section.  
Some details of the content are discussed in the rest of the tutorial.

We'll use a very simple case to illustrate the general form: a file that defines two figures, one that fills 
the frame with blue and one that fills it with red.  The file will define a class, "MyFigures", and
create an instance of it.  We'll start with those operations, then add bit-by-bit until 
we have a complete definition.

    class MyFigures

    end
    
    MyFigures.new

We start adding the missing pieces by requiring rubygems
(`require' tells Ruby to load the required item if it hasn't already been loaded).
The {rubygems}[http://docs.rubygems.org/] package is the Ruby standard for managing
add-on software libraries. 
Once you've done the "require rubygems" command,
the following require command for the tioga FigureMaker will work whether 
tioga was downloaded as a gem or independently.

 >> require 'rubygems'
 >> require 'Tioga/FigureMaker'

    class MyFigures
    
    end
    
    MyFigures.new

Here we are using ">>" to mark the newly added lines.  At the end, we'll give a
complete listing without any of those marks which are not really part of the code.

Next we'll  `include' the Tioga and FigureConstants modules to make it easy to reference the methods and attributes 
they define -- include makes 
everything defined in the module available as if defined here.



    require 'rubygems'
    require 'Tioga/FigureMaker'

    class MyFigures
    
 >>     include Tioga
 >>     include FigureConstants
    
    end
    
    MyFigures.new

The default tioga FigureMaker will be doing the work for us, so our initialize method will get it and
put it in an instance variable for us that we'll name "@figure_maker" (the "@" is Ruby's way of saying 
that this is an attribute associated with a particular object).  The various tioga user interfaces
will also be talking to the default FigureMaker to find out what figures we've defined and ask
for various ones to be created in response to our commands.

    require 'rubygems'
    require 'Tioga/FigureMaker'

    class MyFigures
    
        include Tioga
        include FigureConstants
    
 >>     def initialize
 >>         @figure_maker = FigureMaker.default
 >>     end
        
    end
    
    MyFigures.new

The "MyFigures.new" command is asking Ruby to create an instance of the MyFigures class which in turn involves 
Ruby calling the class's initialize routine.  The command does NOT automatically make the figures at the time 
the class object is created.  The figures get constructed in response to user requests that will be forwarded 
from the user interface to the figure maker and then on to our routines.

But all that comes later.  For now, our next step is to add a method called "t" that simply returns the 
local attribute @figure_maker.  Then we can use "t." to talk to our instance of the FigureMaker rather 
than having to type "@figure_maker" all the time.

The "." in "t." is part of the "object.name" idiom in Ruby.  It refers to the named attribute or method 
in the given object.  In turn, the object can come from some other evaluation.  In the "t." case, our
method called "t" will return the object we've saved in our local attribute called "@figure_maker".

    require 'rubygems'
    require 'Tioga/FigureMaker'

    class MyFigures
    
        include Tioga
        include FigureConstants
    
 >>     def t
 >>         @figure_maker
 >>     end

        def initialize
            @figure_maker = FigureMaker.default
        end
        
    end
    
    MyFigures.new

When we write something like "t.line_width", we're talking about the thing referenced by the name "line_width" in the 
object that is returned by evaluating our method called "t".  It sounds complicated, but you can safely forget the 
details and just remember this:

[] <b><i>"t." is talking to Tioga</b></i>.

The next step is to add to the initialize routine
to tell the FigureMaker about the figures defined in this file.  That is done by calling "def_figure" for each
one, giving a string name and a body of code that will produce the figure.  The names are "Blue" and "Red",
and the command bodies are given inside the curly braces.  In this case the commands are simply calls on
methods with the corresponding name, but in lowercase since all method names begin with lowercase (Ruby is
picky about that).

    require 'rubygems'
    require 'Tioga/FigureMaker'

    class MyFigures
    
        include Tioga
        include FigureConstants
    
        def t
            @figure_maker
        end

        def initialize
            @figure_maker = FigureMaker.default
 >>         t.def_figure('Blue') { blue }
 >>         t.def_figure('Red') { red }
        end
        
    end
    
    MyFigures.new

The figures are defined to call routines "blue" and "red", so we'll add those next.

    require 'rubygems'
    require 'Tioga/FigureMaker'

    class MyFigures
    
        include Tioga
        include FigureConstants
    
        def t
            @figure_maker
        end

        def initialize
            @figure_maker = FigureMaker.default
            t.def_figure('Blue') { blue }
            t.def_figure('Red') { red }
        end
        
 >>     def blue
 >>         t.fill_color = Blue
 >>         t.fill_frame
 >>     end
        
 >>     def red
 >>         t.fill_color = Red
 >>         t.fill_frame
 >>     end
    
    end
    
    MyFigures.new

We could use the file as it is now, but I like to have the output files go in a subfolder rather than
cluttering up the top level folder where the definition file lives.  We can specify a "save directory"
by setting the FigureMaker "save_dir" attribute.  That will be our last addition.  Here's the final version 
of "sample.rb".
    
    require 'rubygems'
    require 'Tioga/FigureMaker'
    
    class MyFigures
    
        include Tioga
        include FigureConstants
    
        def t
            @figure_maker
        end

        def initialize
            @figure_maker = FigureMaker.default
            t.def_figure('Blue') { blue }
            t.def_figure('Red') { red }
            t.save_dir = 'figures_out'
        end
        
        def blue
            t.fill_color = Blue
            t.fill_frame
        end
        
        def red
            t.fill_color = Red
            t.fill_frame
        end
    
    end
    
    MyFigures.new


---

Now let's see about putting tioga to work to create the PDFs for these figures -- next stop: CommandLine.

=end

module DocStructure
end # module DocStructure

end # module Tutorial
end # module Tioga

