.func fopen _wfopen _ufopen
#include <stdio.h>
FILE *fopen( const char *filename, const char *mode );
.ixfunc2 '&StrIo' &func
.if &'length(&wfunc.) ne 0 .do begin
FILE *_wfopen( const wchar_t *filename,
               const wchar_t *mode );
.ixfunc2 '&StrIo' &wfunc
.ixfunc2 '&Wide' &wfunc
.do end
.if &'length(&ufunc.) ne 0 .do begin
FILE *_ufopen( const wchar_t *filename,
               const wchar_t *mode );
.ixfunc2 '&StrIo' &ufunc
.do end
.funcend
.*
.safealt
.*
.desc begin
The &func function opens the file whose name is the string pointed to by
.arg filename
.ct , and associates a stream with it.
The argument
.arg mode
points to a string beginning with one of the following sequences:
.begnote
.termhd1 Mode
.termhd2 Meaning
.note "r"
open file for reading
.note "w"
create file for writing, or truncate to zero length
.note "a"
append: open file or create for writing at end-of-file
.note "r+"
open file for update (reading and/or writing)
.note "w+"
create file for update, or truncate to zero length
.note "a+"
append: open file or create for update, writing at end-of-file
.endnote
.np
In addition to the above characters, you can also include one of the
following characters in
.arg mode
to specify the translation mode for newline characters:
.begnote
.note t
The letter "t" may be added to any of the above sequences in the
second or later position to indicate that the file is (or must be) a
text file.
.if '&machsys' ne 'QNX' .do begin
.if &version ge 110 .do begin
.ix 'BINMODE.OBJ'
It also overrides the global translation mode flag if you link your
program with
.filename BINMODE.OBJ.
The global translation mode flag default is "text" unless you
explicitly link your program with
.filename BINMODE.OBJ.
.do end
.np
When neither "t" nor "b" is specified, the value of the global
variable
.kw _fmode
establishes whether the file is to treated as a binary or a text file.
Unless this value is changed by the
.if &version ge 110 .do begin
program or you have linked your program with
.filename BINMODE.OBJ,
.do end
.el .do begin
program,
.do end
the default will be text mode.
.do end
.note b
The letter "b" may be added to any of the above sequences in the
second or later position to indicate that the file is (or must be) a
binary file (an ANSI requirement for portability to systems that make
a distinction between text and binary files).
.endnote
.if '&machsys' eq 'QNX' .do begin
.np
Under QNX, there is no difference between text files and binary files.
.do end
.if &version ge 110 .do begin
.np
You can also include one of the following characters to enable
or disable the "commit" flag for the associated file.
.begnote
.note c
The letter "c" may be added to any of the above sequences in the
second or later position to indicate that any output is committed by
the operating system whenever a flush (
.ct
.kw fflush
or
.kw flushall
.ct )
is done.
.np
This option is not supported under Netware.
.note n
The letter "n" may be added to any of the above sequences in the
second or later position to indicate that the operating system
need not commit any output whenever a flush is done.
.ix 'COMMODE.OBJ'
It also overrides the global commit flag if you link your program with
.filename COMMODE.OBJ.
The global commit flag default is "no-commit" unless you explicitly
link your program with
.filename COMMODE.OBJ.
.np
This option is not supported under Netware.
.endnote
.np
The "t", "c", and "n" mode options are extensions for &func and
.kw _fdopen
and should not be used where ANSI portability is desired.
.do end
.el .do begin
.np
The "t" mode option is an extension for &func and
.kw _fdopen
and should not be used where ANSI portability is desired.
.do end
.np
Opening a file with read mode (
.ct .id r
as the first character in the
.arg mode
argument) fails if the file does not exist or it cannot be read.
Opening a file with append mode (
.ct .id a
as the first character in the
.arg mode
argument) causes all subsequent writes to the file to be forced to the
current end-of-file, regardless of previous calls to the
.kw fseek
function.
.ix '&StrIo' 'fseek'
When a file is opened with update mode (
.ct .id +
as the second or later character of the
.arg mode
argument), both input and output may be performed on the associated
stream.
.np
When a stream is opened in update mode, both reading and writing
may be performed.
However, writing may not be followed by reading without an
intervening call to the
.kw fflush
function or to a file positioning function (
.ct .kw fseek
.ct ,
.kw fsetpos
.ct ,
.kw rewind
.ct ).
Similarly, reading may not be followed by writing without an
intervening call to a file positioning function, unless the read
resulted in end-of-file.
.if &'length(&wfunc.) ne 0 .do begin
.np
The &wfunc function is identical to &func except that it
accepts wide-character string arguments for
.arg filename
and
.arg mode
.ct .li .
.do end
.if &'length(&ufunc.) ne 0 .do begin
.np
The &ufunc Unicode function is identical to &func except that it
accepts Unicode string arguments for
.arg filename
and
.arg mode
.ct .li .
.do end
.desc end
.return begin
The &func function returns a pointer to the object controlling the
stream.
This pointer must be passed as a parameter to subsequent functions for
performing operations on the file.
If the open operation fails, &func returns
.mono NULL.
.im errnoref
.return end
.see begin
.seelist fopen _dos_open fclose fcloseall fdopen fopen_s freopen freopen_s
.seelist fopen _fsopen _grow_handles _hdopen open _open_osfhandle
.seelist fopen _popen sopen
.see end
.exmp begin
#include <stdio.h>

void main()
{
    FILE *fp;
.exmp break
    fp = fopen( "file", "r" );
    if( fp != NULL ) {
      /* rest of code goes here */
      fclose( fp );
    }
}
.exmp end
.if '&machsys' eq 'QNX' .do begin
.class ANSI
.do end
.el .do begin
.class ANSI, ('t', 'c', 'n' are &company extensions)
.do end
.system
