#include "fail.h"


template< class T >
struct A
{ };


template< class T >
int f( A< T > & )
{
    return 10;
}

template< class T >
int f( A< T * > &)
{
    return 20;
}

int g( int (*fp)( A< char > & ) )
{
    A< char > a;

    return 10 - fp( a );
}

int g( int (*fp)( int & ) )
{
    return 1;
}


typedef A< char > AC;

int main()
{
    int (*fna)( A< char > & ) = f;
    int (*fnac)( AC & ) = f;

    if( g( f ) != 0 ) fail( __LINE__ );
    if( g( fna ) != 0 ) fail( __LINE__ );
    if( g( fnac ) != 0 ) fail( __LINE__ );

    _PASS;
}
