#include "fail.h"

template< class T >
struct A
{
    template< class U >
    struct B
    { };
};

template< class T >
const A< int >::B< T > *f( const A<int>::B<T> & )
{
    return 0;
}

template< class T, class U >
const typename A< T >::template B< U > *g( typename A< T >::template B< U > )
{
    return 0;
}

template< class T >
void h( typename T::U )
{ }

template< class T >
int h( const T * )
{
    return 1;
}

int main()
{
    A< int >::B< long > b;

    const A< int >::B< long > *fb = f( b );
    const A< int >::B< long > *gb = g< int >( b );

    if( h( (const void *) 0 ) != 1 ) fail( __LINE__ );

    _PASS;
}
