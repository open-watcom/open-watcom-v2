#include "fail.h"
#include <string.h>
#include <malloc.h>

const unsigned ASIZE = 128;

int lotsa_parms( int p0, int p1,
int, int, int, int, int, int, int, int,
int, int, int, int, int, int, int, int,
int, int, int, int, int, int, int, int,
int, int, int, int, int, int, int, int,
int p2, int p3, int *p ) {
    for( int i = 0; i < ASIZE; ++i ) {
	if( p[i] != -1 ) {
	    fail(__LINE__);
	    break;
	}
    }
    memset( p, 0, ASIZE * sizeof( int ) );
    return p0 + p1 + p2 + p3;
}

int test() {
    int *p = (int*) alloca( ASIZE * sizeof( int ) );
    int r;

    memset( p, -1, ASIZE * sizeof( int ) );
    r = lotsa_parms( 1, 2,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    4, 8, p );
    for( int i = 0; i < ASIZE; ++i ) {
	if( p[i] != 0 ) {
	    fail(__LINE__);
	    break;
	}
    }
    return( r );
}

int main() {
    if( test() != 15 ) fail(__LINE__);
    _PASS;
}
