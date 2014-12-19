.func isinf
.synop begin
#include <math.h>
int isinf( x );
.synop end
.*
.desc begin
The
.id &funcb.
macro determines whether its argument value is an infinity
(positive or negative).
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
an infinite value.
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
    printf( "zero %s an infinite number\n",
        isinf( 0.0 ) ? "is" : "is not" );
}
.exmp output
zero is not an infinite number
.exmp end
.*
.class ISO C
.system
