.func fprintf fwprintf _ufprintf
.funcw fwprintf
#include <stdio.h>
int fprintf( FILE *fp, const char *format, ... );
.ixfunc2 '&StrIo' &func
.if &'length(&wfunc.) ne 0 .do begin
#include <stdio.h>
#include <wchar.h>
int fwprintf( FILE *fp, const wchar_t *format, ... );
.ixfunc2 '&StrIo' &wfunc
.ixfunc2 '&Wide' &wfunc
.do end
.if &'length(&ufunc.) ne 0 .do begin
int _ufprintf( FILE *fp, const wchar_t *format, ... );
.ixfunc2 '&StrIo' &ufunc
.do end
.funcend
.*
.safealt
.*
.desc begin
The &func function writes output to the file pointed to by
.arg fp
under control of the argument
.arg format
.ct .li .
The
.arg format
string is described under the description of the
.kw printf
function.
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
and writes Unicode characters to the file.
.do end
.desc end
.*
.return begin
The &func function returns the number of characters written, or a
negative value if an output error occurred.
.if &'length(&wfunc.) ne 0 .do begin
The &wfunc function returns the number of wide characters written, or a
negative value if an output error occurred.
.do end
.im errnoref
.return end
.*
.see begin
.im seeprtf fprintf
.see end
.*
.exmp begin
#include <stdio.h>

char *weekday = { "Saturday" };
char *month = { "April" };

void main( void )
{
    fprintf( stdout, "%s, %s %d, %d\n",
          weekday, month, 18, 1987 );
}
.exmp output
Saturday, April 18, 1987
.exmp end
.class ANSI
.system
