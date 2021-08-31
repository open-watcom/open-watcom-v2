.func _strnicmp _fstrnicmp _wcsnicmp _mbsnicmp _fmbsnicmp strnicmp
.ansiname _strnicmp
.synop begin
#include <string.h>
int _strnicmp( const char *s1,
               const char *s2,
               size_t len );
.ixfunc2 '&String' _strnicmp
.ixfunc2 '&Compare' _strnicmp
.if &farfnc ne 0 .do begin
int _fstrnicmp( const char __far *s1,
                const char __far *s2,
                size_t len );
.ixfunc2 '&String' _fstrnicmp
.ixfunc2 '&Compare' _fstrnicmp
.do end
.if &'length(&wfunc.) ne 0 .do begin
#include <wchar.h>
int _wcsnicmp( const wchar_t *s1,
               const wchar_t *s2,
               size_t len );
.ixfunc2 '&String' _wcsnicmp
.ixfunc2 '&Compare' _wcsnicmp
.ixfunc2 '&Wide' _wcsnicmp
.do end
.if &'length(&mfunc.) ne 0 .do begin
#include <mbstring.h>
int _mbsnicmp( const unsigned char *s1,
               const unsigned char *s2,
               size_t n );
.ixfunc2 '&String' _mbsnicmp
.ixfunc2 '&Compare' _mbsnicmp
.ixfunc2 '&Multibyte' _mbsnicmp
.do end
.if &'length(&fmfunc.) ne 0 .do begin
int _fmbsnicmp( const unsigned char __far *s1,
                const unsigned char __far *s2,
                size_t n );
.ixfunc2 '&String' _fmbsnicmp
.ixfunc2 '&Compare' _fmbsnicmp
.ixfunc2 '&Multibyte' _fmbsnicmp
.do end

.deprec
int strnicmp( const char *s1,
              const char *s2,
              size_t len );
.ixfunc2 '&String' strnicmp
.ixfunc2 '&Compare' strnicmp
.synop end
.desc begin
The function compares, without case sensitivity,
the string pointed to by
.arg s1
to the string pointed to by
.arg s2
.ct , for at most
.arg len
characters.
All uppercase characters from
.arg s1
and
.arg s2
are mapped to lowercase for the purposes of doing the comparison.
.farfuncp &ffunc. &funcb.
.widefunc &wfunc. &funcb.
.mbcsfunc &mfunc. &funcb.
.farfuncp &fmfunc. &mfunc.
.np
.deprfunc strnicmp _strnicmp
.desc end
.return begin
The function returns an integer less than, equal to,
or greater than zero, indicating that the string pointed to by
.arg s1
is less than, equal to, or greater than the string pointed to by
.arg s2
.period
.return end
.see begin
.seelist strcmp _stricmp strncmp strcasecmp strncasecmp
.see end
.exmp begin
#include <stdio.h>
#include <string.h>

void main()
  {
    printf( "%d\n", _strnicmp( "abcdef", "ABCXXX", 10 ) );
    printf( "%d\n", _strnicmp( "abcdef", "ABCXXX",  6 ) );
    printf( "%d\n", _strnicmp( "abcdef", "ABCXXX",  3 ) );
    printf( "%d\n", _strnicmp( "abcdef", "ABCXXX",  0 ) );
  }
.exmp output
-20
-20
0
0
.exmp end
.class WATCOM
.system
