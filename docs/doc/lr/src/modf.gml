.func modf
.synop begin
#include <math.h>
double modf( double value, double *iptr );
.ixfunc2 '&Math' &funcb
.synop end
.desc begin
The
.id &funcb.
function breaks the argument
.arg value
into integral and fractional parts, each of which has the same sign as
the argument.
It stores the integral part as a
.id double
in the object pointed to by
.arg iptr
.ct .li .
.desc end
.return begin
The
.id &funcb.
function returns the signed fractional part of
.arg value
.ct .li .
.return end
.see begin
.seelist modf frexp ldexp
.see end
.exmp begin
#include <stdio.h>
#include <math.h>

void main()
  {
    double integral_value, fractional_part;
.exmp break
    fractional_part = modf( 4.5, &integral_value );
    printf( "%f %f\n", fractional_part, integral_value );
    fractional_part = modf( -4.5, &integral_value );
    printf( "%f %f\n", fractional_part, integral_value );
  }
.exmp output
0.500000 4.000000
-0.500000 -4.000000
.exmp end
.class ISO C
.system
