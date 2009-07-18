# -*- mode: ruby; -*-

# begin dirty hack: we need to copy the C files in the split/Tioga/shared/
# directory to the split/Tioga/ so they are found by the usual
# mkmf.rb/extconf.rb
#
# That is pretty unclean, I'd say...

require 'fileutils'
cleanup = []

for file in Dir["split/Tioga/shared/*"]
  cleanup << "split/Tioga/" + File.basename(file)
  FileUtils::cp(file, "split/Tioga/")
end

at_exit do
  FileUtils::rm(cleanup)
end


# end dirty hack

spec = Gem::Specification.new do |s|
  s.files += Dir["split/*/*.c"] + Dir["split/*/*.h"] +
    Dir["split/*/include/*.h"] + Dir["split/**/*.rb"] +
    Dir["split/*.h"] +  Dir["split/*.c"]
  s.files += Dir["tests/*"]
  s.files += %w(Tioga_README lgpl.txt)
  s.test_files = Dir["tests/ts_*.rb"]
  s.extensions +=  
    %w{Flate Tioga Dvector Dtable Function}.map do |d|
    "split/#{d}/extconf.rb"
  end
  s.bindir = 'split/scripts'
  s.executables << 'tioga'
  s.name = 'tioga'
  s.version = '1.11'
  s.summary = 'Tioga - a powerful scientific plotting library'
  s.homepage = 'http://tioga.rubyforge.org'
  s.authors = ['Bill Paxton', 'Vincent Fourmond', 'Taro Sato', 'Jean-Julien Fleck']
  s.rubyforge_project = 'tioga'
  s.license = "LGPL 2.1"
end
