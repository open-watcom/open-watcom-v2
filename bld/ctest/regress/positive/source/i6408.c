#include "fail.h"

__int64 a;

void reset( void ) {
    a = 0x7f7f7f7f7f7f7f7f;
}

void f1( void ) { a &= 4294967295I64; }
__int64 f2( void ) { return 4294967295I64; }
__int64 f3( void ) { return a & 4294967295I64; }

int test( void ) {
    reset();
    f1();
    if( a & 0xffffffff00000000 ) return( 0 );
    reset();
    if( f2() & 0xffffffff00000000 ) return( 0 );
    reset();
    if( f3() & 0xffffffff00000000 ) return( 0 );
    return( 1 );
}

/* exercise setcc optimizations on 386 (broken for 64-bit consts up to OW 1.6 */
__int64 f4( int a )
{
    return( a ? 0x8000000000000000 : 0x7fffffffffffffff );
}

__int64 f5( int a )
{
    return( a ? -1 : -2 );
}

short f6( int a )
{
    return( a ? -4 : -3 );
}

int main( void ) {
    if( !test() ) _fail;
    if( f4( 1 ) != 0x8000000000000000 ) _fail;
    if( f4( 0 ) != 0x7fffffffffffffff ) _fail;
    if( f5( 1 ) != -1 ) _fail;
    if( f5( 0 ) != -2 ) _fail;
    if( f6( 1 ) != -4 ) _fail;
    if( f6( 0 ) != -3 ) _fail;
    _PASS;
}
