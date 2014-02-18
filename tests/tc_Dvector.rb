#  tc_Dvector.rb

require 'Dobjects/Dvector'
require 'stringio'
require 'test/unit'

class TestDvector < Test::Unit::TestCase
    include Dobjects

    # Returns the real file name of a given file.
    def real_file_name(file_name)
      dir = File.dirname(__FILE__)
      if dir.empty?
        return file_name
      else
        return "#{dir}/#{file_name}"
      end
    end

    # Checks if the internal Is_Dvector is working properly,
    # as it seems problematic in Ruby 1.9
    def test_isa_dvector
      a = Dvector.new
      assert(Dvector.is_a_dvector(a))
    end
    
    def test_read_nasty_fortran_data
        row = Dvector.read_row(real_file_name("dvector_read_test.data"))
        assert_equal(4, row.size)
        assert_equal(1.0, row[-1])
    end
    
    def test_vector_length
        a = Dvector[3.0, 4.0]
        assert_equal(a.vector_length, 5.0)
    end

    def test_tridag
        a = Dvector[-0.5, -0.5, -0.5, -1.0/3.0, -0.5]
        b = Dvector[1, 2, 3, -2, 1]
        c = Dvector[-0.25, -1.0/3.0, -0.25, -0.25, -0.25]
        r = 4514 * Dvector[1, 2, 3, 4, 5]
        u = Dvector.new
        u.tridag(a,b,c,r)
        diffs = (u - Dvector[6226, 6848, 4665, -11884, 16628])*1e6
        diffs.round!
        assert_equal(Dvector[0,0,0,0,0], diffs)
    end

    def test_read_row
        row = Dvector.read_row(real_file_name("dvector_test.data"))
        assert_equal(2, row.size)
        assert_equal(Dvector[23, 3], row)
        row = Dvector.read_row(real_file_name("dvector_test.data"), 4)
        assert_equal(4, row.size)
        assert_equal(Dvector[0, -1.1, 2.2, -3.3], row)
    end
    
    def test_read_rows
        rows = [Dvector.new, Dvector.new, Dvector.new, Dvector.new]
        Dvector.read_rows(real_file_name("dvector_test.data"), rows)
        assert_equal(2, rows[0].size)
        assert_equal(Dvector[23, 3], rows[0])
        assert_equal(4, rows[3].size)
        assert_equal(Dvector[0, -1.1, 2.2, -3.3], rows[3])
    end
    
    def test_read
        cols = Dvector.read(real_file_name("dvector_test.data"),nil,2)
        assert_equal(4, cols.size)
        col0 = cols[0]
        col1 = cols[1]
        col2 = cols[2]
        col3 = cols[3]
        assert_equal(100, col0.size)
        assert_equal(100, col1.size)
        assert_equal(100, col2.size)
        assert_equal(100, col3.size)
        assert_equal(0, col0[2])
        assert_equal(-1.1, col1[2])
        assert_equal(2.2, col2[2])
        assert_equal(-3.3, col3[2])
        cols = Dvector.read(real_file_name("dvector_test.data"),
                            [col0,col1,nil,col3],3,3)
        assert_equal(3, col0.size)
        assert_equal(3, col1.size)
        assert_equal(100, col2.size)
        assert_equal(3, col3.size)
        assert_equal(0, col0[1])
        assert_equal(-1.1, col1[1])
        assert_equal(-3.3, col3[1])
    end

    def test_arith_ops
        a = Dvector[11, 22, 33]
        b = Dvector[6.3, -2.1, 3e-40]
        a2 = a.to_a
        b2 = b.to_a
        assert_equal(a.add(4), a + 4)
        assert_equal(a.sub(4), a - 4)
        assert_equal(a.mul(4), a * 4)
        assert_equal(a.div(4), a / 4)
        assert_equal(a.pow(4), a ** 4)
        assert_equal(a.mul(4), 4 * a)
        assert_equal(a.add(4), 4 + a)
        assert_equal(a.neg.add(4), 4 - a)
        assert_equal(a.inv.mul(4), 4 / a)
        assert_equal(a.as_exponent_of(4), 4 ** a)
        assert_equal(a.add(3.5), a + 3.5)
        assert_equal(a.sub(3.5), a - 3.5)
        assert_equal(a.mul(3.5), a * 3.5)
        assert_equal(a.div(3.5), a / 3.5)
        assert_equal(a.pow(3.5), a ** 3.5)
        assert_equal(a.mul(3.5), 3.5 * a)
        assert_equal(a.add(3.5), 3.5 + a)
        assert_equal(a.neg.add(3.5), 3.5 - a)
        assert_equal(a.inv.mul(3.5), 3.5 / a)
        assert_equal(a.as_exponent_of(3.5), 3.5 ** a)
        a = Dvector[ 1.1, -5.7, 12.7 ]
        assert_equal(Dvector[ 1.1 ** 3, (-5.7) ** 3, 12.7 ** 3 ], a.raised_to(3))
        assert_equal(Dvector[ 1.1 ** 3, (-5.7) ** 3, 12.7 ** 3 ], a ** 3) 
        b = Dvector[ 7, 4, -2 ]
        assert_equal(Dvector[ 1.1 ** 7, (-5.7) ** 4, 12.7 ** (-2) ], a.raised_to(b))
        assert_equal(Dvector[ 1.1 ** 7, (-5.7) ** 4, 12.7 ** (-2) ], a ** b) 
        a = Dvector[ 2, -5, 12 ]
        assert_equal(Dvector[ 3.8 ** 2, 3.8 ** (-5), 3.8 ** 12 ], a.as_exponent_of(3.8))
        b = Dvector[ 7.1, 4.9, -10 ]
        assert_equal(Dvector[ 7.1 ** 2, 4.9 ** (-5), (-10) ** 12 ], a.as_exponent_of(b))
    end

    def test_vec_math
        a = Dvector[11, 22, 33]
        b = Dvector[6.3, -2.1, 3e-40]
        a2 = a.to_a
        b2 = b.to_a
        ans = 0
        a.each { |x| ans = ans + x }
        assert_equal(ans, a.sum)
        ans = 0
        a.each2(b) { |x,y| ans = ans + x*y }
        assert_equal(ans, a.dot(b))
        ans = []
        a.each2(b) { |x,y| ans << (x + y) }
        assert_equal(ans, a.add(b))
        ans = []
        a.each2(b) { |x,y| ans << (x - y) }
        assert_equal(ans, a.sub(b))
        ans = []
        a.each2(b) { |x,y| ans << (x * y) }
        assert_equal(ans, a.mul(b))
        ans = []
        a.each2(b) { |x,y| ans << (x / y) }
        assert_equal(ans, a.div(b))
    end

    def test_each3
      a = Dvector[ 1, 0, -1 ]
      b = Dvector[ 3, 4, 5 ]
      c = Dvector[ 6, 9, 2 ]
      new = Dvector[]
      a.each3(b,c){|x,y,z| new << x << y << z}
      assert_equal Dvector[1,3,6,0,4,9,-1,5,2], new
    end

    def test_each3_with_index
      a = Dvector[ 1, 0, -1 ]
      b = Dvector[ 3, 4, 5 ]
      c = Dvector[ 6, 9, 2 ]
      new = Dvector[]
      a.each3_with_index(b,c){|x,y,z, i| new << x << y << z << i}
      assert_equal Dvector[1,3,6,0,0,4,9,1,-1,5,2,2], new
    end

    def test_reverse_each3
      a = Dvector[ 1, 0, -1 ]
      b = Dvector[ 3, 4, 5 ]
      c = Dvector[ 6, 9, 2 ]
      new = Dvector[]
      a.reverse_each3(b,c){|x,y,z| new << x << y << z}
      assert_equal Dvector[-1,5,2,0,4,9,1,3,6], new
    end

    def test_reverse_each3_with_index
      a = Dvector[ 1, 0, -1 ]
      b = Dvector[ 3, 4, 5 ]
      c = Dvector[ 6, 9, 2 ]
      new = Dvector[]
      a.reverse_each3_with_index(b,c){|x,y,z,i| new << x << y << z << i}
      assert_equal Dvector[-1,5,2,2,0,4,9,1,1,3,6,0], new
    end

    def test_math
        a = Dvector[11, 22, 33]
        b = Dvector[6.3, -2.1, 3e-40]
        c = Dvector[0.6, -0.2, 1e-40]
        d = Dvector[-0.5, 0, 0.5]
        e = Dvector[1.25, 1.1, 1.5]
        a2 = a.to_a
        b2 = b.to_a
        c2 = c.to_a
        d2 = d.to_a
        e2 = e.to_a
        cutoff = 1e-10
        y = 2.5
        zeros = Dvector[0, 0, 0]
        assert_equal(zeros, (a2.map{ |x| x.sin }.to_dvector - a.sin).trim)
        assert_equal(zeros, (a2.map{ |x| x.cos }.to_dvector - a.cos).trim)
        assert_equal(zeros, (a2.map{ |x| x.tan }.to_dvector - a.tan).trim)
        assert_equal(zeros, (c2.map{ |x| x.asin }.to_dvector - c.asin).trim)
        assert_equal(zeros, (c2.map{ |x| x.acos }.to_dvector - c.acos).trim)
        assert_equal(zeros, (a2.map{ |x| x.atan }.to_dvector - a.atan).trim)
        assert_equal(zeros, (a2.map{ |x| x.sinh }.to_dvector - a.sinh).trim)
        assert_equal(zeros, (a2.map{ |x| x.cosh }.to_dvector - a.cosh).trim)
        assert_equal(zeros, (a2.map{ |x| x.tanh }.to_dvector - a.tanh).trim)
        assert_equal(zeros, (e2.map{ |x| x.asinh }.to_dvector - e.asinh).trim)
        assert_equal(zeros, (e2.map{ |x| x.acosh }.to_dvector - e.acosh).trim)
        assert_equal(zeros, (d2.map{ |x| x.atanh }.to_dvector - d.atanh).trim)
        assert_equal(zeros, (a2.map{ |x| x.sqrt }.to_dvector - a.sqrt).trim)
        assert_equal(zeros, (a2.map{ |x| x.log }.to_dvector - a.log).trim)
        assert_equal(zeros, (a2.map{ |x| x.log10 }.to_dvector - a.log10).trim)
        assert_equal(zeros, (b2.map{ |x| x.exp }.to_dvector - b.exp).trim)
        assert_equal(zeros, (b2.map{ |x| x.exp10 }.to_dvector - b.exp10).trim)
        assert_equal(zeros, (a2.map{ |x| x.raised_to(y) }.to_dvector - a.raised_to(y)).trim)
        assert_equal(zeros, (a2.map{ |x| x.as_exponent_of(y) }.to_dvector - a.as_exponent_of(y)).trim)
        assert_equal(zeros, (b2.map{ |x| x.trim(cutoff) }.to_dvector - b.trim(cutoff)).trim)
        assert_equal(zeros, (a2.map{ |x| x.safe_log(cutoff) }.to_dvector - a.safe_log(cutoff)).trim)
        assert_equal(zeros, (a2.map{ |x| x.safe_log10(cutoff) }.to_dvector - a.safe_log10(cutoff)).trim)
        assert_equal(zeros, (b2.map{ |x| x.safe_inv(cutoff) }.to_dvector - b.safe_inv(cutoff)).trim)
        assert_equal(zeros, (a2.map{ |x| x.safe_sqrt }.to_dvector - a.safe_sqrt).trim)
        assert_equal(zeros, (a2.map{ |x| x.safe_asin }.to_dvector - a.safe_asin).trim)
        assert_equal(zeros, (a2.map{ |x| x.safe_acos }.to_dvector - a.safe_acos).trim)
        a = Dvector[ 1.1, 5.7, 12.7 ]
        assert_equal(zeros, (Dvector[ (1.1).atan2(3.8), (5.7).atan2(3.8), (12.7).atan2(3.8) ] - a.atan2(3.8)).trim) 
        b = Dvector[ 7.1, 4.9, -10.1 ]
        assert_equal(zeros, (Dvector[ (1.1).atan2(7.1), (5.7).atan2(4.9), (12.7).atan2(-10.1) ] - a.atan2(b)).trim)
    end

    def test_numeric
        a = Dvector[11, 22, 33]
        b = Dvector[6.3, -2.1, 3e-40]
        a2 = a.to_a
        b2 = b.to_a
        assert_equal(a2.map{ |x| x.neg }, a.neg)
        assert_equal(a2.map{ |x| x.inv }, a.inv)
        assert_equal(b2.map{ |x| x.abs }, b.abs)
        assert_equal(b2.map{ |x| x.ceil }, b.ceil)
        assert_equal(b2.map{ |x| x.floor }, b.floor)
        assert_equal(b2.map{ |x| x.round }, b.round)
        assert_equal(b2.map{ |x| x.modulo(3) }, b.modulo(3))
        assert_equal(b2.map{ |x| x.remainder(3) }, b.remainder(3))
        a = Dvector[ 1.1, -5.7, 12.7 ]
        b = Dvector[ 7.1, 4.9, -10.1 ]
        assert_equal(Dvector[1.1.mod(7.1), -5.7.mod(4.9), 12.7.mod(-10.1)], a.mod(b))
        assert_equal(Dvector[1.1.mod(7.1), -5.7.mod(4.9), 12.7.mod(-10.1)], a % b)
    end

    def test_where
        a = Dvector[33, 11, 22, 22, 44, 17, 11, 33 ]
        assert_equal(4, a.where_max)
        assert_equal(1, a.where_min)
        assert_equal(5, a.where_closest(16))
        assert_equal(1, a.where_first_eq(11))
        assert_equal(1, a.where_first_ne(33))
        assert_equal(4, a.where_first_gt(35))
        assert_equal(4, a.where_first_ge(44))
        assert_equal(1, a.where_first_lt(12))
        assert_equal(1, a.where_first_le(11))
        i = a.where_first_lt(0)
        assert_equal(nil, i)
        assert_equal(6, a.where_last_eq(11))
        assert_equal(6, a.where_last_ne(33))
        assert_equal(4, a.where_last_gt(35))
        assert_equal(4, a.where_last_ge(44))
        assert_equal(6, a.where_last_lt(12))
        assert_equal(6, a.where_last_le(11))
        i = a.where_last_lt(0)
        assert_equal(nil, i)
        assert_equal(44, a.max)
        assert_equal(11, a.min)
        a = Dvector[ 1, 2, 3, 4, 5, 4, 3, 5, 2 ]
        b = Dvector[ 8, 3, 0, 7 ]
        assert_equal(0, a.min(b))
        assert_equal(8, a.max(b))
        assert_equal(0, Dvector.min_of_many([a,b]))
        assert_equal(8, Dvector.max_of_many([a,b]))
    end
    
    def test_values_at
        a = Dvector[ 1, 2, 3, 4, 5, 6 ]
        assert_equal(Dvector[ 2, 4, 6 ], a.values_at(1, 3, 5))
        assert_equal(Dvector[ 2, 4, 6 ], a.values_at(1, 3, 5, 7)) 
        assert_equal(Dvector[ 6, 4, 2 ], a.values_at(-1, -3, -5, -7)) 
        assert_equal(Dvector[ 2, 3, 4, 3, 4, 5 ], a.values_at(1..3, 2...5)) 
    end
    
    def test_select
        a = Dvector[ 1, 2, 3, 4, 5, 6 ]
        assert_equal(Dvector[2, 4, 6], a.select {|x| x.modulo(2) == 0 })
    end
    
    def test_unshift
        a = Dvector[11, 22, 33]
        assert_equal(Dvector[44, 55, 11, 22, 33], a.unshift(44, 55))
        a = [ 2, 3, 4 ]
        assert_equal(Dvector[ 1, 2, 3, 4 ], a.unshift(1))
        assert_equal(Dvector[ -1, 0, 1, 2, 3, 4 ], a.unshift(-1, 0)) 
    end
    
    def test_uniq
        a = Dvector[ 1.1, 3.8, 1.7, 3.8, 5 ]
        assert_equal(Dvector[1.1, 1.7, 3.8, 5], a.uniq)
        b = Dvector[ 1.1, 3.8, 1.7, 5 ]
        assert_equal(b, b.uniq)
        assert_equal(nil, b.uniq!)
    end
    
    def test_set
        a = Dvector[33, 11, 22, 44, 17 ]
        b = Dvector[2, 5, 1, -3, 9 ]
        c = Dvector[2, 2, 2, 2, 2 ]
        b.set(a)
        assert_equal(a, b)
        b.set(2)
        assert_equal(c, b)
    end
    
    def test_min_gt
        a = Dvector[2, 5, 1, -3, 9 ]
        b = a.min_gt(0)
        assert_equal(1, b)
        b = a.min_gt(10)
        assert_equal(nil, b)
    end
    
    def test_max_lt
        a = Dvector[2, 5, 1, -3, 9 ]
        b = a.max_lt(5)
        assert_equal(2, b)
        b = a.max_lt(-5)
        assert_equal(nil, b)
    end
    
    def test_sort
        a = Dvector[33, 11, 22, 44, 17 ]
        assert_equal(Dvector[11, 17, 22, 33, 44], a.sort)
        assert_equal(Dvector[44, 33, 22, 17, 11], a.sort {|x,y| y <=> x})
    end
    
    def test_shift
        a = Dvector[11, 22, 33, 44 ]
        assert_equal(11, a.shift)
        assert_equal(Dvector[22, 33, 44], a)
    end
    
    def test_rindex
        a = Dvector[11, 22, 33, 44, 33, 11 ]
        assert_equal(5,a.rindex(11))
        assert_equal(4,a.rindex(33))
        assert_nil(a.rindex(32))
    end
    
    def test_reverse
        a = Dvector[11]
        assert_equal(Dvector[11], a.reverse)
        a = Dvector[11, 22]
        assert_equal(Dvector[22, 11], a.reverse)
        a = Dvector[11, 22, 33]
        assert_equal(Dvector[33, 22, 11], a.reverse)
    end
    
    def test_replace
        a = Dvector[11, 22, 33]
        assert_equal(Dvector[44, 55], a.replace(Dvector[44, 55]))
        a = Dvector[11]
        assert_equal(Dvector[44, 55], a.replace([44, 55]))
        a = Dvector[11, 12]
        assert_equal(Dvector[44, 55], a.replace([44, 55]))
        a = Dvector.new(0)
        assert_equal(Dvector[44, 55], a.replace([44, 55]))
    end
    
    def test_push
        a = Dvector[11, 22, 33]
        assert_equal(Dvector[11, 22, 33, 44, 55], a.push(44, 55))
    end
    
    def test_pop
        a = Dvector[11, 22, 33, 44 ]
        assert_equal(44, a.pop)
        assert_equal(Dvector[11, 22, 33], a)
        a = Dvector[]
        assert_equal(nil, a.pop)
    end
    
    def test_reject
        a = Dvector[11, 22, 33, 44 ]
        assert_equal(Dvector[11, 33], a.reject {|x| x.modulo(2) == 0})
    end
    
    def test_last
        a = Dvector[11, 22, 33, 44 ]
        assert_equal(44, a.last)
        b = a.first(1)
        assert_equal(1, b.length)
        assert_equal(Dvector[44], a.last(1))
        assert_equal(Dvector[22, 33, 44], a.last(3))
    end
    
    def test_join
        a = Dvector[11, 22, 33, 44 ]
        assert_equal("11 22 33 44", a.join)
        assert_equal("11-22-33-44", a.join("-"))
    end
    
    def test_insert
        a = Dvector[11, 22, 33, 44 ]
        assert_equal(Dvector[11,22,99,33,44], a.insert(2,99))
        assert_equal(Dvector[11,22,99,33,1,2,3,44], a.insert(-2, 1, 2, 3))
        assert_equal(Dvector[11,22,99,33,1,2,3,44,1], a.insert(-1, 1))
        a = Dvector[ 1, 2, 3 ]
        assert_equal(Dvector[ 1, 2, 99, 3 ], a.insert(2, 99))
        assert_equal(Dvector[ 1, 2, 99, 1, 2, 3, 3 ], a.insert(-2, 1, 2, 3))
        assert_equal(Dvector[ 1, 2, 99, 1, 2, 3, 3, 0 ], a.insert(-1, 0)) 
    end
    
    def test_index
        a = Dvector[11, 22, 33, 44 ]
        assert_equal(0,a.index(11))
        assert_equal(2,a.index(33))
        assert_nil(a.index(32))
    end
    
    def test_include?
        a = Dvector[11, 22, 33, 44 ]
        assert(a.include?(11))
        assert(!a.include?(12))
        
    end
    
    def test_fetch
        a = Dvector[11, 22, 33, 44 ]
        assert_equal(22, a.fetch(1))
        assert_equal(44, a.fetch(-1))
        assert_equal(33, a.fetch(-2, -1))
        assert_equal(-1, a.fetch(4, -1))
        assert_equal(-1, a.fetch(-8, -1))
        assert_equal(16, a.fetch(4) {|i| i*i })
    end
    
    def test_fill
        a = Dvector[11, 22, 33, 44 ]
        assert_equal(Dvector[1, 1, 1, 1], a.fill(1))
        assert_equal(Dvector[1, 1, -1, -1], a.fill(-1, 2, 2))
        assert_equal(Dvector[9,9,-1,-1], a.fill(9,0..1))
        assert_equal(Dvector[0,1,4,9], a.fill {|i| i*i})
        assert_equal(Dvector[0,101,102,103],a.fill(-3) {|i| i + 100 })
        a = Dvector[ 1, 2, 3, 4, 5 ]
        assert_equal(Dvector[ -1, -1, -1, -1, -1 ], a.fill(-1))
        assert_equal(Dvector[ -1, -1, 7, 7, -1 ], a.fill(7, 2, 2))
        assert_equal(Dvector[ 8, 8, 7, 7, -1 ], a.fill(8, 0..1))
        assert_equal(Dvector[0, 1, 4, 9, 16], a.fill {|i| i*i})
        assert_equal(Dvector[0, 1, 4, 27, 64], a.fill(-2) {|i| i*i*i})
        assert_equal(Dvector[0, 1, 4, 27, 64], a) 
    end
    
    def test_first
        a = Dvector[11, 22, 33, 44 ]
        GC.start
        assert_equal(11, a.first)
        b = a.first(1)
        assert_equal(0, Dvector[].length)
        assert_equal(1, b.length)
        assert_equal(Dvector[11], a.first(1))
        assert_equal(Dvector[11, 22, 33], a.first(3))
    end
    
    def test_new
        @dv = Dvector.new
        assert(@dv.empty?)
        @dv = Dvector.new(10)
        assert_equal(10, @dv.length)
        assert_equal(0, @dv[0])
        @dv = Dvector.new(5,1)
        assert_equal(5, @dv.length)
        assert_equal(1, @dv[0])
        @dv = Dvector.new(3) { |i| i*i }
        assert_equal(3, @dv.length)
        assert_equal(4, @dv[2])
        @dv = Dvector.new(5)
        @dv2 = Dvector.new(@dv)
        assert_equal(@dv, @dv2)
        assert_equal(Dvector[ -1, -1, -1 ], Dvector.new(3, -1)) 
        assert_equal(Dvector[ 1, 2, 5 ], Dvector.new(3) {|i| i**2 + 1})
        @dv.resize(10)
        assert_equal(10, @dv.size)
        assert_equal(0, @dv[9])
    end
    
    def test_append
        @dv = Dvector[0] << 1 << 2 << 3
        assert_equal(4, @dv.length)
        assert_equal(2, @dv[2])
        assert_equal(Dvector[0, 1, 2, 3], @dv)
    end
    
    def test_cmp
        @dv = Dvector[0, 1, 2, 3]
        @dv2 = Dvector[0, 1, 2]
        assert_equal(1, @dv <=> @dv2)
        assert_equal(-1, @dv2 <=> @dv)
        @dv2 = Dvector[0, 1, 2, 3]
        assert_equal(0, @dv <=> @dv2)
        @dv2[0] = 4
        assert_equal(-1, @dv <=> @dv2)
        assert_equal(1, @dv2 <=> @dv)
        assert_equal(-1, Dvector[ 1, 1, 2 ] <=> Dvector[ 1, 2, 3 ])
        assert_equal(1, Dvector[ 1, 1, 2 ] <=> Dvector[ 1, 0, 3 ])
        assert_equal(-1, Dvector[ 1, 1, 2 ] <=> Dvector[ 1, 1, 2, 3 ])
        assert_equal(1, Dvector[ 1, 2, 3, 4, 5, 6 ] <=> Dvector[ 1, 2 ])
    end
    
    def test_eql
        @dv = Dvector.new
        assert_equal(0, @dv.length)
        assert_not_equal(@dv, nil)
        @dv = Dvector[0, 1, 2, 3]
        assert_equal(4, @dv.length)
        @dv2 = Dvector[0, 1, 2]
        assert_equal(1, @dv[1])
        assert_equal(1, @dv2[1])
        assert_not_equal(@dv, @dv2)
        @dv2 << 3
        GC.start
        assert_equal(4, @dv2.length)
        assert_equal(3, @dv2[3])
        assert_equal(@dv2, @dv)
        assert(Dvector[1,2,3].eql?([1,2,3]))
    end
    
    def test_aref
        a = Dvector[ 1, 2, 3, 4, 5 ]
        assert_equal(6, a[2] +  a[0] + a[1])
        assert_equal(nil, a[6])
        assert_equal(Dvector[ 2, 3 ], a[1, 2])
        assert_equal(Dvector[ 2, 3, 4 ], a[1..3]) 
        assert_equal(Dvector[ 5 ], a[4..7])
        assert_equal(nil, a[6..10])
        assert_equal(Dvector[ 3, 4, 5 ], a[-3, 3]) 
        assert_equal(nil, a[5])
        assert_equal(Dvector[], a[5, 1])
        assert_equal(Dvector[], a[5..10])
    end
    
    def test_aset
        @dv = Dvector[]
        @dv[4] = 3.3
        assert_equal(5, @dv.length)
        assert_equal(3.3, @dv[4])
        assert_equal(0, @dv[1])
        @dv[-2] = 2
        assert_equal(5, @dv.length)
        assert_equal(2, @dv[3])
        @dv[1,2] = [5, 4]
        assert_equal(5, @dv.length)
        assert_equal(5, @dv[1])
        assert_equal(4, @dv[2])
        assert_equal(2, @dv[3])
        @dv[2..3] = nil
        assert_equal(3, @dv.length)
        assert_equal(3.3, @dv[-1])
        @dv[1...3] = [6, 7, 8, 9]
        assert_equal(5, @dv.length)
        assert_equal(9, @dv[4])
        a = Dvector.new
        a[4] = 4
        assert_equal(Dvector[ 0, 0, 0, 0, 4 ], a)
        a[0, 3] = [ 1, 2, 3 ]
        assert_equal(Dvector[ 1, 2, 3, 0, 4 ], a)
        a[1..2] = [ 1, 2 ]
        assert_equal(Dvector[ 1, 1, 2, 0, 4 ], a)
        a[0, 2] = -1
        assert_equal(Dvector[ -1, 2, 0, 4 ], a)
        a[0..2] = 1
        assert_equal(Dvector[ 1, 4 ], a)
        a[-1]   = 5
        assert_equal(Dvector[ 1, 5 ], a)
        a[1..-1] = nil
        assert_equal(Dvector[ 1 ], a)
    end
    
    def test_clear
        @dv = Dvector.new(10) {|i| i*3 }
        assert_equal(3, @dv.at(1))
        @dv.clear
        assert_equal(0, @dv.size)
    end
    
    def test_collect!
        @dv = Dvector.new(10) {|i| i*3 }
        assert_equal(3, @dv.at(1))
        @dv.collect! {|x| x-1 }
        assert_equal(2, @dv.at(1))
    end
    
    def test_concat
        @dv = Dvector.new
        assert_equal(0, @dv.length)
        @dv.concat(Dvector.new(2))
        assert_equal(2, @dv.length)
        @dv = Dvector.new(3)
        @dv.concat(Dvector.new(2))
        assert_equal(5, @dv.length)
    end
    
    def test_delete
        @dv = Dvector.new(10) {|i| i*3 }
        assert_equal(6, @dv[2])
        assert_equal(6, @dv.delete(6))
        assert_equal(9, @dv.length)
        assert_equal(3, @dv[1])
        assert_equal(9, @dv[2])
        assert_equal(nil, @dv.delete(2))
        assert_equal("not found", @dv.delete(2) { "not found" })
    end
    
    def test_delete_at
        @dv = Dvector.new(10) {|i| i*3 }
        assert_equal(6, @dv[2])
        x = @dv.delete_at(2)
        GC.start
        assert_equal(6, x)
        assert_equal(9, @dv.length)
        assert_equal(3, @dv[1])
        assert_equal(9, @dv[2])
        assert_equal(nil, @dv.delete_at(20))
    end
    
    def test_prune
        @dv = Dvector.new(5) {|i| i*3 }
        assert_equal(6, @dv[2])
        assert_equal(5, @dv.length)
        x = @dv.prune([0,2])
        assert_equal(3, x.length)
        assert_equal(5, @dv.length)
        assert_equal(3, x[0])
        assert_equal(9, x[1])
    end
    
    def test_prune_bang
        @dv = Dvector.new(5) {|i| i*3 }
        assert_equal(6, @dv[2])
        assert_equal(5, @dv.length)
        x = @dv.prune!([0,2])
        assert_equal(3, x.length)
        assert_equal(3, @dv.length)
        assert_equal(3, x[0])
        assert_equal(9, x[1])
        assert_equal(@dv[0], x[0])
        assert_equal(@dv[1], x[1])
    end
    
    def test_slice!
        a = Dvector.new(5) {|i| i*3 }
        assert_equal(3, a.slice!(1)) 
        assert_equal(Dvector[0, 6, 9, 12], a) 
        assert_equal(12, a.slice!(-1)) 
        assert_equal(Dvector[0, 6, 9], a) 
        assert_equal(nil, a.slice!(100)) 
        assert_equal(Dvector[0, 6, 9], a) 
        assert_equal(Dvector[6, 9], a.slice!(1..2)) 
        assert_equal(Dvector[0], a) 
        assert_equal(Dvector[], a.slice!(1..2)) 
        assert_equal(Dvector[0], a)
        assert_equal(Dvector[0], a.slice!(0..2)) 
        assert_equal(Dvector[], a) 
    end
    
    def test_delete_if
        @dv = Dvector.new(10) {|i| i*3 }
        @dv.delete_if {|x| x > 2 && x < 8}
        assert_equal(8, @dv.length)
        assert_equal(9, @dv[1])
    end
    
    def test_at
        @dv = Dvector.new(10) {|i| i*3 }
        assert_equal(3, @dv.at(1))
    end
    
    def test_freeze
        a = Dvector[1]
        assert_equal(false, a.frozen?)
        a.freeze
        assert_equal(true, a.frozen?)
    end
    
    def test_each
        t = 0
        n = 0
        @dv = Dvector.new(10) {|i| n = n + i; t = t + 2*i; 2*i }
        s = 0
        @dv.each {|x| s = s + x}
        assert_equal(s, t)
        s = 0
        @dv.each_index { |i| s = s + i }
        assert_equal(s, n)
        @dv.each_with_index {|x,i| assert_equal(x, 2*i)}
        @dv2 = []
        @dv.reverse_each_with_index {|x,i| @dv2[9-i] = x}
        assert_equal(10, @dv2.length)
        assert_equal(@dv, @dv2.reverse)
        @dv2 = []
        @dv.reverse_each_with_index {|x,i| @dv2[i] = x}
        assert_equal(10, @dv2.length)
        @dv2.reverse!
        assert_equal(10, @dv2.length)
        @dv3 = @dv2.reverse
        assert_equal(10, @dv3.length)
        assert_equal(@dv, @dv3)
        @dv.reverse!
        @dv = Dvector[]
        @dv = Dvector.new(10)
        @dv1 = Dvector.new(10) {|i| 2*i }
        @dv3.reverse!
        dv4 = @dv.reverse
        dv5 = dv4.reverse
        dv5.reverse!
        assert_equal(dv4, dv5)
    end
    
    def test_linear_interpolate
        len = 11
        xs = Dvector.new(len) {|i| i*1.0/(len-1) }
        ys = Dvector.new(len) {|i| i*2.0/(len-1) }
        ntest = 8
        ntest.times do |i|
            x = i*1.0/(ntest-1)
            diff = 2*x - Dvector.linear_interpolate(x, xs, ys)
            diff = 0 if diff.abs < 1e-10
            assert_equal(0, diff)
        end
    end
    
    def test_spline_interpolate
        len = 8
        xs = Dvector.new(len) {|i| i*1.0/(len-1) }
        ys = Dvector.new(len) {|i| x =i*1.0/(len-1); (x-0.1)*(x-0.33)*(x-0.75) }
        interpolant = Dvector.create_spline_interpolant(xs, ys, false, 0, false, 0)
        ntest = 9
        ntest.times do |i|
            x = i*1.0/(ntest-1)
            diff = (x-0.1)*(x-0.33)*(x-0.75) - Dvector.spline_interpolate(x, interpolant)
            diff = 0 if diff.abs < 1e-2
            assert_equal(0, diff)
            #puts "#{i} #{x} #{diff}"
        end
    end
    
    def test_bezier_control_points
        dest = Dvector.new
        delta_x = 1; x0 = 10; x1 = x0 + delta_x/3.0
        x2 = x1 + delta_x/3.0; x3 = x0 + delta_x
        y0 = 5; y1 = 8; y2 = 3; y3 = 7
        c = 3.0*(y1-y0); b = 3.0*(y2-y1) - c; a = y3 - y0 - c - b
        dest.make_bezier_control_points_for_cubic_in_x(x0, y0, delta_x, a, b, c)
        assert_equal(6, dest.length)
        assert((dest[0]-x1).abs < 1e-6)
        assert((dest[1]-y1).abs < 1e-6)
        assert((dest[2]-x2).abs < 1e-6)
        assert((dest[3]-y2).abs < 1e-6)
        assert((dest[4]-x3).abs < 1e-6)
        assert((dest[5]-y3).abs < 1e-6)
    end

    FANCY_READ_TEXT = <<"EOT"
# some comments

# and a blank line above
1.2  2.4
1.3 2.4\t3.5
1.2
EOT

    FANCY_READ_TEXT_2 = <<"EOT"
# some comments

# and a blank line above
   # note the initial whitespace, stripped by default:
  -1.2  2.4
 -1.3 2.4\t3.5
   -1.2
EOT

    def test_fancy_read 
      stream = StringIO.new(FANCY_READ_TEXT)
      cols = Dvector.fancy_read(stream, nil, 'default'=> 0.0)
      cols2 = [Dvector[1.2, 1.3, 1.2], Dvector[2.4, 2.4, 0.0],
               Dvector[0.0, 3.5, 0.0]]
      3.times do |i|
        assert_equal(cols[i], cols2[i])
      end

      stream = StringIO.new(FANCY_READ_TEXT)
      cols = Dvector.fancy_read(stream, [1,2], 'default'=> 0.0)
      assert_equal(cols[0], cols2[1])
      assert_equal(cols[1], cols2[2])

      stream = StringIO.new(FANCY_READ_TEXT_2)
      cols = Dvector.fancy_read(stream, nil, 'default'=> 0.0)
      cols2 = [Dvector[-1.2, -1.3, -1.2], Dvector[2.4, 2.4, 0.0],
               Dvector[0.0, 3.5, 0.0]]
      assert_equal(cols, cols2)
    end

    def test_fast_fancy_read 
      stream = StringIO.new(FANCY_READ_TEXT)
      ary = []
      cols = Dvector.fast_fancy_read(stream, {
                                       'sep' => /\s+/,
                                       'comments' => /^\s*\#/,
                                       'comment_out' => ary,
                                       'skip_first' => 0,
                                       'index_col' => false,
                                       'remove_space' => true,
                                       'default'=> 0.0})
      assert_equal(ary, ["# some comments\n", "# and a blank line above\n"])
      cols2 = [Dvector[1.2, 1.3, 1.2], Dvector[2.4, 2.4, 0.0],
               Dvector[0.0, 3.5, 0.0]]
      3.times do |i|
        assert_equal(cols[i], cols2[i])
      end

      stream = StringIO.new(FANCY_READ_TEXT_2)
      cols = Dvector.fast_fancy_read(stream, {
                                       'sep' => /\s+/,
                                       'comments' => /^\s*\#/,
                                       'skip_first' => 0,
                                       'index_col' => false,
                                       'remove_space' => true,
                                       'default'=> 0.0})
      cols2 = [Dvector[-1.2, -1.3, -1.2], Dvector[2.4, 2.4, 0.0],
               Dvector[0.0, 3.5, 0.0]]
      assert_equal(cols, cols2)
    end

    def test_ffr_2
      text = ""
      6.times do |i|
        text << "#{i}\t#{i*0.5}\tmy_#{i}\t#{i**2}\tThis is the line number #{i}\n"
      end
      
      stream = StringIO.new(text)
      cols = Dvector.fancy_read(stream)
      assert_equal(cols.size, 10)
      6.times do |i|
        assert_equal(cols[0][i], i.to_f)
        assert_equal(cols[1][i], i.to_f*0.5)
        assert(cols[2][i].nan?)
        assert_equal(cols[3][i], i.to_f**2)
        assert_equal(cols[9][i], i)
      end

      # Next try, slurping all lines
      stream = StringIO.new(text)
      cols = Dvector.fancy_read(stream, nil, { 'last_col' => 4})
      assert_equal(cols.size, 6)
      6.times do |i|
        assert_equal(cols[0][i], i.to_f)
        assert_equal(cols[1][i], i.to_f*0.5)
        assert(cols[2][i].nan?)
        assert_equal(cols[3][i], i.to_f**2)
        assert(cols[4][i].nan?)
        assert_equal(cols[5][i], "is the line number #{i}")
      end

      stream = StringIO.new(text)
      cols = Dvector.fancy_read(stream, nil, { 
                                  'last_col' => 3,
                                  'text_columns' => [0, 2]
                                })
      assert_equal(cols.size, 5)
      6.times do |i|
        assert_equal(cols[0][i], i.to_s)
        assert_equal(cols[1][i], i.to_f*0.5)
        assert_equal(cols[2][i], "my_#{i}")
        assert_equal(cols[3][i], i.to_f**2)
        assert_equal(cols[4][i], "This is the line number #{i}")
      end
    end

    def test_compute_formula
      v = Dvector[1,2,3]
      w = Dvector[3,2,1]
      cal = Dvector.compute_formula("column[0] + column[1]", [v,w])
      assert_equal(cal, Dvector[4,4,4])
      cal = Dvector.compute_formula("sin(column[0]) " +
                                    "** 2 + cos(column[0]) ** 2", [v,w])
      cal.sub!(1)
      cal.abs!
      assert(cal.max < 1e-10)   # We need this trick as computations are
      # not perfectly precise.
    end

    def test_dirtyness
      v = Dvector.new(10)
      assert(! v.dirty?)
      assert(v.clean?)
      v[1] = 1
      assert(v.dirty?)
      v.dirty = false
      assert(! v.dirty?)
    end

    def test_marshall
      v = Dvector[1, 2, 3]
      s = Marshal.dump(v)
      v_bis = Marshal.restore(s)
      assert_equal(v, v_bis)
    end

    NB_NUMBERS = 10000
    def test_stress_marshall
      1.times do 
        v = Dvector.new
        1.upto(NB_NUMBERS) do 
          v << rand * 10 ** (100 - 100 * rand)
        end
        s = Marshal.dump(v)
        v_bis = Marshal.restore(s)
        assert_equal(v, v_bis)
      end
    end

    def test_bounds
      v = Dvector[0.0/0.0, 0.0/0.0, 1,2,4,5,9,0.0/0.0,0.1]
      assert_equal(v.bounds, [0.1, 9])
    end

    def test_write_dvectors
      a = Dvector[1,2,3]
      b = Dvector[3,2,1]
      out = StringIO.new("", "w")
      Dvector.write(out, [a,b])
      out.close
      input = StringIO.new(out.string)
      c,d = Dvector.fancy_read(input)
      assert_equal(a,c)
      assert_equal(b,d)
    end

    # Test the search for local extrema
    def test_extrema
      a = Dvector.new
      101.times do |i|
        a << Math.sin(0.1 * 3.141592 * i)
      end
      
      vals = [[:min, 0], [:max, 5], [:min, 15], 
              [:max, 25], [:min, 35], [:max, 45], 
              [:min, 55], [:max, 65], [:min, 75], [:max, 85], 
              [:min, 95], [:max, 100]]
      vals2 = vals.dup
      vals2.shift
      vals2.pop

      ext = a.extrema
      assert_equal(vals, ext)

      # Same, but with a larger window: we get rid of what is on both
      # sides
      ext = a.extrema('window' => 12)
      assert_equal(vals2, ext)

      ext = a.extrema('threshold' => 0.1) # Also gets rid of artefacts
                                          # on both sides
      assert_equal(vals2, ext)

      ext = a.extrema('dthreshold' => 0.1) # Keep both sides
      assert_equal(vals, ext)


      ext = a.extrema('dthreshold' => 0.1, 'threshold' => 0.1) # Drops sides
      assert_equal(vals2, ext)

      ext = a.extrema('dthreshold' => 0.1, 
                      'threshold' => 0.1, 'or' => true) # Drops sides
      assert_equal(vals, ext)
    end
    
end
