.func sprintf swprintf
.funcw swprintf
#include <stdio.h>
int sprintf( char *buf, const char *format, ... );
.ixfunc2 '&String' &func
.if &'length(&wfunc.) ne 0 .do begin
#include <wchar.h>
int swprintf( wchar_t *buf,
              size_t n,
              const wchar_t *format, ... );
.ixfunc2 '&String' &wfunc
.ixfunc2 '&Wide &wfunc
.do end
.synop end
.*
.safealt
.*
.desc begin
The &func function is equivalent to the
.kw fprintf
function, except that the argument
.arg buf
specifies a character array into which the generated output is placed,
rather than to a file.
A null character is placed at the end of the generated character
string.
The
.arg format
string is described under the description of the
.kw printf
function.
.if &'length(&wfunc.) ne 0 .do begin
.np
The &wfunc function is identical to &func except that the argument
.arg buf
specifies an array of wide characters into which the generated output
is to be written, rather than converted to multibyte characters and
written to a stream.
The maximum number of wide characters to write, including a
terminating null wide character, is specified by
.arg n
.ct .li .
The &wfunc function accepts a wide-character string argument for
.arg format
.do end
.desc end
.*
.return begin
The &func function returns the number of characters written into the
array, not counting the terminating null character.
An error can occur while converting a value for output.
.if &'length(&wfunc.) ne 0 .do begin
The &wfunc function returns the number of wide characters written into
the array, not counting the terminating null wide character, or a
negative value if
.arg n
or more wide characters were requested to be generated.
.do end
.im errnoref
.return end
.*
.see begin
.im seeprtf sprintf
.see end
.*
.exmp begin
#include <stdio.h>

/* Create temporary file names using a counter */

char namebuf[13];
int  TempCount = 0;
.exmp break
char *make_temp_name( void )
{
    sprintf( namebuf, "zz%.6o.tmp", TempCount++ );
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
.class ANSI
.system
