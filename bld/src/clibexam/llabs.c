#include <stdio.h>
#include <stdlib.h>

void main( void )
{
    long long x, y;

    x = -5000000000;
    y = llabs( x );
    printf( "llabs(%lld) = %lld\n", x, y );
}
