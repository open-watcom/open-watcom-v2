.func nearbyint
.synop begin
#include <math.h>
double nearbyint( double x );
.ixfunc2 '&Math' &func
.synop end
.desc begin
The
.id &func.
function rounds the argument
.arg x
to a nearby integer without the possibility of throwing
an exception.  The direction of the rounding is determined by
the current value of
.kw fegetround
.ct .li .
.desc end
.return begin
The rounded value of 
.arg x
.ct .li .
.return end
.see begin
.seelist &function. fegetround fesetround rint round trunc
.see end
.exmp begin
#include <stdio.h>
#include <math.h>

void main()
  {
    fesetround(FE_TONEAREST);
    printf( "%f\n", nearbyint( 1.2 ) );
  }
.exmp output
1.000000
.exmp end
.class ISO C99
.system
