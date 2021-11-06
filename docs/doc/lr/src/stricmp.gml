.func _stricmp _fstricmp _wcsicmp _mbsicmp _fmbsicmp stricmp
.ansiname _stricmp
.synop begin
#include <string.h>
int _stricmp( const char *s1, const char *s2 );
.ixfunc2 '&String' _stricmp
.ixfunc2 '&Compare' _stricmp
.if &farfnc ne 0 .do begin
int _fstricmp( const char __far *s1,
               const char __far *s2 );
.ixfunc2 '&String' _fstricmp
.ixfunc2 '&Compare' _fstricmp
.do end
.if &'length(&wfunc.) ne 0 .do begin
#include <wchar.h>
int _wcsicmp( const wchar_t *s1, const wchar_t *s2 );
.ixfunc2 '&String' _wcsicmp
.ixfunc2 '&Compare' _wcsicmp
.ixfunc2 '&Wide' _wcsicmp
.do end
.if &'length(&mfunc.) ne 0 .do begin
#include <mbstring.h>
int _mbsicmp( const unsigned char *s1,
              const unsigned char *s2 );
.ixfunc2 '&String' _mbsicmp
.ixfunc2 '&Compare' _mbsicmp
.ixfunc2 '&Multibyte' _mbsicmp
.do end
.if &'length(&fmfunc.) ne 0 .do begin
int _fmbsicmp( const unsigned char __far *s1,
               const unsigned char __far *s2 );
.ixfunc2 '&String' _fmbsicmp
.ixfunc2 '&Compare' _fmbsicmp
.ixfunc2 '&Multibyte' _fmbsicmp
.do end

.deprec
int stricmp( const char *s1, const char *s2 );
.ixfunc2 '&String' stricmp
.ixfunc2 '&Compare' stricmp
.synop end
.desc begin
The function compares, without case sensitivity,
the string pointed to by
.arg s1
to the string pointed to by
.arg s2
.period
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
.deprfunc stricmp _stricmp
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
.seelist strcmp _stricmp strncmp _strnicmp strcasecmp strncasecmp
.see end
.exmp begin
#include <stdio.h>
#include <string.h>

void main()
  {
    printf( "%d\n", _stricmp( "AbCDEF", "abcdef" ) );
    printf( "%d\n", _stricmp( "abcdef", "ABC"    ) );
    printf( "%d\n", _stricmp( "abc",    "ABCdef" ) );
    printf( "%d\n", _stricmp( "Abcdef", "mnopqr" ) );
    printf( "%d\n", _stricmp( "Mnopqr", "abcdef" ) );
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
