.func _ismbclegal
#include <mbstring.h>
int _ismbclegal( unsigned int dbch );
.funcend
.desc begin
The &func function tests for a valid multibyte character.
Multibyte characters include both single-byte and double-byte
characters.
For example, in code page 932, a legal double-byte character is one in
which the first byte is within the ranges 0x81 - 0x9F or 0xE0 - 0xFC,
while the second byte is within the ranges 0x40 - 0x7E or 0x80 - 0xFC.
This is summarized in the following diagram.
.millust begin
   [ 1st byte ]    [ 2nd byte ]
    0x81-0x9F       0x40-0xFC
        or          except 0x7F
    0xE0-0xFC
.millust end
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
    0x8131, /* illegal double-byte character */
    0x8140, /* double-byte space */
    0x8143, /* double-byte , */
    0x8260, /* double-byte A */
    0x829F, /* double-byte Hiragana */
    0x8340, /* double-byte Katakana */
    0x837F, /* illegal double-byte character */
    0x889E, /* double-byte L0 character */
    0x889F, /* double-byte L1 character */
    0x989F, /* double-byte L2 character */
    0xEA9E, /* double-byte L2 character */
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
      printf( "%#6.4x is %sa legal "
            "double-byte character\n",
            chars[i],
            ( _ismbclegal( chars[i] ) ) ? "" : "not " );
    }
  }
.exmp output
0x0041 is not a legal double-byte character
0x8131 is not a legal double-byte character
0x8140 is a legal double-byte character
0x8143 is a legal double-byte character
0x8260 is a legal double-byte character
0x829f is a legal double-byte character
0x8340 is a legal double-byte character
0x837f is not a legal double-byte character
0x889e is a legal double-byte character
0x889f is a legal double-byte character
0x989f is a legal double-byte character
0xea9e is a legal double-byte character
0x00a6 is not a legal double-byte character
.exmp end
.class WATCOM
.system
