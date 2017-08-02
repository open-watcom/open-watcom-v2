#include <stdio.h>

union velement {
    char arr[8];
    UBIG val;
};

velement vect[] =
{   { 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55 }
,   { 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33 }
,   { 0x0f, 0x0f, 0x0f, 0x0f, 0x0f, 0x0f, 0x0f, 0x0f }
,   { 0xff, 0x00, 0xff, 0x00, 0xff, 0x00, 0xff, 0x00 }
,   { 0xff, 0xff, 0x00, 0x00, 0xff, 0xff, 0x00, 0x00 }
,   { 0xff, 0xff, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00 }
};

#if 0
unsigned count( unsigned x )
{
    unsigned y;

#ifndef __386__
    y = x & 0x5555;
    x = y + (( x ^ y ) >> 1 );
    y = x & 0x3333;
    x = y + (( x ^ y ) >> 2 );
    y = x & 0x0f0f;
    x = y + (( x ^ y ) >> 4 );
    y = x & 0x00ff;
    x = y + (( x ^ y ) >> 8 );
#else
    y = x & 0x55555555;
    x = y + (( x ^ y ) >> 1 );
    y = x & 0x33333333;
    x = y + (( x ^ y ) >> 2 );
    y = x & 0x0f0f0f0f;
    x = y + (( x ^ y ) >> 4 );
    y = x & 0x00ff00ff;
    x = y + (( x ^ y ) >> 8 );
    y = x & 0x0000ffff;
    x = y + (( x ^ y ) >> 16 );
#endif
    return( x );
}

#else
unsigned count( SBIG x )
{
    UBIG y;

    y = x & vect[0].val;
    x = y + (( x ^ y ) >> 1 );
    y = x & vect[1].val;
    x = y + (( x ^ y ) >> 2 );
    y = x & vect[2].val;
    x = y + (( x ^ y ) >> 4 );
    y = x & vect[3].val;
    x = y + (( x ^ y ) >> 8 );
    if( sizeof( UBIG ) < 4 ) return x;
    y = x & vect[4].val;
    x = y + (( x ^ y ) >> 16 );
    if( sizeof( UBIG ) < 8 ) return x;
    y = x & vect[5].val;
    x = y + (( x ^ y ) >> 32 );
    return x;
}
#endif

void test( unsigned long x )
{
    printf( "# bits in %lx is %u\n", x, count( x ) );
}

int main( void )
{
    test( 23 );
    test( 1 );
    test( -1 );
    test( 255 );
    test( 0xaa );
    test( 0x55 );
    return 0;
}
