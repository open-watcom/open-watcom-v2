.func lgamma
.synop begin
#include <math.h>
double lgamma( double x );
.ixfunc2 '&Math' &func
.synop end
.desc begin
The
.id &func.
function returns the natural logarithm of the
absolute value of the Gamma function of 
.arg x
.ct .li . The sign of the Gamma function after this function
is called will be located in signgam.  This function is not
thread-safe if the user is interested in the sign of Gamma, and
.kw lgamma_r
should be used instead in multithreaded applications.
.desc end
.return begin
If successful, the return value is the natural logarithm of
the absolute value of the Gamma function computed for
.arg x
.ct .li .  When the argument is not-a-number, the function 
returns NAN.  For arguments of the values positive or negative
infinity, the function returns positive or negative infinity
respectively.
.return end
.see begin
.seelist &function. lgamma_r tgamma
.see end
.exmp begin
#include <stdio.h>
#include <math.h>

void main()
  {
    printf( "%f\n", lgamma( 2.0 ) );
    printf( "%d\n", signgam );
  }
.exmp output
0.00000
1
.exmp end
.class ISO C99
.system
