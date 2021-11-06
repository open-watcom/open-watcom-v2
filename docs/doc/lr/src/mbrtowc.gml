.func mbrtowc _fmbrtowc
.synop begin
#include <wchar.h>
size_t mbrtowc( wchar_t *pwc, const char *s,
             size_t n, mbstate_t *ps );
.if &farfnc ne 0 .do begin
size_t _fmbrtowc( wchar_t __far *pwc, const char __far *s,
               size_t n, mbstate_t __far *ps );
.do end
.synop end
.desc begin
If
.arg s
is not a null pointer, the
.id &funcb.
function determines the number of
bytes that are contained in the multibyte character (plus any leading
shift sequences) pointed to by
.arg s
.ct , produces the value of the corresponding wide character and then, if
.arg pwc
is not a null pointer, stores that value in the object pointed to by
.arg pwc
.period
If the corresponding wide character is the null wide character, the
resulting state described will be the initial conversion state.
.millust begin
.millust end
If
.arg s
is a null pointer, the
.id &funcb.
function is equivalent to call
.millust begin
mbrtowc(NULL, "", 1, ps)
.millust end
In this case, the values of the parameters
.arg pwc
and
.arg n
are ignored.
Function determines the number of bytes necessary to enter
the initial shift state (zero if encodings are not state-dependent or
if the initial conversion state is described).
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
    wchar_t     pwc;

    _setmbcp( 932 );
    i = mbrtowc( NULL, NULL, MB_CUR_MAX, NULL );
    printf( "Number of bytes to enter "
            "initial shift state = %d\n", i );
    j = 1;
    for( i = 0; j > 0; i += j ) {
      j = mbrtowc( &pwc, &chars[i], MB_CUR_MAX, NULL );
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
Number of bytes to enter initial shift state = 0
1 bytes in character (0x0020->0x0020)
1 bytes in character (0x002e->0x002e)
1 bytes in character (0x0031->0x0031)
1 bytes in character (0x0041->0x0041)
2 bytes in character (0x8140->0x3000)
2 bytes in character (0x8260->0xff21)
2 bytes in character (0x82a6->0x3048)
2 bytes in character (0x8342->0x30a3)
1 bytes in character (0x00a1->0xff61)
1 bytes in character (0x00a6->0xff66)
1 bytes in character (0x00df->0xff9f)
2 bytes in character (0xe0a1->0x720d)
0 bytes in character (  0000->  0000)
.exmp end
.class ISO C95
.system
