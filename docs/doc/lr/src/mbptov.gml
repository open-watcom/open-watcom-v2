.func _mbptov _fmbptov
.synop begin
#include <mbstring.h>
unsigned int _mbptov( const unsigned char *string );
.if &farfnc eq 1 .do begin
unsigned int _fmbptov( const unsigned char __far *string );
.do end
.synop end
.desc begin
The
.id &funcb.
function returns the multibyte character at the position in
the string pointed to by
.arg string
.ct .li .
.im farparm
.desc end
.return begin
The
.id &funcb.
function returns a multibyte character or null if
.arg string
points at a null character.
.return end
.see begin
.im seembc
.see end
.exmp begin
#include <stdio.h>
#include <mbctype.h>
#include <mbstring.h>

const unsigned char chars[] = {
    ' ',
    '.',
    '1',
    'A',
    0x81,0x40, /* double-byte space */
    0x82,0x60, /* double-byte A */
    0x82,0xA6, /* double-byte Hiragana */
    0x83,0x42, /* double-byte Katakana */
    0xA1,      /* single-byte Katakana punctuation */
    0xA6,      /* single-byte Katakana alphabetic */
    0xDF,      /* single-byte Katakana alphabetic */
    0xE0,0xA1, /* double-byte Kanji */
    0x00
};

void main()
  {
    _setmbcp( 932 );
    printf( "%#6.4x\n", _mbptov( &chars[2] ) );
    printf( "%#6.4x\n", _mbptov( &chars[4] ) );
    printf( "%#6.4x\n", _mbptov( &chars[12] ) );
  }
.exmp output
0x0031
0x8140
0x00a1
.exmp end
.class WATCOM
.system
