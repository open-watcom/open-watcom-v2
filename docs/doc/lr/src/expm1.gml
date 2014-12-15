.func expm1
.synop begin
#include <math.h>
double expm1( double x );
.ixfunc2 '&Math' &funcb
.synop end
.desc begin
The
.id &funcb.
function computes the value of the exponential of
.arg x
minus 1.  This routine provides far better accuracy for
cases where the exponential of the argument
.arg x
is significantly less than 1.0.
.desc end
.return begin
The function returns a the exponential of
,arg x
minus 1 without loss of accuracy due to subtractive cancelation.
.return end
.see begin
.seelist exp
.see end
.exmp begin
#include <stdio.h>
#include <math.h>

void main()
  {
    printf( "%f\n", expm1( -1.0E-3 ) );
  }
.exmp output
-0.000995
.exmp end
.class ISO C99
.system
