.func fesetround
.synop begin
#include <fenv.h>
int fesetround( int mode );
.ixfunc2 'Floating Point Environment' &funcb
.synop end
.desc begin
The
.id &funcb.
function sets the rounding direction mode, specified by
.arg mode
.ct .li , for the current floating point environment.
.np
The rounding direction mode can be one of the following values:
.begnote $setptnt 15
.note FE_TONEAREST
.ix FE_TONEAREST
Round to nearest integer, halfway rounding away from zero
.note FE_DOWNWARD
.ix FE_DOWNWARD
Round downward to the next lowest integer
.note FE_TOWARDZERO
.ix FE_TOWARDZERO
Round to the nearest integer in the direction of zero
.note FE_UPWARD
.ix FE_UPWARD
Round upward to the next highest integer
.endnote
.desc end
.*
.return begin
The
.id &funcb.
function returns a zero value if and only if the requested rounding direction was
established.
.return end
.see begin
.seelist fegetround nearbyint rint
.see end
.exmp begin
#include <stdio.h>
#include <math.h>

void main()
  {
    fesetround(FE_DOWNWARD)
    printf( "%f\n", rint( 1.5 ) );
    fesetround(FE_UPWARD)
    printf( "%f\n", rint( 1.5 ) );
  }
.exmp output
1.00000
2.00000
.exmp end
.class ISO C99
.system
