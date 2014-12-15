.func fpclassify
.synop begin
#include <math.h>
int fpclassify( x );
.synop end
.*
.desc begin
The
.id &funcb.
macro classifies its argument
.arg x
as NaN, infinite, normal, subnormal, or zero.
First, an argument represented in a format wider than its semantic
type is converted to its semantic type. Then classification is based on the
type of the argument.
.np
The argument
.arg x
must be an expression of real floating type.
.np
The possible return values of
.id &funcb.
and their meanings are listed below.
.begterm 20
.termhd1 Constant
.termhd2 Meaning
.*
.term FP_INFINITE
positive or negative infinity
.*
.term FP_NAN
NaN (not-a-number)
.*
.term FP_NORMAL
normal number (neither zero, subnormal, NaN, nor infinity)
.*
.term FP_SUBNORMAL
subnormal number
.*
.term FP_ZERO
positive or negative zero
.endterm
.desc end
.*
.return begin
The
.id &funcb.
macro returns the value of the number classification macro
appropriate to the value of its argument
.arg x
.ct .li .
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
    printf( "infinity %s a normal number\n",
        fpclassify( INFINITY ) == FP_NORMAL ? 
	"is" : "is not" );
}
.exmp output
infinity is not a normal number
.exmp end
.*
.class ANSI
.system
