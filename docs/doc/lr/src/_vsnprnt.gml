.func _vsnprintf _vsnwprintf
.synop begin
#include <stdarg.h>
#include <stdio.h>
int _vsnprintf( char *buf,
		size_t count,
		const char *format,
		va_list arg );
.ixfunc2 '&String' &funcb
.if &'length(&wfunc.) ne 0 .do begin
#include <stdarg.h>
#include <wchar.h>
int _vsnwprintf( wchar_t *buf,
		size_t count,
		const wchar_t *format,
		va_list arg );
.ixfunc2 '&String' &wfunc
.ixfunc2 '&Wide' &wfunc
.do end
.synop end
.desc begin
The
.id &funcb.
function formats data under control of the
.arg format
control string and stores the result in
.arg buf
.ct .li .
The maximum number of characters to store is specified by
.arg count
.ct .li .
A null character is placed at the end of the generated character
string if fewer than
.arg count
characters were stored.
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
The maximum number of wide characters to write is specified by
.arg count
.ct .li .
A null wide character is placed at the end of the generated wide
character string if fewer than
.arg count
wide characters were stored.
.do end
.desc end
.return begin
The
.id &funcb.
function returns the number of characters written into the
array, not counting the terminating null character, or a negative
value if more than
.arg count
characters were requested to be generated.
An error can occur while converting a value for output.
.if &'length(&wfunc.) ne 0 .do begin
The
.id &wfunc.
function returns the number of wide characters written into
the array, not counting the terminating null wide character, or a
negative value if more than
.arg count
wide characters were requested to be generated.
.do end
.im errnoref
.return end
.see begin
.im seevprtf
.see end
.exmp begin
.blktext begin
The following shows the use of
.id &funcb.
in a general error message routine.
.blktext end
.blkcode begin
#include <stdio.h>
#include <stdarg.h>
#include <string.h>

char msgbuf[80];
.exmp break
char *fmtmsg( char *format, ... )
  {
    va_list arglist;
.exmp break
    va_start( arglist, format );
    strcpy( msgbuf, "Error: " );
    _vsnprintf( &msgbuf[7], 80-7, format, arglist );
    va_end( arglist );
    return( msgbuf );
  }
.exmp break
void main()
  {
    char *msg;

    msg = fmtmsg( "%s %d %s", "Failed", 100, "times" );
    printf( "%s\n", msg );
  }
.blkcode end
.exmp end
.class WATCOM
.system
