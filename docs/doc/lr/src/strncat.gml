.func strncat _fstrncat wcsncat _mbsncat _fmbsncat _ustrncat
#include <string.h>
char *strncat( char *dst, const char *src, size_t n );
.ixfunc2 '&String' &func
.ixfunc2 '&Concats' &func
.if &farfnc eq 1 .do begin
char __far *_fstrncat( char __far *dst,
                 const char __far *src,
                       size_t n );
.ixfunc2 '&String' &ffunc
.ixfunc2 '&Concats' &ffunc
.do end
.if &'length(&wfunc.) ne 0 .do begin
#include <wchar.h>
wchar_t *wcsncat( wchar_t *dst,
            const wchar_t *src,
                  size_t n );
.ixfunc2 '&String' &wfunc
.ixfunc2 '&Concats' &wfunc
.ixfunc2 '&Wide' &wfunc
.do end
.if &'length(&mfunc.) ne 0 .do begin
#include <mbstring.h>
unsigned char *_mbsncat( unsigned char *dst,
                   const unsigned char *src,
                         size_t n );
.ixfunc2 '&String' &mfunc
.ixfunc2 '&Concats' &mfunc
.ixfunc2 '&Multibyte' &mfunc
.do end
.if &'length(&fmfunc.) ne 0 .do begin
unsigned char __far *_fmbsncat( unsigned char __far *dst,
                          const unsigned char __far *src,
                                size_t n );
.ixfunc2 '&String' &fmfunc
.ixfunc2 '&Concats' &fmfunc
.ixfunc2 '&Multibyte' &fmfunc
.do end
.if &'length(&ufunc.) ne 0 .do begin
wchar_t *_ustrncat( wchar_t *dst,
                     const wchar_t *src,
                     size_t n );
.ixfunc2 '&String' &ufunc
.ixfunc2 '&Concats' &ufunc
.do end
.funcend
.*
.safealt
.*
.desc begin
The &func function appends not more than
.arg n
characters of the string
pointed to by
.arg src
to the end of the string pointed to by
.arg dst
.ct .li .
The first character of
.arg src
overwrites the null character at the end of
.arg dst
.ct .li .
A terminating null character is always appended to the result.
.im farfunc
.im widefunc
.im mbsffunc
.desc end
.return begin
The &func function returns the value of
.arg dst
.ct .li .
.return end
.see begin
.seelist strncat strcat strlcat strncat_s strcat_s
.see end
.exmp begin
#include <stdio.h>
#include <string.h>

char buffer[80];

void main( void )
{
    strcpy( buffer, "Hello " );
    strncat( buffer, "world", 8 );
    printf( "%s\n", buffer );
    strncat( buffer, "*************", 4 );
    printf( "%s\n", buffer );
}
.exmp output
Hello world
Hello world****
.exmp end
.class ANSI
.system
