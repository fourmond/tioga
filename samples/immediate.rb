# immediate.rb

# to get the pdf's created, just run this file.
# ruby < immediate.rb

require 'rubygems'
require 'Tioga/FigureMaker'

include Tioga
include FigureConstants

FigureMaker.exec do |t| 
   # t is FigureMaker.default
   t.default_page_width = 72*3
   t.default_page_height = 72*2
   t.default_enter_page_function
   t.make_pdf('blue') do
      t.fill_color = Blue
      t.fill_frame
   end   
   t.make_pdf('green') do
      t.fill_color = Green
      t.fill_frame
   end   
end

# here's an alternative way to make a pdf
FigureMaker.make_pdf('red') do |t|
   t.default_page_width = 72*2
   t.default_page_height = 72*3
   t.default_enter_page_function
   t.fill_color = Red
   t.fill_frame
end   




