.func strncmp _fstrncmp wcsncmp _mbsncmp _fmbsncmp
#include <string.h>
int strncmp( const char *s1,
             const char *s2,
             size_t n );
.ixfunc2 '&String' &func
.ixfunc2 '&Compare' &func
.if &farfnc eq 1 .do begin
int _fstrncmp( const char __far *s1,
               const char __far *s2,
               size_t n );
.ixfunc2 '&String' &ffunc
.ixfunc2 '&Compare' &ffunc
.do end
.if &'length(&wfunc.) ne 0 .do begin
#include <wchar.h>
int wcsncmp( const wchar_t *s1,
             const wchar_t *s2,
             size_t n );
.ixfunc2 '&String' &wfunc
.ixfunc2 '&Compare' &wfunc
.ixfunc2 '&Wide' &wfunc
.do end
.if &'length(&mfunc.) ne 0 .do begin
#include <mbstring.h>
int _mbsncmp( const unsigned char *s1,
              const unsigned char *s2,
              size_t n );
.ixfunc2 '&String' &mfunc
.ixfunc2 '&Compare' &mfunc
.ixfunc2 '&Multibyte' &mfunc
.do end
.if &'length(&fmfunc.) ne 0 .do begin
int _fmbsncmp( const unsigned char __far *s1,
               const unsigned char __far *s2,
               size_t n );
.ixfunc2 '&String' &fmfunc
.ixfunc2 '&Compare' &fmfunc
.ixfunc2 '&Multibyte' &fmfunc
.do end
.funcend
.desc begin
The &func compares not more than
.arg n
characters from the string pointed to by
.arg s1
to the string pointed to by
.arg s2
.ct .li .
.im farparm
.im widefunc
.im mbsffunc
.desc end
.return begin
The &func function returns an integer less than, equal to, or greater
than zero, indicating that the string pointed to by
.arg s1
is less than, equal to, or greater than the string pointed to by
.arg s2
.ct .li .
.return end
.see begin
.seelist &function. strcmp stricmp strncmp strnicmp
.see end
.exmp begin
#include <stdio.h>
#include <string.h>

void main()
  {
    printf( "%d\n", strncmp( "abcdef", "abcDEF", 10 ) );
    printf( "%d\n", strncmp( "abcdef", "abcDEF",  6 ) );
    printf( "%d\n", strncmp( "abcdef", "abcDEF",  3 ) );
    printf( "%d\n", strncmp( "abcdef", "abcDEF",  0 ) );
  }
.exmp output
1
1
0
0
.exmp end
.class ANSI
.system
