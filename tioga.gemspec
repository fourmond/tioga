# -*- mode: ruby; -*-

spec = Gem::Specification.new do |s|
  s.files += Dir["split/**/*.c"] + Dir["split/**/*.h"] + Dir["split/**/*.rb"]
  s.files += Dir["tests/*"]
  s.test_files = Dir["tests/ts_*.rb"]
  s.extensions +=  
    %w{Flate Tioga Dvector Dtable Function}.map do |d|
    "split/#{d}/extconf.rb"
  end
  s.bindir = 'split/scripts'
  s.executables << 'tioga'
  s.name = 'tioga'
  s.version = '1.4'
  s.summary = 'Tioga - a powerful scientific plotting library'
  s.homepage = 'http://www.kitp.ucsb.edu/~paxton/tioga.html'
end
