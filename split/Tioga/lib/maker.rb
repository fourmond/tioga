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


def require_pdf(num)
    fm = FigureMaker.default
    puts "#{fm.figure_names[num]}"
    result = fm.require_pdf(num)
    if result == false
        puts "####02FAILED" # GUI uses this
        return result
    end
    puts "####02OK #{result}"
    puts "#{result}"
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


def reload_pdf(pdf_viewer, pdf_name, revert=true, refocus=true)
    syscmd = pdf_viewer + ' ' + pdf_name
    if revert
      syscmd += " True"
    else
      syscmd += " False"
    end
    if refocus
      syscmd += " True"
    else
      syscmd += " False"
    end
    syscmd += " > /dev/null"
    system(syscmd)
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
        elsif (cmd == "eval")
            string = cmd_line[5..-1]
            begin
                result = fm.eval_function(string)
                puts result.to_s
            rescue
            end
            puts "####00" # GUI uses this
        elsif (cmd == "load")
            cmd, fname = cmd_line.scanf("%s %s")
            have_loaded = loadfile(fname, cmd)
            fname = fname[0..-4] if fname[-3..-1] == ".rb"
            pdf_name = fname + ".pdf"
        elsif (cmd == "preview")
            cmd, num = cmd_line.scanf("%s %s")
            result = require_pdf(num.to_i)
            fm = FigureMaker.default
            return result if pdf_viewer == nil
            syscmd = "cp " + result + " " + pdf_name
            system(syscmd)
            result = reload_pdf(pdf_viewer, pdf_name)
        elsif cmd == "show"
            # this is for showing a previously made pdf file in it's own window
            cmd, figure_pdf_name = cmd_line.scanf("%s %s")
            reload_pdf(pdf_viewer, figure_pdf_name)
        elsif (cmd == "make_portfolio" || cmd == "make_all")
            fm.num_figures.times { |i| 
                if fm.figure_pdfs[i] == nil
                  require_pdf(i)
                end
              }
            if cmd == "make_portfolio"
                cmd, name = cmd_line.scanf("%s %s")
                name = "portfolio" if name == nil
                puts "#{name}"
                portfolio_name = fm.make_portfolio(name)
                puts "#{portfolio_name}"
                reload_pdf(pdf_viewer, portfolio_name, false) # current Preview has bug for thumbnails when Revert
                puts " "
                puts "Note: the current Preview fails to remake thumbnails after a Revert,"
                puts "so for now we have to do a Close and Open sequence as a workaround."
                puts "This will be changed as soon as a fixed Preview is available from Apple."
                puts " "
            end
        elsif cmd == "name"
            cmd, num = cmd_line.scanf("%s %s")
            eval_str = "fm.figure_name(#{num})"
            begin
                puts eval(eval_str)
            rescue Exception
                puts "invalid figure number #{num}"
            end
        elsif cmd == "names"
            puts fm.figure_names
        elsif cmd == "list"
            fm.figure_names.each_with_index { |name,i| STDOUT.printf("%3i %s\n",i,name) }
        elsif cmd == "num_figures"
            eval_str = "fm.num_figures"
            begin
                puts eval(eval_str).to_s
            rescue Exception
            end
        elsif cmd == "set_which_pdflatex"
            cmd, pdflatexname = cmd_line.scanf("%s %s")
            FigureMaker.pdflatex = pdflatexname
        elsif cmd == "set_pdf_name"
            cmd, pdf_name = cmd_line.scanf("%s %s")
        elsif cmd == "set_which_viewer"
            cmd, pdf_viewer = cmd_line.scanf("%s %s")
        elsif cmd == "done"
        else
            puts "invalid command <#{cmd}> in command line <#{cmd_line}>"
        end
    end
end

command_loop
