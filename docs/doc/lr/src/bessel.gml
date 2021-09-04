.func begin Bessel Functions
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
.ix '&Math' 'Bessel Functions'
Functions
.reffunc j0
.ct ,
.reffunc j1
.ct , and
.reffunc jn
return Bessel Functions of the first kind.
.pp
Functions
.reffunc y0
.ct ,
.reffunc y1
.ct , and
.reffunc yn
return Bessel Functions of the second kind.
The argument
.arg x
must be positive.
If
.arg x
is negative,
.reffunc _matherr
will be called to print a DOMAIN error message to
.kw stderr
.ct , set
.kw errno
to
.kw EDOM
.ct , and return the value
.id -HUGE_VAL
.period
This error handling can be modified by using the
.reffunc matherr
routine.
.desc end
.return begin
These functions return the result of the desired Bessel Function of
.arg x
.period
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
