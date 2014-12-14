.func jisl1
.synop begin
#include <jstring.h>
int jisl1( JMOJI c );
.ixfunc2 '&CharTest' &func
.synop end
.desc begin
The
.id &func.
function tests if the argument
.arg c
is a JIS (Japan Industrial Standard) level 1 double-byte character
code.
These are any valid double-byte characters for which the following
expression is true:
.millust begin
0x889F <= c <= 0x9872
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
The
.id &func.
function returns zero if the argument is not in the range;
otherwise, a non-zero value is returned.
.return end
.see begin
.im seejto
.see end
.exmp begin
#include <stdio.h>
#include <jstring.h>

JMOJI chars[] = {
    0x8183,
    0x889F,
    0x9872
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
        if( jisl1( c ) ) {
          printf( " is Level 1 JIS" );
        }
        printf( "\n" );
    }
  }
.exmp end
.class WATCOM
.system
