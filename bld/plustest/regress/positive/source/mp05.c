#include "fail.h"

struct V {
    virtual int bar() { return 1; };
    virtual int foo() { return 2; };
    virtual int ack() { return 3; };
};
struct D : virtual V {
};

struct S {
    int v;
    S(int v):v(v){}
};
struct T : S {
    T(int v):S(v){}
};
struct Z : T {
    Z(int v):T(v){}
};

struct Q : virtual Z, virtual V, virtual D {
    Q() : Z(__LINE__) {}
};

typedef int (D::* mpD)( void );
typedef int (Q::* mpQ)( void );

mpQ sam( mpD x, D * )
{
    return x;
}

void foo(int x)
{
    switch( x ) {
	case 0: throw S(x);
	case 1: throw T(x);
	case 2: throw Z(x);
	default: throw int(x);
    }
}

void ack()
{
    try {
	foo( 0 );
    } catch( S &r ) {
	if( r.v != 0 ) fail(__LINE__);
    } catch( ... ) {
	fail(__LINE__);
    }
    try {
	foo( 1 );
    } catch( T &r ) {
	if( r.v != 1 ) fail(__LINE__);
    } catch( ... ) {
	fail(__LINE__);
    }
    try {
	foo( 2 );
    } catch( Z &r ) {
	if( r.v != 2 ) fail(__LINE__);
    } catch( ... ) {
	fail(__LINE__);
    }
    try {
	foo( 3 );
    } catch( int r ) {
	if( r != 3 ) fail(__LINE__);
    } catch( ... ) {
	fail(__LINE__);
    }
}

int main()
{
    D xx;
    Q zz;
    ack();
    if( (zz.*(sam( &V::foo, &xx )))() != 2 ) fail(__LINE__);
    _PASS;
}
