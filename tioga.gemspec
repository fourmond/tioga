# -*- mode: ruby; -*-

# More informaton about the fields available can be found at:
# http://docs.rubygems.org/read/chapter/20

spec = Gem::Specification.new do |s|
  s.files += Dir["ext/**/*.[ch]"] + 
    Dir["lib/**/*.rb"] +
  s.files += Dir["tests/*"]
  s.files += %w(Tioga_README lgpl.txt)
  # We explicitly add TexPreamble so that it doesn't get forgotten
  s.files += ["lib/Tioga/TexPreamble.rb"]
  s.test_files = Dir["tests/tsc_*.rb"]
  s.extensions +=  
    %w{Flate Tioga/FigureMaker Dobjects/Dvector Dobjects/Dtable Dobjects/Function}.map do |d|
    "ext/#{d}/extconf.rb"
  end
  s.bindir = 'bin'
  s.executables << 'tioga'
  s.name = 'tioga'
  s.version = '1.19.1'
  s.summary = 'Tioga - a powerful scientific plotting library'
  s.homepage = 'http://tioga.sf.net'
  s.authors = [
               'Bill Paxton', 'Vincent Fourmond', 'Taro Sato', 
               'Jean-Julien Fleck', 'Benjamin ter Kuile', 
               'David MacMahon'
              ]
  s.description = <<EOD
Tioga is a blend of PDF, pdfTex and ruby into a library to make
scientific graphs of high quality. It is fairly complete and
extensive, and comes with examples and (online) complete
documentation.
EOD
  # s.email = ['tioga-users@rubyforge.org']
  s.license = "LGPL 2.1"
end
