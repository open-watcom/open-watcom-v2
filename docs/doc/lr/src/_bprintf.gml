.func _bprintf _bwprintf _u_bprintf
.funcw _bwprintf
#include <stdio.h>
int _bprintf( char *buf, size_t bufsize,
              const char *format, ... );
.ixfunc2 '&StrIo' &func
.if &'length(&wfunc.) ne 0 .do begin
int _bwprintf( wchar_t *buf, size_t bufsize,
               const wchar_t *format, ... );
.ixfunc2 '&StrIo' &wfunc
.ixfunc2 '&Wide' &wfunc
.do end
.if &'length(&ufunc.) ne 0 .do begin
int _u_bprintf( wchar_t *buf, size_t bufsize,
                const wchar_t *format, ... );
.ixfunc2 '&StrIo' &ufunc
.do end
.funcend
.*
.desc begin
The &func function is equivalent to the
.kw sprintf
function, except that the argument
.arg bufsize
specifies the size of the character array
.arg buf
into which the generated output is placed.
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
The &wfunc function accepts a wide-character string argument for
.arg format
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
The &func function returns the number of characters written into the
array, not counting the terminating null character.
An error can occur while converting a value for output.
.im errnoref
.return end
.*
.see begin
.im seeprtf _bprintf
.see end
.*
.exmp begin
#include <stdio.h>

void main( int argc, char *argv[] )
{
    char file_name[9];
    char file_ext[4];
.exmp break
    _bprintf( file_name, 9, "%s", argv[1] );
    _bprintf( file_ext,  4, "%s", argv[2] );
    printf( "%s.%s\n", file_name, file_ext );
}
.exmp end
.*
.class WATCOM
.system
