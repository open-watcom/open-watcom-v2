.func snprintf snwprintf
.synop begin
#include <stdio.h>
int snprintf( char *buf,
              size_t count,
              const char *format, ... );
.ixfunc2 '&String' &funcb
.if &'length(&wfunc.) ne 0 .do begin
#include <wchar.h>
int snwprintf( wchar_t *buf,
               size_t count,
               const wchar_t *format, ... );
.ixfunc2 '&String' &wfunc
.ixfunc2 '&Wide &wfunc
.do end
.synop end
.*
.safealt
.*
.desc begin
The
.id &funcb.
function is equivalent to the
.kw fprintf
function, except that the argument
.arg buf
specifies a character array into which the generated output is placed,
rather than to a file.
A null character is placed at the end of the generated character
string.
The maximum number of characters to store, including a terminating
null character, is specified by
.arg count
.ct .li .
The
.arg format
string is described under the description of the
.kw printf
function.
.im widefun4
.if &'length(&wfunc.) ne 0 .do begin
The argument
.arg buf
specifies an array of wide characters into which the generated output
is to be written, rather than converted to multibyte characters and
written to a stream.
The maximum number of wide characters to store, including a
terminating null wide character, is specified by
.arg count
.ct .li .
.do end
.desc end
.*
.return begin
The
.id &funcb.
function returns the number of characters that would have been
written had
.arg count
been sufficiently large, not counting the terminating null
character, or a negative value if an encoding error occurred.
Thus, the null-terminated output has been completely written if and only
if the returned value is nonnegative and less than
.arg count
.ct .li .
.if &'length(&wfunc.) ne 0 .do begin
The
.id &wfunc.
function returns the number of wide characters that would have
been written had
.arg count
been sufficiently large, not counting the terminating null wide character,
or a negative value if an encoding error occurred.
Thus, the null-terminated output has been completely written if and only
if the returned value is nonnegative and less than
.arg count
.ct .li .
.do end
.im errnoref
.return end
.*
.see begin
.im seeprtf
.see end
.*
.exmp begin
#include <stdio.h>
#include <stdlib.h>

/* Format output into a buffer after determining its size */

void main( void )
{
    int     bufsize;
    char    *buffer;

    bufsize = snprintf( NULL, 0, "%3d %P", 42, 42 );
    buffer  = malloc( bufsize + 1 );
    snprintf( buffer, bufsize + 1, "%3d %P", 42, 42 );
    free( buffer );
}
.exmp end
.*
.class ISO C
.system
