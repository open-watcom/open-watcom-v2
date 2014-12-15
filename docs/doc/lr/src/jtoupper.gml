.func jtoupper
.synop begin
#include <jstring.h>
JMOJI jtoupper( JMOJI c );
.ixfunc2 '&CharTest' &funcb
.synop end
.desc begin
The
.id &funcb.
function converts a double-byte lowercase character to an
equivalent double-byte uppercase character.
A double-byte lowercase character is any character for which the
following expression is true:
.millust begin
0x8281 <= c <= 0x829A
.millust end
.desc end
.return begin
The
.id &funcb.
function returns the argument value if the argument is not a
double-byte lowercase character;
otherwise, the equivalent uppercase character is returned.
.return end
.see begin
.im seejto
.see end
.exmp begin
#include <stdio.h>
#include <jstring.h>

JMOJI chars[] = {
    0x8281,
    0x8282,
    0x8283,
    0x8284,
    0x8285
};
.exmp break
#define SIZE sizeof( chars ) / sizeof( JMOJI )
.exmp break
void main()
  {
    int   i;
    JMOJI c;
.exmp break
    for( i = 0; i < SIZE; i++ ) {
        c = jtoupper( chars[ i ] );
        printf( "%c%c", c>>8, c );
    }
    printf( "\n" );
  }
.exmp output
 A B C D E
.exmp end
.class WATCOM
.system
