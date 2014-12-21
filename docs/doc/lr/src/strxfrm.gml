.func strxfrm wcsxfrm
.synop begin
#include <string.h>
size_t strxfrm( char *dst,
                const char *src,
                size_t n );
.ixfunc2 '&String' &funcb
.ixfunc2 '&Compare' &funcb
.if &'length(&wfunc.) ne 0 .do begin
#include <wchar.h>
size_t wcsxfrm( wchar_t *dst,
                const wchar_t *src,
                size_t n );
.ixfunc2 '&String' &wfunc
.ixfunc2 '&Search' &wfunc
.ixfunc2 '&Wide' &wfunc
.do end
.synop end
.desc begin
The
.id &funcb.
function transforms, for no more than
.arg n
characters, the string pointed to by
.arg src
to the buffer pointed to by
.arg dst
.ct .li .
The transformation uses the collating sequence selected by the
.kw setlocale
function so that two transformed strings will compare identically
(using the
.kw strncmp
function)
to a comparison of the original two strings using the
.kw strcoll
function.
The function will be equivalent to the
.kw strncpy
function (except there is no padding of the
.arg dst
argument with null characters when the argument
.arg src
is shorter than
.arg n
characters)
when the collating sequence is selected from the
.mono "C"
locale.
.im widefun1
.if &'length(&wfunc.) ne 0 .do begin
For &wfunc, after the string transformation, a call to
.kw wcscmp
with the two transformed strings yields results identical to those of a
call to
.kw wcscoll
applied to the original two strings.
.id &wfunc.
and
.id &funcb.
behave identically otherwise.
.do end
.desc end
.return begin
The
.id &funcb.
function returns the length of the transformed string.
If this length is more than
.arg n
.ct , the contents of the array pointed to by
.arg dst
are indeterminate.
.return end
.see begin
.seelist strxfrm setlocale strcoll
.see end
.exmp begin
#include <stdio.h>
#include <string.h>
#include <locale.h>

char src[] = { "A sample STRING" };
char dst[20];

void main()
  {
    size_t len;
.exmp break
    setlocale( LC_ALL, "C" );
    printf( "%s\n", src );
    len = strxfrm( dst, src, 20 );
    printf( "%s (%u)\n", dst, len );
  }
.exmp output
A sample STRING
A sample STRING (15)
.exmp end
.class ISO C
.system
