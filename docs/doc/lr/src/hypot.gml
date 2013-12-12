.func hypot
#include <math.h>
double hypot( double x, double y );
.ixfunc2 '&Math' &func
.ixfunc2 '&Trig' &func
.synop end
.desc begin
The &func function computes the length of the hypotenuse of a right
triangle whose sides are
.arg x
and
.arg y
adjacent to that right angle.
The calculation is equivalent to
.blkcode begin
    sqrt( x*x + y*y )
.blkcode end
.blktext begin
The computation may cause an overflow, in which case the
.kw matherr
function will be invoked.
.blktext end
.desc end
.return begin
The value of the hypotenuse is returned.
.im errnoref
.return end
.exmp begin
#include <stdio.h>
#include <math.h>

void main()
  {
    printf( "%f\n", hypot( 3.0, 4.0 ) );
  }
.exmp output
5.000000
.exmp end
.class WATCOM
.system
