.func jiskana
#include <jstring.h>
int jiskana( JMOJI c );
.ixfunc2 '&CharTest' &func
.synop end
.desc begin
The &func function tests if the argument
.arg c
is a double-byte Katakana character.
A double-byte Katakana character is any character for which the
following expression is true:
.millust begin
0x8340 <= c <= 0x8396  &&  c != 0x837F
.millust end
.np
.us Note:
The Japanese double-byte character set includes Kanji, Hiragana, and
Katakana characters - both alphabetic and numeric.
Kanji is the ideogram character set of the Japanese character set.
Hiragana and Katakana are two types of phonetic character sets of
the Japanese character set.
The Hiragana code set includes 83 characters and the Katakana code set
includes 86 characters.
.desc end
.return begin
The &func function returns zero if the argument is not a double-byte Katakana
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
    0x8340,
    0x8364,
    0x8396
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
        if( jiskana( c ) ) {
          printf( " is a katakana character" );
        }
        printf( "\n" );
    }
  }
.exmp end
.class WATCOM
.system
