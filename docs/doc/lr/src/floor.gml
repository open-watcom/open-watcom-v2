.func floor
.synop begin
#include <math.h>
double floor( double x );
.ixfunc2 '&Math' &funcb
.synop end
.desc begin
The
.id &funcb.
function computes the largest integer not greater than
.arg x
.ct .li .
.desc end
.return begin
The
.id &funcb.
function computes the largest integer not greater than
.arg x
.ct , expressed as a
.id double.
.return end
.see begin
.seelist floor ceil fmod
.see end
.exmp begin
#include <stdio.h>
#include <math.h>

void main()
  {
    printf( "%f\n", floor( -3.14 ) );
    printf( "%f\n", floor( -3. ) );
    printf( "%f\n", floor( 0. ) );
    printf( "%f\n", floor( 3.14 ) );
    printf( "%f\n", floor( 3. ) );
  }
.exmp output
-4.000000
-3.000000
0.000000
3.000000
3.000000
.exmp end
.class ISO C
.system
