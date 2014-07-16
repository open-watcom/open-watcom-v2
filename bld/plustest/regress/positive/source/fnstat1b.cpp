#include "fail.h"
#include <stdio.h>
#include "fnstat1.h"


int test( int *c1, int *c2, int *s1, int *s2 )
{
    if( c1 != ret_c1(1) ) {
	fail( __LINE__ );
    }
    if( c2 != ret_c2(2) ) {
	fail( __LINE__ );
    }
    if( s1 == ret_s1(3) ) {
	fail( __LINE__ );
    }
    if( *s2 != *ret_s2(4) ) {
	fail( __LINE__ );
    }
    _PASS;
}
