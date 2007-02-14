
module Tioga
module Tutorial

=begin rdoc

= How to get a "Euro" symbol: Adding Packages to the TeX preamble

You'll probably have your favorite packages that you use in TeX all the time.  If you want to have access to them when creating figures
as well, you'll need to append the appropriate 'usepackage' commands to the preamble. 
You can change the preamble used for figures by setting the 'tex_preamble' property in the 'initialize' routine of the Ruby plot file.  For instance, here's an example that adds the 'marvosym' package:

    def initialize
        @figure_maker = FigureMaker.default
        t.tex_preamble = t.tex_preamble + "\n\t\\usepackage{marvosym}\n"
        ...
    end

Take a look at the 'marvosym.rb' file in the 'samples/figures' folder for more about this very useful symbol font.  
Here's a small sample of what it offers.

link:images/marvosym_sample.png

---


= How to and how not to enter text destined for TeX

The rest of the "figures.rb" file gives examples of many of the ways you can use text
and graphics by combining TeX and PDF.  Use it as a source of ideas and a jumping off
point to get more information.  Let's take a quick look.

link:images/parabox.png

You may never have reason to use TeX to format a paragraph as part of a figure,
but now you know you can if you need to.  You are much more likely to use math typesetting,
or some of the other nice things TeX provides.

link:images/math.png

The "math_typesetting" routine in "figures.rb"
can be a useful place to start in putting fancy TeX in one of your own figures or plots.
Here's the "inline" version of the TeX for the equation, followed by the changes to make
the "display" version:

        equation = # inline version
            '\int_{-\infty}^{\infty} e^{\color{Red}-x^{2}}\, \! dx = ' +
            '\color{Green}\sqrt{\pi}'
        # display version puts it in a displaymath and a parbox
        equation = '\begin{displaymath}' + equation + '\end{displaymath}'
        equation = '\parbox{15em}{' + equation + '}'

There are several things to notice here: comments are introduced by "#",
strings can be delimited by single quotes, and concatenation of strings
can be done with "+" notation.  Earlier in this tutorial, we used a string
with double quotes without explaining what was going on.  The crucial difference
between the single vs. double quoted strings is the degree of substitution done
inside the contents of the string: very little for single quotes, lots for double quotes.
Inside single quotes, it is basically what-you-see-is-what-you-get.  The only exceptions
are \\\ which becomes a single backslash and \' which let's you embed a single quote
inside a singly quoted string.  Inside double quotes, you can insert expressions to be
evaluated, converted to a string representation, and substitued into the string.  That's
what was happening when we wrote the following in our test of TeX offsets:

    puts "tex_yoffset is #{t.tex_yoffset}"

The "#{t.tex_yoffset}" part is replaced by a string representing the current value of the expression
t.tex_yoffset.  So if the yoffset is set to -3.55, then the string passed as argument to puts will be
"tex_yoffset is -3.55".  In addition to this expression substitution, double quoted strings
support many escape characters, all using backslashes (e.g., \n, \t, and lots others).
At this point, we should stop to say thank you dear Ruby, for providing singly quoted strings so that the backslashes for
TeX won't get confused with the backslashes for escape characters.  Here's what the equation
text would need to look like with double quotes:

        equation = # inline version -- using double quoted string
            "\\int_{-\\infty}^{\\infty} e^{\\color{Red}-x^{2}}\, \\! dx = " +
            "\\color{Green}\\sqrt{\\pi}"

Undoubtedly, many if not all of those extra backslashes could be skipped in this particular
example.  But consider this one, given in various forms:

   1    '\tau \sim \nu'               the good
   2    "\tau \sim \nu"               the bad
   3    "\\tau \\sim \\nu"            the ugly
   4    "\\tau \sim \\nu"             maybe ok, maybe not

Number 1 is standard TeX that just naturally comes rolling off the fingers.
Number 2 is a disaster since the "\t" and "\n" will be
replaced by tab and newline.  Number 3 is valid, but I never remember to type all the
extra backslashes.  Number 4 is also valid (I think), at least it is if "\s" is not an escape sequence.
Oops, I just checked and "\s" is actually replaced by a space, so that one's wrong too.
The take-home message is get in the habit of using single quotes unless you really need to do substitutions.


For all listing of the methods and attributes in tioga that related to TeX text, see Tioga::TeX_Text.

---

Now let's look at the extras in tioga for making plots -- next stop: Plots.


=end

module TextForTeX
end # module TextForTeX

end # module Tutorial
end # module Tioga

