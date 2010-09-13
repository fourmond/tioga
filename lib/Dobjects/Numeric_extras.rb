#  Numeric_extras.rb

# add some Numeric methods

class Numeric

    # Math functions

    def acos
        Math.acos(self)
    end
    
    def acosh
        Math.acosh(self)
    end
    
    def asin
        Math.asin(self)
    end
    
    def asinh
        Math.asinh(self)
    end
    
    def atan
        Math.atan(self)
    end
    
    def atanh
        Math.atanh(self)
    end
    
    def atan2(n)
        Math.atan2(self,n)
    end
    
    def cos
        Math.cos(self)
    end
    
    def cosh
        Math.cosh(self)
    end
    
    def exp
        Math.exp(self)
    end
    
    def log
        Math.log(self)
    end
    
    def log10
        Math.log10(self)
    end
    
    def sin
        Math.sin(self)
    end
    
    def sinh
        Math.sinh(self)
    end
    
    def sqrt
        Math.sqrt(self)
    end
    
    def tan
        Math.tan(self)
    end
    
    def tanh
        Math.tanh(self)
    end
    
    def neg
        -self
    end
    
    def exp10
        10**self
    end
    
    def inv
        1/self
    end
    
    def trim(cutoff=1e-6)
        (self.abs < cutoff)? 0.0 : self
    end
    
    def pow(y)
        self**y
    end
    
    def raised_to(y)
        self**y
    end
    
    def as_exponent_of(y)
        y**self
    end
    
    def safe_log(cutoff=1e-99)
        ((self > cutoff)? self : cutoff).log
    end
    
    def safe_log10(cutoff=1e-99)
        ((self > cutoff)? self : cutoff).log10
    end
    
    def safe_inv(cutoff=1e-99)
        (self.abs > cutoff)? 1/self : (self > 0)? 1/cutoff : -1/cutoff
    end
    
    def safe_sqrt
        (self > 0.0)? self.sqrt : 0.0
    end
    
    def safe_asin
        ((self > 1.0)? 1.0 : (self < -1.0)? -1.0 : self).asin
    end
    
    def safe_acos
        ((self > 1.0)? 1.0 : (self < -1.0)? -1.0 : self).acos
    end

    def mod(y)
        self.modulo(y)
    end

    
end
