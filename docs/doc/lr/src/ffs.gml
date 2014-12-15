.func ffs
.synop begin
#include <strings.h>
int ffs( int i );
.ixfunc2 '&String' &funcb
.synop end
.desc begin
The
.id &funcb.
finds the first bit set, beginning with the least significant bit, in
.arg i
.ct .li .
Bits are numbered starting at one (the least significant bit).
.desc end
.return begin
The
.id &funcb.
function returns the index of the first bit set. If
.arg i
is 0,
.id &funcb.
returns zero.
.return end
.see begin
.seelist _lrotl _lrotr _rotl _rotr
.see end
.exmp begin
#include <stdio.h>
#include <strings.h>

int main( void )
{
    printf( "%d\n", ffs( 0 ) );
    printf( "%d\n", ffs( 16 ) );
    printf( "%d\n", ffs( 127 ) );
    printf( "%d\n", ffs( -16 ) );
    return( 0 );
}
.exmp output
0
5
1
5
.exmp end
.class POSIX
.system
