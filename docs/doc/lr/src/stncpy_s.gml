.func strncpy_s wcsncpy_s
#define __STDC_WANT_LIB_EXT1__  1
#include <string.h>
errno_t strncpy_s( char * restrict s1,
                   rsize_t s1max,
                   const char * restrict s2,
                   rsize_t n );
.ixfunc2 '&String' &func
.ixfunc2 '&Copy' &func
.if &'length(&wfunc.) ne 0 .do begin
#include <wchar.h>
errno_t wcsncpy_s( wchar_t * restrict s1,
                   rsize_t s1max,
                   const wchar_t * restrict s2,
                   rsize_t n );
.ixfunc2 '&String' &wfunc
.ixfunc2 '&Copy' &wfunc
.ixfunc2 '&Wide' &wfunc
.do end
.synop end
.*
.rtconst begin
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
shall not equal zero. If
.arg n
is not less than
.arg s1max
.ct , then
.arg s1max
shall be greater than
.arg strnlen_s(s2, s1max)
.ct .li .
.np
Copying shall not take place between
objects that overlap.
.np
If there is a runtime-constraint violation, then if
.arg s1
is not a null pointer and
.arg s1max
is greater than zero and not greater than
.kw RSIZE_MAX
.ct , then
.kw strncpy_s
sets
.arg s1[0]
to the null character.
.rtconst end
.*
.desc begin
The &func function copies not more than
.arg n
successive characters (characters that follow a null character are not
copied) from the array pointed to by
.arg s2
to the array pointed to by
.arg s1
.ct .li .
If no null character was copied from
.arg s2
.ct , then
.arg s1[n]
is set to
a null character.
.np
All elements following the terminating null character (if any) written by
.kw strncpy_s
in the array of
.arg s1max
characters pointed to by
.arg s1
take unspecified values when
.kw strncpy_s
returns.
.im widefunc
.desc end
.*
.return begin
.saferet
.return end
.*
.see begin
.seelist &function. strncpy strlcpy strcpy strdup strcpy_s
.see end
.*
.exmp begin
#define __STDC_WANT_LIB_EXT1__  1
#include <stdio.h>
#include <string.h>

void main( void )
{
    char buffer[15];
.exmp break
    strncpy_s( buffer, sizeof( buffer ), "abcdefg", 10 );
    printf( "%s\n", buffer );

    strncpy_s( buffer, sizeof( buffer ), "1234567",  6 );
    printf( "%s\n", buffer );

    strncpy_s( buffer, sizeof( buffer ), "abcdefg",  3 );
    printf( "%s\n", buffer );

    strncpy_s( buffer, sizeof( buffer ), "*******",  0 );
    printf( "%s\n", buffer );
}
.exmp output
abcdefg
123456
abc
(nothing)
.exmp end
.*
.class TR 24731
.system
