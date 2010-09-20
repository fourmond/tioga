#  tc_Flate.rb

require 'test/unit'
require 'Flate'

class TestFlate < Test::Unit::TestCase

  def make_data(size = 10000) 
    srand # intitialize the random seed...
    data = ""
    # For some reason, there is a failure here in Ruby1.9.1, around the 768th
    # iteration.
    size.times do
      data << [rand(256)].pack("C")
    end
    return data
  end

  def test_compression_decompression
    data = make_data
    data_compressed = Flate.compress(data)
    data_second = Flate.expand(data_compressed)
    assert_equal(data_second, data)
  end

end



















