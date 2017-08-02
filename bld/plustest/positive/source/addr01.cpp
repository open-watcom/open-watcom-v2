#include "fail.h"

struct S {
    static int sfn( int, int );
};

int S::sfn( int x, int y )
{
    return x * 3 + y * 2;
}

struct A {
    S m;
};

struct B : S {
};

struct D : B {
};

struct VD : virtual A {
};

int main()
{
    auto int (*fn)( int, int );
    S s;
    A a;
    B b;
    D d;
    VD vd;

    fn = S::sfn;
    if( fn( 1, 2 ) != 7 ) fail(__LINE__);
    fn = &S::sfn;
    if( fn( 1, 2 ) != 7 ) fail(__LINE__);
    fn = s.sfn;
    if( fn( 1, 2 ) != 7 ) fail(__LINE__);
    fn = a.m.sfn;
    if( fn( 1, 2 ) != 7 ) fail(__LINE__);
    fn = b.sfn;
    if( fn( 1, 2 ) != 7 ) fail(__LINE__);
    fn = d.sfn;
    if( fn( 1, 2 ) != 7 ) fail(__LINE__);
    fn = vd.m.sfn;
    if( fn( 1, 2 ) != 7 ) fail(__LINE__);
    _PASS;
}
