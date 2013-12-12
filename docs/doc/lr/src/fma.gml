.func fma
.synop begin
#include <math.h>
double fma( double x, double y, double z );
.ixfunc2 '&Math' &func
.synop end
.desc begin
The &func function performs a fused multiply-add operation. The
resultant value is the product of
.arg x
and
.arg y
summed with
.arg z
.ct .li .
.desc end
.return begin
The x*y+z
.return end
.exmp begin
#include <stdio.h>
#include <math.h>

void main()
  {
    printf( "%f\n", fma( 2.0, 3.0, 1.0 ) );
  }
.exmp output
7.00000
.exmp end
.class ISO C99
.system
