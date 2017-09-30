#include "dump.h"
#pragma inline_depth(0)
class B;

class A {
    public:
	int	public_ai;
	A() GOOD;
	operator B();
	operator float() { GOOD; return 1.0; }
};

class B {
    public:
	B() GOOD;
	int	public_bi;
	operator A() { GOOD; return A(); };
};
A::operator B() { GOOD; return B(); };

class C : public A {
    public:
	C() GOOD;
	int public_ci;
};

class D {
    public:
	int public_di;
	operator C() { GOOD; return C(); };
};

class E {
    public:
	int public_ei;
	E( float ) GOOD;
	E( D ) GOOD;
};

void fa( A ) GOOD;
void fa( ... ) BAD;
void fb( B ) GOOD;
void fb( ... ) BAD;
void ff( float ) GOOD;
void ff( ... ) BAD;
void fd( double ) GOOD;
void fd( ... ) BAD;
void fi( int ) GOOD;
void fi( ... ) BAD;
void fe( E ) GOOD;
void fe( ... ) BAD;

void g( void )
{
    A a;
    B b;
    C c;
    D d;
    fa( b );	// none of these should invoke ellipsis
    fb( a );
    ff( c );
    fd( c );
    fi( c );
    fa( d );
    fe( 1 );
    fe( d );
}
int main( void ) {
    g();
    CHECK_GOOD( 637 );
    return errors != 0;
}
