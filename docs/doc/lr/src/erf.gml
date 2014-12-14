.func erf
.synop begin
#include <math.h>
double erf( double x );
.ixfunc2 '&Math' &func
.synop end
.desc begin
The
.id &func.
function computes the value of the error function, also
known as the Gauss error function, for the argument
.arg x
.ct .li .
.desc end
.return begin
For non-infinite values of 
.arg x
the function returns the value of the error function.  For positive
infinity or negative infinity the function returns positive or
negative one respectively.  For not-a-number the function returns 
NAN.
.return end
.see begin
.seelist erfc
.see end
.exmp begin
#include <stdio.h>
#include <math.h>

void main()
  {
    printf( "%f\n", erf( 0.0 ) );
  }
.exmp output
0.000000
.exmp end
.class ISO C99
.system
