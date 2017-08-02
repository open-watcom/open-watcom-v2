#include "fail.h"

unsigned dtor;
unsigned ctor;

struct A {
    A() { ++ctor; if( ctor != 1 ) fail( __LINE__ ); }
    ~A() { ++dtor; if( dtor != 8 ) fail( __LINE__ ); }
};
struct B {
    B() {++ctor; if( ctor != 2 ) fail( __LINE__ ); }
    ~B() {++dtor; if( dtor != 7 ) fail( __LINE__ ); }
};
struct C : virtual A {
    C() {++ctor; if( ctor != 6 ) fail( __LINE__ ); }
    ~C(){ ++dtor; if( dtor != 3 ) fail( __LINE__ ); }
};
struct D : virtual A, virtual B {
    D() {++ctor; if( ctor != 3 ) fail( __LINE__ ); }
    ~D() {++dtor; if( dtor != 6 ) fail( __LINE__ ); }
};
struct E : virtual B, virtual A {
    E() {++ctor; if( ctor != 4 ) fail( __LINE__ ); }
    ~E(){ ++dtor; if( dtor != 5 ) fail( __LINE__ ); }
};
struct F : C, virtual D {
    F() {++ctor; if( ctor != 7 ) fail( __LINE__ ); }
    ~F(){ ++dtor; if( dtor != 2 ) fail( __LINE__ ); }
};
struct G : virtual D, E {
    G() {++ctor; if( ctor != 5 ) fail( __LINE__ ); }
    ~G() {++dtor; if( dtor != 4 ) fail( __LINE__ ); }
};
struct H : F, virtual G, virtual B {
    H() {++ctor; if( ctor != 8 ) fail( __LINE__ ); }
    ~H(){ ++dtor; if( dtor != 1 ) fail( __LINE__ ); }
};

int main()
{
    {
	H x;
    }
    _PASS;
}
