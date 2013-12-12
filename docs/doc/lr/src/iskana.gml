.func iskana
.synop begin
#include <jctype.h>
int iskana( int c );
.ixfunc2 '&CharTest' &func
.synop end
.desc begin
The &func function tests if the argument
.arg c
is a single-byte Katakana character.
A single-byte Katakana character is any character for which the
following expression is true:
.millust begin
0xA1 <= c <= 0xDF
.millust end
.desc end
.return begin
The &func function returns zero if the argument is not a single-byte Katakana
character;
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
    'A',
    0x99A6,
    0xA4,
    0xD0
};
.exmp break
#define SIZE sizeof( chars ) / sizeof( JMOJI )
.exmp break
void main()
  {
    int   i;
.exmp break
    for( i = 0; i < SIZE; i++ ) {
      printf( "Char is %sa Katakana character\n",
            ( iskana( chars[i] ) ) ? "" : "not " );
    }
  }
.exmp output
Char is not a Katakana character
Char is not a Katakana character
Char is a Katakana character
Char is a Katakana character
.exmp end
.class WATCOM
.system
