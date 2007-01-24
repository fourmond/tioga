#  tc_Dtable.rb

require 'Dobjects/Dtable'
require 'test/unit'

class TestDtable < Test::Unit::TestCase

    include Dobjects

    def read_data  # do this first to read in the test.data array
        return if @arr != nil
        @num_cols = 4; @num_rows = 5
        @arr = Dtable.new(@num_cols, @num_rows)
        dir = File.dirname(__FILE__)
        if dir.empty?
          file = "dtable_test.data"
        else
          file = "#{dir}/dtable_test.data"
        end
        @arr.read(file, 1) # skip the first line
        assert_equal(@num_cols, @arr.num_cols)
        assert_equal(@num_rows, @arr.num_rows)
    end
    
    def test_at
        read_data
        assert_equal(7, @arr.at(1,2))
        assert_equal(0.1, @arr.at(2,0))
        assert_equal(-1.1, @arr.at(2,1))
        assert_equal(2.2, @arr[2,2])
        assert_equal(-3.3, @arr[2,3])
        tmp = @arr[3,2]
        @arr[3,2] = 1e3
        assert_equal(1e3, @arr[3,2])
        @arr[3,2] = tmp
        assert_equal(tmp, @arr[3,2])
    end
    
    def test_array_ops
        read_data
        @ar2 = @arr.dup
        @num_rows.times do |i|
            @num_cols.times do |j|
                assert_equal(@arr[i,j], @ar2[i,j])
            end
        end
        assert_equal(@num_rows, @arr.num_rows)
        @num_rows.times do |i|
            @num_cols.times do |j|
                assert_equal(@arr[i,j], @ar2[i,j])
            end
        end
        last_row = @num_rows - 1
        last_col = @num_cols - 1
        @arr[0,0] = 0
        @arr[0,last_col] = 1
        @arr[last_row,0] = 2
        @arr[last_row,last_col] = 3
        @ar2 = @arr.transpose
        assert_equal(@num_cols, @ar2.num_rows)
        assert_equal(@num_rows, @ar2.num_cols)
        assert_equal(@ar2[0,0], 0)
        assert_equal(@ar2[last_col,0], 1)
        assert_equal(@ar2[0,last_row], 2)
        assert_equal(@ar2[last_col,last_row], 3)
        @num_rows.times do |i|
            @num_cols.times do |j|
                assert_equal(@arr[i,j], @ar2[j,i])
            end
        end
        @ar2 = @arr.reverse_rows
        assert_equal(@num_cols, @ar2.num_cols)
        assert_equal(@num_rows, @ar2.num_rows)
        assert_equal(@ar2[0,0], 2)
        assert_equal(@ar2[0,last_col], 3)
        assert_equal(@ar2[last_row,0], 0)
        assert_equal(@ar2[last_row,last_col], 1)
        @num_rows.times do |i|
            @num_cols.times do |j|
                assert_equal(@arr[i,j], @ar2[last_row-i,j])
            end
        end
        @ar2 = @arr.reverse_cols
        assert_equal(@num_cols, @ar2.num_cols)
        assert_equal(@num_rows, @ar2.num_rows)
        assert_equal(@ar2[0,0], 1)
        assert_equal(@ar2[0,last_col], 0)
        assert_equal(@ar2[last_row,0], 3)
        assert_equal(@ar2[last_row,last_col], 2)
        @num_rows.times do |i|
            @num_cols.times do |j|
                assert_equal(@arr[i,j], @ar2[i,last_col-j])
            end
        end
        @ar2 = @arr.rotate_ccw90
        assert_equal(@num_cols, @ar2.num_rows)
        assert_equal(@num_rows, @ar2.num_cols)
        assert_equal(@ar2[0,0], 1)
        assert_equal(@ar2[0,last_row], 3)
        assert_equal(@ar2[last_col,0], 0)
        assert_equal(@ar2[last_col,last_row], 2)
        @ar2 = @arr.rotate_cw90
        assert_equal(@num_cols, @ar2.num_rows)
        assert_equal(@num_rows, @ar2.num_cols)
        assert_equal(@ar2[0,0], 2)
        assert_equal(@ar2[0,last_row], 0)
        assert_equal(@ar2[last_col,0], 3)
        assert_equal(@ar2[last_col,last_row], 1)
        @ar2.set(-1)
        @num_rows.times do |i|
            @num_cols.times do |j|
                assert_equal(-1, @ar2[j,i])
            end
        end
        assert_equal(-9, @arr.min)
        assert_equal(8, @arr.max)
        assert_equal(2, @arr.min_gt(1))
        assert_equal(7, @arr.max_lt(8))
    end

    def test_math_ops
        read_data
        @ar2 = @arr + 2
        @num_rows.times do |i|
            @num_cols.times do |j|
                assert_equal(@arr[i,j]+2, @ar2[i,j])
            end
        end
        @ar2 = 2 * @arr
        @num_rows.times do |i|
            @num_cols.times do |j|
                assert_equal(2 * @arr[i,j], @ar2[i,j])
            end
        end
        @ar2 = @arr.floor
        @num_rows.times do |i|
            @num_cols.times do |j|
                assert_equal(@arr[i,j].floor, @ar2[i,j])
            end
        end
        @ar2 = @arr.pow(3)
        @num_rows.times do |i|
            @num_cols.times do |j|
                assert_equal(@arr[i,j].pow(3), @ar2[i,j])
            end
        end
        @ar2 = @arr.safe_log10
        @num_rows.times do |i|
            @num_cols.times do |j|
                assert_equal(@arr[i,j].safe_log10, @ar2[i,j])
            end
        end
    end
    
    def test_bang_ops
        read_data
        @ar2 = @arr.dup.abs!.safe_sqrt!
        @num_rows.times do |i|
            @num_cols.times do |j|
                assert_equal(@arr[i,j].abs.safe_sqrt, @ar2[i,j])
            end
        end
    end
    
    def test_dvec_ops
        read_data
        @row1 = @arr.row(1)
        assert_equal(@num_cols, @row1.size)
        @num_cols.times do |j|
            assert_equal(@arr[1,j], @row1[j])
        end
        @col1 = @arr.column(1)
        assert_equal(@num_rows, @col1.size)
        @num_rows.times do |i|
            assert_equal(@arr[i,1], @col1[i])
        end
        @arr.set_row(2,@row1)
        @num_cols.times do |j|
            assert_equal(@arr[2,j], @row1[j])
        end
        @arr.set_column(2,@col1)
        @num_rows.times do |i|
            assert_equal(@arr[i,2], @col1[i])
        end
    end

    def test_marshal
      t = Dtable.new(3,4)
      t[1,1] = 1.2
      t[3,2] = 2.5
      
      str = Marshal.dump(t)
      tbis = Marshal.restore(str)
      assert_equal(t.num_rows, tbis.num_rows)
      i = 0
      while i < t.num_rows
        assert_equal(t.row(i), tbis.row(i))
        i += 1
      end
    end

end



















