#  Tioga::Dvector_extras.rb

class Numeric

    def mod(y)
        self.modulo(y)
    end

end

# add arithmetic operators for Float and Fixnum with Dvec

class Array
    def to_dvector
        Tioga::Dvector[*self]
    end
end

class Float
    alias :pre_dvec_add :+
    def +(a)
        if a.class == Tioga::Dvector
            a+self
        else
            pre_dvec_add(a)
        end
    end
    alias :pre_dvec_sub :-
    def -(a)
        if a.class == Tioga::Dvector
            a.neg+self
        else
            pre_dvec_sub(a)
        end
    end
    alias :pre_dvec_mult :*
    def *(a)
        if a.class == Tioga::Dvector
            a*self
        else
            pre_dvec_mult(a)
        end
    end
    alias :pre_dvec_div :/
    def /(a)
        if a.class == Tioga::Dvector
            a.inv*self
        else
            pre_dvec_div(a)
        end
    end
    alias :pre_dvec_pow :**
    def **(a)
        if a.class == Tioga::Dvector
            a.as_exponent_of(self)
        else
            pre_dvec_pow(a)
        end
    end
end

class Fixnum
    alias :pre_dvec_add :+
    def +(a)
        if a.class == Tioga::Dvector
            a+self
        else
            pre_dvec_add(a)
        end
    end
    alias :pre_dvec_sub :-
    def -(a)
        if a.class == Tioga::Dvector
            a.neg+self
        else
            pre_dvec_sub(a)
        end
    end
    alias :pre_dvec_mult :*
    def *(a)
        if a.class == Tioga::Dvector
            a*self
        else
            pre_dvec_mult(a)
        end
    end
    alias :pre_dvec_div :/
    def /(a)
        if a.class == Tioga::Dvector
            a.inv*self
        else
            pre_dvec_div(a)
        end
    end
    alias :pre_dvec_pow :**
    def **(a)
        if a.class == Tioga::Dvector
            a.as_exponent_of(self)
        else
            pre_dvec_pow(a)
        end
    end
end
