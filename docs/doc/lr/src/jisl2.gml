.func jisl2
.synop begin
#include <jstring.h>
int jisl2( JMOJI c );
.ixfunc2 '&CharTest' &func
.synop end
.desc begin
The &func function tests if the argument
.arg c
is a JIS (Japan Industrial Standard) level 2 double-byte character
code.
These are any characters for which the following expression is true:
.millust begin
0x989F <= c <= 0xEA9E
.millust end
.np
.us Note:
JIS establishes two levels of the Kanji double-byte character set.
One is called double-byte Kanji code set level 1 and the other is
called double-byte Kanji code set level 2.
Usually Japanese personal computers have font ROM/RAM support for both
levels.
.np
Valid double-byte characters are those in which the first byte falls
in the range 0x81 - 0x9F or 0xE0 - 0xFC and whose second byte falls in
the range 0x40 - 0x7E or 0x80 - 0xFC.
.desc end
.return begin
The &func function returns zero if the argument is not in the range;
otherwise, a non-zero value is returned.
.return end
.see begin
.im seejto &function.
.see end
.exmp begin
#include <stdio.h>
#include <jstring.h>

JMOJI chars[] = {
    0x8183,
    0x889F,
    0x989F,
    0xEA9E
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
        if( jisl2( c ) ) {
          printf( " is Level 2 JIS" );
        }
        printf( "\n" );
    }
  }
.exmp end
.class WATCOM
.system
