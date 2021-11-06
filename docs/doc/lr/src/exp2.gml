.func exp2
.synop begin
#include <math.h>
double exp2( double x );
.ixfunc2 '&Math' &funcb
.synop end
.desc begin
The
.id &funcb.
function computes 2.0 raised to the value
.arg x
.period  The current implementation uses a simple power
calculation.
.desc end
.return begin
The function returns two to the power
.arg x
.period
.return end
.see begin
.seelist exp pow
.see end
.exmp begin
#include <stdio.h>
#include <math.h>

void main()
  {
    printf( "%f\n", exp2( 2.0 ) );
  }
.exmp output
4.000000
.exmp end
.class ISO C99
.system
