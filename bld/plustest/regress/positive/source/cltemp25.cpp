#include "fail.h"

template< class T >
struct A
{
    A()
        : val( 0 )
    { }

    operator T ();

    void operator = ( T );

    void operator ++ ();

    void operator ++ ( int );

private:
    T val;
};

template< class T >
A< T >::operator T ()
{
    return val;
}

template< class T >
void A< T >::operator = ( T t )
{
    val = t;
}

template< class T >
void A< T >::operator ++ ()
{
    val += 1;
}

template< class T >
void A< T >::operator ++ ( int )
{
    val += 2;
}

int main()
{
    A< int > a;

    a = 1;
    if( a != 1 ) fail( __LINE__ );

    a.operator = ( 2 );
    if( a != 2 ) fail( __LINE__ );

    ++a;
    if( a != 3 ) fail( __LINE__ );

    a++;
    if( a != 5 ) fail( __LINE__ );

    _PASS;
}
