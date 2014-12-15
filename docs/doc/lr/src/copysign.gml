.func copysign
.synop begin
#include <math.h>
double copysign( double x, double y );
.ixfunc2 '&Math' &funcb
.synop end
.desc begin
The
.id &funcb.
function returns a value whose absolute value is equal
to the absolute value of the argument 
.arg x
and whose sign matches argument
.arg y
.ct .li .
.desc end
.return begin
A value of the same magnitude as
.arg x
and the same sign as
.arg y
.ct .li .
.return end
.exmp begin
#include <stdio.h>
#include <math.h>

void main()
  {
    printf( "%f\n", copysign( -2.0, 3.0 ) );
  }
.exmp output
2.000000
.exmp end
.class ISO C99
.system
