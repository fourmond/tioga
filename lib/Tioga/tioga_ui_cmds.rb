
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
require "rbconfig.rb"
include RbConfig
  
include Tioga
include FigureConstants

class TiogaUI  
  
  def fm
    FigureMaker.default
  end

  
  def append_to_log(str)
    if @no_Tk
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
    
    
  def report_number_and_name(num,name)
    if num < 10
      puts '  ' + num.to_s + '  ' + name
    elsif num < 100
      puts ' ' + num.to_s + '  ' + name
    else
      puts num.to_s + '  ' + name
    end
  end
  
  
  def make_all_pdfs(view = true, fignums = nil)
    return unless check_have_loaded
    fm.make_all(fignums,false)
    if view == false
      if fignums == nil
        fm.figure_pdfs.each_with_index { |name,num| report_number_and_name(num,name) }
      else
        fignums.each { |num| report_number_and_name(num,fm.figure_pdfs[num]) }
      end
      return
    end
    if fignums == nil
      fm.num_figures.times { |i| view_pdf(fm.figure_pdfs[i]) }
    else
      fignums.each { |i| view_pdf(fm.figure_pdfs[i]) }
    end
  end
  
  
  def make_portfolio(view = true, fignums = nil)
    return unless check_have_loaded
    name = @title_name + '_portfolio'
    make_all_pdfs(false,fignums)
    portfolio_name = fm.make_portfolio(name,fignums)
    return unless view
    view_pdf(portfolio_name)
    return if @no_Tk
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
    system($pdf_viewer + ' ' + pdf_file + ' > /dev/null')
    append_to_log pdf_file
  end


  def loadfile(fname,reselect=true)
    @have_loaded = false
    fm.reset_state
    begin
      
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
      
      return if @no_Tk
      
      @root.title('Tioga:' + @title_name)
      @listBox.delete(0, 'end')
      fm.figure_names.each { |name| @listBox.insert('end', name) }
      set_selection(0) if reselect
      
    rescue Exception => er
      report_error(er, "ERROR: load failed for #{fname}\n")
    end
  end
  
  
  def set_working_dir(filename)
    
    return filename unless $change_working_directory
    
    if (filename.length > 2) && (filename[0..1] == '~/')
      filename = ENV['HOME'] + filename[1..-1]
    elsif filename[0..0] != '/'
      # if necessary, add the current directory to the front of the filename
      filename = Dir.getwd + '/' + filename
    end
    
    parts = filename.split('/')
    if parts[-1].length < 2 || parts[-1][-2..-1] != "rb"
      append_to_log "ERROR: filename must have extension 'rb'   instead has <" + parts[-1][-2..-1] + ">"
      return nil
    end
    dir = ""
    parts[0..-2].each {|part| dir << '/' + part unless part.length == 0 }
    
    if dir != Dir.getwd
      append_to_log "changing working directory to " + dir
      Dir.chdir(dir) # change current working directory
    end
    
    return filename

  end
  
  
  def show_help(filename,opt1)
    unless opt1 == '-help' || filename == '-help' || filename == nil
      puts 'Sorry: ' + opt1 + ' is not a recognized option.' 
    end
    puts "\nThis program is a command line interface for the open-source tioga kernel."
    puts "The tioga kernel is for creating figures and plots using Ruby, PDF, and TeX."
    puts "Following is a brief description of the tioga command line options."
    puts "For more information, visit http://www.kitp.ucsb.edu/~paxton/tioga.html."
    puts "\nBefore any command line information is processed, tioga runs ~/.tiogainit if it exists."
    puts "    The primary use of this file is to set your default pdf viewer command (see below)."
    puts "\nIf there are no command line arguments, or the argument is -h, this help info is output."
    puts "\nOtherwise, the command line should start with a tioga file name (with extension .rb)."
    puts "     Since the extension is known, you can skip typing it if you like."
    puts "\nThe remainder of the command line should consist of an optional series of control commands"
    puts "    followed by a figure command."
    puts "\nAny control commands are done after ~/.tiogainit and before the figure file is loaded."
    puts "     -r file      runs the file (using Ruby's require method)."
    puts '     -C dir       changes the working directory.'
    puts '                  If there is no -C command, tioga changes the working directory to the'
    puts '                  location of the figure file .'
    puts '     -v           prints version information.'
    puts "\nThe figure command comes last and should be one of these:"
    puts "     -l           output a list of the defined figures by number and name."
    puts "     -<num>       make and show figure with index equal <num> (0 <= num < num_figures)."
    puts "     -m <figs>    make PDFs without showing them in the viewer."
    puts "     -s <figs>    make and show PDFs, each in a separate viewer window."
    puts "     -p <figs>    make PDFs and show the portfolio as a multi-page document."
    puts "\nIf the figure command is omitted, then it defaults to -0."
    puts "\nIf <figs> is omitted, then tioga does all the figures defined in the file"
    puts "     ordered by their definition index numbers."
    puts "\nOtherwise, <figs> must be either"
    puts "     a defined figure name (as supplied to def_figure in the tioga file), or"
    puts "     a valid ruby array index number for a figure (can be negative), or"
    puts "     a valid ruby range specification selecting a sequence of figures, or"
    puts "     a space-less, comma-separated list of figure indices and ranges."
    puts ''
    puts "     For example, -s Plot1 makes and shows the pdf for the figure named Plot1,"
    puts "     and -p 5,0..3,-1 makes a portfolio with the figure having index 5 on page 1,"
    puts "     followed by pages showing the figures with indices 0, 1, 2, 3, and -1."
    puts "\nThe viewer for showing PDFs is specified by the $pdf_viewer variable in tioga."
    puts "     The default value can be set by creating a .tiogainit file in your home directory."
    puts "     The .tiogainit file is run before any command line options are processed."
    puts "     Your current setting for $pdf_viewer is " + $pdf_viewer + '.'
    puts "     To change it, edit ~/.tiogainit to add the line $pdf_viewer = 'my viewer command'."
    puts "     The command tioga uses to show a pdf is $pdf_viewer + ' ' + full_PDF_filename."
    puts "     You can use the -e control command to try a different viewer setting"
    puts "     by doing the $pdf_viewer assignment from the command line."
    puts ''
  end
  
    
  def report_error(er, msg)
      if msg != nil
          append_to_log msg
          append_to_log ""
      end
      append_to_log "    " + "#{er.message}" + "  [version: " + FigureMaker.version + "]"
      line_count = 0
      show_count = 0
      past_callers_routines = false
      in_callers_routines = false
      er.backtrace.each do |line|
          if (line.include?('Tioga/FigMkr.rb')) || (line.include?('Tioga/tioga_ui.rb'))
            if in_callers_routines
              past_callers_routines = true 
              in_callers_routines = false
            end
          else
            in_callers_routines = true
          end 
          if (show_count < fm.num_error_lines) and in_callers_routines
              append_to_log "    " + line
              show_count = show_count + 1
          end
          line_count = line_count + 1
      end
  end
  
  
  def setdir_and_load(filename)
    filename = set_working_dir(filename)
    return nil if filename == nil
    loadfile(filename)
    return filename
  end
  
  
  def parse_figs(figs)
    return [0] if (figs == nil) || (figs.length == 0)
    return figs unless (/^\d/ === figs) || (/^-\d/ === figs)
    ranges = figs.split(',')
    fignums = Array.new(fm.num_figures) {|i| i}
    result = []
    ranges.each do |r|
      nums = eval('fignums[' + r + ']') # do it this way in case r is negative
      if nums.kind_of?Integer
        result << nums
      else
        nums.each {|n| result << n}
      end
    end
    return result
  end


  def show_version
    puts FigureMaker.version
  end
  
  
  def setup(args)
    
    # set the standard defaults
    $tioga_args = Array.new(args.length) {|i| args[i]} # copy the args
    $change_working_directory = true
    if RbConfig::CONFIG["target"] =~ /darwin/i
      $pdf_viewer = "repreview"
      #$mac_command_key = true
    else
      $pdf_viewer = "xpdf"
      #$mac_command_key = false
    end
 
=begin     
    # Ruby/Tk defaults
    $geometry = '600x250+700+50'
    $background = 'WhiteSmoke'
    $mac_command_key = false
    $log_font = 'system 12'
    $figures_font = 'system 12'
=end
    
    tiogainit_names = %w[ .tiogarc .tiogainit ].map do |f|
      File.join(ENV['HOME'], f)
    end
    tiogainit_name = tiogainit_names.find {|f| test ?e, f}
    load(tiogainit_name) unless tiogainit_name.nil?
    
    @pdf_name = nil
    @have_loaded = false    
    @no_Tk = true

  end
  
  
  def fix_filename(filename)
    if filename != nil && filename[-3..-1] != '.rb' && 
        filename[-3..-1] != '.RB' && filename != '-help' && filename != '-v'
      filename += '.rb'
    end
    return filename
  end
  
  
  def do_fignums(fignums,view)
    fignums.each do |n|
      pdf_name = require_pdf(n)
      if view
        view_pdf(pdf_name)
      else
        report_number_and_name(n,pdf_name)
      end
    end
  end
  
  
  def list_figures
    fm.figure_names.each_with_index do |name,i| 
      puts sprintf("%3i    %s\n",i,name)
    end
  end
 
 
  def initialize(args, just_setup=false)
    
    setup(args)
    
    return if just_setup
    
    if ($tioga_args.length == 0) || ($tioga_args[0] == '-h') || ($tioga_args[0] == '-help')
      show_help(nil,nil)
      return
    end
    
    if ($tioga_args[0] == '-v') || ($tioga_args[0] == '-version') || ($tioga_args[0] == '-V')
      show_version
      return
    end
    
    if $tioga_args.length > 0
      filename = fix_filename($tioga_args[0])
    end
    
    begin
    # currently, we are only supporting command line interface.
    # the code for making a Ruby/Tk interface can be enabled if you want to play with it.
    
    argnum = 1 # the filename is in $tioga_args[0]
    
    # do control commands
    while argnum < $tioga_args.length
      cmd = $tioga_args[argnum]
      argnum = argnum + 1
      if cmd == '-r'
        puts 'require ' + $tioga_args[argnum]
        require $tioga_args[argnum]
        argnum = argnum + 1
      elsif cmd == '-C'
        puts 'chdir to ' + $tioga_args[argnum]
        Dir.chdir($tioga_args[argnum])
        $change_working_directory = false
        argnum = argnum + 1
      elsif cmd == '-v'
        show_version
      else
        argnum = argnum - 1 # backup
        break
      end
    end
    
    setdir_and_load(filename)

    # do the figure command
    
    if argnum == $tioga_args.length
      view_pdf(require_pdf(0))
      return
    end
    
    cmd = $tioga_args[argnum]
    argnum = argnum + 1
    
    if /^-\d+$/ === cmd
      view_pdf(require_pdf(cmd[1..-1].to_i))
    elsif cmd == '-l'
      list_figures
    elsif cmd == '-h' || cmd == '-help'
      show_help(nil,nil)
    elsif (cmd == '-s' || cmd == '-m')
      if argnum == $tioga_args.length
        make_all_pdfs(cmd != '-m')
        return
      end
      do_fignums(parse_figs($tioga_args[argnum]),cmd == '-s')
    elsif cmd == '-p'
      if argnum == $tioga_args.length
        make_portfolio(true)
        return
      end
      make_portfolio(true, parse_figs($tioga_args[argnum]))
    #else # unrecognized command
    #  show_help(filename,cmd)
    end
  rescue
  end

  end

=begin  

  def start_Tk
    # Ruby/Tk
    
    @no_Tk = false
    
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
    
    loadfile unless filename == nil
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
            "Visit http://www.kitp.ucsb.edu/~paxton/tioga.html")
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
