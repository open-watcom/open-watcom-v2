/*
    converts the number 0.11111111111111111111111... in different bases
    to base 10 and sums it, this duplicates the Taylor series for E
*/
#include <stdio.h>
#include <stdlib.h>

#define DIGITS  300           /* number of 5-digit packets */

SBIG d[2*DIGITS+1];

#if (2L*DIGITS*4L) > 32767+1000000L*defined(__386__)
#error too many DIGITS!
#endif

int main( void )
{
    unsigned i;
    unsigned j;
    SBIG c;
    UBIG digits;
    ldiv_t res;

    putchar( '2' );
    putchar( '.' );
    for( j = 2*DIGITS; j > 1; --j ) {
        d[ j ] = 1;
    }
    digits = 0;
    c = 0;
    for( i = 0; i < DIGITS; ++i ) {
        for( j = 2*DIGITS; j != 0; --j ) {
            c += d[j] * SBIG(100000);
            d[j] = c % j;
            c /= j;
        }
        digits += 5;
        printf( "%05lu", c ); fflush( stdout );
        if(( digits % 75 ) == 0 ) {
            printf( "\n%u digits\n", digits );
        }
    }
    putchar( '\n' );
    return( 0 );
}
