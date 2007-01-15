# tpc.rb -- Tioga-Point-and-Click interface based on Ruby/Tk

=begin
   Copyright (C) 2007  Bill Paxton

   This file is part of Tioga.

   Tioga is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Library Public License as published
   by the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.

   Tioga is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with Tioga; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
=end

require 'Tioga/tioga.rb'

class TiogaUI  
  
  include Tioga
  
  
  def fm
    FigureMaker.default
  end

  
  def append_to_log(str)
    if @batch_mode
      puts str
      return
    end
    return if @logText == nil
    return unless str.kind_of?String
    @logText.insert('end', str + "\n")
    @logText.see('end')
  end
  
  
  def check_have_loaded
    return true if @have_loaded
    append_to_log "Must open a file first!"
    return false
  end
  
  
  def make_all_pdfs(view = true)
    return unless check_have_loaded
    fm.num_figures.times { |i|
        pdf_name = require_pdf(i)
        append_to_log pdf_name unless view
        view_pdf(pdf_name) if view && pdf_name != nil
      }
  end
  
  
  def make_portfolio(view = true)
    return unless check_have_loaded
    name = @title_name + '_portfolio'
    make_all_pdfs(false)
    portfolio_name = fm.make_portfolio(name)
    return unless view
    view_pdf(portfolio_name)
    return if @batch_mode
    return unless $mac_command_key
    append_to_log "\nNote: Preview fails to make updated thumbnails after a Revert for a portfolio,"
    append_to_log "so for now you'll have to Close and redo Open as a workaround.\n"
  end


  def require_pdf(arg) # num is either figure number or name
    begin
      num = arg
      num = fm.figure_names.index(num) unless num == nil || num.kind_of?(Integer)
      if fm.num_figures == 0
        puts "\nCan't build pdf because failed to define any figures."
        puts ''
        raise
      end
      if num == nil || num < 0 || num >= fm.num_figures
        puts "\n" + arg.to_s + ' is an invalid figure specification.'
        puts ''
        raise
      end
      result = fm.require_pdf(num)
      if result == nil
        puts "\nFailed during attempt to create pdf file."
        puts ''
        raise
      end
      #append_to_log result + "\n"
      return result
    end
  end


  def view_pdf(pdf_file)
    if pdf_file == nil || pdf_file == false
      puts "\nERROR: invalid pdf file."
      puts ''
      raise
    end
    system($pdf_viewer + ' ' + pdf_file)
  end


  def loadfile(reselect=true)
    @have_loaded = false
    fm.reset_state
    begin
      
      fname = $filename
      append_to_log "loading #{fname}\n"
      load(fname) # this should define the TiogaFigures class
      num_fig = fm.num_figures
      if num_fig == 0 
          raise "Failed to define any figures.  ' +
            'Remember to invoke 'new' for the class containing the figure definitions"
      end
      
      @title_name = fname.split('/')[-1]
      @title_name = @title_name[0..-4] if @title_name[-3..-1] == ".rb"
      fname = fname[0..-4] if fname[-3..-1] == ".rb"
      @pdf_name = fname + ".pdf"
      @have_loaded = true
      
      return if @batch_mode
      
      @root.title('Tioga:' + @title_name)
      @listBox.delete(0, 'end')
      fm.figure_names.each { |name| @listBox.insert('end', name) }
      set_selection(0) if reselect
      
    rescue Exception => er
      report_error(er, "ERROR: load failed for #{fname}\n")
    end
  end
  
  
  def set_working_dir(filename)
    return unless $change_working_directory
    
    # if necessary, add the current directory to the front of the filename
    filename = Dir.getwd + '/' + filename if filename[0..0] != '/'
    
    parts = filename.split('/')
    if parts[-1].length < 2 || parts[-1][-2..-1] != "rb"
      append_to_log "ERROR: filename must have extension 'rb'   instead has <" + parts[-1][-2..-1] + ">"
      exit
    end
    dir = ""
    parts[0..-2].each {|part| dir << '/' + part unless part.length == 0 }
    append_to_log " "
    append_to_log filename
    
    if dir != Dir.getwd
      append_to_log "changing working directory to " + dir
      Dir.chdir(dir) # change current working directory
    end
    
    $filename = filename

  end
  
  
  def show_help(filename,opt1)
    unless opt1 == '-help' || filename == '-help' || filename == nil
      puts 'Sorry: ' + opt1 + ' is not a recognized option.' 
    end
    puts "\nThis is a brief description of the command line options for tioga."
    puts "\nThey include the usual help and version commands."
    puts '    -help       print this message'
    puts '    -v          print the tioga version information'
    puts "\nOther commands all start with the name of a tioga ruby file (with extension .rb)."
    puts "     BTW: since the extension is known, you can skip typing it if you like."
    puts "\nThe input file need not be in your current working directory;"
    puts "     tioga automatically changes its working directory to the directory containing the file,"
    puts "     and that's also the location for the created PDFs."
    puts "\nIf there are no other items on the command line, tioga shows the first figure defined in the file."
    puts "\nIf there are other items, the rest of the command line should be one of the following cases."
    puts '    -l          list the defined figures by number and name'
    puts '    -<num>      show a figure PDF: <num> is the figure number, starting from 0'
    puts '    -s <fig>    make and then show a figure PDF: <fig> is the figure name or number'
    puts '    -s          make and then show all the figure PDFs, each in a separate viewer window'
    puts '    -m <fig>    make a figure PDF without showing it'
    puts '    -m          make all the figure PDFs without showing them'
    puts '    -p          make all the PDFs and show a portfolio combining them as a single, multi-page PDF'
    puts "\nThe viewer for showing PDFs is specified by the $pdf_viewer variable in tioga."
    puts "     That variable can be set by creating a .tiogainit file in your home directory."
    puts "     Your current setting for $pdf_viewer is " + $pdf_viewer + '.'
    puts "     To change it, edit ~/.tiogainit to add the line $pdf_viewer = 'my viewer command'"
    puts "     The shell command tioga uses for show is the $pdf_viewer string followed by the PDF filename."
    puts "\nTo facilitate the use of this interface from scripts, you can insert the following immediately"
    puts '     after the tioga figures filename and before any of the options listed above.'
    puts "     -x <filename>  run the named ruby file before loading the tioga figures file"
    puts "\nFor more information, visit http://theory.kitp.ucsb.edu/~paxton/tioga.html"
    puts ''
  end


  def report_error(er, msg)
    append_to_log msg
    append_to_log " "
    append_to_log "    " + "#{er.message}"
    line_count = 0
    reached_here = false
    er.backtrace.each do |line|
        reached_here = true if line.include?('Tioga/tioga_ui.rb')
        if (line_count < fm.num_error_lines) and (reached_here == false)
            append_to_log "    " + line
        end
        line_count = line_count + 1
    end
  end
  
  
  def setdir_and_load
    set_working_dir($filename)
    loadfile
  end

 
  def initialize(args)
    
    # set the standard defaults
    $tioga_args = args
    $pdf_viewer = "xpdf"
    $change_working_directory = true
 
=begin     
    # Ruby/Tk defaults
    $geometry = '600x250+700+50'
    $background = 'WhiteSmoke'
    $mac_command_key = false
    $log_font = 'system 12'
    $figures_font = 'system 12'
=end
    
    tiogainit_name = ENV['HOME'] + '/.tiogainit'
    if File.exist?(tiogainit_name)
      load(tiogainit_name)
    end
    
    # check for an initialization file
    if ($tioga_args.length >= 3) && ($tioga_args[1] == '-x')
      file = File.open($tioga_args[2], 'r')
      if (file != nil)
        file.close
        load($tioga_args[2])
      end
      opt1 = $tioga_args[3] if $tioga_args.length >= 4
      opt2 = $tioga_args[4] if $tioga_args.length >= 5
    else
      opt1 = $tioga_args[1] if $tioga_args.length >= 2
      opt2 = $tioga_args[2] if $tioga_args.length >= 3
    end

    $filename = $tioga_args[0] if $tioga_args.length >= 1

    if $filename != nil && $filename[-3..-1] != '.rb' && 
        $filename[-3..-1] != '.RB' && $filename != '-help' && $filename != '-v'
      $filename += '.rb'
    end
    
    @pdf_name = nil
    @have_loaded = false

    if (true)
      
      begin
        # currently, we are only supporting command line interface.
        # the code for making a Ruby/Tk interface can be enabled if you want to play with it.
      
        @batch_mode = true
      
        if $filename == nil || $filename == '-help'
          show_help(nil,nil)
        elsif $filename == '-v'
          puts FigureMaker.version
        elsif opt1 == nil
          setdir_and_load
          view_pdf(require_pdf(0))
        elsif opt1 == '-l'
          setdir_and_load
          fm.figure_names.each_with_index { |name,i| puts sprintf("%3i    %s\n",i,name) }
        elsif opt1 != nil && (opt1.kind_of?String) && (/^\d+$/ === opt1[1..-1])
          setdir_and_load
          view_pdf(require_pdf(opt1[1..-1].to_i))
        elsif (opt1 == '-s' || opt1 == '-m') 
          setdir_and_load
          if opt2 != nil
            opt2 = opt2.to_i if (/^\d+$/ === opt2)
            pdf_name = require_pdf(opt2)
            if opt1 == '-s'
              view_pdf(pdf_name)
            else
              puts pdf_name
            end
          else
            make_all_pdfs(opt1 == '-s')
          end
        elsif opt1 == '-p'
          setdir_and_load
          if fm.num_figures == 1
            view_pdf(require_pdf(0))
          else
            make_portfolio(true) # make and show
          end
        else # unrecognized command
          show_help($filename,opt1)
        end
      rescue
      end
      
      return
    
    end

    # call start_Tk here if you want to use the Ruby/Tk interface
    
  end
  


=begin  

  def start_Tk
    # Ruby/Tk
    
    @batch_mode = false
    
    @history = [ ]
    resetHistory
    
    @accel_key = ($mac_command_key)? 'Cmd' : 'Ctrl'
    @bind_key = ($mac_command_key)? 'Command' : 'Control'

    require 'tk'
   
    @root = TkRoot.new { 
      geometry $geometry
      background $background
      pady 2
      }
 
    createMenubar(@root)
    contentFrame = TkFrame.new(@root) { background 'WhiteSmoke' }
    createFigureList(contentFrame)
    createLogText(contentFrame)
    contentFrame.pack('side' => 'top', 'fill' => 'both', 'expand' => true)
    createEvalField(@root)
    @root.bind('Key-Up', proc { prev_in_list })
    @root.bind('Key-Down', proc { next_in_list })
    @root.bind('Key-Left', proc { back })
    @root.bind('Key-Right', proc { forward })
    
    loadfile unless $filename == nil
    Tk.mainloop(false)
  end
  
  
  def figureSelected
    preview(@listBox.curselection[0])
  end
  
  
  def preview(num)
    result = require_pdf(num)
    return result if $pdf_viewer == nil
    syscmd = "cp " + result + " " + @pdf_name
    system(syscmd)
    saveInHistory(num)
    return view_pdf(@pdf_name)
  end
  
  
  def show_in_own_window
    view_pdf(fm.figure_pdfs[@listBox.curselection[0]])
  end
  
  
  def set_selection(num)
    @listBox.see(num)
    @listBox.selection_clear(0,'end')
    @listBox.selection_set(num)
    figureSelected
  end
  
  
  def reload
    return unless check_have_loaded
    selection = @listBox.curselection[0]
    name = (selection.kind_of?(Integer))? fm.figure_names[selection] : nil
    loadfile(false)
    num = fm.figure_names.index(name)
    unless num.kind_of?(Integer)
      reset_history
      num = 0
    end
    set_selection(num)
  end 

  
  def next_in_list
    return unless check_have_loaded
    num = @listBox.curselection[0] + 1
    num = 0 if num >= @listBox.size
    set_selection(num)
  end

  
  def prev_in_list
    return unless check_have_loaded
    num = @listBox.curselection[0] - 1
    num = @listBox.size - 1 if num < 0
    set_selection(num)
  end


  def resetHistory
    @history_loc = -1
    @history_len = 0
    @forward_back = false
  end
 
 
  def saveInHistory(num)
    if @forward_back
      forward_back = false
      return
    end
    return if (@history_len > 0 && @history_loc >= 0 && @history[@history_loc] == num)
    @history_len = @history_loc + 2
    @history_loc = @history_len - 1
    @history[@history_loc] = num
  end


  def back
    return if (@history_loc <= 0 || @history_len == 0)
    @history_loc = @history_loc - 1
    @forward_back = true
    set_selection(@history[@history_loc])
  end


  def forward
    return if (@history_loc + 1 >= @history_len)
    @history_loc = @history_loc + 1
    @forward_back = true
    set_selection(@history[@history_loc])
  end
  
  
  def eval
    begin
        str = @evalEntry.get
        append_to_log "eval " + str
        result = fm.eval_function(str)
        append_to_log result.to_s + "\n"
    rescue Exception => er
      report_error(er, "ERROR: eval failed for #{str}\n")
    end
  end
  
 
  def openDocument
    filetypes = [["Ruby Files", "*.rb"]]
    filename = Tk.getOpenFile('filetypes' => filetypes,
                              'parent' => @root)
    return unless (filename.kind_of?String) && (filename.length > 0)
    set_working_dir(filename)
    loadfile
  end


  def addFileMenu(menubar)
    fileMenuButton = TkMenubutton.new(menubar,
                                      'text' => 'File',
                                      'background' => 'WhiteSmoke',
                                      'underline' => 0)
    fileMenu = TkMenu.new(fileMenuButton, 'tearoff' => false)
 
    fileMenu.add('command',
                 'label' => 'Open',
                 'command' => proc { openDocument },
                 'underline' => 0,
                 'accel' => @accel_key + '+O')
    @root.bind(@bind_key + '-o', proc { openDocument })
         
 
    fileMenu.add('command',
                 'label' => 'Reload',
                 'command' => proc { reload },
                 'underline' => 0,
                 'accel' => @accel_key + '+R')
    @root.bind(@bind_key + '-r', proc { reload })
 
    fileMenuButton.menu(fileMenu)
    fileMenuButton.pack('side' => 'left')
  end
  
  
  def addToolsMenu(menubar)
    toolsMenuButton = TkMenubutton.new(menubar,
                                      'text' => 'Tools',
                                      'background' => 'WhiteSmoke',
                                      'underline' => 0)
    toolsMenu = TkMenu.new(toolsMenuButton, 'tearoff' => false)
    
    acc = ($mac_osx)
 
    toolsMenu.add('command',
                 'label' => 'Portfolio PDF',
                 'command' => proc { make_portfolio },
                 'underline' => 0,
                 'accel' => @accel_key + '+P')
    @root.bind(@bind_key + '-p', proc { make_portfolio })
 
    toolsMenu.add('command',
                 'label' => 'Make All PDFs',
                 'command' => proc { make_all_pdfs },
                 'underline' => 0,
                 'accel' => @accel_key + '+M')
    @root.bind(@bind_key + '-m', proc { make_all_pdfs })
 
    toolsMenu.add('command',
                 'label' => 'Show in Own Window',
                 'command' => proc { show_in_own_window },
                 'underline' => 0,
                 'accel' => @accel_key + '+S')
    @root.bind(@bind_key + '-s', proc { show_in_own_window })
 
    toolsMenuButton.menu(toolsMenu)
    toolsMenuButton.pack('side' => 'left')
  end


  def addTiogaMenu(menubar)
    tiogaMenuButton = TkMenubutton.new(menubar,
                                      'text' => 'Tioga',
                                      'background' => 'WhiteSmoke',
                                      'underline' => 0)
    tiogaMenu = TkMenu.new(tiogaMenuButton, 'tearoff' => false)
 
    tiogaMenu.add('command',
                 'label' => 'About Tioga',
                 'command' => proc { showAboutBox },
                 'underline' => 0)
 
    tiogaMenu.add('separator')
 
    tiogaMenu.add('command',
                 'label' => 'Quit',
                 'command' => proc { exit },
                 'underline' => 0,
                 'accel' => @accel_key + '+Q')
    @root.bind(@bind_key + '-q', proc { exit })
 
    tiogaMenuButton.menu(tiogaMenu)
    tiogaMenuButton.pack('side' => 'left')
  end

 
  def showAboutBox
      Tk.messageBox('icon' => 'info', 'type' => 'ok',
        'title' => 'About Tioga-Point-and-Click',
        'parent' => @root,
        'message' => "Tioga-Point-and-Click is a Ruby/Tk Application.\n" +
            "It uses the Tioga kernel to create PDFs and then calls your favorite viewer to show them.\n\n" +
            "Version 0.1  -- January, 2007\n\n" +
            "Visit http://theory.kitp.ucsb.edu/~paxton/tioga.html")
  end


  def createMenubar(parent)
    menubar = TkFrame.new(parent) { background 'WhiteSmoke' }
    
    addTiogaMenu(menubar)
    addFileMenu(menubar)
    addToolsMenu(menubar)
 
    menubar.pack('side' => 'top', 'fill' => 'x', 'padx' => 8, 'pady' => 8)
  end
  
  
  def createLogText(parent)
    
    logFrame = TkFrame.new(parent) { background 'WhiteSmoke' }
    
    logText = TkText.new(logFrame) {
      borderwidth 0
      selectborderwidth 0
      height 6
      font $log_font
    }

    scrollBar = TkScrollbar.new(logFrame) { command proc { |*args| logText.yview(*args) } }
    logText.yscrollcommand(proc { |first, last| scrollBar.set(first, last) })
 
    scrollBar.pack('side' => 'right', 'fill' => 'y', 'pady' => 3)
    logText.pack('side' => 'right', 'fill' => 'both', 'expand' => true, 'pady' => 2)
    
    logFrame.pack('side' => 'right', 'fill' => 'both', 'expand' => true)
    
    @logText = logText
  end
 
 
  def createFigureList(parent)

    listFrame = TkFrame.new(parent) { background 'WhiteSmoke' }
    listBox = TkListbox.new(listFrame) {
      selectmode 'single'
      background 'white'
      borderwidth 0
      height 6
      font $figures_font
    }
    scrollBar = TkScrollbar.new(listFrame) { command proc { |*args| listBox.yview(*args) } }
    listBox.yscrollcommand(proc { |first, last| scrollBar.set(first, last) })
    
    listBox.bind('ButtonRelease-1') { figureSelected }
 
    spacer = TkFrame.new(listFrame) { background 'WhiteSmoke' }
    spacer.pack('side' => 'left', 'padx' => 4) 
    
    listBox.pack('side' => 'left', 'fill' => 'both', 'expand' => true, 'pady' => 2)
    scrollBar.pack('side' => 'right', 'fill' => 'y', 'pady' => 3)
    
    listFrame.pack('side' => 'left', 'fill' => 'both', 'expand' => true)
 
    @listBox = listBox
  end
  
  
  def createEvalField(parent)
  
    evalFrame = TkFrame.new(parent, 'background' => 'WhiteSmoke') do
    	pack('side' => 'bottom', 'fill' => 'x', 'pady' => 4)
    end
  
    evalLabel = TkLabel.new(evalFrame, 'background' => 'WhiteSmoke') do
      text ' eval'
      font 'courier 12'
    	pack('side' => 'left')
    end
  
    evalEntry = TkEntry.new(evalFrame) do
      borderwidth 0
    	pack('side' => 'left', 'fill' => 'x', 'expand' => true)
    end
    
    TkLabel.new(evalFrame, 'background' => 'WhiteSmoke') do
      text '  '
      font 'courier 12'
    	pack('side' => 'right')
    end    
    
    evalEntry.bind('Key-Return', proc { eval })
    
    @evalEntry = evalEntry

  end
  
=end  
  
end

TiogaUI.new(ARGV)

