#  ts_Tioga.rb


require 'Tioga/tioga'

puts "This is the test suite for Tioga #{Tioga::FigureMaker.version}"
# We add this directory to the current search path
$: << File.dirname(__FILE__)


require 'test/unit'

require 'tc_FMkr.rb'

require 'tc_Dvector.rb'

require 'tc_Dtable.rb'

require 'tc_Flate.rb'

require 'tc_Function.rb'

















