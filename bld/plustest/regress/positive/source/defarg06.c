#include "fail.h"

struct A {
    struct B {
        static int f( int i = c ) {
            return 2 * i;
        }
    };

    static const int c = 3;
};


template< class T >
struct C {
    static int f( int i, int j = 3 ) {
        return i + 10 * j;
    }
};

struct D {
    static int f() {
        return C< int >::f( 2 );
    }
};


template < class T >
struct E {
    static int f( int i = T::c ) {
        return 2 * i;
    }
};

struct F : E< F > {
    static const int c = 2;
};


int main()
{
    if( A::B::f() != 6 ) fail( __LINE__ );
    if( C< int >::f( 2 ) != 32 ) fail( __LINE__ );
    if( F::f() != 4 ) fail( __LINE__ );
    if( F::f( 3 ) != 6 ) fail( __LINE__ );

    _PASS;
}
