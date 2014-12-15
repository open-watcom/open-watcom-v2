.func tgamma
.synop begin
#include <math.h>
double tgamma( double x );
.ixfunc2 '&Math' &funcb
.synop end
.desc begin
The
.id &funcb.
function returns the value of the Gamma function of 
.arg x
.ct .li . 
.desc end
.return begin
If successful, the return value is the value of the Gamma 
function computed for
.arg x
.ct .li .  When the argument is not-a-number or negative infinity, 
the function returns NAN.  If the argument is positive infinity, 
the function returns positive infinity.
.return end
.see begin
.seelist lgamma lgamma_r
.see end
.exmp begin
#include <stdio.h>
#include <math.h>

void main()
  {
    printf( "%f\n", tgamma( 4.0 ) );
  }
.exmp output
6.00000
.exmp end
.class ISO C99
.system
