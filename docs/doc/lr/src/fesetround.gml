.func fesetround
#include <math.h>
void fesetround( int mode );
.ixfunc2 '&Math' &func
.funcend
.desc begin
The &func function sets the current rounding mode, specified by
.arg x
.ct .li , for the current process.  The rounding mode can be one
of the following constants:
.begnote
.termhd1 Argument
.termhd2 Meaning
.note FE_TONEAREST
Round to nearest integer, halfway rounding away from zero
.note FE_DOWNWARD
Round downward to the next lowest integer
.note FE_TOWARDZERO
Round to the nearest integer in the direction of zero
.note FE_UPWARD
Round upward to the next highest integer
.desc end
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
