.func log1p
.synop begin
#include <math.h>
double log1p( double x );
.ixfunc2 '&Math' &func
.synop end
.desc begin
The
.id &func.
function computes the natural logarithm of one plus
.arg x
.ct .li . For small values of
.arg x
this function provides far better accuracy than using the
.kw log
function directly.
.desc end
.return begin
If successful, the return value is the logarithm of one plus
.arg x
.ct .li .  When the argument is -1.0, the function returns
infinity.  If the value of
.arg x
is less than -1.0, the function returns NAN.
.return end
.see begin
.seelist log
.see end
.exmp begin
#include <stdio.h>
#include <math.h>

void main()
  {
    printf( "%f\n", log1p( 0.02 ) );
  }
.exmp output
0.019803
.exmp end
.class ISO C99
.system
