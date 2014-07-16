#include "fail.h"

struct A {
    static int i;
};

int A::i = 1;

namespace ns
{
struct B {
    static int i;
};

struct C : A, B {
    static int i;
};

struct D : A, B {
    typedef B A;

    static int i;
};
}

int ns::B::i = 2;
int ns::C::i = 3;
int ns::D::i = 4;

int main()
{
    int ctr = 5;

    ns::C c;
    ns::C *cp = &c;

    c.A::i = ++ctr;
    if( A::i != ctr ) fail( __LINE__ );
    cp->A::i = ++ctr;
    if( A::i != ctr ) fail( __LINE__ );

    c.B::i = ++ctr;
    if( ns::B::i != ctr ) fail( __LINE__ );
    cp->B::i = ++ctr;
    if( ns::B::i != ctr ) fail( __LINE__ );

    c.ns::C::i = ++ctr;
    if( ns::C::i != ctr ) fail( __LINE__ );
    cp->ns::C::i = ++ctr;
    if( ns::C::i != ctr ) fail( __LINE__ );


    ns::D d;
    ns::D *dp = &d;

    d.::A::i = ++ctr;
    if( A::i != ctr ) fail( __LINE__ );
    dp->::A::i = ++ctr;
    if( A::i != ctr ) fail( __LINE__ );

    d.A::i = ++ctr;
    if( ns::B::i != ctr ) fail( __LINE__ );
    dp->A::i = ++ctr;
    if( ns::B::i != ctr ) fail( __LINE__ );

    d.B::i = ++ctr;
    if( ns::B::i != ctr ) fail( __LINE__ );
    dp->B::i = ++ctr;
    if( ns::B::i != ctr ) fail( __LINE__ );

    d.ns::D::i = ++ctr;
    if( ns::D::i != ctr ) fail( __LINE__ );
    dp->ns::D::i = ++ctr;
    if( ns::D::i != ctr ) fail( __LINE__ );


    _PASS;
}
