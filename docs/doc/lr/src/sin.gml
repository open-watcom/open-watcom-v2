.func sin
#include <math.h>
double sin( double x );
.ixfunc2 '&Math' &func
.ixfunc2 '&Trig' &func
.funcend
.desc begin
The &func function computes the sine of
.arg x
(measured in radians).
A large magnitude argument may yield a result with little or no significance.
.desc end
.return begin
The &func function returns the sine value.
.return end
.see begin
.seelist sin acos asin atan atan2 cos tan
.see end
.exmp begin
#include <stdio.h>
#include <math.h>

void main()
  {
    printf( "%f\n", sin(.5) );
  }
.exmp output
0.479426
.exmp end
.class ANSI
.system
