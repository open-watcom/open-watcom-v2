#include "fail.h"

struct S {
    unsigned char	b0:1;
    unsigned char	b1:1;
    unsigned char	b2:1;
    unsigned char	b3:1;
    unsigned char	b4:1;
    unsigned char	b5:1;
    unsigned char	b6:1;
    unsigned char	b7:1;
    unsigned char	b8:1;
    unsigned char	b9:1;
    unsigned char	b10:1;
    unsigned char	b11:1;
    unsigned char	b12:1;
    unsigned char	b13:1;
    unsigned char	b14:1;
    unsigned char	b15:1;
    unsigned char	b16:1;
    unsigned char	b17:1;
    unsigned char	b18:1;
    unsigned char	b19:1;
};

S x = {
    1,
    0,
    1,
    0,
    1,
    0,
    0,
    1,
    0,
    0,
    0,
    1,
    0,
    0,
    0,
    0,
    0,
    1,
    0,
    0,
};

void pass( S value )
{
    if( value.b0 != x.b0 ) fail(__LINE__);
    if( value.b1 != x.b1 ) fail(__LINE__);
    if( value.b2 != x.b2 ) fail(__LINE__);
    if( value.b3 != x.b3 ) fail(__LINE__);
    if( value.b4 != x.b4 ) fail(__LINE__);
    if( value.b5 != x.b5 ) fail(__LINE__);
    if( value.b6 != x.b6 ) fail(__LINE__);
    if( value.b7 != x.b7 ) fail(__LINE__);
    if( value.b8 != x.b8 ) fail(__LINE__);
    if( value.b9 != x.b9 ) fail(__LINE__);
    if( value.b10 != x.b10 ) fail(__LINE__);
    if( value.b11 != x.b11 ) fail(__LINE__);
    if( value.b12 != x.b12 ) fail(__LINE__);
    if( value.b13 != x.b13 ) fail(__LINE__);
    if( value.b14 != x.b14 ) fail(__LINE__);
    if( value.b15 != x.b15 ) fail(__LINE__);
    if( value.b16 != x.b16 ) fail(__LINE__);
    if( value.b17 != x.b17 ) fail(__LINE__);
    if( value.b18 != x.b18 ) fail(__LINE__);
    if( value.b19 != x.b19 ) fail(__LINE__);
}

int main( void )
{
    pass( x );
    _PASS;
}
