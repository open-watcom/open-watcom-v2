#include "fail.h"

static int foo( int b, int t, int f )
{
    return( b ? t : f );
}

/* Make sure test is done on full 64-bit value */
static long long bar( long long b, long long t, long long f )
{
    return( b ? t : f );
}

int main( void ) {
    if( foo( 1, -1, -2 ) != -1 ) fail(__LINE__);
    if( foo( 0, -1, -2 ) != -2 ) fail(__LINE__);
    if( bar( 0x1000000000, -1, -2 ) != -1 ) fail(__LINE__);
    if( bar( 0x0000000000, -1, -2 ) != -2 ) fail(__LINE__);
    if( (0x1000000000 ? -1 : -2) != -1 ) fail(__LINE__);
    if( (0x0000000000 ? -1 : -2) != -2 ) fail(__LINE__);
    if( ((0xF0000000 + 0x100000000)? -1 : -2) != -1 ) fail(__LINE__);
    if( ((0x20 - 32) ? -1 : -2) != -2 ) fail(__LINE__);
    _PASS;
}
