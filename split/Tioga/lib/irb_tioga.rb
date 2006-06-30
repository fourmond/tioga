=begin
   Copyright (C) 2005  Bill Paxton

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

module Tioga
module IRB_Tioga

$which_pdflatex = 'pdflatex'
# you can modify this to give full pathname for your favorite pdflatex

$have_loaded_figure_file = false

# :call-seq:
#  make_portfolio filename
#  make_portfolio
#
# Creates a simple TeX file holding all of the figures, one per page.
# Uses the _filename_ if it is given, otherwise, appends '.tex' to the
# name of the current '.rb' file.
def make_portfolio(name=nil)
    if !$have_loaded_figure_file
        puts "must load a file before ask to make a portfolio"
        return false
    end
    if name == nil
        name = $figure_filename.split('.')[0] + '.tex'
    end
    fm = FigureMaker.default
    result = fm.make_portfolio_pdf(name)
end

# :call-seq:
#  mp filename
#  mp
#
# Alias for make_portfolio.
def mp(name=nil)
    make_portfolio(name)
end

$irb_figure_num = 0

# :call-seq:
#  make_figure number
#
# Creates the figure files, including running pdflatex to make a sample PDF file for the
# combined graphics and text.
def make_figure(num)
    if !$have_loaded_figure_file
        puts "must load a file before ask to make a figure"
        return false
    end
    fm = FigureMaker.default
    result = fm.make_preview_pdf(num)
    if result != false
        $irb_figure_num = num
        return result
    end
    return false
end

# :call-seq:
#  mk num
#
# Alias for make_figure.
def mk(num)
    make_figure(num)
end

# :call-seq:
#  refresh
#
# Sets 'need_to_reload_data' true and redoes make for the most recently made figure.
def refresh
    fm = FigureMaker.default
    fm.need_to_reload_data = true
    make_figure($irb_figure_num)
end

# :call-seq:
#  rf
#
# Alias for #refresh.
def rf
    refresh
end

# :call-seq:
#  review
#
# Sets 'need_to_reload_data' true and redoes preview for the most recently made figure.
def review
    fm = FigureMaker.default
    fm.need_to_reload_data = true
    preview($irb_figure_num)
end

# :call-seq:
#  rv
#
# Alias for #review.
def rv
    review
end

$open_command = 'open'

# :call-seq:
#  preview figure_number
#
# Calls make_figure, then does a system call to open the resulting file.
# This of course assumes that you have a default PDF previewer set up to
# respond to the "open" command.  The actual command used to do the open
# is taken from the current value of the global variable $open_command
# which is initialize to "open".  You can modify this variable if you
# want to have preview execute someother operation with the new PDF file.
def preview(num)
    fname = make_figure(num)
    return unless fname.kind_of? String
    system($open_command + ' ' + fname)
end

# :call-seq:
#  pv figure_number
#
# Alias for #preview.
def pv(num)
    preview(num)
end

# Load the named Tioga document file.
def load_figures(fname)
    fm = FigureMaker.default
    $have_loaded_figure_file = false
    fm.reset_state
    load(fname)
    $have_loaded_figure_file = true
    $figure_filename = fname
    return fname
end

# :call-seq:
#  ld fname
#
# Alias for load_figures.
def ld(fname)
    load_figures(fname)
end

# :call-seq:
#  reload
#
# Reload the current file.  Useful during development of the figure definitions.
def reload
    if !$have_loaded_figure_file
        puts "must load a file before ask to reload"
        return false
    end
    return load_figures($figure_filename)
end

# :call-seq:
#  rl
#
# Alias for #reload.
def rl
    reload
end

$irb_period_for_refresh = 1.0

# :call-seq:
#  refresh_period seconds
#
# Sets the seconds between auto refreshes.
def refresh_period(seconds)
    $irb_period_for_refresh = seconds
end

# :call-seq:
#  rp seconds
#
# Alias for refresh_period.
def rp(seconds)
    refresh_period(seconds)
end

# :call-seq:
#  auto_refresh cnt
#
# Starts the auto refresh cycle and runs for _cnt_ refreshes.
def auto_refresh(cnt)
    fm = FigureMaker.default
    filename = fm.auto_refresh_filename
    if filename != nil
        modtime = File.mtime(filename)
    end
    cnt = cnt.to_i
    refresh; cnt -= 1
    while cnt > 0
        sleep($irb_period_for_refresh)
        if filename == nil
            refresh; cnt -= 1
        else
            newtime = File.mtime(filename)
            if newtime != modtime
                modtime = newtime
                refresh; cnt -= 1
            end
        end
    end
end

# :call-seq:
#  ar cnt
#
# Alias for auto_refresh.
def ar(cnt)
    auto_refresh(cnt)
end

# :call-seq:
#  eval_function string
#
# call the eval function with the string as arg.
def eval_function(string)
    fm = FigureMaker.default
    result = string
    begin
        result = fm.eval_function(string)
    rescue
    end
    return result
end

# :call-seq:
#  ar cnt
#
# Alias for eval_function.
def ef(string)
    eval_function(string)
end

# :call-seq:
#  make_all
#
# Call make_figure for all of the currently defined figures.
def make_all
    fm = FigureMaker.default
    if !$have_loaded_figure_file
        puts "must load a file before ask to list figure names"
        return false
    end
    fm.num_figures.times {|i| make_figure(i) }
    return true
end

# :call-seq:
#  ma
#
# Alias for make_all.
def ma
    make_all
end

# :call-seq:
#  list_figures
#
# Give a numbered list of the currently defined figures.
def list_figures
    fm = FigureMaker.default
    if !$have_loaded_figure_file
        puts "must load a file before ask to list figure names"
        return false
    end
    fm.figure_names.each_with_index { |name,i| STDOUT.printf("%3i %s\n",i,name) }
    return true
end

# :call-seq:
#  ls
#
# Alias for list_figures.
def ls
    list_figures
end

# :call-seq:
#  quiet
#
# Turn off all but essential messages.
def quiet
    fm = FigureMaker.default
    fm.quiet_mode = true
end

# :call-seq:
#  verbose
#
# Turn on messages.
def verbose
    fm = FigureMaker.default
    fm.quiet_mode = false
end

# :call-seq:
#  version
#
# Print the Tioga FigureMaker version string.
def version
    FigureMaker.version
end

# :call-seq:
#  list_cmds
#
# Prints out a short description of the commands.
def list_cmds
    puts "Command                   short form      description"
    puts "load_figures 'filename'       ld          loads the figure definition file"
    puts "reload                        rl          reloads the most recently loaded file"
    puts "refresh                       rf          sets 'need_to_reload_data' and redoes make"
    puts "review                        rv          sets 'need_to_reload_data' and redoes preview"
    puts "list_figures                  ls          lists the figures in the current file"
    puts "make_figure number            mk          makes the figure"
    puts "preview number                pv          makes the figure and opens the pdf file"
    puts "make_portfolio 'name.tex'     mp          makes a portfolio and saves TeX in 'name.tex'"
    puts "make_all                      ma          makes all of the figures in the current file"
    puts "refresh_period secs           rp          sets the seconds between auto refreshes"
    puts "auto_refresh cnt              ar          runs auto refresh cycle for cnt refreshes"
    puts "eval_function string          ef          call the current file's eval function"
    puts "quiet                                     turn off all but essential messages"
    puts "verbose                                   undo the effect of the quiet command"
    puts "version                                   prints the Tioga FigureMaker version string"
    puts "list_cmds                                 prints this message"
    return true
end

end # module IRB_Tioga

end # module Tioga

