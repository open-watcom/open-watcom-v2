#include <math.h>
#include <stdio.h>

void main( void )
{
    printf( "infinity %s a normal number\n",
        fpclassify( INFINITY ) == FP_NORMAL ?
        "is" : "is not" );
}
