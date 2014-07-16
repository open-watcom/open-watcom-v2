#include "fail.h"

void check( int v, int c, unsigned line )
{
    if( v != c ) {
	fail( line );
    }
}

struct X {
    virtual int AA() { return 'a'; }
    virtual int BB() { return 'b'; }
};

struct A : virtual X {
    virtual int AA() { return 'A'; }
};

struct B : virtual X {
    virtual int BB() { return 'B'; }
};

struct only_A : A {
};

struct only_B : B {
};

struct both_AB : A, B {
};

struct derived_from_both_AB : both_AB {
};

void unknown_ref( only_A &xA, only_B &xB, both_AB &xAB, derived_from_both_AB &xDAB )
{
    check( xA.AA(), 'A', __LINE__ );
    check( xA.BB(), 'b', __LINE__ );
    check( xB.AA(), 'a', __LINE__ );
    check( xB.BB(), 'B', __LINE__ );
    check( xAB.AA(), 'A', __LINE__ );
    check( xAB.BB(), 'B', __LINE__ );
    check( xDAB.AA(), 'A', __LINE__ );
    check( xDAB.BB(), 'B', __LINE__ );
}

void unknown_ptr( A *pA, B *pB, int caa, int cab, int cba, int cbb )
{
    X *pX;

    check( pA->AA(), caa, __LINE__ );
    check( pA->BB(), cab, __LINE__ );
    check( pB->AA(), cba, __LINE__ );
    check( pB->BB(), cbb, __LINE__ );
    pX = pA;
    check( pX->AA(), caa, __LINE__ );
    check( pX->BB(), cab, __LINE__ );
    pX = pB;
    check( pX->AA(), cba, __LINE__ );
    check( pX->BB(), cbb, __LINE__ );
}

int main()
{
    only_A xA;
    only_B xB;
    both_AB xAB;
    derived_from_both_AB xDAB;

    unknown_ref( xA, xB, xAB, xDAB );
    check( xA.AA(), 'A', __LINE__ );
    check( xA.BB(), 'b', __LINE__ );
    check( xB.AA(), 'a', __LINE__ );
    check( xB.BB(), 'B', __LINE__ );
    check( xAB.AA(), 'A', __LINE__ );
    check( xAB.BB(), 'B', __LINE__ );
    check( xDAB.AA(), 'A', __LINE__ );
    check( xDAB.BB(), 'B', __LINE__ );
    unknown_ptr( &xA, &xB, 'A', 'b', 'a', 'B' );
    unknown_ptr( &xAB, &xAB, 'A', 'B', 'A', 'B' );
    unknown_ptr( &xDAB, &xDAB, 'A', 'B', 'A', 'B' );
    _PASS;
}
