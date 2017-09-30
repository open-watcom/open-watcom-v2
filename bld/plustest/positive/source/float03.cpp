#include "fail.h"

#define DEFS \
    /*    c                                                                     n       d */\
    pick( 1.2 - .2,                                                             3,      3 ) \
    pick( .9 + .1,                                                              2,      2 ) \
    pick( .3 * 10,                                                              6,      2 ) \
    pick( 3 / .2,                                                               60,     4 ) \
    pick( .1111111111111111111111111111111 <  .1111111111111111111111111111112, 1,      1 ) \
    pick( .1111111111111111111111111111111 <= .1111111111111111111111111111112, 1,      1 ) \
    pick( .1111111111111111111111111111111 >  .1111111111111111111111111111112, 0,      1 ) \
    pick( .1111111111111111111111111111111 >= .1111111111111111111111111111112, 0,      1 ) \
    pick( .1111111111111111111111111111111 == .1111111111111111111111111111112, 0,      1 ) \
    pick( .1111111111111111111111111111111 != .1111111111111111111111111111112, 1,      1 ) \
    pick( ( .23 , 1.25 ),                                                       10,     8 )

double c[] = {
    #define pick(a,b,c) (a),
    DEFS
    #undef pick
};

unsigned n[] = {
    #define pick(a,b,c) (b),
    DEFS
    #undef pick
};

unsigned d[] = {
    #define pick(a,b,c) (c),
    DEFS
    #undef pick
};

int main()
{
    unsigned *nn;
    unsigned *dd;
    double *p;

    if( (unsigned)34.99999999999999999999999999999999999999 != 34 ) fail(__LINE__);
    if( ((double)2)/2/2*2*2 != 2 ) fail(__LINE__);
    nn = n;
    dd = d;
    for( p = c; p < &c[ sizeof(c) / sizeof(c[0]) ]; ++p ) {
        if( *p != ( (double)*nn ) / *dd ) {
            printf( "index:%d %f != %u/%u\n", (int)( p - c ), *p, *nn, *dd );
            fail(__LINE__);
        }
        ++nn;
        ++dd;
    }
    _PASS;
}
