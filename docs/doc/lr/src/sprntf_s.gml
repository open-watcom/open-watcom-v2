.func begin sprintf_s swprintf_s
.funcw swprintf_s
.func gen
#define __STDC_WANT_LIB_EXT1__ 1
#include <stdio.h>
int sprintf_s( char * restrict s, rsize_t n
         const char * restrict format, ... );
.ixfunc2 '&StrIo' &func
.if &'length(&wfunc.) ne 0 .do begin
#include <wchar.h>
int swprintf_s( char * restrict s, rsize_t n,
       const wchar_t * restrict format, ... );
.ixfunc2 '&StrIo' &wfunc
.ixfunc2 '&Wide' &wfunc
.do end
.if &'length(&ufunc.) ne 0 .do begin
int _usprintf_s( char * restrict s, rsize_t n,
        const wchar_t * restrict format, ... );
.ixfunc2 '&StrIo' &ufunc
.do end
.func end
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
.kw sprintf
function except for the explicit runtime-constraints listed above.
.np
The &func function, unlike
.kw snprintf_s
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
.if &'length(&ufunc.) ne 0 .do begin
.np
The &ufunc Unicode function is identical to &func except that it
accepts a Unicode string argument for
.arg format
and produces Unicode character output.
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
.im seeprtf sprintf_s
.see end
.*
.exmp begin
#define __STDC_WANT_LIB_EXT1__ 1
#include <stdio.h>

/* Create temporary file names using a counter */

char namebuf[13];
int  TempCount = 0;
.exmp break
char *make_temp_name( void )
{
    sprintf_s( namebuf, sizeof( namebuf ),
               "zz%.6o.tmp", TempCount++ );
    return( namebuf );
}
.exmp break
void main( void )
{
    FILE *tf1, *tf2;
.exmp break
    tf1 = fopen( make_temp_name(), "w" );
    tf2 = fopen( make_temp_name(), "w" );
    fputs( "temp file 1", tf1 );
    fputs( "temp file 2", tf2 );
    fclose( tf1 );
    fclose( tf2 );
}
.exmp end
.*
.class TR 24731
.system
