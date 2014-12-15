.func ilogb
.synop begin
#include <math.h>
int ilogb( double x );
.ixfunc2 '&Math' &funcb
.synop end
.desc begin
The
.id &funcb.
function returns the exponent portion of the argument
.arg x
as an int.
.desc end
.return begin
If successful, the return value is the exponent of
.arg x
.ct .li .  When the argument is zero, the function returns
FP_ILOGB0.  When the argument is not-a-number, or NAN, the
function returns FP_ILOGBNAN.  For positive or negative
infinity, the function returns INT_MAX.
.return end
.see begin
.seelist logb
.see end
.exmp begin
#include <stdio.h>
#include <math.h>

void main()
  {
    printf( "%d\n", ilogb( 1024.0 ) );
  }
.exmp output
10
.exmp end
.class ISO C99
.system
