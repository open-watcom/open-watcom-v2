.func begin
.funct_w   wcsrtombs_s   TR 24731
.funct_fw  _fwcsrtombs_s
.func end
.synop begin
#define __STDC_WANT_LIB_EXT1__ 1
errno_t wcsrtombs_s( size_t * restrict retval,
                     char * restrict dst,
                     rsize_t dstmax,
                     const wchar_t ** restrict src,
                     rsize_t len,
                     mbstate_t * restrict ps);
.ixfunc2 '&Wide' &funcb
.ixfunc2 '&Multibyte' &funcb
.if &farfnc eq 1 .do begin
errno_t _fwcsrtombs_s( size_t __far * restrict retval,
                       char __far * restrict dst,
                       rsize_t dstmax,
                       const wchar_t __far * __far * restrict src,
                       rsize_t len,
                       mbstate_t __far * restrict ps);
.ixfunc2 '&Wide' &fwfunc
.ixfunc2 '&Multibyte' &fwfunc
.do end
.synop end
.*
.rtconst begin
None of
.arg retval
.ct ,
.arg src
.ct ,
.arg *src
.ct , or
.arg ps
shall be null pointers. If
.arg dst
is not a null pointer,
then neither
.arg len
nor
.arg dstmax
shall be greater than
.kw RSIZE_MAX
.ct .li .
If
.arg dst
is a null
pointer, then
.arg dstmax
shall equal zero. If
.arg dst
is not a null pointer,then
.arg dstmax
shall
not equal zero. If
.arg dst
is not a null pointer and
.arg len
is not less than
.arg dstmax
.ct , then the
conversion shall have been stopped (see below) because a terminating null wide character
was reached or because an encoding error occurred.
.np
If there is a runtime-constraint violation, then
.id &funcb.
does the following. If
.arg retval
is not a null pointer,then
.id &funcb.
sets
.arg *retval
to (size_t)(-1).
If
.arg dst
is not a null pointer and
.arg dstmax
is greater than zero and less than
.kw RSIZE_MAX
.ct , then
.id &funcb.
sets
.arg dst[0]
to the null character.
.rtconst end
.*
.desc begin
The
.id &funcb.
function converts a sequence of wide characters from the array
indirectly pointed to by
.arg src
into a sequence of corresponding multibyte characters that
begins in the conversion state described by the object pointed to by
.arg ps
.ct .li .
If
.arg dst
is not a null pointer, the converted characters are then stored
into the array pointed to by
.arg dst
.ct .li .
Conversion continues up to and including a terminating null wide character,
which is also stored.
.np
Conversion stops earlier in two cases:
:ul.
:li.
when a wide character is reached that does not correspond to a valid multibyte
character;
:li.
(if
.arg dst
is not a null pointer) when the next multibyte character would exceed the
limit of n total bytes to be stored into the array pointed to by
.arg dst
.ct .li .
If the wide
character being converted is the null wide character, then n is the lesser of
.arg len
or
.arg dstmax
.ct .li .
Otherwise, n is the lesser of
.arg len
or
.arg dstmax-1
.ct .li .
:eul.
If the conversion stops without converting a null wide character and
.arg dst
is not a null
pointer, then a null character is stored into the array pointed to by
.arg dst
immediately
following any multibyte characters already stored. Each conversion takes place as if by a
call to the
.kw wcrtomb
function.
.np
If
.arg dst
is not a null pointer, the pointer object pointed to by
.arg src
is assigned either a null
pointer (if conversion stopped due to reaching a terminating null wide character) or the
address just past the last wide character converted (if any). If conversion stopped due to
reaching a terminating null wide character, the resulting state described is the initial
conversion state.
.np
Regardless of whether
.arg dst
is or is not a null pointer, if the input conversion encounters a
wide character that does not correspond to a valid multibyte character, an encoding error
occurs: the
.id &funcb.
function stores the value (size_t)(-1) into
.arg *retval
and the conversion state is unspecified. Otherwise, the
.id &funcb.
function stores
into
.arg *retval
the number of bytes in the resulting multibyte character sequence, not
including the terminating null character (if any).
.np
All elements following the terminating null character (if any) written by &funcb.
in the array of
.arg dstmax
elements pointed to by
.arg dst
take unspecified values when
.id &funcb.
returns.
.np
If copying takes place between objects that overlap, the objects take on unspecified
values.
.im safefarw
.desc end
.*
.return begin
.saferet
.return end
.*
.see begin
.im seembc
.see end
.*
.exmp begin
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
.classt
.system
