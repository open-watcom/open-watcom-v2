.func logb
#include <math.h>
double logb( double x );
.ixfunc2 '&Math' &func
.synop end
.desc begin
The &func function returns the exponent portion of the argument
.arg x
as a double.
.desc end
.return begin
If successful, the return value is the exponent of
.arg x
.ct .li .  When the argument is zero, the function returns
infinity.  
.return end
.see begin
.seelist &function. ilogb
.see end
.exmp begin
#include <stdio.h>
#include <math.h>

void main()
  {
    printf( "%f\n", logb( 1024.0 ) );
  }
.exmp output
10.0000
.exmp end
.class ISO C99
.system
