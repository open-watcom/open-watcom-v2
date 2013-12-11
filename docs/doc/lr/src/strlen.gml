.func strlen _fstrlen wcslen _mbslen _fmbslen
#include <string.h>
size_t strlen( const char *s );
.ixfunc2 '&String' &func
.if &farfnc eq 1 .do begin
size_t _fstrlen( const char __far *s );
.ixfunc2 '&String' &ffunc
.do end
.if &'length(&wfunc.) ne 0 .do begin
#include <wchar.h>
size_t wcslen( const wchar_t *s );
.ixfunc2 '&String' &wfunc
.ixfunc2 '&Wide' &wfunc
.do end
.if &'length(&mfunc.) ne 0 .do begin
#include <mbstring.h>
size_t _mbslen( const unsigned char *s );
.ixfunc2 '&String' &mfunc
.ixfunc2 '&Multibyte' &mfunc
.do end
.if &'length(&fmfunc.) ne 0 .do begin
size_t _fmbslen( const unsigned char __far *s );
.ixfunc2 '&String' &fmfunc
.ixfunc2 '&Multibyte' &fmfunc
.do end
.funcend
.*
.safealt
.*
.desc begin
The &func function computes the length of the string pointed to by
.arg s
.ct .li .
.im farparm
.im widefunc
.im mbsffunc
.desc end
.return begin
The &func function returns the number of characters that precede the
terminating null character.
.return end
.see begin
.seelist &function. strnlen_s
.see end
.exmp begin
#include <stdio.h>
#include <string.h>

void main()
{
    printf( "%d\n", strlen( "Howdy" ) );
    printf( "%d\n", strlen( "Hello world\n" ) );
    printf( "%d\n", strlen( "" ) );
}
.exmp output
5
12
0
.exmp end
.class ANSI
.system
