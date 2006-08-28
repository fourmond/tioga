#  pre_ddata_extras.rb

# add arithmetic operators for Float and Fixnum with Dtable

class Float
    alias :pre_ddata_add :+
    def +(a)
        if a.class == Dobjects::Dtable
            a+self
        else
            pre_ddata_add(a)
        end
    end
    alias :pre_ddata_sub :-
    def -(a)
        if a.class == Dobjects::Dtable
            a.neg+self
        else
            pre_ddata_sub(a)
        end
    end
    alias :pre_ddata_mult :*
    def *(a)
        if a.class == Dobjects::Dtable
            a*self
        else
            pre_ddata_mult(a)
        end
    end
    alias :pre_ddata_div :/
    def /(a)
        if a.class == Dobjects::Dtable
            a.inv*self
        else
            pre_ddata_div(a)
        end
    end
    alias :pre_ddata_pow :**
    def **(a)
        if a.class == Dobjects::Dtable
            a.as_exponent_of(self)
        else
            pre_ddata_pow(a)
        end
    end
end

class Fixnum
    alias :pre_ddata_add :+
    def +(a)
        if a.class == Dobjects::Dtable
            a+self
        else
            pre_ddata_add(a)
        end
    end
    alias :pre_ddata_sub :-
    def -(a)
        if a.class == Dobjects::Dtable
            a.neg+self
        else
            pre_ddata_sub(a)
        end
    end
    alias :pre_ddata_mult :*
    def *(a)
        if a.class == Dobjects::Dtable
            a*self
        else
            pre_ddata_mult(a)
        end
    end
    alias :pre_ddata_div :/
    def /(a)
        if a.class == Dobjects::Dtable
            a.inv*self
        else
            pre_ddata_div(a)
        end
    end
    alias :pre_ddata_pow :**
    def **(a)
        if a.class == Dobjects::Dtable
            a.as_exponent_of(self)
        else
            pre_ddata_pow(a)
        end
    end
end

