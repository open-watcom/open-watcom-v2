.func vfscanf vfwscanf _uvfscanf
.funcw vfwscanf
#include <stdio.h>
#include <stdarg.h>
int vfscanf( FILE *fp,
             const char *format,
             va_list arg );
.ixfunc2 '&StrIo' &func
.if &'length(&wfunc.) ne 0 .do begin
int vfwscanf( FILE *fp,
              const wchar_t *format,
              va_list arg );
.ixfunc2 '&StrIo' &wfunc
.ixfunc2 '&Wide' &wfunc
.do end
.if &'length(&ufunc.) ne 0 .do begin
int _uvfscanf( FILE *fp,
               const wchar_t *format,
               va_list arg );
.ixfunc2 '&StrIo' &ufunc
.do end
.funcend
.*
.safealt
.*
.desc begin
The &func function scans input from the file designated by
.arg fp
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
.kw fscanf
function, with a variable argument list replaced with
.arg arg
.ct , which has been initialized using the
.kw va_start
macro.
.if &'length(&wfunc.) ne 0 .do begin
.np
The &wfunc function is identical to &func except that it accepts a
wide-character string argument for
.arg format
.ct .li .
.do end
.if &'length(&ufunc.) ne 0 .do begin
.np
The &ufunc Unicode function is identical to &func except that it
accepts a Unicode string argument for
.arg format
and the input consists of 16-bit Unicode characters.
.do end
.desc end
.*
.return begin
The &func function returns
.kw EOF
if an input failure occurred before any conversion.
Otherwise, the number of input arguments for which values were
successfully scanned and stored is returned.
When a file input error occurs, the
.kw errno
global variable may be set.
.return end
.*
.see begin
.im seevscnf vfscanf
.see end
.*
.exmp begin
#include <stdio.h>
#include <stdarg.h>
.exmp break
void ffind( FILE *fp, char *format, ... )
{
    va_list arglist;

    va_start( arglist, format );
    vfscanf( fp, format, arglist );
    va_end( arglist );
}
.exmp break
void main( void )
{
    int day, year;
    char weekday[10], month[10];
.exmp break
    ffind( stdin,
            "%s %s %d %d",
            weekday, month, &day, &year );
    printf( "\n%s, %s %d, %d\n",
            weekday, month, day, year );
}
.exmp end
.*
.class ISO C99
.system
