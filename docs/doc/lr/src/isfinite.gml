.func isfinite
.synop begin
#include <math.h>
int isfinite( x );
.synop end
.*
.desc begin
The
.id &func.
macro determines whether its argument
.arg x
has a finite value (zero, subnormal, or normal, and not infinite or NaN).
First, an argument represented in a format wider than its semantic type is
converted to its semantic type. Then determination
is based on the type of the argument.
.np
The argument
.arg x
must be an expression of real floating type.
.desc end
.*
.return begin
The
.id &func.
macro returns a nonzero value if and only if its argument has
a finite value.
.return end
.*
.see begin
.im seefpcls
.seelist _finite
.see end
.*
.exmp begin
#include <math.h>
#include <stdio.h>
.exmp break
void main( void )
{
    printf( "zero %s a finite number\n",
        isfinite( 0.0 ) ? "is" : "is not" );
}
.exmp output
zero is a finite number
.exmp end
.*
.class ANSI
.system
