#include "fail.h"

struct B {
    int b;
    B();
    B(B&);
};
struct D : B {
    B d;
};

B::B() : b(0) {
}
B::B(B&s) : b(s.b) {
}

D foo( int x )
{
    D v;

    v.b = x;
    v.d.b = x;
    return v;
}

#pragma warning 665 9

int main()
{
    D i = foo(1);
    D a;
    a = foo(2);
    if( i.b != 1 ) fail(__LINE__);
    if( i.d.b != 1 ) fail(__LINE__);
    if( a.b != 2 ) fail(__LINE__);
    if( a.d.b != 2 ) fail(__LINE__);
    _PASS;
}
