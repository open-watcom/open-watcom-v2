.func cbrt
.synop begin
#include <math.h>
double cbrt( double x );
.ixfunc2 '&Math' &func
.synop end
.desc begin
The &func function computes the cubed root of the argument
.arg x
.ct .li .
.desc end
.return begin
The cubed root of the value.
.return end
.exmp begin
#include <stdio.h>
#include <math.h>

void main()
  {
    printf( "%f\n", cbrt( 8.0 ) );
  }
.exmp output
2.000000
.exmp end
.class ISO C99
.system
