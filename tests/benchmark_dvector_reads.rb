# A small benchmarking file for fancy_read and fast_fancy_read, just to make
# sure the latter deserves its name ;-)...
#
# Now the comparison between fancy_read and fast_fancy_read 

require 'Dobjects/Dvector'
require 'benchmark'
require 'stringio'
require 'tempfile'

Benchmark.bm do |x|
  # We first create a 'dummy file':
  f = Tempfile.new("data")
  x.report("data writing(100000):") do 
    100000.times do |i|
      f.puts "#{i*1.0}\t#{i**1.3}\t#{i**0.7}"
    end
    f.close
  end
  x.report("fancy_read(100 000):") do 
    stream = File.open(f.path)
    Dobjects::Dvector.fancy_read(stream, nil, 'default'=> 0.0).size
  end
  x.report("fancy_read(100 000), 2nd time:") do 
    stream = File.open(f.path)
    Dobjects::Dvector.fancy_read(stream, nil, 'default'=> 0.0).size
  end

  # Trying pre-allocation.
  x.report("fancy_read(100 000), preallocation to 1 :") do 
    stream = File.open(f.path)
    Dobjects::Dvector.fancy_read(stream, nil, 'default'=> 0.0, 
                                 'initial_size' => 1).size
  end
  x.report("fancy_read(100 000), preallocation to 49999 :") do 
    stream = File.open(f.path)
    Dobjects::Dvector.fancy_read(stream, nil, 'default'=> 0.0, 
                                 'initial_size' => 49999).size
  end
  x.report("fancy_read(100 000), preallocation to 100 000 :") do 
    stream = File.open(f.path)
    Dobjects::Dvector.fancy_read(stream, nil, 'default'=> 0.0, 
                                 'initial_size' => 100000).size
  end

  # We create a smaller file:
  f = Tempfile.new("data")
  1000.times do |i|
    f.puts "#{i*1.0}\t#{i**1.3}\t#{i**0.7}"
  end
  f.close
  x.report("fancy_read(100 * 1000):") do 
    100.times do 
      stream = File.open(f.path)
      Dobjects::Dvector.fancy_read(stream, nil, 'default'=> 0.0)
    end
  end

  # We use a StringIO
  string = ""
  x.report("string creation:") do 
    50000.times do |i|
      string.concat("#{i*1.0}\t#{i**1.3}\t#{i**0.7}\n")
    end
  end
  x.report("fancy_read(50000):") do 
    stream = StringIO.new(string)
    Dobjects::Dvector.fancy_read(stream, nil, 'default'=> 0.0).size
  end

end



















