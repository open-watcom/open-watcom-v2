#include <stdarg.h>
#include "fail.h"

#pragma inline_depth 0

struct F {
    static int count;
    int f;
    F( int f ) : f(f) {
	//printf( "F(%p,%d)\n", this, f );
	++count;
    }
    F( F const &s ) : f(s.f) {
	//printf( "F(%p,%p)\n", this, &s );
	++count;
    }
    ~F() {
	//printf( "~F(%p)\n", this );
	--count;
	if( count < 0 ) fail(__LINE__);
    }
};

int F::count;

void test( int n, ... )
{
    va_list args;

    va_start( args, n );
    va_end( args );
}

struct C1 {
    static int count;
    int x,y,z;
    C1( F x, F y, F z ) : x(x.f), y(y.f), z(z.f) {
	//printf( "C1(%p,%d,%d,%d)\n", this, x, y, z );
	test( 3, x, y, z );
	++count;
    }
    C1( C1 const &s ) : x(s.x), y(s.y), z(s.z) {
	//printf( "C1(%p,%p)\n", this, &s );
	++count;
    }
    ~C1() {
	//printf( "~C1(%p)\n", this );
	--count;
	if( count < 0 ) fail(__LINE__);
    }
};

int C1::count;

void bad() {
    auto C1 x( F(1), F(2), F(3) );
    auto C1 y( F(1), F(2), F(3) );
}

int main() {
    try {
	bad();
    } catch( ... ) {
	fail(__LINE__);
    }
    if( F::count != 0 ) fail(__LINE__);
    if( C1::count != 0 ) fail(__LINE__);
    _PASS;
}
