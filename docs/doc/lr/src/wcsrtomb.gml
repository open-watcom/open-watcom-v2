.func wcsrtombs _fwcsrtombs
.synop begin
#include <wchar.h>
size_t wcsrtombs( char *dst,
         const wchar_t **src,
         size_t n, mbstate_t *ps );
.ixfunc2 '&Wide' &func
.ixfunc2 '&Multibyte' &func
.if &farfnc eq 1 .do begin
#include <wchar.h>
size_t _fwcsrtombs( char __far *dst,
        const wchar_t __far * __far *src,
        size_t n, mbstate_t __far *ps );
.ixfunc2 '&Wide' &ffunc
.ixfunc2 '&Multibyte' &ffunc
.do end
.synop end
.*
.safealt
.*
.desc begin
The &func function converts a sequence of wide characters from the
array indirectly pointed to by
.arg src
into a sequence of corresponding multibyte characters that begins
in the shift state described by
.arg ps
.ct , which, if
.arg dst
is not a null pointer, are then stored into the array pointed to by
.arg dst
.ct .li .
Conversion continues up to and including a terminating null wide
character, but the terminating null character (byte) will not be
stored.
Conversion will stop earlier in two cases: when a code is reached that
does not correspond to a valid multibyte character, or (if
.arg dst
is not a null pointer) when the next multibyte character would exceed
the limit of
.arg len
total bytes to be stored into the array pointed to by
.arg dst
.ct .li .
Each conversion takes place as if by a call to the
.kw wcrtomb
function.
.np
If
.arg dst
is not a null pointer, the pointer object pointed to by
.arg src
will be assigned either a null pointer (if conversion stopped due to
reaching a terminating null wide character) or the address just past
the last wide character converted.
If conversion stopped due to reaching a terminating null wide character
and if
.arg dst
is not a null pointer, the resulting state described will be the
initial conversion state.
.im farparm
.im mbsrstrt
.desc end
.return begin
If the first code is not a valid wide character, an encoding error
occurs: The &func function stores the value of the macro
.kw EILSEQ
in
.kw errno
and returns
.mono (size_t)&minus.1,
but the conversion state will be unchanged.
Otherwise, it returns the number of bytes in the resulting multibyte
characters sequence, which is the same as the number of array elements
modified when
.arg dst
is not a null pointer.
.return end
.see begin
.im seembc &function.
.see end
.exmp begin
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

void main()
{
    int             i;
    size_t          elements;
    const wchar_t   *src;
    char            mb[50];
    mbstate_t       pstate;

    _setmbcp( 932 );
    src = wc;
    elements = wcsrtombs( mb, &src, 50, &pstate );
    if( errno == EILSEQ ) {
        printf( "Error in wide character string\n" );
    } else {
        for( i = 0; i < elements; i++ ) {
            printf( "0x%2.2x\n", mb[i] );
        }
    }
}
.exmp output
0x20
0x2e
0x31
0x41
0x81
0x40
0x82
0x60
0x82
0xa6
0x83
0x42
0xa1
0xa6
0xdf
0xe0
0xa1
.exmp end
.class ANSI
.system
