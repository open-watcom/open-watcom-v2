.func iskanji
#include <jctype.h>
int iskanji( int fb );
.ixfunc2 '&CharTest' &func
.synop end
.desc begin
The &func function tests if the argument
.arg fb
is valid as the first byte of a double-byte Kanji character.
A valid first byte is any 8-bit value for which the following
expression is true:
.millust begin
( 0x81 <= fb <= 0x9F ) || ( 0xE0 <= fb <= 0xFC )
.millust end
.desc end
.return begin
The &func function returns zero if the argument is not valid for the first byte
of a double-byte Kanji character;
otherwise, a non-zero value is returned.
.return end
.see begin
.im seejis &function.
.see end
.exmp begin
#include <stdio.h>
#include <jstring.h>
#include <jctype.h>

JMOJI chars[] = {
    '.',
    0x9941,
    0xA4,
    0xA6
};
.exmp break
#define SIZE sizeof( chars ) / sizeof( JMOJI )
.exmp break
void main()
  {
    int   i;
.exmp break
    for( i = 0; i < SIZE; i++ ) {
      printf( "Char does %shave a valid first byte\n",
            ( iskanji( chars[i]>>8 ) ) ? "" : "not " );
    }
  }
.exmp output
Char does not have a valid first byte
Char does have a valid first byte
Char does not have a valid first byte
Char does not have a valid first byte
.exmp end
.class WATCOM
.system
