#include "fail.h"

__int64 foo( __int64 x ) {
    return x * 2;
}
__int64 bar( __int64 x ) {
    return x / 3;
}

__int64 baz( void )
{
    unsigned __int64    ll = 1;

    ll <<= 34;
    return( ll >> 33 );
}

__int64 bat( long x )
{
    return( x );    /* Generates CDQ with most settings. */
}

int main() {
    if( foo(  2 ) !=  4 ) _fail;
    if( bar(  9 ) !=  3 ) _fail;
    if( baz(    ) !=  2 ) _fail;
    if( bat(  7 ) !=  7 ) _fail;
    if( bat( -8 ) != -8 ) _fail;
    _PASS;
}
