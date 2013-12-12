.func acos
.synop begin
#include <math.h>
double acos( double x );
.ixfunc2 '&Math' &func
.ixfunc2 '&Trig' &func
.ixfunc2 '&Trig' &func
.ixfunc2 '&Hyper' &func
.synop end
.desc begin
The &func function computes the principal value of the
arccosine of
.arg x
.ct .li .
A domain error occurs for arguments not in the range [&minus.1,1].
.desc end
.return begin
The &func function returns the arccosine in the range [0,&pi.].
.im errnodom
.return end
.see begin
.seelist acos asin atan atan2 matherr
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
