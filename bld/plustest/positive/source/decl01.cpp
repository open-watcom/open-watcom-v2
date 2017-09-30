#include "fail.h"

typedef int (* __pascal PF)( int,int,int,int );

int __pascal add3( int x, int y, int z, int w )
{
    return x + y + z;
}

int main()
{
    PF x;

    x = add3;
    if( x( 1, 2, 4, 8 ) != 7 ) fail(__LINE__);
    _PASS;
}
