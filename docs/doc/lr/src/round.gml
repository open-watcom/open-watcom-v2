.func round
.synop begin
#include <math.h>
double round( double x );
.ixfunc2 '&Math' &funcb
.synop end
.desc begin
The
.id &funcb.
function rounds the argument
.arg x
to the nearest integer. Values halfway between integers
always rounded away from zero.
.desc end
.return begin
The rounded value of 
.arg x
.ct .li .
.return end
.see begin
.seelist nearbyint rint trunc
.see end
.exmp begin
#include <stdio.h>
#include <math.h>

void main()
  {
    printf( "%f\n", round( 1.5 ) );
  }
.exmp output
2.000000
.exmp end
.class ISO C99
.system
