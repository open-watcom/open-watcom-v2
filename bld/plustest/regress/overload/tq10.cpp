#include "dump.h"
// complains about mpb
// ``pointers to class members reference different object types''
// pointer to member function initialization/assignment is not overloading
// for overloaded members
// GRB
int xx;
struct A {
    void foo( char );
    void foo( int );
    void bar( char );
    void bar( int );
    void goo1( char );
    void goo2( int );
    void fnor( double );
    void fnor( double ) const;
    void fnor( double ) volatile;
    void fnor( double ) const volatile;
};
void A::foo( char ) { GOOD; xx = __LINE__; };
void A::foo( int ) { GOOD; xx = __LINE__; };
void A::bar( char ) { GOOD; xx = __LINE__; };
void A::bar( int ) { GOOD; xx = __LINE__; };
void A::goo1( char ) { GOOD; xx = __LINE__; };
void A::goo2( int ) { GOOD; xx = __LINE__; };
void A::fnor( double ) { GOOD; xx = __LINE__; };
void A::fnor( double ) const { GOOD; xx = __LINE__; };
void A::fnor( double ) volatile { GOOD; xx = __LINE__; };
void A::fnor( double ) const volatile { GOOD; xx = __LINE__; };
void f( A &a ) {
    auto void (A::*mpa)( int ) = &A::foo;
    auto void (A::*mpb)( char ) = &A::foo;
    (a.*mpa)( 1 );
    (a.*mpb)( 'a' );
    mpa = &A::bar;
    mpb = &A::bar;
    (a.*mpa)( 1 );
    (a.*mpb)( 'a' );
    mpa = &A::goo2;
    mpb = &A::goo1;
    (a.*mpa)( 1 );
    (a.*mpb)( 'a' );
}
void g( A &a, A const &b, A volatile &c, A const volatile &d ) {
    auto void (A::*mpa)( double ) = &A::fnor;
    auto void (A::*mpb)( double ) const = &A::fnor;
    auto void (A::*mpc)( double ) volatile = &A::fnor;
    auto void (A::*mpd)( double ) const volatile = &A::fnor;
    (a.*mpa)( 1.0 );
    (b.*mpb)( 1.0 );
    (c.*mpc)( 1.0 );
    (d.*mpd)( 1.0 );
}
int main( void ) {
    A a;
    f( a );
    g( a, a, a, a );
    CHECK_GOOD( 245 );
    return errors != 0;
}
