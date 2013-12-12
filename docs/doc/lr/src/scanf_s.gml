.func begin scanf_s wscanf_s
.funcw wscanf_s
.func end
.synop begin
#define __STDC_WANT_LIB_EXT1__ 1
#include <stdio.h>
int scanf_s( const char * restrict format, ... );
.ixfunc2 '&StrIo' &func
.if &'length(&wfunc.) ne 0 .do begin
#include <wchar.h>
int wscanf_s( const wchar_t * restrict format, ... );
.ixfunc2 '&StrIo' &wfunc
.ixfunc2 '&Wide' &wfunc
.do end
.synop end
.*
.rtconst begin
The
.arg format
argument shall not be a null pointer.
Any argument indirected through in order to store converted input shall
not be a null pointer.
.np
If there is a runtime-constraint violation, the &func function does not
attempt to perform further input, and it is unspecified to what extent
&func performed input before discovering the runtime-constraint violation.
.rtconst end
.*
.desc begin
The &func function is equivalent to
.kw fscanf_s
with the argument
.arg stdin
interposed before the arguments to &func.
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
The &func function returns
.kw EOF
if an input failure occurred before any conversion or if there was
a runtime-constraint violation.
Otherwise, the &func function returns the number of input items
successfully assigned, which can be fewer than provided for, or even zero.
.np
When a file input error occurs, the
.kw errno
global variable may be set.
.return end
.*
.see begin
.im seevscnf scanf_s
.see end
.*
.exmp begin
.blktext begin
To scan a date in the form "Friday August 13 2004":
.blktext end
.blkcode begin
#define __STDC_WANT_LIB_EXT1__ 1
#include <stdio.h>

void main( void )
{
    int day, year;
    char weekday[10], month[10];
.exmp break
    scanf_s( "%s %s %d %d",
             weekday, sizeof( weekday ),
             month, sizeof( month ),
             &day, &year );
}
.blkcode end
.exmp end
.*
.class TR 24731
.system
