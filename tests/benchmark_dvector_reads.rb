# A small benchmarking file for fancy_read and fast_fancy_read, just to make
# sure the latter deserves its name ;-)...

require 'Dobjects/Dvector'
require 'benchmark'
require 'tempfile'

# We first create a 'dummy file':
f = Tempfile.new("data")
10000.times do |i|
  f.puts "#{i*1.0}\t#{i**1.3}\t#{i**0.7}"
end
f.close

stream = File.open(f.path)
puts Dobjects::Dvector.fancy_read(stream, nil, 'default'=> 0.0).size
stream = File.open(f.path)
puts Dobjects::Dvector.fast_fancy_read(stream, {
                                         'sep' => /\s+/,
                                         'comments' => /^\s*\#/,
                                         'skip_first' => 0,
                                         'index_col' => false,
                                         'remove_space' => true,
                                         'default'=> 0.0}).size

Benchmark.bm do |x|
  x.report("fancy_read:") do 
    stream = File.open(f.path)
    puts Dobjects::Dvector.fancy_read(stream, nil, 'default'=> 0.0).size
  end
  x.report("fast_fancy_read:") do 
    stream = File.open(f.path)
    puts Dobjects::Dvector.fast_fancy_read(stream, {
                                             'sep' => /\s+/,
                                             'comments' => /^\s*\#/,
                                             'skip_first' => 0,
                                             'index_col' => false,
                                             'remove_space' => true,
                                             'default'=> 0.0}).size
  end
end



















