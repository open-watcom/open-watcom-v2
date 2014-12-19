.func isnan
.synop begin
#include <math.h>
int isnan( x );
.synop end
.*
.desc begin
The
.id &funcb.
macro determines whether its argument
.arg x
is a NaN.
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
.id &funcb.
macro returns a nonzero value if and only if its argument has
a NaN value.
.return end
.*
.see begin
.im seefpcls
.see end
.*
.exmp begin
#include <math.h>
#include <stdio.h>
.exmp break
void main( void )
{
    printf( "NAN %s a NaN\n",
        isnan( NAN ) ? "is" : "is not" );
}
.exmp output
NAN is a NaN
.exmp end
.*
.class ISO C
.system
