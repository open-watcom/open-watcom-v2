#include "fail.h"

#ifdef __WATCOM_RTTI__

int dummy;

struct P {
    int p[10];
};
struct D : P {
    virtual void g(){}
};
struct A0 : D {
    virtual void f(){}
    A0();
};
struct A1 : virtual A0 {
    virtual void f(){}
};
struct A2 : virtual A1 {
    virtual void f(){}
};
struct A3 : virtual A1 {
    virtual void f(){}
};
struct A4 : D, virtual A2, virtual A3 {
    virtual void f(){}
    A4();
};
struct A5 : virtual A4 {
    A5() {
	++dummy;
    }
};


A0::A0() {
    ++dummy;
}

A4::A4() {
    ++dummy;
}

A0 *f0( A0 *p ) {
    return dynamic_cast<A0*>( p );
}
A1 *f1( A0 *p ) {
    return dynamic_cast<A1*>( p );
}
A2 *f2( A0 *p ) {
    return dynamic_cast<A2*>( p );
}
A3 *f3( A0 *p ) {
    return dynamic_cast<A3*>( p );
}
A4 *f4( A0 *p ) {
    return dynamic_cast<A4*>( p );
}

int main() {
    A4 *p = new A4();
    if( f4(p) != (A4*)p ) fail(__LINE__);
    if( f3(p) != (A3*)p ) fail(__LINE__);
    if( f2(p) != (A2*)p ) fail(__LINE__);
    if( f1(p) != (A1*)p ) fail(__LINE__);
    if( f0(p) != (A0*)p ) fail(__LINE__);
    _PASS;
}

#else

ALWAYS_PASS

#endif
