#include "fail.h"

struct S {
    S( char c ) {
	memset( a, c, sizeof(a) );
    }
    virtual ~S(){};
    char a[39];
};
S foo() {
    S v( 'b' );
    v = S( 'a' );
    return v;
}

int main() {
    S x = foo();
    for( int i = 0; i < sizeof( x.a ); ++i ) {
	if( x.a[i] != 'a' ) {
	    fail(__LINE__);
	    break;
	}
    }
    _PASS;
}
