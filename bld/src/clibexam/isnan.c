#include <math.h>
#include <stdio.h>

void main( void )
{
    printf( "NAN %s a NaN\n",
        isnan( NAN ) ? "is" : "is not" );
}
