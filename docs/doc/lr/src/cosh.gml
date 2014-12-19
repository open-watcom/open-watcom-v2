.func cosh
.synop begin
#include <math.h>
double cosh( double x );
.ixfunc2 '&Math' &funcb
.ixfunc2 '&Trig' &funcb
.ixfunc2 '&Hyper' &funcb
.synop end
.desc begin
The
.id &funcb.
function computes the hyperbolic cosine of
.arg x
.ct .li .
A range error occurs if the magnitude of
.arg x
is too large.
.desc end
.return begin
The
.id &funcb.
function returns the hyperbolic cosine value.
.im errnorng
.return end
.see begin
.seelist sinh tanh matherr
.see end
.exmp begin
#include <stdio.h>
#include <math.h>

void main()
  {
    printf( "%f\n", cosh(.5) );
  }
.exmp output
1.127626
.exmp end
.class ISO C
.system
