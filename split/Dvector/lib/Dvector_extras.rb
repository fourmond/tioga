#  Tioga::Dvector_extras.rb

class Numeric

    def mod(y)
        self.modulo(y)
    end

end

# Modified by Vincent Fourmond to have a nice Dvector module
# instead of the Tioga module.
# add arithmetic operators for Float and Fixnum with Dvec

class Array
    def to_dvector
        Dobjects::Dvector[*self]
    end
end

class Float
    alias :pre_dvec_add :+
    def +(a)
        if a.class == Dobjects::Dvector
            a+self
        else
            pre_dvec_add(a)
        end
    end
    alias :pre_dvec_sub :-
    def -(a)
        if a.class == Dobjects::Dvector
            a.neg+self
        else
            pre_dvec_sub(a)
        end
    end
    alias :pre_dvec_mult :*
    def *(a)
        if a.class == Dobjects::Dvector
            a*self
        else
            pre_dvec_mult(a)
        end
    end
    alias :pre_dvec_div :/
    def /(a)
        if a.class == Dobjects::Dvector
            a.inv*self
        else
            pre_dvec_div(a)
        end
    end
    alias :pre_dvec_pow :**
    def **(a)
        if a.class == Dobjects::Dvector
            a.as_exponent_of(self)
        else
            pre_dvec_pow(a)
        end
    end
end

class Fixnum
    alias :pre_dvec_add :+
    def +(a)
        if a.class == Dobjects::Dvector
            a+self
        else
            pre_dvec_add(a)
        end
    end
    alias :pre_dvec_sub :-
    def -(a)
        if a.class == Dobjects::Dvector
            a.neg+self
        else
            pre_dvec_sub(a)
        end
    end
    alias :pre_dvec_mult :*
    def *(a)
        if a.class == Dobjects::Dvector
            a*self
        else
            pre_dvec_mult(a)
        end
    end
    alias :pre_dvec_div :/
    def /(a)
        if a.class == Dobjects::Dvector
            a.inv*self
        else
            pre_dvec_div(a)
        end
    end
    alias :pre_dvec_pow :**
    def **(a)
        if a.class == Dobjects::Dvector
            a.as_exponent_of(self)
        else
            pre_dvec_pow(a)
        end
    end
end
