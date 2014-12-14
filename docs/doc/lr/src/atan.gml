.func atan
.synop begin
#include <math.h>
double atan( double x );
.ixfunc2 '&Math' &func
.ixfunc2 '&Trig' &func
.ixfunc2 '&Trig' &func
.ixfunc2 '&Hyper' &func
.synop end
.desc begin
The
.id &func.
function computes the principal value of the
arctangent of
.arg x
.ct .li .
.desc end
.return begin
The
.id &func.
function returns the arctangent in the range (&minus.&pi./2,&pi./2).
.return end
.see begin
.seelist acos asin atan2
.see end
.exmp begin
#include <stdio.h>
#include <math.h>

void main()
  {
    printf( "%f\n", atan(.5) );
  }
.exmp output
0.463648
.exmp end
.class ANSI
.system
