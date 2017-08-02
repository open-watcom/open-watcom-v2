#include "fail.h"
#include <float.h>

int is_one( double d )
{
    d -= 1.0;
    if( d < 0 ) {
	d = -d;
    }
    if( d < 2 * DBL_EPSILON ) {
	return 1;
    }
    return 0;
}

#define test_cg( k )	\
	{ \
	    double t1, t2; \
	    t1 = (double) k; \
	    t2 = t1 / (double) k; \
	    if( ! is_one( t2 ) ) fail(__LINE__); \
	    alt_test( t1, (double) k, __LINE__); \
	}

void alt_test( double t1, double k, unsigned line )
{
    double r;

    r = t1 / k;
    if( ! is_one( r ) ) fail(line);
}

int main( void )
{
    test_cg( 0x00000001 );
    test_cg( 0x00000002 );
    test_cg( 0x00000004 );
    test_cg( 0x00000008 );
    test_cg( 0x00000010 );
    test_cg( 0x00000020 );
    test_cg( 0x00000040 );
    test_cg( 0x00000080 );
    test_cg( 0x00000100 );
    test_cg( 0x00000200 );
    test_cg( 0x00000400 );
    test_cg( 0x00000800 );
    test_cg( 0x00001000 );
    test_cg( 0x00002000 );
    test_cg( 0x00004000 );
    test_cg( 0x00008000 );
    test_cg( 0x00010000 );
    test_cg( 0x00020000 );
    test_cg( 0x00040000 );
    test_cg( 0x00080000 );
    test_cg( 0x00100000 );
    test_cg( 0x00200000 );
    test_cg( 0x00400000 );
    test_cg( 0x00800000 );
    test_cg( 0x01000000 );
    test_cg( 0x02000000 );
    test_cg( 0x04000000 );
    test_cg( 0x08000000 );
    test_cg( 0x10000000 );
    test_cg( 0x20000000 );
    test_cg( 0x40000000 );
    test_cg( 0x80000000 );
    _PASS;
}
