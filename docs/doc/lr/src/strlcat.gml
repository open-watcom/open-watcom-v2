.func strlcat wcslcat
#include <string.h>
size_t strlcat( char *dst, const char *src, size_t n );
.ixfunc2 '&String' &func
.ixfunc2 '&Concats' &func
.if &'length(&wfunc.) ne 0 .do begin
size_t *wcslcat( wchar_t *dst,
                 const wchar_t *src,
                 size_t n );
.ixfunc2 '&String' &wfunc
.ixfunc2 '&Concats' &wfunc
.ixfunc2 '&Wide' &wfunc
.do end
.synop end
.desc begin
The &func function appends characters of the string pointed to by
.arg src
to the end of the string in a buffer pointed to by
.arg dst
that can hold up to
.arg n
characters.
The first character of
.arg src
overwrites the null character at the end of
.arg dst
.ct .li .
A terminating null character is always appended to the result, unless
.arg n
characters of
.arg dst
are scanned and no null character is found.
.im widefunc
.desc end
.return begin
The &func function returns the total length of string it tried to create,
that is the number of characters in both
.arg src
and
.arg dst
strings, not counting the terminating null characters.
If
.arg n
characters of
.arg dst
were scanned without finding a null character,
.arg n
is returned.
.return end
.see begin
.seelist strlcat strlcpy strncat strcat
.see end
.exmp begin
#include <stdio.h>
#include <string.h>

char buffer[80];

void main( void )
{
    strcpy( buffer, "Hello " );
    strlcat( buffer, "world", 12 );
    printf( "%s\n", buffer );
    strlcat( buffer, "*************", 16 );
    printf( "%s\n", buffer );
}
.exmp output
Hello world
Hello world****
.exmp end
.class WATCOM
.system
