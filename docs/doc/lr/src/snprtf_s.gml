.func begin snprintf_s snwprintf_s
.funcw snwprintf_s
.func gen
#define __STDC_WANT_LIB_EXT1__ 1
#include <stdio.h>
int snprintf_s( char * restrict s, rsize_t n
          const char * restrict format, ... );
.ixfunc2 '&StrIo' &func
.if &'length(&wfunc.) ne 0 .do begin
#include <wchar.h>
int snwprintf_s( char * restrict s, rsize_t n,
        const wchar_t * restrict format, ... );
.ixfunc2 '&StrIo' &wfunc
.ixfunc2 '&Wide' &wfunc
.do end
.if &'length(&ufunc.) ne 0 .do begin
int _usnprintf_s( char * restrict s, rsize_t n,
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
.kw snprintf
function except for the explicit runtime-constraints listed above.
.np
The &func function, unlike
.kw sprintf_s
.ct , will truncate the result to fit within the array pointed to by
.arg s
.ct .li .
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
The &func function returns the number of characters that would have been
written had
.arg n
been sufficiently large, not counting the terminating null character, or a
negative value if a runtime-constraint violation occurred. Thus, the
null-terminated output has been completely written if and only if the
returned value is nonnegative and less than
.arg n
.ct .li .
.if &'length(&wfunc.) ne 0 .do begin
.np
The &wfunc function returns the number of wide characters that would have been
written had
.arg n
been sufficiently large, not counting the terminating wide null character, or
a negative value if a runtime-constraint violation occurred. Thus, the
null-terminated output has been completely written if and only if the
returned value is nonnegative and less than
.arg n
.ct .li .
.return end
.*
.see begin
.im seeprtf snprintf_s
.see end
.*
.exmp begin
#define __STDC_WANT_LIB_EXT1__ 1
#include <stdio.h>
#include <stdlib.h>

/* Format output into a buffer after determining its size */

void main( void )
{
    int     bufsize;
    char    *buffer;

    bufsize = snprintf( NULL, 0, "%3d %P", 42, 42 ) + 1;
    buffer  = malloc( bufsize );
    snprintf_s( buffer, bufsize, "%3d %P", 42, 42 );
    free( buffer );
}
.exmp end
.*
.class TR 24731
.system
