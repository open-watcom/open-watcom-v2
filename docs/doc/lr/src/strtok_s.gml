.func strtok_s wcstok_s
.synop begin
#define __STDC_WANT_LIB_EXT1__ 1
#include <string.h>
char *strtok_s( char * restrict s1,
                rsize_t * restrict s1max,
                const char * restrict s2,
                char ** restrict ptr);
.ixfunc2 '&String' &func
.ixfunc2 '&Search' &func
.if &'length(&wfunc.) ne 0 .do begin
#include <wchar.h>
wchar_t *wcstok_s( wchar_t * restrict s1,
                   rsize_t * restrict s1max,
                   const wchar_t * restrict s2,
                   wchar_t ** restrict ptr);
.ixfunc2 '&String' &wfunc
.ixfunc2 '&Search' &wfunc
.ixfunc2 '&Wide' &wfunc
.do end
.synop end
.*
.rtconst begin
None of
.arg s1max
.ct ,
.arg s2
.ct , or
.arg ptr
shall be a null pointer. If
.arg s1
is a null pointer,then
.arg *ptr
shall not be a null pointer. The value of
.arg *s1max
shall not be greater than
.kw RSIZE_MAX
.ct .li .
The end of the token found shall occur within the first
.arg *s1max
characters of
.arg s1
for the first call, and shall occur within the first
.arg *s1max
characters of where searching resumes on subsequent calls.
If there is a runtime-constraint violation, the &func function does not indirect
through the
.arg s1
or
.arg s2
pointers, and does not store a value in the object pointed to by
.arg ptr
.ct .li .
.rtconst end
.*
.desc begin
A sequence of calls to the &func function breaks the string pointed to by
.arg s1
into a sequence of tokens, each of which is delimited by a character
from the string pointed to by
.arg s2
.ct .li .
The fourth argument points to a caller-provided char pointer into which the
&func function stores information necessary for it to continue scanning the same
string.
The first call in a sequence has a non-null first argument and
.arg s1max
points to an object whose value is the number of elements in the
character array pointed to by the first argument.
The first call stores an initial value in the object pointed to by
.arg ptr
and updates the value pointed to by
.arg s1max
to reflect the number of elements that remain in
relation to
.arg ptr
.ct .li .
Subsequent calls in the sequence have a null first argument and the
objects pointed to by
.arg s1max
and
.arg ptr
are required to have the values stored by the
previous call in the sequence, which are then updated. The separator string pointed to by
.arg s2
may be different from call to call.
The first call in the sequence searches the string pointed to by
.arg s1
for the first character that is not contained in the current separator
string pointed to by
.arg s2
.ct .li .
If no such character is found, then there are no tokens in the string pointed to by
.arg s1
and the &func function returns a null pointer. If such a character is found,
it is the start of the first token. The &func function then searches from there for the
first character in
.arg s1
that is contained in the current separator string. If no such character is found, the
current token extends to the end of the string pointed to by
.arg s1
.ct , and subsequent searches in the same string for a token return a null pointer.
If such a character is found, it is overwritten by a null character,
which terminates the current token.
In all cases, the &func function stores sufficient information in the pointer pointed
to by
.arg ptr
so that subsequent calls, with a null pointer for
.arg s1
and the unmodified pointer
value for
.arg ptr
.ct , shall start searching just past the element overwritten by a null character (if any).
.im widefunc
.desc end
.*
.return begin
The &func function returns a pointer to the first character of a token, or a null
pointer if there is no token or there is a runtime-constraint violation.
.return end
.*
.see begin
.seelist &function. strtok strcspn strpbrk
.see end
.*
.exmp begin
#define __STDC_WANT_LIB_EXT1__ 1
#include <stdio.h>
#include <string.h>
.exmp break
void main( void )
{
    char    *p;
    char    *buffer;
    char    *delims = { " .," };
    size_t  buflen;
    char    *ptr;
.exmp break
    buffer = strdup( "Find words, all of them." );
    printf( "%s\n", buffer );
    buflen = strlen( buffer );
    p = strtok_s( buffer, &buflen, delims, &ptr );
    while( p != NULL ) {
      printf( "word: %s\n", p );
      p = strtok_s( NULL, &buflen, delims, &ptr );
    }
    printf( "%s\n", buffer );
}
.exmp output
Find words, all of them.
word: Find
word: words
word: all
word: of
word: them
Find
.exmp end
.*
.class TR 24731
.system
