.func strcmpi wcscmpi
.synop begin
#include <string.h>
int strcmpi( const char *s1, const char *s2 );
.ixfunc2 '&String' &func
.ixfunc2 '&Compare' &func
.if &'length(&wfunc.) ne 0 .do begin
int wcscmpi( const wchar_t *s1, const wchar_t *s2 );
.ixfunc2 '&String' &wfunc
.ixfunc2 '&Compare' &wfunc
.ixfunc2 '&Wide' &wfunc
.do end
.synop end
.desc begin
The &func function compares, with case insensitivity, the string
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
The &func function is identical to the
.kw stricmp
function.
.im widefunc
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
.seelist &function. strcmp strcmpi stricmp strncmp strnicmp
.see end
.exmp begin
#include <stdio.h>
#include <string.h>

void main()
  {
    printf( "%d\n", strcmpi( "AbCDEF", "abcdef" ) );
    printf( "%d\n", strcmpi( "abcdef", "ABC"    ) );
    printf( "%d\n", strcmpi( "abc",    "ABCdef" ) );
    printf( "%d\n", strcmpi( "Abcdef", "mnopqr" ) );
    printf( "%d\n", strcmpi( "Mnopqr", "abcdef" ) );
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
