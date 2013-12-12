.func strcasecmp
#include <strings.h>
int strcasecmp( const char *s1, const char *s2 );
.ixfunc2 '&String' &func
.ixfunc2 '&Compare' &func
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
.np
The &func function is identical to the
.kw stricmp
function.
.desc end
.return begin
The &func function returns an integer less than, equal to, or greater
than zero, indicating that the string pointed to by
.arg s1
is, ignoring case, less than, equal to, or greater than the string pointed
to by
.arg s2
.ct .li .
.return end
.see begin
.seelist &function. strcmp strcmpi stricmp strncmp strnicmp strncasecmp
.see end
.exmp begin
#include <stdio.h>
#include <strings.h>

int main( void )
{
    printf( "%d\n", strcasecmp( "AbCDEF", "abcdef" ) );
    printf( "%d\n", strcasecmp( "abcdef", "ABC"    ) );
    printf( "%d\n", strcasecmp( "abc",    "ABCdef" ) );
    printf( "%d\n", strcasecmp( "Abcdef", "mnopqr" ) );
    printf( "%d\n", strcasecmp( "Mnopqr", "abcdef" ) );
    return( 0 );
}
.exmp output
0
100
-100
-12
12
.exmp end
.class POSIX
.system
