.func strncasecmp
.synop begin
#include <strings.h>
int strncasecmp( const char *s1,
                 const char *s2,
                 size_t len );
.ixfunc2 '&String' &func
.ixfunc2 '&Compare' &func
.synop end
.desc begin
The
.id &func.
function compares, without case sensitivity, the string
pointed to by
.arg s1
to the string pointed to by
.arg s2
.ct , for at most
.arg len
characters.
.np
The
.id &func.
function is identical to the
.kw strnicmp
function.
.desc end
.return begin
The
.id &func.
function returns an integer less than, equal to, or greater
than zero, indicating that the string pointed to by
.arg s1
is, ignoring case, less than, equal to, or greater than the string pointed
to by
.arg s2
.ct .li .
.return end
.see begin
.seelist strnicmp strcmp stricmp strncmp strcasecmp
.see end
.exmp begin
#include <stdio.h>
#include <strings.h>

int main( void )
{
    printf( "%d\n", strncasecmp( "abcdef", "ABCXXX", 10 ) );
    printf( "%d\n", strncasecmp( "abcdef", "ABCXXX",  6 ) );
    printf( "%d\n", strncasecmp( "abcdef", "ABCXXX",  3 ) );
    printf( "%d\n", strncasecmp( "abcdef", "ABCXXX",  0 ) );
    return( 0 );
}
.exmp output
-20
-20
0
0
.exmp end
.class POSIX
.system
