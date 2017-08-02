// verify that X is properly constructed
#include "fail.h"

struct A {
    A();                               
    virtual char f( void );
};

A::A() {};

char A::f( void ) { return 'A'; };

struct B : public A {
    virtual char f( void );
};

char B::f( void ) { return 'B'; };

struct X {
    B b;
};

char bar( B &rb ) {
    return rb.f();
}

int main( void ) {
    X x;
    char chr = bar( x.b );
    if( chr == 'A' ) {
	fail(__LINE__);
    } else if( chr == 'B' ) {
    } else {
	fail(__LINE__);
    }
    _PASS;
}
