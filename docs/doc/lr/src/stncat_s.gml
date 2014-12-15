.func strncat_s wcsncat_s
.synop begin
#define __STDC_WANT_LIB_EXT1__  1
#include <string.h>
errno_t strncat_s( char * restrict s1,
                   rsize_t s1max,
                   const char * restrict s2,
                   rsize_t n )
.ixfunc2 '&String' &funcb
.ixfunc2 '&Concats' &funcb
.if &'length(&wfunc.) ne 0 .do begin
#include <wchar.h>
errno_t wcsncat_s( wchar_t * restrict s1,
                   rsize_t s1max,
                   const wchar_t * restrict s2,
                   rsize_t n )
.ixfunc2 '&String' &wfunc
.ixfunc2 '&Concats' &wfunc
.ixfunc2 '&Wide' &wfunc
.do end
.synop end
.*
.rtconst begin
Let
.arg m
denote the value
.arg s1max - strnlen_s(s1, s1max)
upon entry to &funcb.
.np
Neither
.arg s1
nor
.arg s2
shall be a null pointer. Neither
.arg s1max
nor
.arg n
shall be greater than
.kw RSIZE_MAX
.ct .li .
.arg s1max
shall not equal zero.
.arg m
shall not equal zero. If
.arg n
is not less than
.arg m, then
.arg m
shall be greater than
.arg strnlen_s(s2, m)
.ct .li .
Copying shall not take place between objects that overlap.
.np
If there is a runtime-constraint violation, then if
.arg s1
is not a null pointer and
.arg s1max
is
greater than zero and not greater than
.kw RSIZE_MAX
.ct , then
.id &funcb.
sets
.arg s1[0]
to the null character.
.rtconst end
.*
.desc begin
The
.id &funcb.
function appends not more than
.arg n
successive characters (characters that follow a null character are not copied)
from the array pointed to by
.arg s2
to the end of the string pointed to by
.arg s1
.ct .li .
The initial character from
.arg s2
overwrites the null character at the end of
.arg s1
.ct .li .
If no null character was copied from
.arg s2,then
.arg s1[s1max-m+n]
is set to a null character.
All elements following the terminating null character (if any) written by
.id &funcb.
in
the array of
.arg s1max
characters pointed to by
.arg s1
take unspecified values when
.id &funcb.
returns.
.im widefunc
.desc end
.*
.return begin
.saferet
.return end
.*
.see begin
.seelist strncat strcat strlcat strcat_s
.see end
.*
.exmp begin
#define __STDC_WANT_LIB_EXT1__  1
#include <stdio.h>
#include <string.h>

char buffer[80];

void main( void )
{
    strcpy( buffer, "Hello " );
    strncat_s( buffer, sizeof( buffer ), "world", 8 );
    printf( "%s\n", buffer );
    strncat( buffer, "*************", 4 );
    printf( "%s\n", buffer );
}
.exmp output
Hello world
Hello world****
.exmp end
.*
.class TR 24731
.system
