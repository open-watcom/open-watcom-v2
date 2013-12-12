.func ldexp
.synop begin
#include <math.h>
double ldexp( double x, int exp );
.ixfunc2 '&Math' &func
.synop end
.desc begin
The &func
function multiplies a floating-point number by an integral power of 2.
A range error may occur.
.desc end
.return begin
The &func function returns the value of
.arg x
times 2 raised to the power
.arg exp
.ct .li .
.return end
.see begin
.seelist ldexp frexp modf
.see end
.exmp begin
#include <stdio.h>
#include <math.h>

void main()
  {
    double value;
.exmp break
    value = ldexp( 4.7072345, 5 );
    printf( "%f\n", value );
  }
.exmp output
150.631504
.exmp end
.class ANSI
.system
