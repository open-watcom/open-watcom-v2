.func atanh
.synop begin
#include <math.h>
double atanh( double x );
.ixfunc2 '&Math' &funcb
.ixfunc2 '&Trig' &funcb
.ixfunc2 '&Hyper' &funcb
.synop end
.desc begin
The
.id &funcb.
function computes the inverse hyperbolic tangent of
.arg x
.ct .li .
A domain error occurs if the value of
.arg x
is outside the range (&minus.1,1).
.desc end
.return begin
The
.id &funcb.
function returns the inverse hyperbolic tangent value.
.im errnodom
.return end
.see begin
.seelist acosh asinh matherr tanh
.see end
.exmp begin
#include <stdio.h>
#include <math.h>

void main()
  {
    printf( "%f\n", atanh( 0.5 ) );
  }
.exmp output
0.549306
.exmp end
.class WATCOM
.system
