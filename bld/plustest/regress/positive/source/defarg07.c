#include "fail.h"

namespace ns
{
    typedef int A;

    template< class T, class U >
    int f1( int i, T t = A(1), U u = A(2) )
    {
        return i + t + u;
    }

    template< class T >
    int f2( int i, T t, ... )
    {
        return i + t;
    }
}

struct B
{
    typedef int A;

    template< class T, class U >
    int f1( int i, T t = A(3), U u = A(4) )
    {
        return i + t + u;
    }

    template< class T >
    int f2( int i, T t, ... )
    {
        return i + t;
    }
};


int main()
{
    using ns::f1;
    using ns::f2;

    if( f1< int, int >( 3 ) != 6 ) fail( __LINE__ );
    if( f1< int, int >( 3, 5 ) != 10 ) fail( __LINE__ );
    if( f1< int, int >( 3, 5, 7 ) != 15 ) fail( __LINE__ );

    if( f2( 3, 5, 7, 9 ) != 8 ) fail( __LINE__ );
    if( f2( 3, 5, 7 ) != 8 ) fail( __LINE__ );
    if( f2( 3, 5 ) != 8 ) fail( __LINE__ );


    B b;

    if( b.f1< int, int >( 2 ) != 9 ) fail( __LINE__ );
    if( b.f1< int, int >( 2, 4 ) != 10 ) fail( __LINE__ );
    if( b.f1< int, int >( 2, 4, 6 ) != 12 ) fail( __LINE__ );

    if( b.f2( 3, 4, 5, 6 ) != 7 ) fail( __LINE__ );
    if( b.f2( 3, 4, 5 ) != 7 ) fail( __LINE__ );
    if( b.f2( 3, 4 ) != 7 ) fail( __LINE__ );


    _PASS;
}
