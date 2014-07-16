#include "fail.h"
#include <string.h>

struct B {
    int b[8];
    B();
    B( B const & );
};
B::B() {
    for( int i = 0; i < 8; ++i ) {
	b[i] = i+0xb00;
    }
}
B::B( B const &s ) {
    memcpy( b, s.b, sizeof( b ) );
}
struct D : B {
    int d[16];
    D();
    D( D const & );
};
D::D() {
    for( int i = 0; i < 16; ++i ) {
	d[i] = i+0xd00;
    }
}
D::D( D const &s ) : B(s) {
    memcpy( d, s.d, sizeof( d ) );
    b[0] ='d';
}

D x;

void foo() {
    throw x;
}

int main() {
    try {
	foo();
    } catch( B x ) {
	if( x.b[0] != 'd' ) fail(__LINE__);
	for( int i = 1; i < 8; ++i ) {
	    if( x.b[i] != i+0xb00 ) fail(__LINE__);
	}
    }
    _PASS;
}
