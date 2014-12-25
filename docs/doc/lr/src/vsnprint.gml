.func vsnprintf vsnwprintf
.synop begin
#include <stdarg.h>
#include <stdio.h>
int vsnprintf( char *buf,
               size_t count,
               const char *format,
               va_list arg );
.ixfunc2 '&String' &funcb
.if &'length(&wfunc.) ne 0 .do begin
#include <stdarg.h>
#include <wchar.h>
int vsnwprintf( wchar_t *buf,
                size_t count,
                const wchar_t *format,
                va_list arg );
.ixfunc2 '&String' &wfunc
.ixfunc2 '&Wide' &wfunc
.do end
.synop end
.*
.safealt
.*
.desc begin
The
.id &funcb.
function formats data under control of the
.arg format
control string and stores the result in
.arg buf
.ct .li .
The maximum number of characters to store, including a terminating
null character, is specified by
.arg count
.ct .li .
The
.arg format
string is described under the description of the
.kw printf
function.
The
.id &funcb.
function is equivalent to the
.kw _snprintf
function, with the variable argument list replaced with
.arg arg
.ct , which has been initialized by the
.kw va_start
macro.
.im widefun4
.if &'length(&wfunc.) ne 0 .do begin
The argument
.arg buf
specifies an array of wide characters into which the generated output
is to be written, rather than converted to multibyte characters and
written to a stream.
The maximum number of wide characters to write, including a
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
.im seevprtf
.see end
.*
.exmp begin
.blktext begin
The following shows the use of
.id &funcb.
in a general error message routine.
.blktext end
.blkcode begin
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>

char *fmtmsg( char *format, ... )
{
    char    *msgbuf;
    int     len;
    va_list arglist;

    va_start( arglist, format );
    len = vsnprintf( NULL, 0, format, arglist );
    va_end( arglist );
    len = len + 1 + 7;
    msgbuf = malloc( len );
    strcpy( msgbuf, "Error: " );
    va_start( arglist, format );
    vsnprintf( &msgbuf[7], len, format, arglist );
    va_end( arglist );
    return( msgbuf );
}

void main( void )
{
    char *msg;

    msg = fmtmsg( "%s %d %s", "Failed", 100, "times" );
    printf( "%s\n", msg );
    free( msg );
}
.blkcode end
.exmp end
.*
.class ISO C
.system
