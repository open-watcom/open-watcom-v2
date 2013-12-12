.func frexp
.synop begin
#include <math.h>
double frexp( double value, int *exp );
.ixfunc2 '&Math' &func
.synop end
.desc begin
The &func function breaks a floating-point number into a
normalized fraction and an integral power of 2.
It stores the integral power of 2 in the
.arg int
object pointed to by
.arg exp
.ct .li .
.desc end
.return begin
The &func function returns the value of
.arg x
.ct , such that
.arg x
is a
.id double
with magnitude in the interval [0.5,1) or zero, and
.arg value
equals
.arg x
times 2 raised to the power
.arg *exp
.ct .li .
If
.arg value
is zero, then both parts of the result are zero.
.return end
.see begin
.seelist frexp ldexp modf
.see end
.exmp begin
#include <stdio.h>
#include <math.h>

void main()
  {
    int    expon;
    double value;
.exmp break
    value = frexp(  4.25, &expon );
    printf( "%f %d\n", value, expon );
    value = frexp( -4.25, &expon );
    printf( "%f %d\n", value, expon );
  }
.exmp output
0.531250 3
-0.531250 3
.exmp end
.class ANSI
.system
