#include "fail.h"

struct B {
    virtual int foo();
};
int B::foo()
{
    return 'B';
}

struct D : B {
    virtual int foo() const;
};
int D::foo() const
{
    return 'D';
}

void foo( B &b, D const &d, D &r )
{
    if( b.foo() != 'B' ) fail(__LINE__);
    if( d.foo() != 'D' ) fail(__LINE__);
    if( r.foo() != 'D' ) fail(__LINE__);
    B &q = r;
    if( q.foo() != 'B' ) fail(__LINE__);
}

int main()
{
    D x;

    foo( x, x, x );
    _PASS;
}
