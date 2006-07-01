
module Tioga
module Tutorial

=begin rdoc

= "Do it yourself" using the tioga kernel for making figures

It seems that making plots is an activity about which many people have strong opinions.
I think that's partly a result of systems that make it trivially easy to do plots the "standard" way
but painfully difficult to do them any other way.   We've probably all had frustrating experiences and
been left thinking "I could do that better".  

I've tried to reduce the frustrations by making it possible to work around problems through lower level
routines.  But I'm still sure that my way of structuring a plotting program will seem
off target to some users.  If you find yourself in that situation, consider using the tioga kernel
at the "figure maker" level to do PDF and TeX for you, and "do it yourself" for plotting.

Look back at the sections of the tutorial.  Most of them are not specific to plot making.
Most of the methods could just as
easily be used with your plotting system as mine.  And since it is structured as a Ruby extension,
you can build your own Ruby program that loads tioga as a utility package for writing PDF and TeX output.
And if you find places where you need a performance boost, the Ruby C API is great for that.

If you do your implementation using tioga as your graphics package, a lot of messy issues go away.
You can focus on specifying plots as figures, and let it deal with the problems of turning figures into PDF and TeX.
If you decide to go that route, you have my blessings and please let me know how it goes!

=end

module Epilog
end # module Epilog

end # module Tutorial
end # module Tioga

