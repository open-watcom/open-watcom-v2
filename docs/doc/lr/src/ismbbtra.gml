.func _ismbbtrail
.synop begin
#include <mbstring.h>
int _ismbbtrail( unsigned int ch );
.synop end
.desc begin
The &func function tests if
.arg ch
is a valid second byte of a multibyte character.
.np
For example, in code page 932, valid ranges are 0x40 through 0x7E and
0x80 through 0xFC.
.np
.us Note:
Only the least significant (trailing) byte of the argument
.arg ch
is tested.
If the argument is a double-byte character, the leading byte is
ignored and may be invalid.
This is shown by the example below.
.desc end
.return begin
&func returns a non-zero value if the argument is valid as the second
byte of a multibyte character; otherwise zero is returned.
.return end
.see begin
.im seeismbb &function.
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
      printf( "%#6.4x does %shave a valid second byte\n",
            chars[i],
            ( _ismbbtrail(chars[i]&0xff) ) ? "" : "not " );
    }
  }
.exmp output
0x0020 does not have a valid second byte
0x002e does not have a valid second byte
0x0031 does not have a valid second byte
0x0041 does have a valid second byte
0x8140 does have a valid second byte
0x8260 does have a valid second byte
0x82a6 does have a valid second byte
0x8342 does have a valid second byte
0x00a1 does have a valid second byte
0x00a6 does have a valid second byte
0x00df does have a valid second byte
0xe0a1 does have a valid second byte
.exmp end
.class WATCOM
.system
