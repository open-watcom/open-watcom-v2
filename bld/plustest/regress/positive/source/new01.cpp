#include "fail.h"
#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>


struct CC {
    CC()
    {
	fail(__LINE__);
    }
};

void test( unsigned n )
{
    int i;
    int *p = new int[n+1];
    for( i = 0; i < ( n + 1 ); ++i ) {
	p[i] = i;
    }
    int *q = new (int[n+2]);
    for( i = 0; i < ( n + 1 ); ++i ) {
	q[i] = p[i];
    }
    int *r = new (int[n]);
    if( r == NULL ) {
	fail( __LINE__ );
    }
    CC *s = new(CC[n-n]);
    if( s == NULL ) {
	fail( __LINE__ );
    }
    if( _heapchk() != _HEAPOK ) {
	fail( __LINE__ );
    }
}

int main()
{
    for( int i = 0; i < 10; ++i ) {
	test(i);
    }
    _PASS;
}
