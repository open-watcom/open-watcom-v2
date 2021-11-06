.func strspn _fstrspn wcsspn _mbsspn _fmbsspn
.synop begin
#include <string.h>
size_t strspn( const char *s,
               const char *charset );
.ixfunc2 '&String' &funcb
.ixfunc2 '&Search' &funcb
.if &farfnc ne 0 .do begin
size_t _fstrspn( const char __far *s,
                 const char __far *charset );
.ixfunc2 '&String' &ffunc
.ixfunc2 '&Search' &ffunc
.do end
.if &'length(&wfunc.) ne 0 .do begin
#include <wchar.h>
size_t wcsspn( const wchar_t *s,
               const wchar_t *charset );
.ixfunc2 '&String' &wfunc
.ixfunc2 '&Search' &wfunc
.ixfunc2 '&Wide' &wfunc
.do end
.if &'length(&mfunc.) ne 0 .do begin
#include <wchar.h>
size_t _mbsspn( const unsigned char *s,
                const unsigned char *charset );
.ixfunc2 '&String' &mfunc
.ixfunc2 '&Search' &mfunc
.ixfunc2 '&Multibyte' &mfunc
.do end
.if &'length(&fmfunc.) ne 0 .do begin
size_t _fmbsspn( const unsigned char __far *s,
                 const unsigned char __far *charset );
.ixfunc2 '&String' &fmfunc
.ixfunc2 '&Search' &fmfunc
.ixfunc2 '&Multibyte' &fmfunc
.do end
.synop end
.desc begin
The
.id &funcb.
function computes the length, in bytes, of the initial
segment of the string pointed to by
.arg s
which consists of characters from the string pointed to by
.arg charset
.period
The terminating null character is not considered to be part of
.arg charset
.period
.farfuncp &ffunc. &funcb.
.widefunc &wfunc. &funcb.
.mbcsfunc &mfunc. &funcb.
.farfuncp &fmfunc. &mfunc.
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
.class ISO C
.system
