#include "fail.h"

#pragma inline_depth(0);

void bad();

struct F {
    static int count;
    int f;
    F( int f ) : f(f) {
	//printf( "F(%p,%d)\n", this, f );
	if( f == -12 ) bad();
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
	switch( f ) {
	case 2:
	    //printf( "throw f;\n" );
	    throw f;
	    break;
	case -2:
	    {
		bad();
	    }
	    break;
	}
    }
};

int F::count;

struct C1 {
    static int count;
    int x,y,z;
    C1( F x, F y, F z ) : x(x.f), y(y.f), z(z.f) {
	//printf( "C1(%p,%d,%d,%d)\n", this, this->x, this->y, this->z );
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

struct C2 {
    static int count;
    int x,y,z;
    C2( F x, F y, F z ) : x(x.f), y(y.f), z(z.f) {
	//printf( "C2(%p,%d,%d,%d)\n", this, this->x, this->y, this->z );
	++count;
    }
    C2( C2 const &s ) : x(s.x), y(s.y), z(s.z) {
	//printf( "C2(%p,%p)\n", this, &s );
	++count;
    }
    ~C2() {
	//printf( "~C2(%p)\n", this );
	--count;
	if( count < 0 ) fail(__LINE__);
    }
};

int C2::count;

void bad() {
    auto C1 x( F(1), F(2), F(3) );
    auto C1 y( F(1), F(2), F(3) );
}

void test() {
    auto C2 x( F(-1), F(-2), F(-3) );
    auto C2 y( F(-1), F(-2), F(-3) );
}

void alt_test() {
    auto C2 x( F(-11), F(-12), F(-13) );
    auto C2 y( F(-11), F(-12), F(-13) );
}

int main() {
    try {
	test();
    } catch( int x ) {
	//printf( "caught %d\n", x );
	if( x != 2 ) fail(__LINE__);
    } catch( ... ) {
	fail(__LINE__);
    }
    try {
	alt_test();
    } catch( int x ) {
	//printf( "caught %d\n", x );
	if( x != 2 ) fail(__LINE__);
    } catch( ... ) {
	fail(__LINE__);
    }
    if( F::count != 0 ) fail(__LINE__);
    if( C1::count != 0 ) fail(__LINE__);
    if( C2::count != 0 ) fail(__LINE__);
    _PASS;
}
