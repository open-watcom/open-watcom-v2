.func acos
.synop begin
#include <math.h>
double acos( double x );
.ixfunc2 '&Math' &funcb
.ixfunc2 '&Trig' &funcb
.ixfunc2 '&Trig' &funcb
.ixfunc2 '&Hyper' &funcb
.synop end
.desc begin
The
.id &funcb.
function computes the principal value of the
arccosine of
.arg x
.ct .li .
A domain error occurs for arguments not in the range [&minus.1,1].
.desc end
.return begin
The
.id &funcb.
function returns the arccosine in the range [0,&pi.].
.im errnodom
.return end
.see begin
.seelist asin atan atan2 matherr
.see end
.exmp begin
#include <stdio.h>
#include <math.h>

void main()
  {
    printf( "%f\n", acos(.5) );
  }
.exmp output
1.047197
.exmp end
.class ANSI
.system
