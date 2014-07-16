#include "fail.h"

template< class U >
int f(U u);

template< class T >
int f(T t)
{
    const T my_t = t;
    return 2*my_t;
}

int f(short s)
{
    return 4*s;
}

int f(int i)
{
    return 3*i;
}

template< class T >
int g(int i, T t)
{
    return i + t + T();
}

template< int i >
int h()
{
    return 5*i;
}

template< class T >
int k( const T a, T b )
{
    return 1;
}


int main()
{
    if( f(1) != 3 ) fail( __LINE__ );
    if( f((short) 1) != 4 ) fail( __LINE__ );
    if( f<>(1) != 2 ) fail( __LINE__ );
    if( f<int>(1) != 2 ) fail( __LINE__ );
    if( f<int>((short) 1) != 2 ) fail( __LINE__ );
    if( g((short) 1, (short) 2) != 3 ) fail( __LINE__ );
    if( h<3>() != 15 ) fail( __LINE__ );

    int *p = 0;
    if( k( p, p ) != 1 ) fail( __LINE__ );
    if( k( ( int * const ) p, p ) != 1 ) fail( __LINE__ );
    if( k( p, ( int * const ) p ) != 1 ) fail( __LINE__ );
    if( k( ( int * const ) p, ( int * const ) p ) != 1 ) fail( __LINE__ );

    _PASS;
}
