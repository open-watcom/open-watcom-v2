.func begin
.funct_m  mbstowcs_s TR 24731
.funct_fm _fmbstowcs_s
.func gen
#define __STDC_WANT_LIB_EXT1__  1
#include <stdlib.h>
errno_t mbstowcs_s( size_t * restrict retval,
                    wchar_t * restrict dst,
                    rsize_t dstmax,
                    const char * restrict src, rsize_t len);
.ixfunc2 '&Multibyte' &func
.if &farfnc eq 1 .do begin
.ixfunc2 '&Multibyte' &fmfunc
errno_t _fmbstowcs_s( size_t __far * restrict retval,
                    wchar_t __far * restrict dst,
                    rsize_t dstmax,
                    const char __far * restrict src, rsize_t len);
.do end
.func end
.*
.rtconst begin
Neither
.arg retval
nor
.arg src
shall be a null pointer. If
.arg dst
is not a null pointer, then neither
.arg len
nor
.arg dstmax
shall be greater than
.kw RSIZE_MAX
.ct .li .
If
.arg dst
is a null pointer, then
.arg dstmax
shall equal zero. If
.arg dst
is not a null pointer, then
.arg dstmax
shall not equal
zero. If
.arg dst
is not a null pointer and
.arg len
is not less than
.arg dstmax
.ct , then a null character shall occur within the first
.arg dstmax
multibyte characters of the array pointed to by
.arg src
.ct .li .
.np
If there is a runtime-constraint violation, then &func. does the following. If
.arg retval
is not a null pointer, then &func. sets
.arg *retval
to (size_t)(-1). If
.arg dst
is not a null pointer and
.arg dstmax
is greater than zero and less than
.kw RSIZE_MAX
.ct , then &func. sets
.arg dst[0]
to the null wide character.
.rtconst end
.*
.desc begin
The &func. function converts a sequence of multibyte characters that begins in
the initial shift state from the array pointed to by
.arg src
into a sequence of corresponding wide characters. If
.arg dst
is not a null pointer, the converted characters are stored into the
array pointed to by
.arg dst
.ct .li .
.np
Conversion continues up to and including a terminating null
character, which is also stored. Conversion stops earlier in two cases: when a sequence of
bytes is encountered that does not form a valid multibyte character, or (if
.arg dst
is not a null pointer) when
.arg len
wide characters have been stored into the array pointed to by
.arg dst
.ct .li .
If
.arg dst
is not a null pointer and no null wide character was stored into the array
pointed to by
.arg dst
.ct , then
.arg dst[len]
is set to the null wide character. Each conversion
takes place as if by a call to the
.kw mbrtowc
function.
.np
Regardless of whether
.arg dst
is or is not a null pointer, if the input conversion encounters a
sequence of bytes that do not form a valid multibyte character, an encoding error occurs:
the &func. function stores the value (size_t)(-1) into
.arg *retval
.ct .li .
Otherwise, the &func function stores into
.arg *retval
the number of multibyte
characters successfully converted, not including the terminating null character (if any).
.np
All elements following the terminating null wide character (if any) written by
&func in the array of
.arg dstmax
wide characters pointed to by
.arg dst
take unspecified values when &func returns.
.np
If copying takes place between objects that overlap, the objects take on unspecified
values.
.im safefarm
.desc end
.*
.return begin
.saferet
.return end
.*
.see begin
.seelist mbstowcs_s mbstowcs mblen mbtowc wctomb wctomb_s wcstombs wcstombs_s
.see end
.*
.exmp begin
#define __STDC_WANT_LIB_EXT1__  1
#include <stdio.h>
#include <stdlib.h>

int  main()
{
    char    *wc = "string";
    wchar_t wbuffer[50];
    int     i;
    errno_t rc;
    size_t  retval;
.exmp break
    rc = mbstowcs_s( &retval, wbuffer, 50, wc, 10);
    if( rc == 0 ) {
      wbuffer[retval] = L'\0';
      printf( "%s(%d)\n", wc, retval );
      for( i = 0; i < retval; i++ )
        printf( "/%4.4x", wbuffer[i] );
      printf( "\n" );
    }
    return( 0 );
}
.exmp output
string(6)
/0073/0074/0072/0069/006e/0067
.exmp end
.classt
.system
