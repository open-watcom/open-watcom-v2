#include <math.h>
#include <stdio.h>

void main( void )
{
    printf( "zero %s an infinite number\n",
        isinf( 0.0 ) ? "is" : "is not" );
}
