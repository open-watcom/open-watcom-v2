.func acosh
.synop begin
#include <math.h>
double acosh( double x );
.ixfunc2 '&Math' &func
.ixfunc2 '&Trig' &func
.ixfunc2 '&Hyper' &func
.synop end
.desc begin
The &func function computes the inverse hyperbolic cosine of
.arg x
.ct .li .
A domain error occurs if the value of
.arg x
is less than 1.0.
.desc end
.return begin
The &func function returns the inverse hyperbolic cosine value.
.im errnodom
.return end
.see begin
.seelist acosh asinh atanh cosh matherr
.see end
.exmp begin
#include <stdio.h>
#include <math.h>

void main()
  {
    printf( "%f\n", acosh( 1.5 ) );
  }
.exmp output
0.962424
.exmp end
.class WATCOM
.system
