.func ldiv
#include <stdlib.h>
ldiv_t ldiv( long int numer, long int denom );

typedef struct {
    long int quot;     /* quotient */
    long int rem;      /* remainder */
} ldiv_t;
.funcend
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
.kw ldiv_t
that contains the fields
.kw quot
and
.kw rem
.ct,
which are both of type
.id long int.
.return end
.*
.see begin
.seelist ldiv div lldiv imaxdiv
.see end
.*
.exmp begin
#include <stdio.h>
#include <stdlib.h>

void print_time( long int ticks )
{
    ldiv_t sec_ticks;
    ldiv_t min_sec;

    sec_ticks = ldiv( ticks, 100L );
    min_sec   = ldiv( sec_ticks.quot, 60L );
    printf( "It took %ld minutes and %ld seconds\n",
            min_sec.quot, min_sec.rem );
}
.exmp break
void main( void )
{
    print_time( 86712L );
}
.exmp output
It took 14 minutes and 27 seconds
.exmp end
.*
.class ISO C90
.system
