# -*- mode: ruby; -*-

# More informaton about the fields available can be found at:
# http://docs.rubygems.org/read/chapter/20

spec = Gem::Specification.new do |s|
  s.files += Dir["ext/**/*.[ch]"] + 
    Dir["lib/**/*.rb"] +
    Dir["split/*/include/*.h"] + Dir["split/**/*.rb"] +
    Dir["split/*.h"] +  Dir["split/*.c"]
  s.files += Dir["tests/*"]
  s.files += %w(Tioga_README lgpl.txt)
  s.test_files = Dir["tests/tsc_*.rb"]
  s.extensions +=  
    %w{Flate Tioga/FigureMaker Dobjects/Dvector Dobjects/Dtable Dobjects/Function}.map do |d|
    "ext/#{d}/extconf.rb"
  end
  s.bindir = 'bin'
  s.executables << 'tioga'
  s.name = 'tioga'
  s.version = '1.13'
  s.summary = 'Tioga - a powerful scientific plotting library'
  s.homepage = 'http://tioga.rubyforge.org'
  s.authors = [
               'Bill Paxton', 'Vincent Fourmond', 'Taro Sato', 
               'Jean-Julien Fleck', 'Benjamin ter Kuile', 
               'David MacMahon'
              ]
  s.rubyforge_project = 'tioga'
  s.email = ['tioga-users@rubyforge.org']
  s.license = "LGPL 2.1"
  s.has_rdoc = true
end
