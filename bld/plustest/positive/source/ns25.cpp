#include "fail.h"

#ifdef __WATCOM_NAMESPACE__
namespace A {
    int a = 17;
};
namespace B {
    using A::a;
    int foo() {
	return ++a;
    }
};
int foo() {
    return B::a++;
}

int which;

namespace C {
    void foo( int x ) {
	if( x != 1 ) _fail;
	which |= 1;
    }
    void foo( char x ) {
	if( x != '2' ) _fail;
	which |= 2;
    }
};
namespace D {
    using C::foo;
    void foo( double x ) {
	if( x != 0.25 ) _fail;
	which |= 4;
    }
};
namespace E {
    void foo( int ) {
	which |= 2;
    }
    void foo( double ) {
	which |= 4;
    }
};
namespace F {
    using E::foo;
    void ack();
};
void ack() {
    using E::foo;
    foo( '0' );
}
namespace E {
    void foo( char ) {
	which |= 1;
    }
};
void F::ack() {
    foo( '0' );
}
namespace G {
    void foo( int ) {
	which |= 8;
    }
};
namespace H {
    using G::foo;
    void ack() {
	foo( 0 );
    }
};

int main() {
    if( foo() != 17 ) _fail;
    if( foo() != 18 ) _fail;
    if( B::foo() != 20 ) _fail;
    if( foo() != 20 ) _fail;
    D::foo( 1 );
    if( which != 1 ) _fail;
    D::foo( '2' );
    if( which != 3 ) _fail;
    D::foo( 0.25 );
    if( which != 7 ) _fail;
    which = 0;
    F::ack();
    if( which != 2 ) _fail;
    H::ack();
    if( which != 10 ) _fail;
    _PASS;
}
#else
ALWAYS_PASS
#endif
