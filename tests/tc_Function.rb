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

  def test_strip
    x = Dvector[1,3,2,4]
    y = Dvector[2,3,4,5]
    x[1] = 0.0/0.0
    y[2] = 0.0/0.0
    f = Function.new(x,y)
    assert_equal(f.strip_nan, 2)
    assert_equal(f.x, Dvector[1,4])
    assert_equal(f.y, Dvector[2,5])
  end

  def test_monotonic
    x = Dvector[1,3,2,4,5,6]
    y = x.dup
    f = Function.new(x,y)
    ary = f.split_monotonic
    assert_equal(ary.size, 3)
    x = Dvector[1,3]
    assert_equal(ary[0].x, x)
    x = Dvector[3,2]
    assert_equal(ary[1].x, x)
    x = Dvector[2,4,5,6]
    assert_equal(ary[2].x, x)
  end


  def test_nan
    nan = 0.0/0.0
    x = Dvector[1,nan,2,nan,5,6]
    y = Dvector[2,3,nan,3,4,5]
    f = Function.new(x,y)
    ary = f.split_on_nan(nil)
    assert_equal(ary.size, 4)

    ary = f.split_on_nan(:y)
    assert_equal(ary.size, 2)

    ary = f.split_on_nan(:x)
    assert_equal(ary.size, 3)
  end

  def test_integrate
    x = Dvector[1,2,4]
    y = Dvector[0,1,2]
    f = Function.new(x,y)
    # integral should be 0.5 + 1.5 * 2
    assert_equal(f.integrate, 3.5)
    assert_equal(f.integrate(0,1), 0.5)
    assert_equal(f.integrate(1,2), 3)

    g = f.primitive
    assert_equal(f.x, g.x)
  end

  def test_length
    x = Dvector[1,2,4]
    y = Dvector[0,1,2]
    f = Function.new(x,y)
    assert_equal(f.size, 3)
    assert_equal(f.length, 3)
  end

  def test_distance
    f = Function.new(Dvector[0],Dvector[0])
    assert_equal(f.distance(3,4), 5.0)
    f = Function.new(Dvector[0,1],Dvector[0,1])
    assert_equal(f.distance(1,1), 0.0)
    assert_equal(f.distance(0,1), 1.0)
    assert_equal(f.distance(1,0), 1.0)
    assert_equal(f.distance(1,0), 1.0)
  end

  def test_fuzzy_ops
    f = Function.new(Dvector[1,2,3,4],Dvector[1,2,3,4])
    g = Function.new(Dvector[1,2,4],Dvector[1,2,3])
    a = g.fuzzy_sub!(f)
    assert_equal(a,0.0)
    assert_equal(g.y,  Dvector[0,0,-1])
  end

  def test_bounds
    x = Dvector[1,2,3,4,5]
    y = Dvector[0,4,3,4,2]
    
    f = Function.new(x,y)

    # First, big boundaries
    g = f.bound_values(0, 10, 0, 10)
    assert_equal(f.x, g.x)
    assert_equal(f.y, g.y)

    # Too small boundaries
    g = f.bound_values(0,0,0,0)
    assert_equal(0, g.size)

    # Real boundaries, but taking the sides make so
    # that we have the same in the end that at the beginning
    g = f.bound_values(2,4,0,10)
    assert_equal(f.x, g.x)
    assert_equal(f.y, g.y)

    # It really should be fine.
  end

  # Testing derivatives
  def test_derivatives
    x = Dvector.new
    y = Dvector.new
    d1 = Dvector.new
    d2 = Dvector.new
    30.times do |i|
      x << i
      y << i*i*i
      d1 << 3 * i * i
      d2 << 6 * i
    end


    f = Function.new(x,y)

    # First derivative
    f1 = f.diff_5p
    d1.sub!(f1.y)
    d1.each do |x|
      assert_in_delta(x,0,1e-10)
    end

    # Second derivative
    f1 = f.diff2_5p
    d2.sub!(f1.y)
    d2.each do |x|
      assert_in_delta(x,0,1e-10)
    end
    
  end

  # Test revert!
  def test_revert
    a = Dvector[1,2,3,4]
    b = Dvector[5,6,7,8]
    f = Function.new(a,b)
    f.reverse!
    assert_equal(f.x, Dvector[4,3,2,1])
    assert_equal(f.y, Dvector[8,7,6,5])

    a = Dvector[1,2,4]
    b = Dvector[5,6,8]
    f = Function.new(a,b)
    f.reverse!
    assert_equal(f.x, Dvector[4,2,1])
    assert_equal(f.y, Dvector[8,6,5])
  end

  # Test the linear regression
  def test_reglin
    x = Dvector.new(20) do |i|
      i
    end
    y = Dvector.new(20) do |i|
      1.23 * i + 2.04
    end
    f = Function.new(x,y)
    a,b = f.reglin
    assert_equal(a, 1.23)
    assert_equal(b, 2.04)
  end

  # There is unfortunately no simple way to test the interpolations...
end
