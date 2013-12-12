.func jisprint
#include <jstring.h>
int jisprint( JMOJI c );
.ixfunc2 '&CharTest' &func
.synop end
.desc begin
The &func function tests if the argument
.arg c
is a printable single- or double-byte character.
These are any characters for which the following expression is true:
.millust begin
isprint( c ) || iskana( c ) || jiszen( c )
.millust end
.np
This set of characters includes all printable single-byte ASCII
characters (0x20-0x7E), all single-byte Katakana characters
(0xA1-0xDF), and all valid double-byte characters.
Valid double-byte characters are those in which the first byte falls
in the range 0x81 - 0x9F or 0xE0 - 0xFC and whose second byte falls in
the range 0x40 - 0x7E or 0x80 - 0xFC.
.desc end
.return begin
The &func function returns zero if the argument does not meet one of the above
tests;
otherwise, a non-zero value is returned.
.return end
.see begin
.im seejto &function.
.see end
.exmp begin
#include <stdio.h>
#include <jstring.h>

JMOJI chars[] = {
    0x8143,
    0x8183,
    0x8254,
    0x8298
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
        if( jisprint( c ) ) {
          printf( " is a printable character" );
        }
        printf( "\n" );
    }
  }
.exmp output
,  is a printable character
<  is a printable character
5  is a printable character
x  is a printable character
.exmp end
.class WATCOM
.system
