.func _ismbbkpunct
.synop begin
#include <mbctype.h>
int _ismbbkpunct( unsigned int ch );
.synop end
.desc begin
The
.id &func.
function tests if the argument
.arg ch
is a non-ASCII punctuation character.
.np
For example, in code page 932,
.id &func.
tests if the argument
.arg ch
is a single-byte Katakana punctuation character.
A single-byte Katakana punctuation character is any character for
which the following expression is true:
.millust begin
0xA1 <= ch <= 0xA5
.millust end
.np
.us Note:
The argument
.arg ch
must represent a single-byte value (i.e., 0 <=
.arg ch
<= 255 ).
Incorrect results occur if the argument is a double-byte character.
This is shown by the example below.
.desc end
.return begin
The
.id &func.
function returns a non-zero value if the argument satisfies
the condition; otherwise a zero value is returned.
.return end
.see begin
.im seeismbb
.see end
.exmp begin
#include <stdio.h>
#include <mbctype.h>

unsigned int chars[] = {
    ' ',
    '.',
    '1',
    'A',
    0x8140, /* double-byte space */
    0x8260, /* double-byte A */
    0x82A6, /* double-byte Hiragana */
    0x8342, /* double-byte Katakana */
    0xA1,   /* single-byte Katakana punctuation */
    0xA6,   /* single-byte Katakana alphabetic */
    0xDF,   /* single-byte Katakana alphabetic */
    0xE0A1  /* double-byte Kanji */
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
      printf( "%#6.4x is %sa single-byte "
              "Katakana punctuation character\n",
            chars[i],
            ( _ismbbkpunct( chars[i] ) ) ? "" : "not " );
    }
  }
.exmp output
0x0020 is not a single-byte Katakana punctuation character
0x002e is not a single-byte Katakana punctuation character
0x0031 is not a single-byte Katakana punctuation character
0x0041 is not a single-byte Katakana punctuation character
0x8140 is not a single-byte Katakana punctuation character
0x8260 is not a single-byte Katakana punctuation character
0x82a6 is not a single-byte Katakana punctuation character
0x8342 is not a single-byte Katakana punctuation character
0x00a1 is a single-byte Katakana punctuation character
0x00a6 is not a single-byte Katakana punctuation character
0x00df is not a single-byte Katakana punctuation character
0xe0a1 is a single-byte Katakana punctuation character
.exmp end
.class WATCOM
.system
