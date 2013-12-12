.func imaxabs
#include <inttypes.h>
intmax_t imaxabs( intmax_t j );
.synop end
.*
.desc begin
The &func function returns the absolute value
of its maximum-size integer argument
.arg j
.ct .li .
.desc end
.return begin
The &func function returns the absolute value of its argument.
.return end
.*
.see begin
.seelist imaxabs labs llabs abs fabs
.see end
.*
.exmp begin
#include <stdio.h>
#include <inttypes.h>

void main( void )
{
    intmax_t    x, y;
.exmp break
    x = -500000000000;
    y = imaxabs( x );
    printf( "imaxabs(%jd) = %jd\n", x, y );
}
.exmp output
imaxabs(-500000000000) = 500000000000
.exmp end
.*
.class ISO C99
.system
