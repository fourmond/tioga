#  pre_ddata_extras.rb

class Numeric

    def mod(y)
        self.modulo(y)
    end

end

# add arithmetic operators for Float and Fixnum with Dtable

class Float
    alias :pre_ddata_add :+
    def +(a)
        if a.class == Tioga::Dtable
            a+self
        else
            pre_ddata_add(a)
        end
    end
    alias :pre_ddata_sub :-
    def -(a)
        if a.class == Tioga::Dtable
            a.neg+self
        else
            pre_ddata_sub(a)
        end
    end
    alias :pre_ddata_mult :*
    def *(a)
        if a.class == Tioga::Dtable
            a*self
        else
            pre_ddata_mult(a)
        end
    end
    alias :pre_ddata_div :/
    def /(a)
        if a.class == Tioga::Dtable
            a.inv*self
        else
            pre_ddata_div(a)
        end
    end
    alias :pre_ddata_pow :**
    def **(a)
        if a.class == Tioga::Dtable
            a.as_exponent_of(self)
        else
            pre_ddata_pow(a)
        end
    end
end

class Fixnum
    alias :pre_ddata_add :+
    def +(a)
        if a.class == Tioga::Dtable
            a+self
        else
            pre_ddata_add(a)
        end
    end
    alias :pre_ddata_sub :-
    def -(a)
        if a.class == Tioga::Dtable
            a.neg+self
        else
            pre_ddata_sub(a)
        end
    end
    alias :pre_ddata_mult :*
    def *(a)
        if a.class == Tioga::Dtable
            a*self
        else
            pre_ddata_mult(a)
        end
    end
    alias :pre_ddata_div :/
    def /(a)
        if a.class == Tioga::Dtable
            a.inv*self
        else
            pre_ddata_div(a)
        end
    end
    alias :pre_ddata_pow :**
    def **(a)
        if a.class == Tioga::Dtable
            a.as_exponent_of(self)
        else
            pre_ddata_pow(a)
        end
    end
end

