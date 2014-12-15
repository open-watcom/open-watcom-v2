.func llabs
.synop begin
#include <stdlib.h>
long long int llabs( long long int j );
.synop end
.desc begin
The
.id &funcb.
function returns the absolute value
of its long long integer argument
.arg j
.ct .li .
.desc end
.return begin
The
.id &funcb.
function returns the absolute value of its argument.
.return end
.see begin
.seelist labs abs imaxabs fabs
.see end
.exmp begin
#include <stdio.h>
#include <stdlib.h>

void main( void )
{
    long long x, y;
.exmp break
    x = -5000000000;
    y = llabs( x );
    printf( "llabs(%lld) = %lld\n", x, y );
}
.exmp output
llabs(-5000000000) = 5000000000
.exmp end
.class ISO C99
.system
