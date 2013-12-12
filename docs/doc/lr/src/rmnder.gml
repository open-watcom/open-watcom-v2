.func remainder
.synop begin
#include <math.h>
double remainder( double x, double y );
.ixfunc2 '&Math' &func
.synop end
.desc begin
The
.id &func.
function computes remainder of the division of
.arg x
by
.arg y
.ct .li .
.desc end
.return begin
The remainder of the division of
.arg x
by
.arg y
.ct .li .
.return end
.exmp begin
#include <stdio.h>
#include <math.h>

void main()
  {
    printf( "%f\n", remainder( 7.0, 2.0 ) );
  }
.exmp output
1.00000
.exmp end
.class ISO C99
.system
