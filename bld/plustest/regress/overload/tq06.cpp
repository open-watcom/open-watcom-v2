#include "dump.h"
int xx;
struct A {
    void foo( int );
    void foo( int ) const;
    void foo( int ) volatile;
    void foo( int ) const volatile;
};
void A::foo( int ) { GOOD; xx = __LINE__; };
void A::foo( int ) const { GOOD; xx = __LINE__; };
void A::foo( int ) volatile { GOOD; xx = __LINE__; };
void A::foo( int ) const volatile { GOOD; xx = __LINE__; };

void f( A &a, A const &b, A volatile &c, A const volatile &d ) {
    auto void (A::*mpa)( int );
    auto void (A::*mpb)( int ) const;
    auto void (A::*mpc)( int ) volatile;
    auto void (A::*mpd)( int ) const volatile;
    mpa = &A::foo;
    mpb = &A::foo;
    mpc = &A::foo;
    mpd = &A::foo;
    (a.*mpa)( 1 );
    (b.*mpb)( 2 );
    (c.*mpc)( 3 );
    (d.*mpd)( 4 );
}
int main( void ) {
    A a;
    f( a, a, a, a );
    CHECK_GOOD( 42 );
    return errors != 0;
}
