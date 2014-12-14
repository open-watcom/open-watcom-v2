.func fmax
.synop begin
#include <math.h>
double fmax( double x, double y );
.ixfunc2 '&Math' &func
.synop end
.desc begin
The
.id &func.
function returns the larger of
.arg x
and
.arg y
.ct .li .
.desc end
.return begin
The routine will return the larger of
.arg x
or
.arg y
.ct .li .
.return end
.see begin
.seelist fdim fmin
.see end
.exmp begin
#include <stdio.h>
#include <math.h>

void main()
  {
    printf( "%f\n", fmax( 3.0, 2.0 ) );
  }
.exmp output
3.000000
.exmp end
.class ISO C99
.system
