.func vprintf vwprintf
.synop begin
.funcw vwprintf
#include <stdarg.h>
#include <stdio.h>
int vprintf( const char *format, va_list arg );
.ixfunc2 '&StrIo' &func
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
The &func function writes output to the file
.kw stdout
under control of the argument
.arg format
.ct .li .
The
.arg format
string is described under the description of the
.kw printf
function.
The &func function is equivalent to the
.kw printf
function, with the variable argument list replaced with
.arg arg
.ct , which has been initialized by the
.kw va_start
macro.
.if &'length(&wfunc.) ne 0 .do begin
.np
The &wfunc function is identical to &func except that it accepts a
wide-character string argument for
.arg format
.ct .li .
.do end
.desc end
.*
.return begin
The &func function returns the number of characters written, or a
negative value if an output error occurred.
.if &'length(&wfunc.) ne 0 .do begin
The &wfunc function returns the number of wide characters written, or
a negative value if an output error occurred.
.do end
.im errnoref
.return end
.*
.see begin
.im seevprtf vprintf
.see end
.*
.exmp begin
.blktext begin
The following shows the use of &func in a general error message
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
.class ANSI
.system
