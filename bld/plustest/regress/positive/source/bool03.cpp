#include "fail.h"

struct A {
    A( bool b )
        : b_( b )
    { }

    operator int () {
        return b_ ? 0 : 1;
    }

    operator bool () {
        return b_;
    }

    bool b_;
};

int main() {
    A a1( true );;
    A a2( false );;

    if( a2 ) fail(__LINE__);
    if( ! a1 ) fail(__LINE__);

    // see 13.6 Built-in operators
    // operator == only exists for promoted arithmetic types
    if( a1 == true ) fail(__LINE__);
    if( a1 != false ) fail(__LINE__);
    if( a2 == false ) fail(__LINE__);
    if( a2 != true ) fail(__LINE__);

    if( a1 == 1 ) fail(__LINE__);
    if( a1 != 0 ) fail(__LINE__);
    if( a2 == 0 ) fail(__LINE__);
    if( a2 != 1 ) fail(__LINE__);

    if( ! ( a1 && true ) ) fail(__LINE__);
    if( ! ( a1 || false ) ) fail(__LINE__);
    if( a2 && true ) fail(__LINE__);
    if( a2 || false ) fail(__LINE__);

    if( a1 ? false : true ) fail(__LINE__);
    if( a2 ? true : false ) fail(__LINE__);

    _PASS;
}
