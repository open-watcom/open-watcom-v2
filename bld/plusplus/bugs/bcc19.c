// (JWW) -- should convert to common reference

struct base
{
    int base1[3];
};

struct der : base
{
    int der1[5];
};

base b1;
der d1;

base& foo( int p, base& b1, der& d1 )
{
    return p ? b1 : d1;
}
