// ensure these overload ok

struct S {
    operator float() const;
    operator int() const;
};

float f;
int i;


// #pragma on ( dump_rank )

void foo( S& s )
{
    f = s;
    f += s;
    f -= s;
    f *= s;
    f /= s;

    i = s;
    i += s;
    i -= s;
    i *= s;
    i %= s;
    i &= s;
    i |= s;
    i ^= s;
    i <<= s;
    i >>= s;
};


#error this should be the only error


// #pragma on (dump_exec_ic)
