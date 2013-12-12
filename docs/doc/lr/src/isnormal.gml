.func isnormal
.synop begin
#include <math.h>
int isnormal( x );
.synop end
.*
.desc begin
The &func macro determines whether its argument value is normal (neither
zero, subnormal, infinite, nor NaN).
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
The &func macro returns a nonzero value if and only if its argument has
a normal value.
.return end
.*
.see begin
.im seefpcls isnormal
.see end
.*
.exmp begin
#include <math.h>
#include <stdio.h>
.exmp break
void main( void )
{
    printf( "zero %s a normal number\n",
        isnormal( 0.0 ) ? "is" : "is not" );
}
.exmp output
zero is not a normal number
.exmp end
.*
.class ANSI
.system
