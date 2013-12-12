.func nextafter
#include <math.h>
double nextafter( double x, double y );
.ixfunc2 '&Math' &func
.synop end
.desc begin
The &func function returns the next machine floating point
number of
.arg x
in the direction towards
.arg y
.ct .li .
.desc end
.return begin
The next representable floating point value after or before
.arg x
in the direction of
.arg y
.ct .li .
.return end
.exmp begin
#include <stdio.h>
#include <math.h>

void main()
  {
    printf( "%f\n", nextafter( 8.0, 9.0 ) );
  }
.exmp output
8.000000
.exmp end
.class ISO C99
.system
