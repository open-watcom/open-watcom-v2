.func tanh
#include <math.h>
double tanh( double x );
.ixfunc2 '&Math' &func
.ixfunc2 '&Trig' &func
.ixfunc2 '&Hyper' &func
.funcend
.desc begin
The &func function computes the hyperbolic tangent of
.arg x
.ct .li .
.np
When the
.arg x
argument is large, partial or total loss of significance may occur.
The
.kw matherr
function will be invoked in this case.
.desc end
.return begin
The &func function returns the hyperbolic tangent value.
.im errnoref
.return end
.see begin
.seelist tanh cosh sinh matherr
.see end
.exmp begin
#include <stdio.h>
#include <math.h>

void main()
  {
    printf( "%f\n", tanh(.5) );
  }
.exmp output
0.462117
.exmp end
.class ANSI
.system
