.func mbsinit sisinit
.synop begin
#include <wchar.h>
int mbsinit( const mbstate_t *ps );
int sisinit( const mbstate_t *ps );
.ixfunc2 '&Multibyte' &funcb
.synop end
.*
.desc begin
If
.arg ps
is not a null pointer, the
.id &funcb.
function determines whether
the pointed-to
.kw mbstate_t
object describes an initial conversion state.
.desc end
.*
.return begin
The
.id &funcb.
function returns nonzero if
.arg ps
is a null pointer or if the pointed-to object describes an initial
conversion state; otherwise, it returns zero.
.return end
.*
.see begin
.im seembc
.see end
.*
.exmp begin
#include <stdio.h>
#include <wchar.h>
#include <mbctype.h>
#include <errno.h>


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

void main( void )
{
    int         i, j, k;
    wchar_t     pwc;
    mbstate_t   pstate = { 0 };

    _setmbcp( 932 );
    j = 1;
    for( i = 0; j > 0; i += j ) {
      printf( "We are %sin an initial conversion state\n",
              mbsinit( &pstate ) ? "not " : "" );
      j = mbrtowc( &pwc, &chars[i], MB_CUR_MAX, &pstate );
      printf( "%d bytes in character ", j );
      if( errno == EILSEQ ) {
        printf( " - illegal multibyte character\n" );
      } else {
        if( j == 0 ) {
          k = 0;
        } else if ( j == 1 ) {
          k = chars[i];
        } else if( j == 2 ) {
          k = chars[i]<<8 | chars[i+1];
        }
        printf( "(%#6.4x->%#6.4x)\n", k, pwc );
      }
    }
}
.exmp output
We are in an initial conversion state
1 bytes in character (0x0020->0x0020)
We are in an initial conversion state
1 bytes in character (0x002e->0x002e)
We are in an initial conversion state
1 bytes in character (0x0031->0x0031)
We are in an initial conversion state
1 bytes in character (0x0041->0x0041)
We are in an initial conversion state
2 bytes in character (0x8140->0x3000)
We are in an initial conversion state
2 bytes in character (0x8260->0xff21)
We are in an initial conversion state
2 bytes in character (0x82a6->0x3048)
We are in an initial conversion state
2 bytes in character (0x8342->0x30a3)
We are in an initial conversion state
1 bytes in character (0x00a1->0xff61)
We are in an initial conversion state
1 bytes in character (0x00a6->0xff66)
We are in an initial conversion state
1 bytes in character (0x00df->0xff9f)
We are in an initial conversion state
2 bytes in character (0xe0a1->0x720d)
We are in an initial conversion state
0 bytes in character (  0000->  0000)
.exmp end
.class ANSI
.system
