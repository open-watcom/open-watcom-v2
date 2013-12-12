.func ceil
#include <math.h>
double ceil( double x );
.ixfunc2 '&Math' &func
.synop end
.desc begin
The &func function (ceiling function)
computes the smallest integer not less than
.arg x
.ct .li .
.desc end
.return begin
The &func function returns the smallest integer not less than
.arg x
.ct , expressed as a
.id double.
.return end
.see begin
.seelist ceil floor
.see end
.exmp begin
#include <stdio.h>
#include <math.h>

void main()
  {
    printf( "%f %f %f %f %f\n", ceil( -2.1 ), ceil( -2. ),
        ceil( 0.0 ), ceil( 2. ), ceil( 2.1 ) );
  }
.exmp output
-2.000000 -2.000000 0.000000 2.000000 3.000000
.exmp end
.class ANSI
.system
