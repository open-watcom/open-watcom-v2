.func mblen _fmblen
#include <stdlib.h>
    or
#include <mbstring.h>
int mblen( const char *s, size_t n );
.ixfunc2 '&Multibyte' &func
.if &farfnc eq 1 .do begin
int _fmblen( const char __far *s, size_t n );
.ixfunc2 '&Multibyte' &ffunc
.do end
.funcend
.desc begin
The &func function determines the number of bytes comprising the
multibyte character pointed to by
.arg s
.ct .li .
At most
.arg n
bytes of the array pointed to by
.arg s
will be examined.
.im farfunc
.desc end
.return begin
If
.arg s
is a NULL pointer, the &func function returns zero if multibyte
character encodings are not state dependent, and non-zero otherwise.
If
.arg s
is not a NULL pointer, the &func function returns:
.begnote $setptnt 6
.termhd1 Value
.termhd2 Meaning
.note 0
if
.arg s
points to the null character
.note len
the number of bytes that comprise the multibyte character (if the next
.arg n
or fewer bytes form a valid multibyte character)
.note &minus.1
if the next
.arg n
bytes do not form a valid multibyte character
.endnote
.return end
.see begin
.im seembc &function.
.see end
.exmp begin
#include <stdio.h>
#include <mbstring.h>


const char chars[] = {
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
    int         i, j, k;

    _setmbcp( 932 );
    printf( "Character encodings are %sstate dependent\n",
            ( mblen( NULL, MB_CUR_MAX ) ) ? "" : "not " );
    j = 1;
    for( i = 0; j > 0; i += j ) {
      j = mblen( &chars[i], MB_CUR_MAX );
      printf( "%d bytes in character ", j );
      if( j == 0 ) {
        k = 0;
      } else if ( j == 1 ) {
        k = chars[i];
      } else if( j == 2 ) {
        k = chars[i]<<8 | chars[i+1];
      }
      printf( "(%#6.4x)\n", k );
    }
  }
.exmp output
Character encodings are not state dependent
1 bytes in character (0x0020)
1 bytes in character (0x002e)
1 bytes in character (0x0031)
1 bytes in character (0x0041)
2 bytes in character (0x8140)
2 bytes in character (0x8260)
2 bytes in character (0x82a6)
2 bytes in character (0x8342)
1 bytes in character (0x00a1)
1 bytes in character (0x00a6)
1 bytes in character (0x00df)
2 bytes in character (0xe0a1)
0 bytes in character (  0000)
.exmp end
.class ANSI
.system
