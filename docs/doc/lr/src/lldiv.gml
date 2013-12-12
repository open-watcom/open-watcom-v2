.func lldiv
.synop begin
#include <stdlib.h>
lldiv_t lldiv( long long int numer, 
               long long int denom );

typedef struct {
    long long int quot; /* quotient */
    long long int rem;  /* remainder */
} lldiv_t;
.synop end
.*
.desc begin
The &func
function calculates the quotient and remainder of the division of the
numerator
.arg numer
by the denominator
.arg denom
.ct .li .
.desc end
.*
.return begin
The &func function returns a structure of type
.kw lldiv_t
that contains the fields
.kw quot
and
.kw rem
.ct,
which are both of type
.id long long int.
.return end
.*
.see begin
.seelist ldiv div imaxdiv
.see end
.*
.exmp begin
#include <stdio.h>
#include <stdlib.h>

void print_time( long long int ticks )
{
    lldiv_t sec_ticks;
    lldiv_t min_sec;

    sec_ticks = lldiv( ticks, 100 );
    min_sec   = lldiv( sec_ticks.quot, 60 );
    printf( "It took %lld minutes and %lld seconds\n",
            min_sec.quot, min_sec.rem );
}
.exmp break
void main( void )
{
    print_time( 73495132 );
}
.exmp output
It took 12249 minutes and 11 seconds
.exmp end
.*
.class ISO C99
.system
