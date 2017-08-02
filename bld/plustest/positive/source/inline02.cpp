#include "fail.h"

// test inlining & dtoring

#pragma inline_depth(3)

int dtored;

struct A {
    int a;
    A( int v ) : a(v) {}
    virtual ~A() { ++dtored; }
};

struct B : A {
    B( int v ) : A(v) {}
};

struct C : B {
    C( int v ) : B(v) {}
};

struct D : C {
    D( int v ) : C(v) {}
    ~D() { ++dtored; }
};

struct E : D {
    E( int v ) : D(v) {}
};

int main()
{
    {
        E e1( 19 );
        E e2 = e1;
    }
    if( dtored != 4 ) fail(__LINE__);
    _PASS;
}
