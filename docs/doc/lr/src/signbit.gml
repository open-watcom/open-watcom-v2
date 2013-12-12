.func signbit
.synop begin
#include <math.h>
int signbit( x );
.synop end
.*
.desc begin
The
.id &func.
macro determines whether the sign of its argument value is negative.
.np
The argument
.arg x
must be an expression of real floating type.
.desc end
.*
.return begin
The
.id &func.
macro returns a nonzero value if and only if the sign of its
argument has value is negative.
.return end
.*
.see begin
.im seefpcls signbit
.see end
.*
.exmp begin
#include <math.h>
#include <stdio.h>
.exmp break
void main( void )
{
    printf( "-4.5 %s negative\n",
        signbit( -4.5 ) ? "is" : "is not" );
}
.exmp output
-4.5 is negative
.exmp end
.*
.class ANSI
.system
