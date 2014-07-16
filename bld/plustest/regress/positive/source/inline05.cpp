#include "fail.h"
#include <string.h>

struct S {
    int a[5];
    S & operator =( S const &s )
    {
	create( s );
	return *this;
    }
    void create( S const &s )
    {
	memcpy( this, &s, sizeof( s ) );
    }
};

S & foo( int i, S const &x ) {
    static S sa[10];
    return sa[i] = x;
}

int main() {
    S v;
    memset( &v, 0xff, sizeof( v ) );
    v.a[0] = 0;
    S &r0 = foo( 0, v );
    v.a[1] = 0;
    S &r1 = foo( 1, v );
    v.a[2] = 0;
    S &r2 = foo( 2, v );
    v.a[3] = 0;
    S &r3 = foo( 3, v );
    v.a[4] = 0;
    S &r4 = foo( 4, v );
    if( r0.a[0] != 0 ) fail(__LINE__);
    if( r0.a[1] != -1 ) fail(__LINE__);
    if( r0.a[2] != -1 ) fail(__LINE__);
    if( r0.a[3] != -1 ) fail(__LINE__);
    if( r0.a[4] != -1 ) fail(__LINE__);
    if( r1.a[0] != 0 ) fail(__LINE__);
    if( r1.a[1] != 0 ) fail(__LINE__);
    if( r1.a[2] != -1 ) fail(__LINE__);
    if( r1.a[3] != -1 ) fail(__LINE__);
    if( r1.a[4] != -1 ) fail(__LINE__);
    if( r2.a[0] != 0 ) fail(__LINE__);
    if( r2.a[1] != 0 ) fail(__LINE__);
    if( r2.a[2] != 0 ) fail(__LINE__);
    if( r2.a[3] != -1 ) fail(__LINE__);
    if( r2.a[4] != -1 ) fail(__LINE__);
    if( r3.a[0] != 0 ) fail(__LINE__);
    if( r3.a[1] != 0 ) fail(__LINE__);
    if( r3.a[2] != 0 ) fail(__LINE__);
    if( r3.a[3] != 0 ) fail(__LINE__);
    if( r3.a[4] != -1 ) fail(__LINE__);
    if( r4.a[0] != 0 ) fail(__LINE__);
    if( r4.a[1] != 0 ) fail(__LINE__);
    if( r4.a[2] != 0 ) fail(__LINE__);
    if( r4.a[3] != 0 ) fail(__LINE__);
    if( r4.a[4] != 0 ) fail(__LINE__);
    if( &r0 >= &r1 ) fail(__LINE__);
    if( &r1 >= &r2 ) fail(__LINE__);
    if( &r2 >= &r3 ) fail(__LINE__);
    if( &r3 >= &r4 ) fail(__LINE__);
    _PASS;
}
