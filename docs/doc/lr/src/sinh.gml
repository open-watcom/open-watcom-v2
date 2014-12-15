.func sinh
.synop begin
#include <math.h>
double sinh( double x );
.ixfunc2 '&Math' &funcb
.ixfunc2 '&Trig' &funcb
.ixfunc2 '&Hyper' &funcb
.synop end
.desc begin
The
.id &funcb.
function computes the hyperbolic sine of
.arg x
.ct .li .
A range error occurs if the magnitude of
.arg x
is too large.
.desc end
.return begin
The
.id &funcb.
function returns the hyperbolic sine value.
.im errnorng
.return end
.see begin
.seelist sinh cosh tanh matherr
.see end
.exmp begin
#include <stdio.h>
#include <math.h>

void main()
  {
    printf( "%f\n", sinh(.5) );
  }
.exmp output
0.521095
.exmp end
.class ANSI
.system
