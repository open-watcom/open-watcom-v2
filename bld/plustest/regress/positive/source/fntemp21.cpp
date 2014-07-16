#include "fail.h"

template< typename T >
int f( T *, void ( T::* p )( const T & ) const)
{
    return 0;
}

template< typename T >
int f( T *, ... )
{
    return 1;
}

struct A
{
    void m( const A & ) const
    { }
};


int main()
{
    A a;
    int i;

    if( f( &a, &A::m ) != 0 ) fail( __LINE__ );
    if( f( &a, 2 ) != 1 ) fail( __LINE__ );
    if( f( &i, &A::m ) != 1 ) fail( __LINE__ );
    if( f( &i, 2 ) != 1 ) fail( __LINE__ );

    if( f( &i ) != 1 ) fail( __LINE__ );
    if( f( &i, 2, 3 ) != 1 ) fail( __LINE__ );
    if( f( &a, &A::m, 3 ) != 1 ) fail( __LINE__ );


    _PASS;
}
