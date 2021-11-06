.func vsscanf vswscanf
.synop begin
#include <stdio.h>
#include <stdarg.h>
int vsscanf( const char *s,
             const char *format,
             va_list arg );
.ixfunc2 '&String' &funcb
.if &'length(&wfunc.) ne 0 .do begin
int vswscanf( const wchar_t *s,
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
function scans input from the string designated by
.arg s
under control of the argument
.arg format
.period
The
.arg format
string is described under the description of the
.reffunc scanf
function.
.pp
The
.id &funcb.
function is equivalent to the
.reffunc sscanf
function, with a variable argument list replaced with
.arg arg
.ct , which has been initialized using the
.reffunc va_start
macro.
.if &'length(&wfunc.) ne 0 .do begin
.np
The
.id &wfunc.
function is identical to
.id &funcb.
except that it accepts a
wide character string argument for
.arg format
.period
.do end
.desc end
.*
.return begin
The
.id &funcb.
function returns
.kw EOF
if the end of the input string was reached before any conversion.
Otherwise, the number of input arguments for which
values were successfully scanned and stored is returned.
.return end
.see begin
.im seevscnf
.see end
.*
.exmp begin
#include <stdio.h>
#include <stdarg.h>
.exmp break
void sfind( char *string, char *format, ... )
{
    va_list arglist;
.exmp break
    va_start( arglist, format );
    vsscanf( string, format, arglist );
    va_end( arglist );
}
.exmp break
void main( void )
{
    int day, year;
    char weekday[10], month[10];
.exmp break
    sfind( "Saturday April 18 1987",
            "%s %s %d %d",
            weekday, month, &day, &year );
    printf( "\n%s, %s %d, %d\n",
            weekday, month, day, year );
}
.exmp end
.*
.class ISO C99
.system
