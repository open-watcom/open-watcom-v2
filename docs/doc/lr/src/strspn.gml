.func strspn _fstrspn wcsspn _mbsspn _fmbsspn _ustrspn
#include <string.h>
size_t strspn( const char *str,
               const char *charset );
.ixfunc2 '&String' &func
.ixfunc2 '&Search' &func
.if &farfnc eq 1 .do begin
size_t _fstrspn( const char __far *str,
                 const char __far *charset );
.ixfunc2 '&String' &ffunc
.ixfunc2 '&Search' &ffunc
.do end
.if &'length(&wfunc.) ne 0 .do begin
#include <wchar.h>
size_t wcsspn( const wchar_t *str,
               const wchar_t *charset );
.ixfunc2 '&String' &wfunc
.ixfunc2 '&Search' &wfunc
.ixfunc2 '&Wide' &wfunc
.do end
.if &'length(&mfunc.) ne 0 .do begin
#include <wchar.h>
size_t _mbsspn( const unsigned char *str,
                const unsigned char *charset );
.ixfunc2 '&String' &mfunc
.ixfunc2 '&Search' &mfunc
.ixfunc2 '&Multibyte' &mfunc
.do end
.if &'length(&fmfunc.) ne 0 .do begin
size_t _fmbsspn( const unsigned char __far *str,
                 const unsigned char __far *charset );
.ixfunc2 '&String' &fmfunc
.ixfunc2 '&Search' &fmfunc
.ixfunc2 '&Multibyte' &fmfunc
.do end
.if &'length(&ufunc.) ne 0 .do begin
size_t _ustrspn( const wchar_t *str,
                 const wchar_t *charset );
.ixfunc2 '&String' &ufunc
.ixfunc2 '&Search' &ufunc
.do end
.funcend
.desc begin
The &func function computes the length, in bytes, of the initial
segment of the string pointed to by
.arg str
which consists of characters from the string pointed to by
.arg charset
.ct .li .
The terminating null character is not considered to be part of
.arg charset
.ct .li .
.im farparm
.im widefunc
.im mbsffunc
.im unifunc
.desc end
.return begin
The length, in bytes, of the initial segment is returned.
.return end
.see begin
.seelist strspn strcspn strspnp
.see end
.exmp begin
#include <stdio.h>
#include <string.h>

void main()
  {
    printf( "%d\n", strspn( "out to lunch", "aeiou" ) );
    printf( "%d\n", strspn( "out to lunch", "xyz" ) );
  }
.exmp output
2
0
.exmp end
.class ANSI
.system
