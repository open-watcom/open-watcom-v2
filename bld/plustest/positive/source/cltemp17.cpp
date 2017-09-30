#include "fail.h"

template< int I > struct X {
    static int const c = 2;
};
template<> struct X< 0 > {
    typedef int c;
};
template< typename T > struct Y {
    static int const c = 3;
};

static int const c = 4;

int main() {
    if( (Y<X<1> >::c >::c>::c) != 0 ) fail( __LINE__ );
    if( (Y<X< 1>>::c >::c>::c) != 0 ) fail( __LINE__ );


    _PASS;
}
