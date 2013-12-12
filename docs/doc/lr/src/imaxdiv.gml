.func imaxdiv
.synop begin
#include <stdlib.h>
imaxdiv_t imaxdiv( intmax_t numer, intmax_t denom );

typedef struct {
    intmax_t    quot;  /* quotient */
    intmax_t    rem;   /* remainder */
} imaxdiv_t;
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
The
.id &func.
function returns a structure of type
.kw imaxdiv_t
that contains the fields
.kw quot
and
.kw rem
.ct,
which are both of type
.id intmax_t.
.return end
.*
.see begin
.seelist imaxdiv div ldiv lldiv
.see end
.*
.exmp begin
#include <stdio.h>
#include <inttypes.h>

void print_time( intmax_t ticks )
{
    imaxdiv_t sec_ticks;
    imaxdiv_t min_sec;

    sec_ticks = imaxdiv( ticks, 1000000 );
    min_sec   = imaxdiv( sec_ticks.quot, 60 );
    printf( "It took %jd minutes and %jd seconds\n",
            min_sec.quot, min_sec.rem );
}
.exmp break
void main( void )
{
    print_time( 9876543210 );
}
.exmp output
It took 164 minutes and 36 seconds
.exmp end
.class ISO C99
.system
