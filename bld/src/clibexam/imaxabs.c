#include <stdio.h>
#include <inttypes.h>

void main( void )
{
    intmax_t    x, y;

    x = -500000000000;
    y = imaxabs( x );
    printf( "imaxabs(%jd) = %jd\n", x, y );
}
