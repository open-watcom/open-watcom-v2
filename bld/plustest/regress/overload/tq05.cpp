#include "dump.h"
// encapsulate distinction between const,volatile member functions
int xx;
struct A {
    void bar( void );
    void bar( void ) const;
    void bar( void ) volatile;
    void bar( void ) const volatile;
    void foo( void );
    void foo( void ) const;
    void foo( void ) volatile;
    void foo( void ) const volatile;
};
void A::bar( void ) { GOOD; xx = __LINE__; }
void A::bar( void ) const { GOOD; xx = __LINE__; }
void A::bar( void ) volatile { GOOD; xx = __LINE__; }
void A::bar( void ) const volatile { GOOD; xx = __LINE__; }
void A::foo( void ) { GOOD; xx = __LINE__; bar(); }
void A::foo( void ) const { GOOD; xx = __LINE__; bar(); }
void A::foo( void ) volatile { GOOD; xx = __LINE__; bar(); }
void A::foo( void ) const volatile { GOOD; xx = __LINE__; bar(); }
void g( A &a, A const &b, A volatile &c, A const volatile &d ) {
    a.foo();
    b.foo();
    c.foo();
    d.foo();
}
int main( void ) {
    A a;
    g( a, a, a, a );
    CHECK_GOOD( 140 );
    return errors != 0;
}
