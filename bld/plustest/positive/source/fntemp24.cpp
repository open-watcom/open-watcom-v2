#include "fail.h"

template< class T >
void g( T i, T j = T() )
{ }

template< class T >
struct A
{
    template< class U >
    void f( U i, U j = U() )
    { }
};



template< typename T, bool TB >
struct B
{
    B( )
        : v( 0 )
    { }

    template< typename TT, bool TTB >
    B( const B< TT, TTB > &V )
        : v( 1 )
    { }

    T v;
};


int main()
{
    g( 1, 2 );

    A< int > a;
    a.f( 1, 2 );

    B< int, true > b1;
    B< long, false > b2( b1 );

    if( ( b1.v != 0 ) && ( b2.v != 1 ) ) fail( __LINE__ );


    _PASS;
}
