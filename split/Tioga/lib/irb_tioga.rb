require "Tioga/tioga_ui_cmds.rb"

module Tioga
module IRB_Tioga
  
$tioga_figure_filename = nil
$tioga_figure_num = nil

def o(filename=nil)
  filename = $tioga_figure_filename if filename == nil
  filename = $tioga_ui.fix_filename(filename)
  result = $tioga_ui.setdir_and_load(filename)
  return false if result == nil
  $tioga_figure_filename = result
  $tioga_figure_num = 0
  return true
end

def do_figs(fignums,view)
  if fignums.kind_of?String
    fignums = $tioga_ui.parse_figs(fignums)
  end
  if fignums == nil
    $tioga_ui.make_all_pdfs(view)
  elsif fignums.kind_of?Integer
    fignums += FigureMaker.default.num_figures if fignums < 0
    $tioga_ui.do_fignums([fignums],view)
  else
    $tioga_ui.do_fignums(fignums,view)
  end
  set_figure_num(fignums[-1])
  return true
end

def s(fignums=nil)
  do_figs(fignums,true)
  return true
end

def m(fignums=nil)
  do_figs(fignums,false)
  return true
end

def set_figure_num(num)
  num = FigureMaker.default.figure_names.index(num) unless num == nil || num.kind_of?(Integer)
  $tioga_figure_num = num
end

def p(fignums=nil)
  if fignums.kind_of?String
    fignums = $tioga_ui.parse_figs(fignums)
  end
  if fignums == nil
    $tioga_ui.make_portfolio(true)
  elsif fignums.kind_of?Integer
    fignums += FigureMaker.default.num_figures if fignums < 0
    $tioga_ui.make_portfolio(true,[fignums])
  else
    $tioga_ui.make_portfolio(true,fignums)
  end
  set_figure_num(fignums[-1])
  return true
end

def r
  if $tioga_figure_num != nil
    figname = FigureMaker.default.figure_names[$tioga_figure_num] 
  else
    figname = nil
  end
  o(nil)
  if figname != nil
    num = FigureMaker.default.figure_names.index(figname)
    if num != nil
      s(num)
    end
  end
end


def l
  $tioga_ui.list_figures
  return true
end

def h
    puts ''
    puts "   Command               description"
    puts "   o 'filename'          open tioga file (with extension .rb)."
    puts "   l                     output a list of the defined figures by number and name."
    puts "   m <figs>              make PDFs without showing them in the viewer."
    puts "   s <figs>              make and show PDFs, each in a separate viewer window."
    puts "   p <figs>              make PDFs and show the portfolio as a multi-page document."
    puts "   r                     reloads the current tioga file and reshows the current figure."
    puts "   h                     helpfully print this list of commands."
    puts "\n   Since the filename extension is known, you can skip typing it if you like."
    puts "\n   If <figs> is omitted, then tioga does all the figures defined in the file"
    puts "        ordered by their definition index numbers."
    puts "\n   Otherwise, <figs> must be either"
    puts "        a valid ruby array index number for a figure (can be negative), or"
    puts "        an array of index numbers for a set of figures, or"
    puts "        a string with a space-less, comma-separated list of figure indices and ranges, or"
    puts "        a string giving a defined figure name (as supplied to def_figure in the tioga file)."
    puts ''
    puts "        For example, s 'Plot1' makes and shows the pdf for the figure named Plot1,"
    puts "        and p '5,0..3,-1' makes a portfolio with the figure having index 5 on page 1,"
    puts "        followed by pages showing the figures with indices 0, 1, 2, 3, and -1."
    puts "\n   The viewer for showing PDFs is specified by the $pdf_viewer variable."
    puts "        The default value can be set by creating a .tiogainit file in your home directory."
    puts "        Your current setting for $pdf_viewer is " + $pdf_viewer + '.'
    puts "        To change it, edit ~/.tiogainit to add the line $pdf_viewer = 'my viewer command'."
    puts "        The command tioga uses to show a pdf is $pdf_viewer + ' ' + full_PDF_filename."
    puts ''

    return true
end

end # module IRB_Tioga
end # module Tioga
  
$tioga_ui = TiogaUI.new([],true)