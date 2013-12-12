.func cos
.synop begin
#include <math.h>
double cos( double x );
.ixfunc2 '&Math' &func
.ixfunc2 '&Trig' &func
.synop end
.desc begin
The &func function computes the cosine of
.arg x
(measured in radians).
A large magnitude argument may yield a result
with little or no significance.
.desc end
.return begin
The &func function returns the cosine value.
.return end
.see begin
.seelist cos acos sin tan
.see end
.exmp begin
#include <math.h>

void main()
  {
    double value;
    value = cos( 3.1415278 );
  }
.exmp end
.class ANSI
.system
