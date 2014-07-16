#include "fail.h"

enum A {
    c0 = 0,
    c1 = 120
};

int f( const A &a )
{
    return 1;
}

int f( A &a )
{
    return 2;
}


int g( const void * )
{
    return 0;
}

int g( ... )
{
    return 1;
}

int main()
{
    A a1 = c1;
    const A a2 = c1;

    if( f( a1 ) != 2 ) fail(__LINE__);
    if( f( a2 ) != 1 ) fail(__LINE__);

    if( g( c0 ) != 1 ) fail(__LINE__);
    if( g( 0 ) != 0 ) fail(__LINE__);


    _PASS;
}
