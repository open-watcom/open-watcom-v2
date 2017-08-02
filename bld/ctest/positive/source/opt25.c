#include "fail.h"
#include <string.h>
#include <malloc.h>

const unsigned ASIZE = 128;

int lotsa_parms( int p0, int p1,
int a, int b, int c, int d, int e, int f, int g, int h,
int I, int j, int k, int l, int m, int n, int o, int P,
int q, int r, int s, int t, int u, int v, int w, int x,
int y, int z, int A, int B, int C, int D, int E, int F,
int p2, int p3, int *p ) {
    int i;

    for( i = 0; i < ASIZE; ++i ) {
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
    int r, i;

    memset( p, -1, ASIZE * sizeof( int ) );
    r = lotsa_parms( 1, 2,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    4, 8, p );
    for( i = 0; i < ASIZE; ++i ) {
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
