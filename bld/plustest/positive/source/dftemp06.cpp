#include "fail.h"

template< int i1, int i2 = 2 >
struct A;

template< int i1 = 1, int i2 >
struct A;

template< int i1, int i2 >
struct A
{
    static const int val = i1 + i2;
};


template< class T1, class T2 = T1 * >
struct B;

template< class U1 = char, class U2 >
struct B;

template< class T1, class T2 >
struct B
{
    static const int s1 = sizeof( T1 );
    static const int s2 = sizeof( T2 );
};



int main()
{
    A<> a1;
    if( a1.val != 3 ) fail( __LINE__ );

    A< 3 > a2;
    if( a2.val != 5 ) fail( __LINE__ );

    A< 4, 5 > a3;
    if( a3.val != 9 ) fail( __LINE__ );


    B<> b1;
    if( b1.s1 != sizeof( char ) ) fail( __LINE__ );
    if( b1.s2 != sizeof( char * ) ) fail( __LINE__ );

    B< short > b2;
    if( b2.s1 != sizeof( short ) ) fail( __LINE__ );
    if( b2.s2 != sizeof( short * ) ) fail( __LINE__ );

    B< short, char > b3;
    if( b3.s1 != sizeof( short ) ) fail( __LINE__ );
    if( b3.s2 != sizeof( char ) ) fail( __LINE__ );


    _PASS;
}
