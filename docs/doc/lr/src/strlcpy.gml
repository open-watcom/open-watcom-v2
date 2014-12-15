.func strlcpy wcslcpy
.synop begin
#include <string.h>
size_t strlcpy( char *dst,
                const char *src,
                size_t n );
.ixfunc2 '&String' &funcb
.ixfunc2 '&Copy' &funcb
.if &'length(&wfunc.) ne 0 .do begin
size_t wcslcpy( wchar_t *dst,
                const wchar_t *src,
                size_t n );
.ixfunc2 '&String' &wfunc
.ixfunc2 '&Copy' &wfunc
.ixfunc2 '&Wide' &wfunc
.do end
.synop end
.desc begin
The
.id &funcb.
function copies no more than
.arg n
characters from the string pointed to by
.arg src
into the array pointed to by
.arg dst
.ct .li .
Copying of overlapping objects is not guaranteed to work properly.
See the
.kw memmove
function if you wish to copy objects that overlap.
.np
If the string pointed to by
.arg src
is longer than
.arg n
characters, then only
.arg n
- 1 characters will be copied and the result will be null terminated.
.im widefunc
.desc end
.return begin
The
.id &funcb.
function returns the number of characters in the
.arg src
string, not including the terminating null character.
.return end
.see begin
.seelist strlcpy strlcat strncpy strcpy
.see end
.exmp begin
#include <stdio.h>
#include <string.h>

void main( void )
{
    char    buffer[10];
.exmp break 
    printf( "%d:'%s'\n", strlcpy( buffer,
        "Buffer overflow", sizeof( buffer ) ), buffer );
}
.exmp output
15:'Buffer ov'
.exmp end
.class WATCOM
.system
