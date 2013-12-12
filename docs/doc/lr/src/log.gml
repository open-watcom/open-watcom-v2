.func log
#include <math.h>
double log( double x );
.ixfunc2 '&Math' &func
.synop end
.desc begin
The &func function computes the natural logarithm (base e) of
.arg x
.ct .li .
A domain error occurs if the argument is negative.
A range error occurs if the argument is zero.
.desc end
.return begin
The &func function returns the natural logarithm of the argument.
.im errnodom
.return end
.see begin
.seelist log exp log10 log2 pow matherr
.see end
.exmp begin
#include <stdio.h>
#include <math.h>

void main()
  {
    printf( "%f\n", log(.5) );
  }
.exmp output
-0.693147
.exmp end
.class ANSI
.system
