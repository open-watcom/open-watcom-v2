#include "fail.h"

template< class T >
struct A {
    int f();
};

template< class T >
int A< T >::f() {
    return 1;
}

template<>
int A< int >::f() {
    return 2;
}


template<>
struct A< short > {
    int f();
};

int A< short >::f() {
    return 3;
}

int main() {
    A< long > al;
    if( al.f() != 1 ) fail( __LINE__ );

    A< int > ai;
    if( ai.f() != 2 ) fail( __LINE__ );

    A< short > as;
    if( as.f() != 3 ) fail( __LINE__ );


    _PASS;
}

