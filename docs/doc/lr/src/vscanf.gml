.func vscanf vwscanf
.funcw vwscanf
#include <stdarg.h>
#include <stdio.h>
int vscanf( const char *format,
            va_list arg );
.ixfunc2 '&StrIo' &func
.if &'length(&wfunc.) ne 0 .do begin
#include <stdarg.h>
#include <wchar.h>
int vwscanf( const wchar_t *format,
              va_list arg );
.ixfunc2 '&StrIo' &wfunc
.ixfunc2 '&Wide' &wfunc
.do end
.funcend
.*
.safealt
.*
.desc begin
The &func function scans input from the file designated by
.arg stdin
under control of the argument
.arg format
.ct .li .
The
.arg format
string is described under the description of the
.kw scanf
function.
.np
The &func function is equivalent to the
.kw scanf
function, with a variable argument list replaced with
.arg arg
.ct , which has been initialized using the
.kw va_start
macro.
.if &'length(&wfunc.) ne 0 .do begin
.np
The &wfunc function is identical to &func except that it
accepts a wide-character string argument for
.arg format
.ct .li .
.do end
.desc end
.*
.return begin
The &func function returns
.kw EOF
if an input failure occurred before any conversion.
values were successfully scanned and stored is returned.
.return end
.*
.see begin
.im seevscnf vscanf
.see end
.*
.exmp begin
#include <stdio.h>
#include <stdarg.h>

void find( char *format, ... )
{
    va_list arglist;
.exmp break
    va_start( arglist, format );
    vscanf( format, arglist );
    va_end( arglist );
}
.exmp break
void main( void )
{
    int day, year;
    char weekday[10], month[10];
.exmp break
    find( "%s %s %d %d",
            weekday, month, &day, &year );
    printf( "\n%s, %s %d, %d\n",
            weekday, month, day, year );
}
.exmp end
.class ISO C99
.system
