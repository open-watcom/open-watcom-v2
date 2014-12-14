.func _mbislead
.synop begin
#include <mbctype.h>
int _mbislead( unsigned int ch );
.ixfunc2 '&Multibyte' &func
.synop end
.desc begin
The
.id &func.
function tests if the argument
.arg ch
is a valid first byte of a multibyte character.
.np
For example, in code page 932, valid ranges are 0x81 through 0x9F and
0xE0 through 0xFC.
.np
.us Note:
The argument
.arg ch
must represent a single-byte value (i.e., 0 <=
.arg ch
<= 255 ).
Incorrect results occur if the argument is a double-byte character.
.desc end
.return begin
.id &func.
returns a non-zero value if the argument is valid as the first
byte of a multibyte character; otherwise zero is returned.
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

#define SIZE sizeof( chars ) / sizeof( unsigned int )

void main()
  {
    int   i;

    _setmbcp( 932 );
    for( i = 0; i < SIZE; i++ ) {
      printf( "%#6.4x does %shave a valid first byte\n",
            chars[i],
            ( _mbislead( chars[i]>>8 ) ) ? "" : "not " );
    }
  }
.exmp output
0x0020 does not have a valid first byte
0x002e does not have a valid first byte
0x0031 does not have a valid first byte
0x0041 does not have a valid first byte
0x8140 does have a valid first byte
0x8260 does have a valid first byte
0x82a6 does have a valid first byte
0x8342 does have a valid first byte
0x00a1 does not have a valid first byte
0x00a6 does not have a valid first byte
0x00df does not have a valid first byte
0xe0a1 does have a valid first byte
.exmp end
.class WATCOM
.system
