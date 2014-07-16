#include "fail.h"

#if __WATCOM_REVISION__ >= 8

#pragma inline_depth(20)

struct D {
    D( const char * what_arg ) throw() {
    }
    ~D();
};
int q;
D::~D() {
    ++q;
}

static void f() {
    throw D( "?" );
}

void ack( void f() ) {
    f = f;
}

void boo( void *p ) {
    if( p == 0 ) {
	f();
    }
    ack( f );
}

int main() {
    try {
	f();
	_fail;
    } catch( ... ) {
    }
    try {
	boo( 0 );
	_fail;
    } catch( ... ) {
    }
    _PASS;
}
#else
ALWAYS_PASS
#endif
