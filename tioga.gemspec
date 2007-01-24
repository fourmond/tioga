# -*- mode: ruby; -*-

spec = Gem::Specification.new do |s|
  s.files += Dir["split/**/*.c"] + Dir["split/**/*.h"] + Dir["split/**/*.rb"]
  s.extensions +=  
    %w{Flate Tioga Dvector Dtable Function}.map do |d|
    "split/#{d}/extconf.rb"
  end
  s.name = 'tioga'
  s.version = '1.3.99'
  s.summary = 'Tioga - a powerful scientific plotting library'
end
