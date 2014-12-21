.func stricmp _stricmp _fstricmp _wcsicmp _mbsicmp _fmbsicmp
.synop begin
#include <string.h>
int stricmp( const char *s1, const char *s2 );
.ixfunc2 '&String' &funcb
.ixfunc2 '&Compare' &funcb
.if &'length(&_func.) ne 0 .do begin
int _stricmp( const char *s1, const char *s2 );
.ixfunc2 '&String' &_func
.ixfunc2 '&Compare' &_func
.do end
.if &farfnc eq 1 .do begin
int _fstricmp( const char __far *s1,
               const char __far *s2 );
.ixfunc2 '&String' &ffunc
.ixfunc2 '&Compare' &ffunc
.do end
.if &'length(&wfunc.) ne 0 .do begin
#include <wchar.h>
int _wcsicmp( const wchar_t *s1, const wchar_t *s2 );
.ixfunc2 '&String' &wfunc
.ixfunc2 '&Compare' &wfunc
.ixfunc2 '&Wide' &wfunc
.do end
.if &'length(&mfunc.) ne 0 .do begin
#include <mbstring.h>
int _mbsicmp( const unsigned char *s1,
              const unsigned char *s2 );
.ixfunc2 '&String' &mfunc
.ixfunc2 '&Compare' &mfunc
.ixfunc2 '&Multibyte' &mfunc
.do end
.if &'length(&fmfunc.) ne 0 .do begin
int _fmbsicmp( const unsigned char __far *s1,
               const unsigned char __far *s2 );
.ixfunc2 '&String' &fmfunc
.ixfunc2 '&Compare' &fmfunc
.ixfunc2 '&Multibyte' &fmfunc
.do end
.synop end
.desc begin
The
.id &funcb.
function compares, with case insensitivity, the string
pointed to by
.arg s1
to the string pointed to by
.arg s2
.ct .li .
All uppercase characters from
.arg s1
and
.arg s2
are mapped to lowercase for the purposes of doing the comparison.
.im ansiconf
.im farparm
.im widefun1
.im mbsffunc
.desc end
.return begin
The
.id &funcb.
function returns an integer less than, equal to, or greater
than zero, indicating that the string pointed to by
.arg s1
is less than, equal to, or greater than the string pointed to by
.arg s2
.ct .li .
.return end
.see begin
.seelist strcmp strcmpi stricmp strncmp strnicmp
.see end
.exmp begin
#include <stdio.h>
#include <string.h>

void main()
  {
    printf( "%d\n", stricmp( "AbCDEF", "abcdef" ) );
    printf( "%d\n", stricmp( "abcdef", "ABC"    ) );
    printf( "%d\n", stricmp( "abc",    "ABCdef" ) );
    printf( "%d\n", stricmp( "Abcdef", "mnopqr" ) );
    printf( "%d\n", stricmp( "Mnopqr", "abcdef" ) );
  }
.exmp output
0
100
-100
-12
12
.exmp end
.class WATCOM
.system
