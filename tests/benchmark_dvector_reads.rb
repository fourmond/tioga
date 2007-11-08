# A small benchmarking file for fancy_read and fast_fancy_read, just to make
# sure the latter deserves its name ;-)...

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
  x.report("fancy_read(100000):") do 
    stream = File.open(f.path)
    Dobjects::Dvector.fancy_read(stream, nil, 'default'=> 0.0).size
  end
  x.report("fancy_read(100000, 2nd):") do 
    stream = File.open(f.path)
    Dobjects::Dvector.fancy_read(stream, nil, 'default'=> 0.0).size
  end
  x.report("fast_fancy_read(100000):") do 
    stream = File.open(f.path)
    Dobjects::Dvector.fast_fancy_read(stream, {
                                        'sep' => /\s+/,
                                        'comments' => /^\s*\#/,
                                        'skip_first' => 0,
                                        'index_col' => false,
                                        'remove_space' => true,
                                        'default'=> 0.0}).size
  end
  x.report("fast_fancy_read(100000, 2nd):") do 
    stream = File.open(f.path)
    Dobjects::Dvector.fast_fancy_read(stream, {
                                        'sep' => /\s+/,
                                        'comments' => /^\s*\#/,
                                        'skip_first' => 0,
                                        'index_col' => false,
                                        'remove_space' => true,
                                        'default'=> 0.0}).size
  end
  # We create a smaller file:
  f = Tempfile.new("data")
  1000.times do |i|
    f.puts "#{i*1.0}\t#{i**1.3}\t#{i**0.7}"
  end
  f.close
  x.report("fast_fancy_read(100 * 1000):") do 
    stream = File.open(f.path)
    100.times do 
      Dobjects::Dvector.fast_fancy_read(stream, {
                                          'sep' => /\s+/,
                                          'comments' => /^\s*\#/,
                                          'skip_first' => 0,
                                          'index_col' => false,
                                          'remove_space' => true,
                                          'default'=> 0.0})
    end
  end
  x.report("fancy_read(100 * 1000):") do 
    stream = File.open(f.path)
    100.times do 
      Dobjects::Dvector.fancy_read(stream, nil, 'default'=> 0.0)
    end
  end

  # We use a StringIO
  string = ""
  x.report("string creation:") do 
    50000.times do |i|
      string += "#{i*1.0}\t#{i**1.3}\t#{i**0.7}\n"
    end
  end
  x.report("fancy_read(50000):") do 
    stream = StringIO.new(string)
    Dobjects::Dvector.fancy_read(stream, nil, 'default'=> 0.0).size
  end
  x.report("fast_fancy_read(50000):") do 
    stream = StringIO.new(string)
    Dobjects::Dvector.fast_fancy_read(stream, {
                                        'sep' => /\s+/,
                                        'comments' => /^\s*\#/,
                                        'skip_first' => 0,
                                        'index_col' => false,
                                        'remove_space' => true,
                                        'default'=> 0.0}).size
  end

end



















