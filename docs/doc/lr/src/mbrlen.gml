.func mbrlen _fmbrlen
.synop begin
#include <wchar.h>
size_t mbrlen( const char *s, size_t n, mbstate_t *ps );
.if &farfnc ne 0 .do begin
size_t _fmbrlen( const char __far *s, size_t n, mbstate_t __far *ps );
.do end
.synop end
.desc begin
The
.id &funcb.
function determines the number of bytes comprising the
multibyte character pointed to by
.arg s
.period
The
.id &funcb.
function is equivalent to the following call:
.millust begin
mbrtowc(NULL, s, n, ps != NULL ? ps : &internal)
.millust end
where
.mono &internal
is the address of the internal
.kw mbstate_t
object for the
.id &funcb.
function.
.farfuncp &ffunc. &funcb.
.im mbcrstrt
.desc end
.return begin
.im _mbsret2
.return end
.see begin
.im seembc
.see end
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
.exmp break
void main()
  {
    int         i, j, k;

    _setmbcp( 932 );
    j = 1;
    for( i = 0; j > 0; i += j ) {
      j = mbrlen( &chars[i], MB_CUR_MAX, NULL );
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
        printf( "(%#6.4x)\n", k );
      }
    }
  }
.exmp output
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
.class ISO C95
.system
