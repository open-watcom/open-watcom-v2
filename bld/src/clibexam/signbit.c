#include <math.h>
#include <stdio.h>

void main( void )
{
    printf( "-4.5 %s negative\n",
        signbit( -4.5 ) ? "is" : "is not" );
}
