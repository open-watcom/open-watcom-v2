.func _ismbcalpha
#include <mbstring.h>
int _ismbcalpha( unsigned int ch );
.synop end
.desc begin
The &func function tests if the multibyte character argument
.arg ch
is an alphabetic character.
For example, in code page 932, 'A' through 'Z' or 'a' through 'z' and
its corresponding double-byte versions and the Katakana letters (0xA6
through 0xDF) are alphabetic.
.desc end
.return begin
The &func function returns zero if the argument is not an alphabetic
character; otherwise, a non-zero value is returned.
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
    '.',
    '1',
    'A',
    0x8143, /* double-byte , */
    0x8254, /* double-byte 5 */
    0x8260, /* double-byte A */
    0x8279, /* double-byte Z */
    0x8281, /* double-byte a */
    0x829A, /* double-byte z */
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
            "multibyte alphabetic character\n",
            chars[i],
            ( _ismbcalpha( chars[i] ) ) ? "" : "not " );
    }
  }
.exmp output
0x002e is not a valid multibyte alphabetic character
0x0031 is not a valid multibyte alphabetic character
0x0041 is a valid multibyte alphabetic character
0x8143 is not a valid multibyte alphabetic character
0x8254 is not a valid multibyte alphabetic character
0x8260 is a valid multibyte alphabetic character
0x8279 is a valid multibyte alphabetic character
0x8281 is a valid multibyte alphabetic character
0x829a is a valid multibyte alphabetic character
0x829f is a valid multibyte alphabetic character
0x8340 is a valid multibyte alphabetic character
0x837f is not a valid multibyte alphabetic character
0x889e is not a valid multibyte alphabetic character
0x889f is a valid multibyte alphabetic character
0x989f is a valid multibyte alphabetic character
0x00a6 is a valid multibyte alphabetic character
.exmp end
.class WATCOM
.system
