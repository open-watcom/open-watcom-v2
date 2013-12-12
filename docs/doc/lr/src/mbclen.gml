.func _mbclen _fmbclen
#include <mbstring.h>
size_t _mbclen( const unsigned char *ch );
.ixfunc2 '&Multibyte' &func
.if &farfnc eq 1 .do begin
size_t far _fmbclen( const unsigned char __far *ch );
.ixfunc2 '&Multibyte' &ffunc
.do end
.synop end
.desc begin
The &func function determines the number of bytes comprising the
multibyte character pointed to by
.arg ch
.ct .li .
.im farparm
.desc end
.return begin
If
.arg ch
is a NULL pointer, the &func function returns zero if multibyte
character encodings do not have state-dependent encoding, and non-zero
otherwise.
If
.arg ch
is not a NULL pointer, the &func function returns:
.begnote $setptnt 6
.termhd1 Value
.termhd2 Meaning
.note 0
if
.arg ch
points to the null character
.note 1
if
.arg ch
points to a single-byte character
.note 2
if
.arg ch
points to a double-byte character
.note &minus.1
if
.arg ch
does not point to a valid multibyte character
.endnote
.return end
.see begin
.im seembc &function.
.see end
.exmp begin
#include <stdio.h>
#include <mbctype.h>
#include <mbstring.h>

unsigned char chars[] = {
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
    0x00       /* null character */
};
.exmp break
void main()
  {
    int     i, j;

    _setmbcp( 932 );
    for( i = 0; i < sizeof(chars); i += j ) {
        j = _mbclen( &chars[i] );
        printf( "%d bytes in character\n", j );
    }
  }
.exmp output
1 bytes in character
1 bytes in character
1 bytes in character
1 bytes in character
2 bytes in character
2 bytes in character
2 bytes in character
2 bytes in character
1 bytes in character
1 bytes in character
1 bytes in character
2 bytes in character
1 bytes in character
.exmp end
.class WATCOM
.system
