#include <iostream.h>
#include <strstrea.h>

#include "fail.h"

#ifdef __WATCOM_INT64__

int main() {

#if( -6 % -10 != -6 )
    _fail;
#endif

#if( -6 % 10 != -6 )
    _fail;
#endif

#if( 6 % -10 != 6 )
    _fail;
#endif

#if( 6 % 10 != 6 )
    _fail;
#endif


    __int64 i = -6;
    __int64 j = -10;


    if( i % j != -6i64 % -10i64 ) {
	_fail;
    }
    if( -6i64 % -10i64 != -6i64 ) {
	_fail;
    }

    i = -6;
    j = 10;

    if( i % j != -6i64 % 10i64 ) {
	_fail;
    }
    if( -6i64 % 10i64 != -6i64 ) {
	_fail;
    }

    i = 6;
    j = -10;
    if( i % j != 6i64 % -10i64 ) {
	_fail;
    }
    if( 6i64 % -10i64 != 6i64 ) {
	_fail;
    }

    i = 6;
    j = 10;
    if( i % j != 6i64 % 10i64 ) {
	_fail;
    }
    if( 6i64 % 10i64 != 6i64 ) {
	_fail;
    }
    _PASS;
}


#else

ALWAYS_PASS

#endif
