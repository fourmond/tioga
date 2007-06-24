# plots_batch_driver.rb

# This is a simple example of using tioga in "batch mode".
# It makes copies of the Blues plot with 3 different data sets.

# Simply run this in ruby
#    ruby < plots_driver.rb
# It should produce terminal output as follows:
#
#    Making multiple versions of the Blues plot
#    mv plots_out/Blues.pdf Blues1.pdf
#    mv plots_out/Blues.pdf Blues2.pdf
#    mv plots_out/Blues.pdf Blues3.pdf
# 


require 'Tioga/tioga'
include Tioga

load 'plots.rb' 

class MyPlots # add to the methods for MyPlots
   
   def run
      puts "Making multiple versions of the Blues plot"
      do_one('Blues', 'datalog.data', 'Blues1.pdf')
      do_one('Blues', 'datalog2.data', 'Blues2.pdf')
      do_one('Blues', 'datalog3.data', 'Blues3.pdf')
   end
   
   def do_one(figure_name, datafile, pdffile)
      @data_filename = 'data/' + datafile
      t.need_to_reload_data = true
      t.make_pdf(figure_name)
      syscmd = 'mv ' + t.save_dir + '/' + figure_name + '.pdf ' + pdffile
      puts syscmd
      system(syscmd)
   end
   
end

FigureMaker.default.eval_function('run')