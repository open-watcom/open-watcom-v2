.func log10
.synop begin
#include <math.h>
double log10( double x );
.ixfunc2 '&Math' &funcb
.synop end
.desc begin
The
.id &funcb.
function computes the logarithm (base 10) of
.arg x
.ct .li .
A domain error occurs if the argument is negative.
A range error occurs if the argument is zero.
.desc end
.return begin
The
.id &funcb.
function returns the logarithm (base 10) of the argument.
.im errnodom
.return end
.see begin
.seelist log10 exp log log2 pow matherr
.see end
.exmp begin
#include <stdio.h>
#include <math.h>

void main()
  {
    printf( "%f\n", log10(.5) );
  }
.exmp output
-0.301030
.exmp end
.class ISO C
.system
