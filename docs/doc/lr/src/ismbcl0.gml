.func _ismbcl0
.synop begin
#include <mbstring.h>
int _ismbcl0( unsigned int ch );
.synop end
.desc begin
The &func function tests if the argument
.arg ch
is in the set of double-byte characters that include Hiragana,
Katakana, punctuation symbols, graphical symbols, Roman and Cyrillic
alphabets, etc.
Double-byte Kanji characters are not in this set.
These are any characters for which the following expression is true:
.millust begin
0x8140 <= ch <= 0x889E  &&  ch != 0x837F
.millust end
.np
The &func function tests if the argument is a valid double-byte
character (i.e., it checks that the lower byte is not in the ranges
0x00 - 0x3F, 0x7F, or 0xFD - 0xFF).
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
The &func function returns a non-zero value when the argument is a
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
    0x889E, /* double-byte L0 character */
    0x889F, /* double-byte L1 character */
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
            "JIS L0 character\n",
            chars[i],
            ( _ismbcl0( chars[i] ) ) ? "" : "not " );
    }
  }
.exmp output
0x0041 is not a valid JIS L0 character
0x8140 is a valid JIS L0 character
0x8143 is a valid JIS L0 character
0x8260 is a valid JIS L0 character
0x829f is a valid JIS L0 character
0x8340 is a valid JIS L0 character
0x837f is not a valid JIS L0 character
0x889e is a valid JIS L0 character
0x889f is not a valid JIS L0 character
0x989f is not a valid JIS L0 character
0x00a6 is not a valid JIS L0 character
.exmp end
.class WATCOM
.system
