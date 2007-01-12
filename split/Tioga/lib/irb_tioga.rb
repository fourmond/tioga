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

FigureMaker.pdflatex = 'pdflatex'
# you can modify this to give full pathname for your favorite pdflatex

$have_loaded_figure_file = false
$irb_figure_num = 0
$open_command = 'open'
$open_list_command = 'open'

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
    result = fm.make_pdf(num.to_i)
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
#  review
#
# Redoes preview for the most recently made figure.
def review
    fm = FigureMaker.default
    preview($irb_figure_num)
end

# :call-seq:
#  rv
#
# Alias for #review.
def rv
    review
end

# :call-seq:
#  preview figure_number
#
# Calls make_figure, then does a system call to open the resulting file.
# This of course assumes that you have a default PDF viewer set up to
# respond to the "open" command.  The actual command used to do the open
# is taken from the current value of the global variable $open_command
# which is initialize to "open".  You can modify this variable if you
# want to have preview execute someother operation with the new PDF file.
def preview(num)
    fname = make_figure(num)
    return false unless fname.kind_of? String
    system($open_command + ' ' + fname)
    return true
end

# :call-seq:
#  preview_all
#
# Calls #preview for all figures
def preview_all
    fm = FigureMaker.default
    if !$have_loaded_figure_file
        puts "must load a file before ask to list figure names"
        return false
    end
    fm.num_figures.times {|i| preview(i) }
    return true
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
    fm.figure_names.each_with_index { |name,i| puts "#{name}"; make_figure(i) }
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
  write_list_to_file(STDOUT)
end

# :call-seq:
#  ls
#
# Alias for list_figures.
def ls
    list_figures
end

# :call-seq:
#  save_list(fname)
#
# Save a list of the currently defined figures to the file.
def save_list(fname)
  file = File.new(fname,'w')
  write_list_to_file(file)
  file.close
  return true
end

# :call-seq:
#  open_list(fname = 'names.txt')
#
# Calls save_list and then opens the file.
def open_list(fname = 'names.txt')
    save_list(fname)
    system($open_list_command + ' ' + fname)
    return true
end

# :call-seq:
#  ol
#
# Alias for open_list.
def ol
    open_list
end

def write_list_to_file(file)
    fm = FigureMaker.default
    if !$have_loaded_figure_file
        puts "must load a file before ask to list figure names"
        return false
    end
    fm.figure_names.each_with_index { |name,i| file.printf("%3i    %s\n",i,name) }
    return true
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
#  reload_and_review
#
# Does reload followed by review.
def reload_and_review
    reload
    review
end

# :call-seq:
#  rr
#
# Alias for reload_and_review.
def rr
    reload_and_review
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
# Print the tioga FigureMaker version string.
def version
    FigureMaker.version
end

# :call-seq:
#  list_cmds
#
# Prints out a short description of the commands.
def list_cmds
    puts ""
    puts "Command                   short form      description"
    puts "load_figures 'tiogafile'      ld          loads the tioga figure definition file into ruby"
    puts "preview number                pv          makes the figure and opens the pdf file in a viewer"
    puts "          the pdf viewer is specified by the value of the $open_command variable in irb"
    puts "reload_and_review             rr          reloads the figures file and redoes the preview"
    puts "open_list 'outputfile'        ol          saves and opens a numbered list of figure names"
    puts "          'outputfile' can be omitted -- it defaults to 'names.txt'"
    puts "          the text editor is specified by the value of the $open_list_command variable in irb"
    puts ""
    puts "Those are the four essential commands.  Here are some others that are also available."
    puts "list_figures                  ls          lists the figures by number for the current file"
    puts "save_list 'outputfile'        sl          saves list of figures to the named file"
    puts "make_figure number            mk          makes the pdf file for the figure"
    puts "make_all                      ma          makes all of the figures in the current file"
    puts "reload                        rl          reloads the most recently loaded file"
    puts "review                        rv          redoes preview for the most recently viewed figure"
    puts "eval_function string          ef          calls the current file's eval function"
    puts "quiet                                     turns off all but essential messages"
    puts "verbose                                   undoes the effect of the quiet command"
    puts "version                                   prints the Tioga FigureMaker version string"
    puts "cmds                                      prints this message"
    puts ""
    return true
end

# :call-seq:
#  cmds
#
# Prints out a short description of the commands.
def cmds
  list_cmds
end

end # module IRB_Tioga

end # module Tioga

