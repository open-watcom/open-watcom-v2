.func asinh
#include <math.h>
double asinh( double x );
.ixfunc2 '&Math' &func
.ixfunc2 '&Trig' &func
.ixfunc2 '&Hyper' &func
.synop end
.desc begin
The &func function computes the inverse hyperbolic sine of
.arg x
.ct .li .
.desc end
.return begin
The &func function returns the inverse hyperbolic sine value.
.return end
.see begin
.seelist asinh acosh atanh sinh matherr
.see end
.exmp begin
#include <stdio.h>
#include <math.h>

void main()
  {
    printf( "%f\n", asinh( 0.5 ) );
  }
.exmp output
0.481212
.exmp end
.class WATCOM
.system
