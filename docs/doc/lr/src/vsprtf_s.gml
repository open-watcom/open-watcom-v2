.func begin vsprintf_s vswprintf_s
.funcw vswprintf_s
.func end
.synop begin
#define __STDC_WANT_LIB_EXT1__ 1
#include <stdarg.h>
#include <stdio.h>
int vsprintf_s( char * restrict s, rsize_t n
          const char * restrict format, va_list arg );
.ixfunc2 '&StrIo' &func
.if &'length(&wfunc.) ne 0 .do begin
#include <stdarg.h>
#include <wchar.h>
int vswprintf_s( char * restrict s, rsize_t n,
        const wchar_t * restrict format, va_list arg );
.ixfunc2 '&StrIo' &wfunc
.ixfunc2 '&Wide' &wfunc
.do end
.synop end
.*
.rtconst begin
Neither
.arg s
nor
.arg format
shall be a null pointer. The
.arg n
argument shall neither equal zero nor be greater than
.mono RSIZE_MAX.
The number of characters (including the trailing null) required for the
result to be written to the array pointed to by
.arg s
shall not be greater than
.arg n
.ct .li .
The
.mono %n
specifier (modified or not by flags, field width, or precision) shall not
appear in the string pointed to by
.arg format
.ct .li .
Any argument to &func corresponding to a
.mono %s
specifier shall not be a null pointer. No encoding error shall occur.
.np
If there is a runtime-constraint violation, then if
.arg s
is not a null pointer and
.arg n
is greater than zero and less than
.mono RSIZE_MAX,
then the &func function sets
.arg s[0]
to the null character.
.rtconst end
.*
.desc begin
The &func function is equivalent to the
.kw vsprintf
function except for the explicit runtime-constraints listed above.
.np
The &func function, unlike
.kw vsnprintf_s
.ct , treats a result too big for the array pointed to by
.arg s
as a runtime-constraint violation.
.if &'length(&wfunc.) ne 0 .do begin
.np
The &wfunc function is identical to &func except that it accepts a
wide-character string argument for
.arg format
and produces wide character output.
.do end
.desc end
.*
.return begin
If no runtime-constraint violation occurred, the &func function returns the
number of characters written in the array, not counting the terminating null
character. If an encoding error occurred, &func returns a negative value. If
any other runtime-constraint violation occurred, &func returns zero.
.if &'length(&wfunc.) ne 0 .do begin
.np
If no runtime-constraint violation occurred, the &wfunc function returns the
number of wide characters written in the array, not counting the terminating
null wide character. If an encoding error occurred or if
.arg n
or more wide characters are requested to be written, &wfunc returns a negative
value. If any other runtime-constraint violation occurred, &wfunc returns zero.
.do end
.return end
.*
.see begin
.im seeprtf vsprintf_s
.see end
.*
.exmp begin
.blktext begin
The following shows the use of &func in a general error message routine.
.blktext end
.blkcode begin
#define __STDC_WANT_LIB_EXT1__ 1
#include <stdio.h>
#include <stdarg.h>
#include <string.h>

char msgbuf[80];
.exmp break
char *fmtmsg( char *format, ... )
{
    va_list arglist;
.exmp break
    va_start( arglist, format );
    strcpy_s( msgbuf, sizeof( buffer ), "Error: " );
    vsprintf_s( &msgbuf[7], sizeof( msgbuf ) - 7,
                format, arglist );
    va_end( arglist );
    return( msgbuf );
}
.exmp break
void main( void )
{
    char *msg;

    msg = fmtmsg( "%s %d %s", "Failed", 100, "times" );
    printf( "%s\n", msg );
}
.blkcode end
.exmp end
.*
.class TR 24731
.system
