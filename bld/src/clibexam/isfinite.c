#include <math.h>
#include <stdio.h>

void main( void )
{
    printf( "zero %s a finite number\n",
        isfinite( 0.0 ) ? "is" : "is not" );
}
