#define __STDC_WANT_LIB_EXT1__  1
#include <stdio.h>
#include <wchar.h>
#include <mbctype.h>
#include <errno.h>

const wchar_t wc[] = {
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

int main()
{
    int         i, j, k;
    char        s[2];
    errno_t     rc;
    size_t      retval;
    mbstate_t   state;

    _setmbcp( 932 );
    j = 1;
    for( i = 0; i < SIZE; i++ ) {
        rc = wcrtomb_s( &retval, s, 2, wc[i], &state );
        if( rc != 0 ) {
          printf( " - illegal wide character\n" );
        } else {
          printf( "%d bytes in character ", retval );
          if ( retval == 0 ) {
              k = 0;
          } else if ( retval == 1 ) {
              k = s[0];
          } else if( retval == 2 ) {
              k = s[0]<<8 | s[1];
          }
          printf( "(%#6.4x->%#6.4x)\n", wc[i], k );
        }
    }
    return( 0 );
}
