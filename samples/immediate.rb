# immediate.rb

require 'rubygems'
require 'Tioga/FigureMaker'

include Tioga
include FigureConstants

FigureMaker.page_style do |t|
   t.default_page_width = 72*3
   t.default_page_height = 72*2
   t.default_enter_page_function
end

FigureMaker.make_pdf('blue') do |t|
  t.fill_color = Blue
  t.fill_frame
end

FigureMaker.make_pdf('red') do |t|
  t.fill_color = Red
  t.fill_frame
end
