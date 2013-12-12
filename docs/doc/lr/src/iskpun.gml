.func iskpun
.synop begin
#include <jctype.h>
int iskpun( int c );
.ixfunc2 '&CharTest' &func
.synop end
.desc begin
The &func function tests if the argument
.arg c
is a single-byte Katakana punctuation character.
A single-byte Katakana punctuation character is any character for
which the following expression is true:
.millust begin
0xA1 <= c <= 0xA5
.millust end
.desc end
.return begin
The &func function returns zero if the argument is not a single-byte Katakana
punctuation character;
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
      printf( "Char is %sa single-byte "
              "Katakana punctuation character\n",
            ( iskpun( chars[i] ) ) ? "" : "not " );
    }
  }
.exmp output
Char is not a single-byte Katakana punctuation character
Char is not a single-byte Katakana punctuation character
Char is a single-byte Katakana punctuation character
Char is not a single-byte Katakana punctuation character
.exmp end
.class WATCOM
.system
