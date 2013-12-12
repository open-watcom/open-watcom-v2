.func scalbn
.synop begin
#include <math.h>
double scalbn( double x, int y );
.ixfunc2 '&Math' &func
.synop end
.desc begin
The
.id &func.
function computes 
.arg x
*(2**
.arg y
) via exponent manipulation.
.blkcode begin
fmax( x - y, 0.0 );
.blkcode end
.desc end
.return begin
The value of
.arg x
times two raised to 
.arg y
.ct .li .
.return end
.exmp begin
#include <stdio.h>
#include <math.h>

void main()
  {
    printf( "%f\n", scalbn( 1.0, 3.0 ) );
  }
.exmp output
8.000000
.exmp end
.class ISO C99
.system
