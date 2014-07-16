#include "fail.h"

template< class T >
T g( void ){ return 13;}

template< class T >
T h( T i )
{
    T (*fp)(void);
    fp = g<T>;
    return fp()*i;
}

int main()
{
    int a = 3;
    if( h( a ) != 39 ) fail( __LINE__ );

   _PASS;
}
