.func strcat _fstrcat wcscat _mbscat _fmbscat _ustrcat
#include <string.h>
char *strcat( char *dst, const char *src );
.ixfunc2 '&String' &func
.ixfunc2 '&Concats' &func
.if &farfnc eq 1 .do begin
char __far *_fstrcat( char __far *dst,
                      const char __far *src );
.ixfunc2 '&String' &ffunc
.ixfunc2 '&Concats' &ffunc
.do end
.if &'length(&wfunc.) ne 0 .do begin
#include <wchar.h>
wchar_t *wcscat( wchar_t *dst, const wchar_t *src );
.ixfunc2 '&String' &wfunc
.ixfunc2 '&Concats' &wfunc
.ixfunc2 '&Wide' &wfunc
.do end
.if &'length(&mfunc.) ne 0 .do begin
#include <mbstring.h>
unsigned char *_mbscat( unsigned char *dst,
                  const unsigned char *src );
.ixfunc2 '&String' &mfunc
.ixfunc2 '&Concats' &mfunc
.do end
.if &'length(&fmfunc.) ne 0 .do begin
.ixfunc2 '&Multibyte' &mfunc
unsigned char __far *_fmbscat( unsigned char __far *dst,
                         const unsigned char __far *src );
.ixfunc2 '&String' &fmfunc
.ixfunc2 '&Concats' &fmfunc
.ixfunc2 '&Multibyte' &fmfunc
.do end
.if &'length(&ufunc.) ne 0 .do begin
wchar_t *_ustrcat( wchar_t *dst, const wchar_t *src );
.ixfunc2 '&String' &ufunc
.ixfunc2 '&Concats' &ufunc
.do end
.funcend
.*
.safealt
.*
.desc begin
The &func function appends a copy of the string pointed to
by
.arg src
(including the terminating null character)
to the end of the string pointed to by
.arg dst
.ct .li .
The first character of
.arg src
overwrites the null character at the end of
.arg dst
.ct .li .
.im farfunc
.im widefunc
.im mbsfunc
.im unifunc
.desc end
.return begin
The value of
.arg dst
is returned.
.return end
.see begin
.seelist &function. strcat strncat strcat_s strncat_s
.see end
.exmp begin
#include <stdio.h>
#include <string.h>

void main()
{
    char buffer[80];
.exmp break
    strcpy( buffer, "Hello " );
    strcat( buffer, "world" );
    printf( "%s\n", buffer );
}
.exmp output
Hello world
.exmp end
.class ANSI
.system
