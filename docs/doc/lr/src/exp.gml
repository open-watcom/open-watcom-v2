.func exp
.synop begin
#include <math.h>
double exp( double x );
.ixfunc2 '&Math' &funcb
.synop end
.desc begin
The
.id &funcb.
function computes the exponential function of
.arg x
.ct .li .
A range error occurs if the magnitude of
.arg x
is too large.
.desc end
.return begin
The
.id &funcb.
function returns the exponential value.
.im errnorng
.return end
.see begin
.seelist exp log matherr
.see end
.exmp begin
#include <stdio.h>
#include <math.h>

void main()
  {
    printf( "%f\n", exp(.5) );
  }
.exmp output
1.648721
.exmp end
.class ISO C
.system
