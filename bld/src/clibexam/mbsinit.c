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
