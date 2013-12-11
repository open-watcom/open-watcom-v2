.func strcspn _fstrcspn wcscspn _mbscspn _fmbscspn
#include <string.h>
size_t strcspn( const char *str,
                const char *charset );
.ixfunc2 '&String' &func
.ixfunc2 '&Search' &func
.if &farfnc eq 1 .do begin
size_t _fstrcspn( const char __far *str,
                  const char __far *charset );
.ixfunc2 '&String' &ffunc
.ixfunc2 '&Search' &ffunc
.do end
.if &'length(&wfunc.) ne 0 .do begin
#include <wchar.h>
size_t wcscspn( const wchar_t *str,
                const wchar_t *charset );
.ixfunc2 '&String' &wfunc
.ixfunc2 '&Search' &wfunc
.ixfunc2 '&Wide' &wfunc
.do end
.if &'length(&mfunc.) ne 0 .do begin
#include <mbstring.h>
size_t _mbscpsn( const unsigned char *str,
                 const unsigned char *charset );
.ixfunc2 '&String' &mfunc
.ixfunc2 '&Copy' &mfunc
.ixfunc2 '&Multibyte' &mfunc
.do end
.if &'length(&fmfunc.) ne 0 .do begin
size_t _fmbscpsn( const unsigned char __far *str,
                  const unsigned char __far *charset );
.ixfunc2 '&String' &fmfunc
.ixfunc2 '&Copy' &fmfunc
.ixfunc2 '&Multibyte' &fmfunc
.do end
.funcend
.desc begin
The &func function computes the length, in bytes, of the initial
segment of the string pointed to by
.arg str
which consists entirely of characters
.us not
from the string pointed to by
.arg charset
.ct .li .
The terminating null character is not considered part of
.arg str
.ct .li .
.im farparm
.im widefunc
.im mbsfunc
.desc end
.return begin
The length, in bytes, of the initial segment is returned.
.return end
.see begin
.seelist strcspn strspn
.see end
.exmp begin
#include <stdio.h>
#include <string.h>

void main()
  {
    printf( "%d\n", strcspn( "abcbcadef", "cba" ) );
    printf( "%d\n", strcspn( "xxxbcadef", "cba" ) );
    printf( "%d\n", strcspn( "123456789", "cba" ) );
  }
.exmp output
0
3
9
.exmp end
.class ANSI
.system
