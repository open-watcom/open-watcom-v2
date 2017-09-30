#include <iostream.h>
#include <strstrea.h>

#include "fail.h"

#ifdef __WATCOM_INT64__

__int64 a;

void reset( void ) {
    a = 0x7f7f7f7f7f7f7f7f;
}

void f1() { a &= 4294967295I64; }
__int64 f2() { return 4294967295I64; }
__int64 f3() { return a & 4294967295I64; }

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

int main( void ) {
    if( !test() ) _fail;
    _PASS;
}

#else

ALWAYS_PASS

#endif
