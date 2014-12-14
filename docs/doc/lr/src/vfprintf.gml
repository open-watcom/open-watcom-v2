.func vfprintf vfwprintf
.synop begin
.funcw vfwprintf
#include <stdarg.h>
#include <stdio.h>
int vfprintf( FILE *fp,
              const char *format,
              va_list arg );
.ixfunc2 '&StrIo' &func
.if &'length(&wfunc.) ne 0 .do begin
#include <stdarg.h>
#include <stdio.h>
#include <wchar.h>
int vfwprintf( FILE *fp,
               const wchar_t *format,
               va_list arg );
.ixfunc2 '&StrIo' &wfunc
.ixfunc2 '&Wide' &wfunc
.do end
.synop end
.*
.safealt
.*
.desc begin
The
.id &func.
function writes output to the file pointed to by
.arg fp
under control of the argument
.arg format
.ct .li .
The
.arg format
string is described under the description of the
.kw printf
function.
The
.id &func.
function is equivalent to the
.kw fprintf
function, with the variable argument list replaced with
.arg arg
.ct , which has been initialized by the
.kw va_start
macro.
.if &'length(&wfunc.) ne 0 .do begin
.np
The
.id &wfunc.
function is identical to
.id &func.
except that it accepts a
wide-character string argument for
.arg format
.ct .li .
.do end
.desc end
.*
.return begin
The
.id &func.
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
#include <stdio.h>
#include <stdarg.h>

FILE *LogFile;
.exmp break
/* a general error routine */

void errmsg( char *format, ... )
{
    va_list arglist;
.exmp break
    fprintf( stderr, "Error: " );
    va_start( arglist, format );
    vfprintf( stderr, format, arglist );
    va_end( arglist );
    if( LogFile != NULL ) {
        fprintf( LogFile, "Error: " );
        va_start( arglist, format );
        vfprintf( LogFile, format, arglist );
        va_end( arglist );
    }
}
.exmp break
void main( void )
{
    LogFile = fopen( "error.log", "w" );
    errmsg( "%s %d %s", "Failed", 100, "times" );
}
.exmp end
.*
.class ANSI
.system
