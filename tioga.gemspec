# -*- mode: ruby; -*-

spec = Gem::Specification.new do |s|
  s.files += Dir["split/**/*.c"] + Dir["split/**/*.h"] + Dir["split/**/*.rb"]
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
  s.version = '1.7'
  s.summary = 'Tioga - a powerful scientific plotting library'
  s.homepage = 'http://tioga.rubyforge.org'
end
