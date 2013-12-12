.func iskanji2
.synop begin
#include <jctype.h>
int iskanji2( int sb );
.ixfunc2 '&CharTest' &func
.synop end
.desc begin
The
.id &func.
function tests if the argument
.arg sb
is valid for the second byte of a double-byte Kanji character.
A valid second byte is any 8-bit value for which the following
expression is true:
.millust begin
( 0x40 <= sb <= 0x7E ) || ( 0x80 <= sb <= 0xFC )
.millust end
.desc end
.return begin
The
.id &func.
function returns zero if the argument is not valid for the second
byte of a double-byte Kanji character;
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
      printf( "Char does %shave a valid second byte\n",
            ( iskanji2( chars[i] ) ) ? "" : "not " );
    }
  }
.exmp output
Char does not have a valid second byte
Char does have a valid second byte
Char does have a valid second byte
Char does have a valid second byte
.exmp end
.class WATCOM
.system
