.func nan
.synop begin
#include <math.h>
float nanf( const char *s );
double nan( const char *s );
long double nanl( const char *s );
.ixfunc2 '&Math' &funcb
.synop end
.desc begin
The
.id &funcb.
function returns not-a-number, or NAN.  The argument
.arg s
is ignored.
.desc end
.return begin
The proper not-a-number value.
.return end
.exmp begin
#include <stdio.h>
#include <math.h>

void main()
  {
    printf( "%f\n", nan("") );
  }
.exmp output
nan
.exmp end
.class ISO C99
.system
