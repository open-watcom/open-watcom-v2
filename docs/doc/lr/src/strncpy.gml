.func strncpy _fstrncpy wcsncpy _mbsncpy _fmbsncpy
.synop begin
#include <string.h>
char *strncpy( char *dst,
               const char *src,
               size_t n );
.ixfunc2 '&String' &func
.ixfunc2 '&Copy' &func
.if &farfnc eq 1 .do begin
char __far *_fstrncpy( char __far *dst,
                       const char __far *src,
                       size_t n );
.ixfunc2 '&String' &ffunc
.ixfunc2 '&Copy' &ffunc
.do end
.if &'length(&wfunc.) ne 0 .do begin
#include <wchar.h>
wchar_t *wcsncpy( wchar_t *dst,
                  const wchar_t *src,
                  size_t n );
.ixfunc2 '&String' &wfunc
.ixfunc2 '&Copy' &wfunc
.ixfunc2 '&Wide' &wfunc
.do end
.if &'length(&mfunc.) ne 0 .do begin
#include <mbstring.h>
unsigned char *_mbsncpy( unsigned char *dst,
                   const unsigned char *src,
                         size_t n );
.ixfunc2 '&String' &mfunc
.ixfunc2 '&Copy' &mfunc
.ixfunc2 '&Multibyte' &mfunc
.do end
.if &'length(&fmfunc.) ne 0 .do begin
unsigned char __far *_fmbsncpy( unsigned char __far *dst,
                          const unsigned char __far *src,
                                size_t n );
.ixfunc2 '&String' &fmfunc
.ixfunc2 '&Copy' &fmfunc
.ixfunc2 '&Multibyte' &fmfunc
.do end
.synop end
.*
.safealt
.*
.desc begin
The
.id &func.
function copies no more than
.arg n
characters from the string pointed to by
.arg src
into the array pointed to by
.arg dst
.ct .li .
Copying of overlapping objects is not guaranteed to work properly.
See the
.kw memmove
function if you wish to copy objects that overlap.
.np
If the string pointed to by
.arg src
is shorter than
.arg n
characters, null characters are appended to the copy in the array pointed
to by
.arg dst
.ct , until
.arg n
characters in all have been written.
If the string pointed to by
.arg src
is longer than
.arg n
characters, then the result will not be terminated by a null character.
.im farfunc
.im widefunc
.im mbsffunc
.desc end
.return begin
The
.id &func.
function returns the value of
.arg dst
.ct .li .
.return end
.see begin
.seelist strncpy strlcpy strcpy strdup strncpy_s strcpy_s
.see end
.exmp begin
#include <stdio.h>
#include <string.h>

void main( void )
{
    char buffer[15];
.exmp break
    printf( "%s\n", strncpy( buffer, "abcdefg", 10 ) );
    printf( "%s\n", strncpy( buffer, "1234567",  6 ) );
    printf( "%s\n", strncpy( buffer, "abcdefg",  3 ) );
    printf( "%s\n", strncpy( buffer, "*******",  0 ) );
}
.exmp output
abcdefg
123456g
abc456g
abc456g
.exmp end
.class ANSI
.system
