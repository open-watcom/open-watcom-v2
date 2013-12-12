.func _ismbchira
.synop begin
#include <mbstring.h>
int _ismbchira( unsigned int ch );
.synop end
.desc begin
The
.id &func.
function tests for a double-byte Hiragana character.
A double-byte Hiragana character is any character for which the
following expression is true:
.millust begin
0x829F <= ch <= 0x82F1
.millust end
.np
.us Note:
The Japanese double-byte character set includes Kanji, Hiragana, and
Katakana characters - both alphabetic and numeric.
Kanji is the ideogram character set of the Japanese character set.
Hiragana and Katakana are two types of phonetic character sets of the
Japanese character set.
The Hiragana code set includes 83 characters and the Katakana code set
includes 86 characters.
.desc end
.return begin
The
.id &func.
function returns a non-zero value when the argument is a
member of this set of characters; otherwise, zero is returned.
.return end
.see begin
.im seeismbc &function.
.see end
.exmp begin
#include <stdio.h>
#include <mbctype.h>
#include <mbstring.h>
.exmp break
unsigned int chars[] = {
    'A',
    0x8140, /* double-byte space */
    0x8143, /* double-byte , */
    0x8260, /* double-byte A */
    0x829F, /* double-byte Hiragana */
    0x8340, /* double-byte Katakana */
    0x837F, /* illegal double-byte character */
    0x989F, /* double-byte L2 character */
    0xA6    /* single-byte Katakana */
};
.exmp break
#define SIZE sizeof( chars ) / sizeof( unsigned int )
.exmp break
void main()
  {
    int   i;
.exmp break
    _setmbcp( 932 );
    for( i = 0; i < SIZE; i++ ) {
      printf( "%#6.4x is %sa valid "
            "Hiragana character\n",
            chars[i],
            ( _ismbchira( chars[i] ) ) ? "" : "not " );
    }
  }
.exmp output
0x0041 is not a valid Hiragana character
0x8140 is not a valid Hiragana character
0x8143 is not a valid Hiragana character
0x8260 is not a valid Hiragana character
0x829f is a valid Hiragana character
0x8340 is not a valid Hiragana character
0x837f is not a valid Hiragana character
0x989f is not a valid Hiragana character
0x00a6 is not a valid Hiragana character
.exmp end
.class WATCOM
.system
