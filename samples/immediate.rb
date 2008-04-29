# immediate.rb

# to get the pdf's created, just run this file.
# ruby < immediate.rb

require 'rubygems'
require 'Tioga/FigureMaker'

include Tioga
include FigureConstants

def page_style(t,w,h)
   t.default_page_width = 72*w
   t.default_page_height = 72*h
   t.default_enter_page_function
end

FigureMaker.exec do |t| 
   # t is FigureMaker.default
   t.make_pdf('blue') do
      page_style(t,2,3)
      t.fill_color = Blue
      t.fill_frame
   end   
   t.make_pdf('green') do
      page_style(t,3,3)
      t.fill_color = Green
      t.fill_frame
   end   
end

# here's an alternative way to make a pdf
FigureMaker.make_pdf('red') do |t|
   page_style(t,3,2)
   t.fill_color = Red
   t.fill_frame
end   




