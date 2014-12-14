.func _mbctokata
.synop begin
#include <mbstring.h>
unsigned int _mbctokata( unsigned int ch );
.ixfunc2 '&CharTest' &func
.synop end
.desc begin
The
.id &func.
converts a double-byte Hiragana character to a Katakana
character.
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
.np
.us Note:
This function was called
.kw jtokata
in earlier versions.
.desc end
.return begin
The
.id &func.
function returns the argument value if the argument is not a
double-byte Hiragana character;
otherwise, the equivalent Katakana character is returned.
.return end
.see begin
.im seembc
.see end
.exmp begin
#include <stdio.h>
#include <mbctype.h>
#include <mbstring.h>

unsigned int chars[] = {
    0x829F,
    0x82B0,
    0x82F1
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
              _mbctokata( chars[ i ] ) );
    }
  }
.exmp output
0x829f - 0x8340
0x82b0 - 0x8351
0x82f1 - 0x8393
.exmp end
.class WATCOM
.system
