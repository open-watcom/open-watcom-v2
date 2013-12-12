.func trunc
.synop begin
#include <math.h>
double trunc( double x );
.ixfunc2 '&Math' &func
.synop end
.desc begin
The &func function truncates the argument
.arg x
to the appropriate integer.  The function is equivalent to
.kw floor
for positive numbers and 
.kw ceil
for negative numbers.
.desc end
.return begin
The value of
.arg x
without any fractional values.
.return end
.see begin
.seelist &function. nearbyint rint round floor ceil
.see end
.exmp begin
#include <stdio.h>
#include <math.h>

void main()
  {
    printf( "%f\n", round( 1.5 ) );
  }
.exmp output
1.000000
.exmp end
.class ISO C99
.system
