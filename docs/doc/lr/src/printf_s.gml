.func begin printf_s wprintf_s
.funcw wprintf_s
.func end
.synop begin
#define __STDC_WANT_LIB_EXT1__ 1
#include <stdio.h>
int printf_s( const char * restrict format, ... );
.ixfunc2 '&StrIo' &func
.if &'length(&wfunc.) ne 0 .do begin
#include <wchar.h>
int wprintf_s( const wchar_t * restrict format, ... );
.ixfunc2 '&StrIo' &wfunc
.ixfunc2 '&Wide' &wfunc
.do end
.synop end
.*
.rtconst begin
The
.arg format
argument shall not be a null pointer. The
.mono %n
specifier (modified or not by flags, field width, or precision) shall not
appear in the string pointed to by
.arg format
.ct .li .
Any argument to
.id &func.
corresponding to a
.mono %s
specifier shall not be a null pointer.
.np
If there is a runtime-constraint violation, the
.id &func.
function does not
attempt to produce further output, and it is unspecified to what extent
.id &func.
produced output before discovering the runtime-constraint violation.
.rtconst end
.*
.desc begin
The
.id &func.
function is equivalent to the
.kw printf
function except for the explicit runtime-constraints listed above.
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
negative value if an output error or runtime-constraint violation occurred.
.if &'length(&wfunc.) ne 0 .do begin
.np
The
.id &wfunc.
function returns the number of wide characters written, or a
negative value if an output error or runtime-constraint violation occurred.
.do end
.return end
.*
.see begin
.im seeprtf printf_s
.see end
.*
.exmp begin
#define __STDC_WANT_LIB_EXT1__ 1
#include <stdio.h>

void main( void )
{
    char *weekday, *month;
.exmp break
    weekday = "Saturday";
    month = "April";
    printf_s( "%s, %s %d, %d\n",
              weekday, month, 18, 1987 );
    printf_s( "f1 = %8.4f f2 = %10.2E x = %#08x i = %d\n",
              23.45, 3141.5926, 0x1db, -1 );
}
.exmp output
Saturday, April 18, 1987
f1 =  23.4500 f2 =  3.14E+003 x = 0x0001db i = -1
.exmp end
.*
.class TR 24731
.system
