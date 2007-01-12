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

require 'tk'
require 'Tioga/tioga.rb'

class TiogaPointClick  
  
  include Tioga
  
  def fm
    FigureMaker.default
  end
  
  
  def make_all_pdfs
    return unless @have_loaded
    fm.num_figures.times { |i| 
        if fm.figure_pdfs[i] == nil
          require_pdf(i)
        end
      }
  end
  
  def make_portfolio(name = nil)
    return unless @have_loaded
    make_all_pdfs
    name = @title_name + '_portfolio' if name == nil
    puts "#{name}"
    portfolio_name = fm.make_portfolio(name)
    puts "#{portfolio_name}"
    view_pdf(portfolio_name)
    return unless $mac_command_key
    puts " "
    puts "Note: Preview fails to make updated thumbnails after a Revert for a portfolio,"
    puts "so for now you'll have to Close and redo Open as a workaround."
    puts " "
  end


  def require_pdf(num)
    puts "\n#{fm.figure_names[num]}"
    begin
      result = fm.require_pdf(num)
      puts result
      return result
    rescue
    end
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


  def view_pdf(pdf_file)
    system($pdf_viewer + ' ' + pdf_file + " > /dev/null")
  end
  
  
  def figureSelected
    preview(@listBox.curselection[0])
  end


  def loadfile(fname, reselect=true)
    @have_loaded = false
    fm.reset_state
    begin
      puts "\nloading #{fname}"
      load(fname) # this should define the TiogaFigures class
      num_fig = fm.num_figures
      if num_fig == 0 
          raise "Failed to define any figures.  ' +
            'Remember to invoke 'new' for the class containing the figure definitions"
      end
      @title_name = fname.split('/')[-1]
      @title_name = @title_name[0..-4] if @title_name[-3..-1] == ".rb"
      @root.title('Tioga:' + @title_name)
      @listBox.delete(0, 'end')
      fm.figure_names.each { |name| @listBox.insert('end', name) }
      fname = fname[0..-4] if fname[-3..-1] == ".rb"
      @pdf_name = fname + ".pdf"
      @have_loaded = true
      set_selection(0) if reselect
    rescue Exception => er
      report_error(er, "ERROR: load failed for #{fname}\n")
    end
  end

  
  def report_error(er, msg)
    puts msg
    puts " "
    puts "    " + "#{er.message}"
    line_count = 0
    er.backtrace.each do |line|
        if line_count < fm.num_error_lines
            puts "    " + line
        end
        line_count = line_count + 1
    end
  end
  
  
  def set_selection(num)
    @listBox.see(num)
    @listBox.selection_clear(0,'end')
    @listBox.selection_set(num)
    figureSelected
  end
  
  
  def reload
    return unless @have_loaded
    selection = @listBox.curselection[0]
    name = (selection.kind_of?(Integer))? fm.figure_names[selection] : nil
    loadfile(@tioga_filename, false)
    num = fm.figure_names.index(name)
    unless num.kind_of?(Integer)
      reset_history
      num = 0
    end
    set_selection(num)
  end 

  
  def next_in_list
    return unless @have_loaded
    num = @listBox.curselection[0] + 1
    num = 0 if num >= @listBox.size
    set_selection(num)
  end

  
  def prev_in_list
    return unless @have_loaded
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
        puts "\neval " + str
        result = fm.eval_function(str)
        puts result
    rescue Exception => er
      report_error(er, "ERROR: eval failed for #{str}\n")
    end
  end

 
  def openDocument
    filetypes = [["Ruby Files", "*.rb"]]
    filename = Tk.getOpenFile('filetypes' => filetypes,
                              'parent' => @root)
    return unless filename.kind_of?String && filename.length > 0
    set_working_dir(filename)
    loadfile(filename)
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


  def createMenubar
    menubar = TkFrame.new(@root) { background 'WhiteSmoke' }
    
    addTiogaMenu(menubar)
    addFileMenu(menubar)
    addToolsMenu(menubar)
 
    menubar.pack('side' => 'top', 'fill' => 'x', 'padx' => 8, 'pady' => 8)
  end
 
 
  def createFigureList
    # Figure List
    listFrame = TkFrame.new(@root) { background 'WhiteSmoke' }
    listBox = TkListbox.new(listFrame) {
      selectmode 'single'
      background 'white'
      borderwidth 0
      height 6
    }
    scrollBar = TkScrollbar.new(listFrame) {
      command proc { |*args| listBox.yview(*args) } }
    listBox.yscrollcommand(proc { |first, last|
      scrollBar.set(first, last)
    })
    
    listBox.bind('ButtonRelease-1') { figureSelected }
 
    spacer = TkFrame.new(listFrame) { background 'WhiteSmoke' }
    spacer.pack('side' => 'left', 'padx' => 4) 
    listBox.pack('side' => 'left', 'fill' => 'both', 'expand' => true, 'pady' => 2)
    scrollBar.pack('side' => 'right', 'fill' => 'y', 'pady' => 3)
    listFrame.pack('side' => 'top', 'fill' => 'both', 'expand' => true)
 
    @listBox = listBox
  end
  
  
  def createEvalField
  
    evalFrame = TkFrame.new(@root, 'background' => 'WhiteSmoke') do
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
  
  
  def set_working_dir(filename)
    if $change_working_directory && filename[0..0] == '/'
      
      parts = filename.split('/')
      if parts[-1].length < 2 || parts[-1][-2..-1] != "rb"
        puts "ERROR: filename must have extension 'rb'   instead has <" + parts[-1][-2..-1] + ">"
        exit
      end
      dir = ""
      parts[0..-2].each {|part| dir << '/' + part unless part.length == 0 }
      puts " "
      puts filename
      
      puts "changing working directory to " + dir
      Dir.chdir(dir) # change current working directory
    end
  end

 
  def initialize(filename)
      
    # set the standard defaults
    $pdf_viewer = "xpdf"
    $geometry = '250x180+750+50'
    $background = 'WhiteSmoke'
    $mac_command_key = false
    $change_working_directory = true
    
    tpcrcname = ENV['HOME'] + '/.tpcrc'
    tpcrc = File.open(tpcrcname, 'r')
    if (tpcrc != nil)
      tpcrc.close
      load(tpcrcname)
    end
    
    set_working_dir(filename)
    
    @root = TkRoot.new { 
      geometry $geometry
      background $background
      pady 2
      }

    @tioga_filename = filename
    @pdf_name = nil
    
    @accel_key = ($mac_command_key)? 'Cmd' : 'Ctrl'
    @bind_key = ($mac_command_key)? 'Command' : 'Control'
    
    @history = [ ]
    resetHistory
 
    createMenubar
    createFigureList
    createEvalField
    @root.bind('Key-Up', proc { prev_in_list })
    @root.bind('Key-Down', proc { next_in_list })
    @root.bind('Key-Left', proc { back })
    @root.bind('Key-Right', proc { forward })
    
    loadfile(filename) unless filename == ""
    Tk.mainloop
    
  end
  
end

TiogaPointClick.new(ARGV[0])

