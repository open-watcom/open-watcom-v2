#define __STDC_WANT_LIB_EXT1__ 1
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

int main()
{
    int             i;
    size_t          retval;
    const wchar_t   *src;
    char            mb[50];
    mbstate_t       pstate;
    errno_t         rc;

    _setmbcp( 932 );
    src = wc;
    rc = wcsrtombs_s( &retval, mb, 50, &src, sizeof(wc), &pstate );
    if( rc != 0 ) {
        printf( "Error in wide character string\n" );
    } else {
        for( i = 0; i < retval; i++ ) {
            printf( "0x%2.2x\n", mb[i] );
        }
    }
    return( rc );
}
