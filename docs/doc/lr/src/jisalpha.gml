.func jisalpha
#include <jstring.h>
int jisalpha( JMOJI c );
.ixfunc2 '&CharTest' &func
.synop end
.desc begin
The &func function tests if the argument
.arg c
is a double-byte character equivalent to the single-byte
alphabetic characters "a" to "z" and "A" to "Z".
A double-byte alphabetic character is any character for which the
following expression is true:
.millust begin
0x8260 <= c <= 0x8279 or 0x8281 <= c <= 0x829A
.millust end
.desc end
.return begin
The &func function returns zero if the argument is not a double-byte alphabetic
character;
otherwise, a non-zero value is returned.
.return end
.see begin
.im seejto &function.
.see end
.exmp begin
#include <stdio.h>
#include <jstring.h>

JMOJI chars[] = {
    0x8260,
    0x8279,
    0x8281,
    0x829A
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
        c = chars[ i ];
        printf( "%c%c", c>>8, c );
        if( jisalpha( c ) ) {
          printf( " is alphabetic" );
        }
        printf( "\n" );
    }
  }
.exmp output
A  is alphabetic
Z  is alphabetic
a  is alphabetic
z  is alphabetic
.exmp end
.class WATCOM
.system
