.func acosh
.synop begin
#include <math.h>
double acosh( double x );
.ixfunc2 '&Math' &funcb
.ixfunc2 '&Trig' &funcb
.ixfunc2 '&Hyper' &funcb
.synop end
.desc begin
The
.id &funcb.
function computes the inverse hyperbolic cosine of
.arg x
.ct .li .
A domain error occurs if the value of
.arg x
is less than 1.0.
.desc end
.return begin
The
.id &funcb.
function returns the inverse hyperbolic cosine value.
.im errnodom
.return end
.see begin
.seelist asinh atanh cosh matherr
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
