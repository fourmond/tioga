# multipage.rb

require 'Tioga/FigureMaker'
include Tioga

class MyFigures

   include Tioga
   include FigureConstants

   def t
      @figure_maker
   end

   def initialize
      @figure_maker = FigureMaker.default
      t.def_eval_function { |str| eval(str) }
      t.def_figure("one_page") { one_page }
      t.save_dir = 'figures_out'
      @page_number = 0
   end

   def one_page # show the value of @page_number
      t.fill_color = Red
      t.fill_frame
      t.show_label('text'=>sprintf('%i',@page_number),
         'x' => 0.5, 'y' => 0.5, 'scale' => 2)
   end

   def do_one(n)
      @page_number = n
      t.make_pdf('one_page')
      syscmd = 'mv ' + t.save_dir + '/one_page.pdf page_' + n.to_s + '.pdf '
      puts syscmd
      system(syscmd)
   end

   def do_lots(first,cnt)
      cnt.times { |i| do_one(first+i) }
   end

end

MyFigures.new
FigureMaker.default.eval_function("do_lots(1,5)")
