.func begin vprintf
.func2 vwprintf ISO C95
.func end
.synop begin
#include <stdarg.h>
#include <stdio.h>
int vprintf( const char *format, va_list arg );
.ixfunc2 '&StrIo' &funcb
.if &'length(&wfunc.) ne 0 .do begin
#include <stdarg.h>
#include <wchar.h>
int vwprintf( const wchar_t *format, va_list arg );
.ixfunc2 '&StrIo' &wfunc
.ixfunc2 '&Wide' &wfunc
.do end
.synop end
.*
.safealt
.*
.desc begin
The
.id &funcb.
function writes output to the file
.kw stdout
under control of the argument
.arg format
.period
The
.arg format
string is described under the description of the
.reffunc printf
function.
The
.id &funcb.
function is equivalent to the
.reffunc printf
function, with the variable argument list replaced with
.arg arg
.ct , which has been initialized by the
.reffunc va_start
macro.
.widefunc &wfunc. &funcb. <form>
.desc end
.*
.return begin
The
.id &funcb.
function returns the number of characters written, or a
negative value if an output error occurred.
.if &'length(&wfunc.) ne 0 .do begin
The
.id &wfunc.
function returns the number of wide characters written, or
a negative value if an output error occurred.
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
in a general error message
routine.
.blktext end
.blkcode begin
#include <stdio.h>
#include <stdarg.h>
.exmp break
void errmsg( char *format, ... )
{
    va_list arglist;

    printf( "Error: " );
    va_start( arglist, format );
    vprintf( format, arglist );
    va_end( arglist );
}
.exmp break
void main( void )
{
    errmsg( "%s %d %s", "Failed", 100, "times" );
}
.blkcode end
.exmp output
Error: Failed 100 times
.exmp end
.*
.class ISO C
.system
