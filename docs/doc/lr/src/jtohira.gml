.func jtohira
#include <jstring.h>
JMOJI jtohira( JMOJI c );
.ixfunc2 '&CharTest' &func
.synop end
.desc begin
The &func converts a double-byte Katakana character to a Hiragana
character.
A double-byte Katakana character is any character for which
the following expression is true:
.millust begin
0x8340 <= c <= 0x8396  &&  c != 0x837F
.millust end
.np
Any Katakana character whose less value is less than 0x8393 is
converted to Hiragana (there are 3 extra Katakana characters
that have no equivalent).
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
The &func function returns the argument value if the argument is not a
double-byte Katakana character;
otherwise, the equivalent Hiragana character is returned.
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
    JMOJI c1, c2;
.exmp break
    for( i = 0; i < SIZE; i++ ) {
        c1 = chars[ i ];
        c2 = jtohira( c1 );
        printf( "%c%c - %c%c\n", c1>>8, c1, c2>>8, c2 );
    }
  }
.exmp end
.class WATCOM
.system
