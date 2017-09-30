#include "dump.h"
struct A {
    operator int ();
    operator int () const;
    operator int () volatile;
    operator int () const volatile;
};
A::operator int () { GOOD; return __LINE__; };
A::operator int () const { GOOD; return __LINE__; };
A::operator int () volatile { GOOD; return __LINE__; };
A::operator int () const volatile { GOOD; return __LINE__; };
int f( A &a, A const &b, A volatile &c, A const volatile &d ) {
    return a + b + c + d;
}
int main( void ) {
    A a;
    f( a, a, a, a );
    CHECK_GOOD( 38 );
    return errors != 0;
}
