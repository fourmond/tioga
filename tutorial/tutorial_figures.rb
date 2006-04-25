
module Tioga
module Tutorial

=begin rdoc

= How Tioga uses PDF and TeX to make figures

Find the "samples/figures" folder and
open "figures.rb" in your text editor.  A quick glance shows that the overall
structure is the same as in "sample.rb", there are just a lot more figures defined,
are the definitions are a lot more complicated.  Go back to irb, load this file, and
list the contents as follows:

    >> ld 'figures.rb'
    => "figures.rb"
    >> ls
      0 Icon
      1 Rounded_Rect
      2 Curve
      3 Arc
      4 Fill_Rules
      5 Line_Types
      6 Caps_and_Joins
      7 Rendering_Modes
      8 Marker_Names
      9 Text_J_and_A
     10 Test_Pattern
     11 Strings
     12 Arrows
     13 Marker_Horizontal_Scaling
     14 Marker_Vertical_Scaling
     15 Marker_Italic_Angle
     16 Marker_Ascent_Angle
     17 Marker_Shadow_Effect
     18 Framebox
     19 Parbox
     20 Minipages
     21 Minipage_List
     22 Minipage_Table
     23 Math_Typesetting
     24 Squares
     25 Sample_Jpegs
     26 Axial_Shading
     27 Radial_Shading
     28 Dingbats
     29 PDF_Fonts
     30 Subfigures
     31 Colors1
     32 Colors2
     33 Colors3
     34 Colors4
    => true
    
If you've glanced through the FigureMaker documentation, you've probably seen a lot of these
figures already.  Before looking at details, lets make a portfolio to see what's there:

    >> quiet
    >> ma
    => true
    >> mp
    => true

Open "figures.pdf" in a preview and see what it has.  You should find lots of pages,
starting with the "icon".

By the way: since Ruby is a scripting language, it lets you
make system calls, so you can open the file directly from irb if you wish.  On my
Mac it is done by entering this:

    >> system('open figures_out/figures.pdf')
    
On Linux, you might type something like this:

    >> system('xpdf figures_out/figures.pdf')

---

Let's take a
quick detour to look at the "figures.tex" file too.  Open it in your TeX editor.
It starts by setting the "documentclass", then loads some standard TeX packages that Tioga needs,
and finally gives the contents as one figure per page using command sequences like this:

    \begin{figure}
    \ShowFigure{Icon}
    \end{figure}
    \clearpage

The definition of the "ShowFigure" command is included in the preamble.
It takes the name of the figure, puts the directory name on the
front (in this case, "figure_out/"), then passes that to "TiogaFigure".  The TiogaFigure
command is also one of the commands defined in the preamble.  The real work is carried out
by "TiogaFigureShow".

    \newcommand{\TiogaFigureShow}[1]{
        \rotatebox{0.0}{
        \begin{picture}(0,0)(0,0)
        \includegraphics[scale=1.0,clip]{#1_figure.pdf}
        \end{picture}
        \input{#1_figure.txt}}}

It basically does two things: first, it does an "includegraphics"
with the figure graphics, and then it does an "input" with the figure text.  For the
"Icon" figure, the graphics are in "Icon_figure.pdf" and the text is in "Icon_figure.txt",
both of which are found in the "figures_out" directory.  Go there and open "Icon_figure.pdf"
in a PDF previewer along with "Icon.pdf".  Also open "Icon_figure.txt" in a text editor.  
Buried in the jumble of the text file are the strings to show "Ruby", "PDF", and "TeX".
For each one, there is a "put" to place it, a "rotatebox" to rotate it, and a "scalebox"
to scale it.   The figure PDF file is imaged first to supply a backdrop, then the TeX text is
placed on top.

Notice that the complete figure is not actually created until the TeX document is typeset.
Only then is the text file, in this case "Icon_figure.txt", processed by TeX.  The result
is that you can produce all the figures in the form of background graphics and foreground
TeX text, and only later specify the exact details of how the text should be typeset.

For example, you can change all of the document fonts, retypeset the document, and it will
change the text in the figures to match the text in the body.  Pretty cool!  We can show this
with the icon by editing the "Icon.tex" file.  My local TeX system includes the +times+ package (from the +PSNFSS+
distribution), so by adding "\usepackage{times}" to the preamble, the ComputerModern fonts
are replaced by fonts from the Times family.  Here's what the icon looks like after doing that
one line change to the TeX file, without redoing anything in the figure files themselves.  The ComputerModern
sans serif font has been replaced by Helvetica in "Ruby" and "PDF", and even the TeX logo is different.

http://theory.kitp.ucsb.edu/~paxton/tioga_jpegs/Icon_times.jpg

Here's the icon with the ComputerModern fonts again after removing the one line that added the +times+ package.

http://theory.kitp.ucsb.edu/~paxton/tioga_jpegs/Icon_cm.jpg

Try it yourself.  Add the line "\usepackage{times}" the figures.tex in your TeX editor
and retypeset the document.  Check the font changes in the icon and in the other figures
as well.  Remove the line and typeset again.  The fonts should change back to the defaults.

Even if you never need to make wholesale changes in the fonts, it is nice to have the
same typography in figures as in the document body.  For example, if you are using Euler fonts in the
text, then it's nice to have Euler in the figures too
rather than switching to something else such as Times, Helvetica, or Courier.


---

= Alignment of Graphics and Text

As you can imagine, since the graphics and text are put down as separate layers, one from
PDF and the other from TeX, it is crucial that the layers be aligned properly.  Let's
check it.  Back in irb, make the "Test_Pattern" figure (do a "ls" to find the number again).
Open the "Test_Pattern.pdf" file in your PDF previewer.

It should look like this:

http://theory.kitp.ucsb.edu/~paxton/tioga_jpegs/Test_Pattern.jpg

If the text is happily sitting on the lines, and the "Left", "Center", and "Right" labels
are where they belong, all is well.  If not, some tweaking of the values of "tex_xoffset"
and "tex_yoffset" may help.  Even if the alignment is fine, let's change it just to see
how this works.  Open "figures.rb" and find the definition starting with the line "def test_pattern".
Add these two lines at the start of the definition:

    puts "tex_xoffset is #{t.tex_xoffset}"
    puts "tex_yoffset is #{t.tex_yoffset}"

Then reload the "figures.rb" file in irb, and remake the
test pattern figure.  You should get something like this from irb:

    >> rl
    => "figures.rb"
    >> mk 10
    tex_xoffset is -0.2
    tex_yoffset is -3.55

Who knows what those numbers mean?  Let's just set them to 0 and see what
happens.  Add these lines before the "puts" lines and repeat the previous to make another version of the figure.

    t.tex_xoffset = 0
    t.tex_yoffset = 0
    
Now the irb output should be like this:

    >> mk 10
    tex_xoffset is 0
    tex_yoffset is 0

Open this version of "Test_Pattern.pdf" in your previewer.  Hmmm.  No change that I can see.
Oh right!  We need to change the offsets BEFORE we start running the figure definition.  It needs
to be done in the initialization routine.  So move the two lines that set the offset to 0 from
the test_pattern definition to the initialization method and try again.  (You can place the
lines right after the line that does "@figure_maker = FigureMaker.default".)
Now I get this figure with really bad vertical alignment:

http://theory.kitp.ucsb.edu/~paxton/tioga_jpegs/bad_test_pattern.jpg

That should give you an idea for what's going on with graphics and text as separate layers.
Go back to the "figures.rb" file and remove the lines related to the offsets (or set them
to values that give you the alignment you want).

=end

module Figures
end # module Figures

end # module Tutorial
end # module Tioga

