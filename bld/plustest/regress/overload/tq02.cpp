#include "dump.h"
struct A {
    int operator + ( int );
    int operator + ( int ) const;
    int operator + ( int ) volatile;
    int operator + ( int ) const volatile;
};
int A::operator + ( int ) { GOOD; return __LINE__; };
int A::operator + ( int ) const { GOOD; return __LINE__; };
int A::operator + ( int ) volatile { GOOD; return __LINE__; };
int A::operator + ( int ) const volatile { GOOD; return __LINE__; };
void f( A &a, A const &b, A volatile &c, A const volatile &d ) {
    a + 1;
    b + 2;
    c + 3;
    d + 4;
}
int main( void ) {
    A a;
    f( a, a, a, a );
    CHECK_GOOD( 38 );
    return errors != 0;
}

