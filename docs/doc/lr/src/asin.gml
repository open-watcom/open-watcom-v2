.func asin
.synop begin
#include <math.h>
double asin( double x );
.ixfunc2 '&Math' &funcb
.ixfunc2 '&Trig' &funcb
.synop end
.desc begin
The
.id &funcb.
function computes the principal value of the
arcsine of
.arg x
.ct .li .
A domain error occurs for arguments not in the range [&minus.1,1].
.desc end
.return begin
The
.id &funcb.
function returns the arcsine in the range [&minus.&pi./2,&pi./2].
.im errnodom
.return end
.see begin
.seelist acos atan atan2 matherr
.see end
.exmp begin
#include <stdio.h>
#include <math.h>

void main()
  {
    printf( "%f\n", asin(.5) );
  }
.exmp output
0.523599
.exmp end
.class ANSI
.system
