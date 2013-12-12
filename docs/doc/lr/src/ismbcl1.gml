.func _ismbcl1
#include <mbstring.h>
int _ismbcl1( unsigned int ch );
.synop end
.desc begin
The &func function tests if the argument
.arg ch
is a JIS (Japan Industrial Standard) level 1 double-byte character
code.
These are any valid double-byte characters for which the following
expression is true:
.millust begin
0x889F <= ch <= 0x9872
.millust end
.np
The &func function tests if the argument is a valid double-byte
character (i.e., it checks that the lower byte is not in the ranges
0x00 - 0x3F, 0x7F, or 0xFD - 0xFF).
.np
.us Note:
JIS establishes two levels of the Kanji double-byte character set.
One is called double-byte Kanji code set level 1 and the other is
called double-byte Kanji code set level 2.
Usually Japanese personal computers have font ROM/RAM support for both
levels.
.np
Valid double-byte characters are those in which the first byte falls
in the range 0x81 - 0x9F or 0xE0 - 0xFC and whose second byte falls in
the range 0x40 - 0x7E or 0x80 - 0xFC.
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
            "JIS L1 character\n",
            chars[i],
            ( _ismbcl1( chars[i] ) ) ? "" : "not " );
    }
  }
.exmp output
0x0041 is not a valid JIS L1 character
0x8140 is not a valid JIS L1 character
0x8143 is not a valid JIS L1 character
0x8260 is not a valid JIS L1 character
0x829f is not a valid JIS L1 character
0x8340 is not a valid JIS L1 character
0x837f is not a valid JIS L1 character
0x889e is not a valid JIS L1 character
0x889f is a valid JIS L1 character
0x989f is not a valid JIS L1 character
0x00a6 is not a valid JIS L1 character
.exmp end
.class WATCOM
.system
