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
require 'scanf'

include Tioga

require 'readline'
include Readline
# for some reason, the pipes don't work unless I use the Readline package

$open_command = 'repreview'

def report_error(er, msg)
    fm = FigureMaker.default
    puts msg
    puts ""
    puts "    " + "#{er.message}"
    line_count = 0
    er.backtrace.each do |line|
        if line_count < fm.num_error_lines
            puts "    " + line
        end
        line_count = line_count + 1
    end
    puts "ERROR"  # GUI uses this
end

def make_and_preview(num, pdf_viewer, name = nil)
    fm = FigureMaker.default
    result = fm.make_pdf(num)
    if result == false
        puts "####02FAILED" # GUI uses this
        return result
    end
    puts "####02OK #{result}"
    return result if pdf_viewer == nil
    if (name != nil)
      pdf_name = name
      syscmd = "cp " + result + " " + pdf_name
      puts "#{result}"
      system(syscmd)
    else
      pdf_name = result
    end
    system(pdf_viewer + ' ' + pdf_name)
    return result
end

def loadfile(fname, cmd)
    fm = FigureMaker.default
    have_loaded = false
    fm.reset_state
    begin
        load(fname) # this should define the TiogaFigures class
        have_loaded = true
        num_fig = fm.num_figures
        if num_fig == 0
            puts "ERROR: Failed to define any figures.  Remember to invoke 'new' for the class containing the figure definitions"
        end
        response = "####01OK " + num_fig.to_s
        num_fig.times { |i| response = response + ' ' + fm.figure_name(i) }
        puts response # GUI uses this
    rescue Exception => er
        report_error(er, "ERROR: load failed for #{fname}\n####01")
    end
    return have_loaded
end

def command_loop
    fm = FigureMaker.default
    fname = nil # the name of the current tioga figures file
    pdf_viewer = nil
    pdf_name = nil # if not nil, use this name for all the pdfs
    have_loaded = false
    
    loop do
        cmd_line = readline("\n")
        #cmd_line = gets
        break if cmd_line.nil?
        cmd = cmd_line.scanf("%s")
        cmd = cmd.to_s
        puts "\n\n"
        if cmd == "exit"
            puts "###\n"  # this marks end of command -- DON'T change it since the GUI depends on it!
            exit
        elsif (cmd == "need_to_reload_data")
            fm.need_to_reload_data = true
        elsif (cmd == "eval_function")
            skip = "eval_function".length
            string = cmd_line[skip+1..-1]
            begin
                result = fm.eval_function(string)
                puts result.to_s
            rescue
            end
            puts "####00" # GUI uses this
        elsif (cmd == "load")
            cmd, fname = cmd_line.scanf("%s %s")
            if fname == nil
                puts "must give file name as argument for load command"
            else
                have_loaded = loadfile(fname, cmd)
            end
            pdf_name = fname + ".pdf"
        elsif (cmd == "make" || cmd == "need_to_reload_data_and_make")
            if cmd == "need_to_reload_data_and_make"
                fm.need_to_reload_data = true
            end
            if !have_loaded
                puts "must load a file before make a figure"
            else
                cmd, num = cmd_line.scanf("%s %s")
                if num == nil || (num.to_i == 0 && num != "0")
                    puts "must provide integer figure index as arg make"
                else
                    result = make_and_preview(num.to_i, pdf_viewer, pdf_name)
                end
            end
        elsif cmd == "make_all"
            if !have_loaded
                puts "must load a file before make_all"
            else
                fm.figure_names.each_with_index { |name,i| result = fm.make_pdf(i); puts "#{result}";  }
                puts "---done---"
            end
            puts "####00" # GUI uses this
        elsif cmd == "view_all"
            if !have_loaded
                puts "must load a file before make_all"
            else
                fm.figure_names.each_with_index { |name,i| make_and_preview(i, pdf_viewer, pdf_name) }
                puts "---done---"
            end
            puts "####00" # GUI uses this
        elsif cmd == "name"
            if !have_loaded
                puts "must load a file before ask for figure names"
            else
                cmd, num = cmd_line.scanf("%s %s")
                if (num == nil)
                    puts "must give index of figure as argument for name command"
                else
                    eval_str = "fm.figure_name(#{num})"
                    begin
                        puts eval(eval_str)
                    rescue Exception
                        puts "invalid figure number #{num}"
                    end
                end
            end
        elsif cmd == "names"
            if !have_loaded
                puts "must load a file before ask for figure names"
            else
                puts fm.figure_names
            end
        elsif cmd == "list"
            if !have_loaded
                puts "must load a file before ask to list figure names"
            else
                fm.figure_names.each_with_index { |name,i| STDOUT.printf("%3i %s\n",i,name) }
            end
        elsif cmd == "num_figures"
            if !have_loaded
                puts "must load a file before ask how many figures"
            else
                eval_str = "fm.num_figures"
                begin
                    puts eval(eval_str).to_s
                rescue Exception
                end
            end
        elsif cmd == "set_which_pdflatex"
            cmd, pdflatexname = cmd_line.scanf("%s %s")
            if pdflatexname == nil
                puts "must give pdflatex name as argument for set_which_pdflatex command"
            else
                FigureMaker.pdflatex = pdflatexname
            end
        elsif cmd == "review"
            # this is for reviewing a previously made pdf file
            if pdf_name == nil
               puts "must set pdf_name before calling review"
            elsif pdf_viewer == nil
               puts "must set pdf_viewer before calling review"
            else
              cmd, figure_pdf_name = cmd_line.scanf("%s %s")
              if figure_pdf_name == nil
                 puts "must give figure pdf name as argument for review"
              else
                 system("cp " + figure_pdf_name + " " + pdf_name)
                 system(pdf_viewer + ' ' + pdf_name)
              end
            end
        elsif cmd == "set_pdf_name"
            cmd, pdf_name = cmd_line.scanf("%s %s")
        elsif cmd == "set_which_viewer"
            cmd, viewername = cmd_line.scanf("%s %s")
            if viewername == nil
                puts "must give viewer name as argument for set_which_viewer command"
            else
                pdf_viewer = viewername
            end
        else
            puts "invalid command <#{cmd}> in command line <#{cmd_line}>"
        end
    end
end

command_loop
