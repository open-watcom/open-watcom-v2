.func begin fprintf_s fwprintf_s
.funcw fwprintf_s
.func gen
#define __STDC_WANT_LIB_EXT1__ 1
#include <stdio.h>
int fprintf_s( FILE * restrict stream,
         const char * restrict format, ... );
.ixfunc2 '&StrIo' &func
.if &'length(&wfunc.) ne 0 .do begin
#include <wchar.h>
int fwprintf_s( FILE * restrict stream.
       const wchar_t * restrict format, ... );
.ixfunc2 '&StrIo' &wfunc
.ixfunc2 '&Wide' &wfunc
.do end
.if &'length(&ufunc.) ne 0 .do begin
int _uprintf_s( FILE * restrict stream,
     const wchar_t * restrict format, ... );
.ixfunc2 '&StrIo' &ufunc
.do end
.func end
.*
.rtconst begin
Neither
.arg stream
nor
.arg format
shall be a null pointer. The
.mono %n
specifier (modified or not by flags, field width, or precision) shall not
appear in the string pointed to by
.arg format
.ct .li .
Any argument to &func corresponding to a
.mono %s
specifier shall not be a null pointer.
.np
If there is a runtime-constraint violation, the &func function does not
attempt to produce further output, and it is unspecified to what extent
&func produced output before discovering the runtime-constraint violation.
.rtconst end
.*
.desc begin
The &func function is equivalent to the
.kw fprintf
function except for the explicit runtime-constraints listed above.
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
and writes Unicode characters to the file
.kw stdout
.ct .li .
.do end
.desc end
.*
.return begin
The &func function returns the number of characters written, or a
negative value if an output error or runtime-constraint violation occurred.
.if &'length(&wfunc.) ne 0 .do begin
.np
The &wfunc function returns the number of wide characters written, or a
negative value if an output error or runtime-constraint violation occurred.
.do end
.return end
.*
.see begin
.im seeprtf fprintf_s
.see end
.*
.exmp begin
#define __STDC_WANT_LIB_EXT1__ 1
#include <stdio.h>

char *weekday = { "Friday" };
char *month = { "August" };

void main( void )
{
    fprintf_s( stdout, "%s, %s %d, %d\n",
               weekday, month, 13, 2004 );
}
.exmp output
Friday, August 13, 2004
.exmp end
.*
.class TR 24731
.system
