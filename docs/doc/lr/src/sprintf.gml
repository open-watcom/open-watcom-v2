.func begin sprintf
.func2 swprintf ISO C95
.func end
.synop begin
#include <stdio.h>
int sprintf( char *buf, const char *format, ... );
.ixfunc2 '&String' &funcb
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
The
.id &funcb.
function is equivalent to the
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
.im widefun4
.if &'length(&wfunc.) ne 0 .do begin
The argument
.arg buf
specifies an array of wide characters into which the generated output
is to be written, rather than converted to multibyte characters and
written to a stream.
The maximum number of wide characters to write, including a
terminating null wide character, is specified by
.arg n
.ct .li .
.do end
.desc end
.*
.return begin
The
.id &funcb.
function returns the number of characters written into the
array, not counting the terminating null character.
An error can occur while converting a value for output.
.if &'length(&wfunc.) ne 0 .do begin
The
.id &wfunc.
function returns the number of wide characters written into
the array, not counting the terminating null wide character, or a
negative value if
.arg n
or more wide characters were requested to be generated.
.do end
.im errnoref
.return end
.*
.see begin
.im seeprtf
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
.class ISO C
.system
