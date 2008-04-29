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

require 'rubygems' rescue nil
require 'Tioga/tioga.rb'
require 'scanf'

include Tioga

require 'readline'
include Readline
# for some reason, the pipes don't work unless I use the Readline package

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

def loadfile(fname, cmd)
    fm = FigureMaker.default
    have_loaded = false
    fm.reset_state
    begin
        puts "load #{fname}"
        load(fname) # this should define the TiogaFigures class
        refresh_fname = fm.auto_refresh_filename
        if refresh_fname != nil
            refresh_fname = "#{fm.run_dir}/#{refresh_fname}" if fm.run_dir != nil && refresh_fname[0..0] != '/'
            puts "####03OK #{refresh_fname}" # GUI uses this
        end
        have_loaded = true
        if cmd == "load_and_list"
            num_fig = fm.num_figures
            if num_fig == 0
                puts "ERROR: Failed to define any figures.  Remember to invoke 'new' for the class containing the figure definitions"
            end
            response = "####01OK " + num_fig.to_s
            num_fig.times { |i| response = response + ' ' + fm.figure_name(i) }
            puts response # GUI uses this
        end
    rescue Exception => er
        report_error(er, "ERROR: load failed for #{fname}\n####01")
    end
    return have_loaded
end

def command_loop
    fm = FigureMaker.default
    fname = nil
    pname = nil
    have_loaded = false
    
    loop do
        cmd_line = readline("\n")
        #cmd_line = gets
        break if cmd_line.nil?
        cmd = cmd_line.scanf("%s")
        cmd = cmd.to_s
        puts "\n\n"
        if cmd == "exit"
            puts "exiting"
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
        elsif (cmd == "load" || cmd == "load_and_list")
            cmd, fname = cmd_line.scanf("%s %s")
            if fname == nil
                puts "must give file name as argument for load command"
            else
                have_loaded = loadfile(fname, cmd)
            end
        elsif (cmd == "make_portfolio")
            if !have_loaded
                puts "must load a file before make a portfolio"
            else
                cmd, fname = cmd_line.scanf("%s %s")
                if fname == nil
                    puts "must give file name as argument for make_portfolio command"
                else
                    result = fm.make_portfolio(fname,nil,true)
                    puts result
                    puts "####00" # GUI uses this
                end
            end
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
                    result = fm.make_pdf(num.to_i)
                    if result == false
                        puts "####02FAILED" # GUI uses this
                    else
                        puts "####02OK #{result}"
                    end
                    puts "#{result}"
                end
            end
        elsif cmd == "make_all"
            if !have_loaded
                puts "must load a file before make_all"
            else
                fm.require_all(nil, true)
                #fm.num_figures.times {|i| fm.make_preview_pdf(i) }
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
            cmd, fname = cmd_line.scanf("%s %s")
            if fname == nil
                puts "must give pdflatex name as argument for set_which_pdflatex command"
            else
                FigureMaker.pdflatex = fname
                puts "use #{fname}"
            end
        else
            puts "invalid command <#{cmd}> in command line <#{cmd_line}>"
        end
    end
end

command_loop
