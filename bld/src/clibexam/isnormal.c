#include <math.h>
#include <stdio.h>

void main( void )
{
    printf( "zero %s a normal number\n",
        isnormal( 0.0 ) ? "is" : "is not" );
}
