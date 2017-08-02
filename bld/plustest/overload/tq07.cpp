#include "dump.h"
int xx;
struct A {
    void foo( int );
    void foo( int ) const;
    void foo( int ) volatile;
    void foo( int ) const volatile;
    void bar1( auto void (A::*)( int ) );
    void bar2( auto void (A::*)( int ) const );
    void bar3( auto void (A::*)( int ) volatile );
    void bar4( auto void (A::*)( int ) const volatile );
};
void A::foo( int ) { GOOD; xx = __LINE__; };
void A::foo( int ) const { GOOD; xx = __LINE__; };
void A::foo( int ) volatile { GOOD; xx = __LINE__; };
void A::foo( int ) const volatile { GOOD; xx = __LINE__; };
void A::bar1( auto void (A::*mp)( int ) ) { GOOD; (this->*mp)( __LINE__ ); }
void A::bar2( auto void (A::*mp)( int ) const ) { GOOD; (this->*mp)( __LINE__ ); }
void A::bar3( auto void (A::*mp)( int ) volatile ) { GOOD; (this->*mp)( __LINE__ ); }
void A::bar4( auto void (A::*mp)( int ) const volatile ) { GOOD; (this->*mp)( __LINE__ ); }

void f( A &a ) {
    a.bar1( &A::foo );
    a.bar2( &A::foo );
    a.bar3( &A::foo );
    a.bar4( &A::foo );
}
int main( void ) {
    A a;
    f( a );
    CHECK_GOOD( 132 );
    return errors != 0;
}
