.func strcmpi wcscmpi
.synop begin
.deprec
#include <string.h>
int strcmpi( const char *s1, const char *s2 );
.ixfunc2 '&String' strcmpi
.ixfunc2 '&Compare' strcmpi
.if &'length(&wfunc.) ne 0 .do begin
int wcscmpi( const wchar_t *s1, const wchar_t *s2 );
.ixfunc2 '&String' wcscmpi
.ixfunc2 '&Compare' wcscmpi
.ixfunc2 '&Wide' wcscmpi
.do end
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
.widefunc &wfunc. &funcb.
.np
.deprfunc &funcb. _stricmp
.br
.deprfunc &wfunc. _wcsicmp
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
    printf( "%d\n", _strcmpi( "AbCDEF", "abcdef" ) );
    printf( "%d\n", _strcmpi( "abcdef", "ABC"    ) );
    printf( "%d\n", _strcmpi( "abc",    "ABCdef" ) );
    printf( "%d\n", _strcmpi( "Abcdef", "mnopqr" ) );
    printf( "%d\n", _strcmpi( "Mnopqr", "abcdef" ) );
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
