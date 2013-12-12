.func exp2
#include <math.h>
double exp2( double x );
.ixfunc2 '&Math' &func
.synop end
.desc begin
The &func function computes 2.0 raised to the value 
.arg x
.ct .li .  The current implementation uses a simple power
calculation.
.desc end
.return begin
The function returns two to the power
.arg x
.ct .li .
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
