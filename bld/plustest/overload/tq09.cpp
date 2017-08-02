#include "dump.h"
// does not generate all member pointer thunks required
// seems to ignore const / volatile member function distinction
// GRB
struct A {
    void foo( void );
    void foo( void ) const;
};
void A::foo( void ) const GOOD;
void A::foo( void ) GOOD;
void f( A& a, A const &b ) {
    auto void (A::*mpa)( void );
    auto void (A::*mpb)( void ) const;
    mpa = &A::foo;
    mpb = &A::foo;
    (a.*mpa)();
    (b.*mpb)();
}
struct B {
    void foo( void ) const;
    void foo( void );
};
void B::foo( void ) const GOOD;
void B::foo( void ) GOOD;
void g( B &a, B const &b ) {
    auto void (B::*mpa)( void );
    auto void (B::*mpb)( void ) const;
    mpa = &B::foo;
    mpb = &B::foo;
    (a.*mpa)();
    (b.*mpb)();
}
int main( void ) {
    A a;
    B b;
    f( a, a );
    g( b, b );
    CHECK_GOOD( 66 );
    return errors != 0;
}
