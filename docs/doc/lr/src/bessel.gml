.func begin bessel Functions
.func2 j0
.func2 j1
.func2 jn
.func2 y0
.func2 y1
.func2 yn
.func end
.synop begin
#include <math.h>
double j0( double x );
double j1( double x );
double jn( int n, double x );
double y0( double x );
double y1( double x );
double yn( int n, double x );
.ixfunc2 '&Math' j0
.ixfunc2 '&Math' j1
.ixfunc2 '&Math' jn
.ixfunc2 '&Math' y0
.ixfunc2 '&Math' y1
.ixfunc2 '&Math' yn
.synop end
.desc begin
.ix '&Math' 'bessel Functions'
Functions
.kw j0
.ct,
.kw j1
.ct,
and
.kw jn
return Bessel functions of the first kind.
.pp
Functions
.kw y0
.ct,
.kw y1
.ct,
and
.kw yn
return Bessel functions of the second kind.
The argument
.arg x
must be positive.
If
.arg x
is negative,
.kw _matherr
will be called to print a DOMAIN error message to
.kw stderr
.ct,
set
.kw errno
to
.kw EDOM
.ct,
and return the value
.id -HUGE_VAL.
This error handling can be modified by using the
.kw matherr
routine.
.desc end
.return begin
These functions return the result of the desired Bessel function of
.arg x
.ct .li .
.return end
.see begin
.seelist matherr
.see end
.exmp begin
#include <stdio.h>
#include <math.h>

void main()
  {
     double x, y, z;
.exmp break
     x = j0( 2.4 );
     y = y1( 1.58 );
     z = jn( 3, 2.4 );
     printf( "j0(2.4) = %f, y1(1.58) = %f\n", x, y );
     printf( "jn(3,2.4) = %f\n", z );
  }
.exmp end
.class WATCOM
.system
