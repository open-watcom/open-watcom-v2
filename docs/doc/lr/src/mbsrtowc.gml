.func mbsrtowcs _fmbsrtowcs
#include <wchar.h>
size_t mbsrtowcs( wchar_t *dst,
               const char **src,
               size_t len, mbstate_t *ps );
.ixfunc2 '&Multibyte' &func
.if &farfnc eq 1 .do begin
#include <mbstring.h>
size_t _fmbsrtowcs( wchar_t __far *dst,
         const char __far * __far *src,
         size_t len, mbstate_t __far *ps );
.ixfunc2 '&Multibyte' &ffunc
.do end
.funcend
.*
.safealt
.*
.desc begin
The &func function converts a sequence of multibyte characters
that begins in the shift state described by
.arg ps
from the array indirectly pointed
to by
.arg src
into a sequence of corresponding wide characters, which, if
.arg dst
is not a null pointer, are then stored into the array pointed to by
.arg dst
.ct .li .
Conversion continues up to and including a terminating null character,
but the terminating null wide character will not be stored.
Conversion will stop earlier in two cases: when a sequence of bytes is
reached that does not form a valid multibyte character, or (if
.arg dst
is not a null pointer)
when
.arg len
codes have been stored into the array pointed to by
.arg dst
.ct .li .
Each conversion takes place as if by a call to the
.kw mbrtowc
function.
.np
If
.arg dst
is not a null pointer, the pointer object pointed to by
.arg src
will be assigned either a null pointer (if conversion stopped due to
reaching a terminating null character) or the address just past the
last multibyte character converted.
If conversion stopped due to reaching a terminating null character and
if
.arg dst
is not a null pointer, the resulting state described will be the
initial conversion state.
.im farparm
.im mbsrstrt
.desc end
.return begin
If the input string does not begin with a valid multibyte character, an
encoding error occurs: The &func function stores the value of the macro
.kw EILSEQ
in
.kw errno
and returns
.mono (size_t)&minus.1,
but the conversion state will be unchanged.
Otherwise, it returns the number of multibyte characters successfully
converted, which is the same as the number of array elements modified
when
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
    int         i;
    size_t      elements;
    const char  *src;
    wchar_t     wc[50];
    mbstate_t   pstate;

    _setmbcp( 932 );
    src = chars;
    elements = mbsrtowcs( wc, &src, 50, &pstate );
    if( errno == EILSEQ ) {
        printf( "Error in multibyte character string\n" );
    } else {
        for( i = 0; i < elements; i++ ) {
            printf( "%#6.4x\n", wc[i] );
        }
    }
}
.exmp output
0x0020
0x002e
0x0031
0x0041
0x3000
0xff21
0x3048
0x30a3
0xff61
0xff66
0xff9f
0x720d
.exmp end
.class ANSI
.system
