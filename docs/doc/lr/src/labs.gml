.func labs
.synop begin
#include <stdlib.h>
long int labs( long int j );
.synop end
.*
.desc begin
The
.id &func.
function returns the absolute value
of its long-integer argument
.arg j
.ct .li .
.desc end
.*
.return begin
The
.id &func.
function returns the absolute value of its argument.
.return end
.*
.see begin
.seelist labs abs llabs imaxabs fabs
.see end
.*
.exmp begin
#include <stdio.h>
#include <stdlib.h>

void main( void )
{
    long x, y;
.exmp break
    x = -50000L;
    y = labs( x );
    printf( "labs(%ld) = %ld\n", x, y );
}
.exmp output
labs(-50000) = 50000
.exmp end
.*
.class ISO C90
.system
