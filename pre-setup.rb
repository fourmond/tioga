# We do two things here:
require 'fileutils'

# First, generate include files and preambles
system "#{config("rubyprog")} misc/mk_tioga_sty.rb"

if Config::CONFIG["target"] =~ /darwin/i
  File.open("bin/repreview", 'w') do |f|
    f.puts '#!/bin/sh'
    f.puts "osascript #{config('bindir')}/Reload_Preview_Document.scpt $*"
  end
  FileUtils.copy("misc/Reload_Preview_Document.scpt", "bin")
else
  puts "Skipping MacOS-specific files"
end
