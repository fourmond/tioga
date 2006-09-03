require 'Dobjects/Function'
require 'test/unit'

class TestFunction < Test::Unit::TestCase
  include Dobjects

  def test_sorted
    x_1 = Dvector[1,2,3]
    x_2 = Dvector[1,3,2]
    f_1 = Function.new(x_1, x_2)
    f_2 = Function.new(x_2, x_1)
    
    assert(f_1.sorted?)
    assert(! f_2.sorted?)
  end

  NUMBER = 20

  def test_joint_sort
    x_1 = Dvector.new(NUMBER)
    x_1.collect! { |x|
      rand
    }
    x_2 = x_1.dup
    Function.joint_sort(x_1,x_2)
    NUMBER.times do |i|
      assert_equal(x_1[i],x_2[i])
    end

    f = Function.new(x_1,x_2)
    assert(f.sorted?)
  end

  def test_point
    x = Dvector[1,3,2]
    y = Dvector[2,3,4]
    f = Function.new(x,y)
    p = f.point(2)
    assert_equal(p[0],2.0)
    assert_equal(p[1],4.0)
    f.sort
    p = f.point(2)
    assert_equal(p[0],3.0)
    assert_equal(p[1],3.0)
  end

  def test_bounds
    x_1 = Dvector[1,2,3,4]
    x_2 = Dvector[1,9,2,0.1]
    f = Function.new(x_1, x_2)
    assert_equal(f.bounds, [1,0.1,4,9])
  end

  # There is unfortunately no simple way to test the interpolations...
end
