.func jtolower
.synop begin
#include <jstring.h>
JMOJI jtolower( JMOJI c );
.ixfunc2 '&CharTest' &funcb
.synop end
.desc begin
The
.id &funcb.
function converts a double-byte uppercase character to an
equivalent double-byte lowercase character.
A double-byte uppercase character is any character for which the
following expression is true:
.millust begin
0x8260 <= c <= 0x8279
.millust end
.desc end
.return begin
The
.id &funcb.
function returns the argument value if the argument is not a
double-byte uppercase character;
otherwise, the equivalent lowercase character is returned.
.return end
.see begin
.im seejto
.see end
.exmp begin
#include <stdio.h>
#include <jstring.h>

JMOJI chars[] = {
    0x8260,
    0x8261,
    0x8262,
    0x8263,
    0x8264
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
        c = jtolower( chars[ i ] );
        printf( "%c%c", c>>8, c );
    }
    printf( "\n" );
  }
.exmp output
 a b c d e
.exmp end
.class WATCOM
.system
