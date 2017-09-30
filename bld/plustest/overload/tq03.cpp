#include "dump.h"
int xx;
struct A {
    A( void );
    void operator = ( A const & );
    void operator = ( A const & ) const;
    void operator = ( A const & ) volatile;
    void operator = ( A const & ) const volatile;
};
A::A( void ) {};
void A::operator = ( A const & ) { GOOD; xx = __LINE__; };
void A::operator = ( A const & ) const { GOOD; xx = __LINE__; };
void A::operator = ( A const & ) volatile { GOOD; xx = __LINE__; };
void A::operator = ( A const & ) const volatile { GOOD; xx = __LINE__; };
struct B {
    A a[8];
    B();
    void operator = ( B const & );
    void operator = ( B const & ) const;
    void operator = ( B const & ) volatile;
    void operator = ( B const & ) const volatile;
};
B::B( void ) {};
void B::operator = ( B const & ) { GOOD; xx = __LINE__; };
void B::operator = ( B const & ) const { GOOD; xx = __LINE__; };
void B::operator = ( B const & ) volatile { GOOD; xx = __LINE__; };
void B::operator = ( B const & ) const volatile { GOOD; xx = __LINE__; };
void g( B& q ) {
    B a;
    const B b = q;
    volatile B c;
    const volatile B d = q;
    a = q;
    b = q;
    c = q;
    d = q;
}
int main( void ) {
    B b;
    g( b );
    CHECK_GOOD( 102 );
    return errors != 0;
}
