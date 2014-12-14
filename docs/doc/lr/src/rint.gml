.func rint
.synop begin
#include <math.h>
double rint( double x );
.ixfunc2 '&Math' &func
.synop end
.desc begin
The
.id &func.
function rounds the argument
.arg x
to a nearby integer.  The direction of the rounding 
is determined by the current value of
.kw fegetround
.ct .li . If supported, this function will throw a 
floating point error if an overflow occurs due to the
current rounding mode.
.desc end
.return begin
The rounded value of 
.arg x
.ct .li .
.return end
.see begin
.seelist fegetround fesetround nearbyint round trunc
.see end
.exmp begin
#include <stdio.h>
#include <math.h>

void main()
  {
    fesetround(FE_TONEAREST);
    printf( "%f\n", rint( 1.2 ) );
  }
.exmp output
1.000000
.exmp end
.class ISO C99
.system
