.func _snprintf _snwprintf
.synop begin
.funcw _snwprintf
#include <stdio.h>
int _snprintf( char *buf,
	       size_t count,
	       const char *format, ... );
.ixfunc2 '&String' &func
.if &'length(&wfunc.) ne 0 .do begin
#include <wchar.h>
int _snwprintf( wchar_t *buf,
		size_t count,
		const wchar_t *format, ... );
.ixfunc2 '&String' &wfunc
.ixfunc2 '&Wide &wfunc
.do end
.synop end
.desc begin
The
.id &func.
function is equivalent to the
.kw fprintf
function, except that the argument
.arg buf
specifies a character array into which the generated output is placed,
rather than to a file.
The maximum number of characters to store is specified by
.arg count
.ct .li .
A null character is placed at the end of the generated character
string if fewer than
.arg count
characters were stored.
The
.arg format
string is described under the description of the
.kw printf
function.
.if &'length(&wfunc.) ne 0 .do begin
.np
The
.id &wfunc.
function is identical to
.id &func.
except that the argument
.arg buf
specifies an array of wide characters into which the generated output
is to be written, rather than converted to multibyte characters and
written to a stream.
The maximum number of wide characters to store is specified by
.arg count
.ct .li .
A null wide character is placed at the end of the generated wide
character string if fewer than
.arg count
wide characters were stored.
The
.id &wfunc.
function accepts a wide-character string argument for
.arg format
.do end
.desc end
.return begin
The
.id &func.
function returns the number of characters written into the
array, not counting the terminating null character, or a negative
value if more than
.arg count
characters were requested to be generated.
An error can occur while converting a value for output.
.if &'length(&wfunc.) ne 0 .do begin
The
.id &wfunc.
function returns the number of wide characters written into
the array, not counting the terminating null wide character, or a
negative value if more than
.arg count
wide characters were requested to be generated.
.do end
.im errnoref
.return end
.see begin
.im seeprtf _snprintf
.see end
.exmp begin
#include <stdio.h>

/* Create temporary file names using a counter */

char namebuf[13];
int  TempCount = 0;
.exmp break
char *make_temp_name()
  {
    _snprintf( namebuf, 13, "ZZ%.6o.TMP", TempCount++ );
    return( namebuf );
  }
.exmp break
void main()
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
.class WATCOM
.system
