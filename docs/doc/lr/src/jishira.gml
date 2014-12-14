.func jishira
.synop begin
#include <jstring.h>
int jishira( JMOJI c );
.ixfunc2 '&CharTest' &func
.synop end
.desc begin
The
.id &func.
function tests if the argument
.arg c
is a double-byte Hiragana character.
A double-byte Hiragana character is any character for which the
following expression is true:
.millust begin
0x829F <= c <= 0x82F1
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
The
.id &func.
function returns zero if the argument is not a double-byte Hiragana
character;
otherwise, a non-zero value is returned.
.return end
.see begin
.im seejto
.see end
.exmp begin
#include <stdio.h>
#include <jstring.h>

JMOJI chars[] = {
    0x829F,
    0x82B0,
    0x82F1
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
        if( jishira( c ) ) {
          printf( " is a hiragana character" );
        }
        printf( "\n" );
    }
  }
.exmp end
.class WATCOM
.system
