.func _mbctohira
.synop begin
#include <mbstring.h>
unsigned int _mbctohira( unsigned int ch );
.ixfunc2 '&CharTest' &funcb
.synop end
.desc begin
The
.id &funcb.
converts a double-byte Katakana character to a Hiragana
character.
A double-byte Katakana character is any character for which
the following expression is true:
.millust begin
0x8340 <= ch <= 0x8396  &&  ch != 0x837F
.millust end
.np
Any Katakana character whose value is less than 0x8393 is converted to
Hiragana (there are 3 extra Katakana characters that have no
equivalent).
.np
.us Note:
The Japanese double-byte character set includes Kanji, Hiragana, and
Katakana characters - both alphabetic and numeric.
Kanji is the ideogram character set of the Japanese character set.
Hiragana and Katakana are two types of phonetic character sets of
the Japanese character set.
The Hiragana code set includes 83 characters and the Katakana code set
includes 86 characters.
.np
.us Note:
This function was called
.kw jtohira
in earlier versions.
.desc end
.return begin
The
.id &funcb.
function returns the argument value if the argument is not a
double-byte Katakana character;
otherwise, the equivalent Hiragana character is returned.
.return end
.see begin
.im seembc
.see end
.exmp begin
#include <stdio.h>
#include <mbctype.h>
#include <mbstring.h>

unsigned int chars[] = {
    0x8340,
    0x8364,
    0x8396
};
.exmp break
#define SIZE sizeof( chars ) / sizeof( unsigned int )

void main()
  {
    int   i;

    _setmbcp( 932 );
    for( i = 0; i < SIZE; i++ ) {
      printf( "%#6.4x - %#6.4x\n",
              chars[ i ],
              _mbctohira( chars[ i ] ) );
    }
  }
.exmp output
0x8340 - 0x829f
0x8364 - 0x82c3
0x8396 - 0x8396
.exmp end
.class WATCOM
.system
