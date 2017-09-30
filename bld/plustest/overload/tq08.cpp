#include "dump.h"
int xx;
struct A {
    void foo( int );
    void foo( int ) const;
    void foo( int ) volatile;
    void foo( int ) const volatile;
    void bar( int, auto void (A::*)( int ) );
    void bar( double, auto void (A::*)( int ) const );
    void bar( char *, auto void (A::*)( int ) volatile );
};
void A::foo( int ) { BAD; xx = __LINE__; };
void A::foo( int ) const { GOOD; xx = __LINE__; };
void A::foo( int ) volatile { BAD; xx = __LINE__; };
void A::foo( int ) const volatile { BAD; xx = __LINE__; };
void A::bar( int, auto void (A::*mp)( int ) ) { BAD; (this->*mp)( __LINE__ ); }
void A::bar( double, auto void (A::*mp)( int ) const ) { GOOD; (this->*mp)( __LINE__ ); }
void A::bar( char *, auto void (A::*mp)( int ) volatile ) { BAD; (this->*mp)( __LINE__ ); }

void f( A &a ) {
    a.bar( 1.0, &A::foo );
}
int main( void ) {
    A a;
    f( a );
    CHECK_GOOD( 30 );
    return errors != 0;
}
