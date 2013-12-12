.func atan2
.synop begin
#include <math.h>
double atan2( double y, double x );
.ixfunc2 '&Math' &func
.ixfunc2 '&Trig' &func
.synop end
.desc begin
The
.id &func.
function computes the principal value of the arctangent of
.arg y/x
.ct , using the signs of both arguments
to determine the quadrant of the return value.
A domain error occurs if both arguments are zero.
.desc end
.return begin
The
.id &func.
function returns the arctangent of
.arg y/x
.ct , in the range (&minus.&pi.,&pi.).
.im errnodom
.return end
.see begin
.seelist atan2 acos asin atan matherr
.see end
.exmp begin
#include <stdio.h>
#include <math.h>

void main()
  {
    printf( "%f\n", atan2( .5, 1. ) );
  }
.exmp output
0.463648
.exmp end
.class ANSI
.system
