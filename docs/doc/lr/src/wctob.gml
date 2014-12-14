.func wctob
.synop begin
#include <wchar.h>
int wctob( wint_t wc );
.ixfunc2 '&Wide' &func
.ixfunc2 '&Multibyte' &func
.synop end
.desc begin
The
.id &func.
function determines whether
.arg wc
corresponds to a member of the extended character set whose multibyte
character representation is as a single byte when in the initial shift
state.
.desc end
.return begin
The
.id &func.
function returns
.mono EOF
if
.arg wc
does not correspond to a multibyte character with length one;
otherwise, it returns the single byte representation.
.return end
.see begin
.im seembc
.see end
.exmp begin
#include <stdio.h>
#include <wchar.h>
#include <mbctype.h>

const wint_t wc[] = {
    0x0020,
    0x002e,
    0x0031,
    0x0041,
    0x3000,     /* double-byte space */
    0xff21,     /* double-byte A */
    0x3048,     /* double-byte Hiragana */
    0x30a3,     /* double-byte Katakana */
    0xff61,     /* single-byte Katakana punctuation */
    0xff66,     /* single-byte Katakana alphabetic */
    0xff9f,     /* single-byte Katakana alphabetic */
    0x720d,     /* double-byte Kanji */
    0x0000
};

#define SIZE sizeof( wc ) / sizeof( wchar_t )

void main()
{
    int         i, j;

    _setmbcp( 932 );
    for( i = 0; i < SIZE; i++ ) {
      j = wctob( wc[i] );
      if( j == EOF ) {
        printf( "%#6.4x EOF\n", wc[i] );
      } else {
        printf( "%#6.4x->%#6.4x\n", wc[i], j );
      }
    }
}
.exmp output
0x0020->0x0020
0x002e->0x002e
0x0031->0x0031
0x0041->0x0041
0x3000 EOF
0xff21 EOF
0x3048 EOF
0x30a3 EOF
0xff61->0x00a1
0xff66->0x00a6
0xff9f->0x00df
0x720d EOF
  0000->0x0000
.exmp end
.class ANSI
.system
