.func strcspn _fstrcspn wcscspn _mbscspn _fmbscspn
.synop begin
#include <string.h>
size_t strcspn( const char *s,
                const char *charset );
.ixfunc2 '&String' &funcb
.ixfunc2 '&Search' &funcb
.if &farfnc ne 0 .do begin
size_t _fstrcspn( const char __far *s,
                  const char __far *charset );
.ixfunc2 '&String' &ffunc
.ixfunc2 '&Search' &ffunc
.do end
.if &'length(&wfunc.) ne 0 .do begin
#include <wchar.h>
size_t wcscspn( const wchar_t *s,
                const wchar_t *charset );
.ixfunc2 '&String' &wfunc
.ixfunc2 '&Search' &wfunc
.ixfunc2 '&Wide' &wfunc
.do end
.if &'length(&mfunc.) ne 0 .do begin
#include <mbstring.h>
size_t _mbscpsn( const unsigned char *s,
                 const unsigned char *charset );
.ixfunc2 '&String' &mfunc
.ixfunc2 '&Copy' &mfunc
.ixfunc2 '&Multibyte' &mfunc
.do end
.if &'length(&fmfunc.) ne 0 .do begin
size_t _fmbscpsn( const unsigned char __far *s,
                  const unsigned char __far *charset );
.ixfunc2 '&String' &fmfunc
.ixfunc2 '&Copy' &fmfunc
.ixfunc2 '&Multibyte' &fmfunc
.do end
.synop end
.desc begin
The
.id &funcb.
function computes the length, in bytes, of the initial
segment of the string pointed to by
.arg s
which consists entirely of characters
.us not
from the string pointed to by
.arg charset
.period
The terminating null character is not considered part of
.arg s
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
.class ISO C
.system
