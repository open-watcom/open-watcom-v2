#include "fail.h"

template< class T >
struct A { };

template< >
struct A< int > {
    static const int val = 0;
};

template< class T, class U = A< T > >
struct B {
    static const int val = U::val;
};

template< class T, T n >
struct C {
    static const int size = sizeof( T );
    static const int val = n;
};

template< class T, int n = T() >
struct D {
    static const int size = sizeof( T );
    static const int val = n;
};

template< class T, T n = T() >
struct E {
    static const int size = sizeof( T );
    static const int val = n;
};


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


int main()
{
    if( B< int >::val != 0) fail( __LINE__ );

    if( C< char, 1 >::size != sizeof( char ) ) fail( __LINE__ );
    if( C< short, 1 >::size != sizeof( short ) ) fail( __LINE__ );
    if( C< int, 1 >::size != sizeof( int ) ) fail( __LINE__ );
    if( C< int, 1 >::val != 1 ) fail( __LINE__ );
    if( C< int, 2 >::val != 2 ) fail( __LINE__ );

    if( D< int >::size != sizeof( int ) ) fail( __LINE__ );
    if( D< int >::val != 0 ) fail( __LINE__ );

    if( E< int >::size != sizeof( int ) ) fail( __LINE__ );
    if( E< int >::val != 0 ) fail( __LINE__ );

    if( (Y<X<1> >::c >::c>::c) != 0 ) fail( __LINE__ );
    if( (Y<X< 1>>::c >::c>::c) != 3 ) fail( __LINE__ );

    _PASS;
}
