.func fabs
.synop begin
#include <math.h>
double fabs( double x );
.ixfunc2 '&Math' &funcb
.synop end
.desc begin
The
.id &funcb.
function computes the absolute value of the argument
.arg x
.ct .li .
.desc end
.return begin
The
.id &funcb.
function returns the absolute value of
.arg x
.ct .li .
.return end
.see begin
.seelist fabs abs labs imaxabs
.see end
.exmp begin
#include <stdio.h>
#include <math.h>

void main()
{
    printf( "%f %f\n", fabs(.5), fabs(-.5) );
}
.exmp output
0.500000 0.500000
.exmp end
.class ISO C
.system
