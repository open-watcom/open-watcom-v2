.func begin
.funct   fopen_s TR 24731
.funct_w _wfopen_s
.func gen
#define __STDC_WANT_LIB_EXT1__ 1
#include <stdio.h>
errno_t fopen_s( FILE * restrict * restrict streamptr,
                 const char * restrict filename,
                 const char * restrict mode);
.ixfunc2 '&StrIo' &func
.if &'length(&wfunc.) ne 0 .do begin
errno_t _wfopen_s( FILE * restrict * restrict streamptr,
                   const wchar_t * restrict filename,
                   const wchar_t * restrict mode);
.ixfunc2 '&StrIo' &wfunc
.ixfunc2 '&Wide' &wfunc
.do end
.func end
.*
.rtconst begin
None of
.arg streamptr
.ct ,
.arg filename
.ct , or
.arg mode
shall be a null pointer.
If there is a runtime-constraint violation, &func does not attempt to open a file.
Furthermore, if
.arg streamptr
is not a null pointer, &func sets
.arg *streamptr
to the null pointer.
.rtconst end
.*
.desc begin
The &func function opens the file whose name is the string pointed to by
.arg filename
.ct , and associates a stream with it.
The
.arg mode
string shall be as described for fopen, with the addition that modes starting
with the character 'w' or 'a' may be preceded by the character 'u', see below:
.begnote
.termhd1 Mode
.termhd2 Meaning
.note "uw"
truncate to zero length or create text file for writing, default permissions
.note "ua"
append; open or create text file for writing at end-of-file, default permissions
.note "uwb"
truncate to zero length or create binary file for writing, default permissions
.note "uab"
append; open or create binary file for writing at end-of-file, default
permissions
.note "uw+"
truncate to zero length or create text file for update, default permissions
.note "ua+"
append; open or create text file for update, writing at end-of-file, default
permissions
.note "uw+b or uwb+"
truncate to zero length or create binary file for update, default
permissions
.note "ua+b or uab+"
append; open or create binary file for update, writing at end-of-file,
default permissions
.endnote
.np
.np
To the extent that the underlying system supports the concepts, files opened for writing
shall be opened with exclusive (also known as non-shared) access. If the file is being
created, and the first character of the
.arg mode
string is not 'u', to the extent that the
underlying system supports it, the file shall have a file permission that prevents other
users on the system from accessing the file. If the file is being created and first character
of the mode string is 'u', then by the time the file has been closed, it shall have the
system default file access permissions.
If the file was opened successfully, then the pointer to FILE pointed to by
.arg streamptr
will be set to the pointer to the object controlling the opened file. Otherwise, the pointer
to FILE pointed to by
.arg streamptr
will be set to a null pointer.
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
The "t", "c", and "n" mode options are extensions for &func
and should not be used where ANSI portability is desired.
.do end
.el .do begin
.np
The "t" mode option is an extension for &func
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
.desc end
.return begin
The &func function returns zero if it opened the file. If it did not open the file or if
there was a runtime-constraint violation, &func returns a non-zero value.
.return end
.see begin
.seelist fopen_s _dos_open fclose fcloseall fdopen fopen freopen freopen_s
.seelist fopen_s _fsopen _grow_handles _hdopen open _open_osfhandle
.seelist fopen_s _popen sopen
.see end
.exmp begin
#define __STDC_WANT_LIB_EXT1__ 1
#include <stdio.h>

void main()
{
    errno_t rc;
    FILE    *fp;
.exmp break
    rc = fopen_s( &fp, "file", "r" );
    if( fp != NULL ) {
      /* rest of code goes here */
      fclose( fp );
    }
}
.exmp end
.classt
.system
