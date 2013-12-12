.func _fsopen _wfsopen
.synop begin
#include <stdio.h>
FILE *_fsopen( const char *filename,
               const char *mode, int share );
.ixfunc2 '&StrIo' &func
.if &'length(&wfunc.) ne 0 .do begin
FILE *_wfsopen( const wchar_t *filename,
                const wchar_t *mode, int share );
.ixfunc2 '&StrIo' &wfunc
.ixfunc2 '&Wide' &wfunc
.do end
.synop end
.desc begin
The &func function opens the file whose name is the string pointed to by
.arg filename
.ct .li ,
and associates a stream with it.
The arguments
.arg mode
and
.arg share
control shared reading or writing.
The argument
.arg mode
points to a string beginning with one of the following sequences:
.begnote
.termhd1 Mode
.termhd2 Meaning
.note "r"
.if '&machsys' eq 'QNX' .do begin
open file for reading
.do end
.el .do begin
open file for reading; use default file translation
.do end
.note "w"
.if '&machsys' eq 'QNX' .do begin
create file for writing, or truncate to zero length
.do end
.el .do begin
create file for writing, or truncate to zero length; use default file
translation
.do end
.note "a"
.if '&machsys' eq 'QNX' .do begin
append: open text file or create for writing at end-of-file
.do end
.el .do begin
append: open text file or create for writing at end-of-file; use default
file translation
.do end
.if '&machsys' ne 'QNX' .do begin
.note "rb"
open binary file for reading
.do end
.if '&machsys' ne 'QNX' .do begin
.note "rt"
open text file for reading
.do end
.if '&machsys' ne 'QNX' .do begin
.note "wb"
create binary file for writing, or truncate to zero length
.do end
.if '&machsys' ne 'QNX' .do begin
.note "wt"
create text file for writing, or truncate to zero length
.do end
.if '&machsys' ne 'QNX' .do begin
.note "ab"
append; open binary file or create for writing at end-of-file
.do end
.if '&machsys' ne 'QNX' .do begin
.note "at"
append; open text file or create for writing at end-of-file
.do end
.note "r+"
open file for update (reading and/or writing); use default file
translation
.note "w+"
create file for update, or truncate to zero length; use default file
translation
.note "a+"
append; open file or create for update, writing at end-of-file;
use default file translation
.if '&machsys' ne 'QNX' .do begin
.note "r+b", "rb+"
open binary file for update (reading and/or writing)
.do end
.if '&machsys' ne 'QNX' .do begin
.note "r+t", "rt+"
open text file for update (reading and/or writing)
.do end
.if '&machsys' ne 'QNX' .do begin
.note "w+b", "wb+"
create binary file for update, or truncate to zero length
.do end
.if '&machsys' ne 'QNX' .do begin
.note "w+t", "wt+"
create text file for update, or truncate to zero length
.do end
.if '&machsys' ne 'QNX' .do begin
.note "a+b", "ab+"
append; open binary file or create for update, writing at end-of-file
.do end
.if '&machsys' ne 'QNX' .do begin
.note "a+t", "at+"
append; open text file or create for update, writing at end-of-file
.do end
.endnote
.if '&machsys' eq 'QNX' .do begin
The letter "b" may be added to any of the above sequences in the
second or third position to indicate that the file is (or must be) a
binary file (an ANSI requirement for portability to systems that make
a distinction between text and binary files).
Under QNX, there is no difference between text files and binary files.
.do end
.el .do begin
.pp
When default file translation is specified, the value of the global
variable
.kw _fmode
establishes whether the file is to treated as a binary or a text file.
Unless this value is changed by the program, the default will be text
mode.
.do end
.pp
Opening a file with read mode (
.ct .id 'r'
as the first character in the
.arg mode
argument) fails if the file does not exist or it cannot be read.
Opening a file with append mode (
.ct .id 'a'
as the first character in the
.arg mode
argument) causes all subsequent writes to the file to be forced to the
current end-of-file, regardless of previous calls to the
.kw fseek
function.
.ix '&StrIo' 'fseek'
When a file is opened with update mode (
.ct .id '+'
as the second or third character of the
.arg mode
argument), both input and output may be
performed on the associated stream.
.np
When a stream is opened in update mode, both reading and writing
may be performed.
However, writing may not be followed by reading without an
intervening call to the
.kw fflush
function or to a file positioning function (
.ct .kw fseek
,
.kw fsetpos
,
.kw rewind
.ct ).
Similarly, reading may not be followed by writing without an
intervening call to a file positioning function, unless the read
resulted in end-of-file.
.np
The shared access for the file,
.arg share
.ct , is established by a combination of bits defined in the
.hdrfile share.h
header file.
The following values may be set:
.begterm 12 $compact
.termhd1 Value
.termhd2 Meaning
.term SH_COMPAT
Set compatibility mode.
.term SH_DENYRW
Prevent read or write access to the file.
.term SH_DENYWR
Prevent write access of the file.
.term SH_DENYRD
Prevent read access to the file.
.term SH_DENYNO
Permit both read and write access to the file.
.endterm
.if '&machsys' eq 'QNX' .do begin
.np
Note that
.millust begin
fopen( filename, mode );
.millust end
is the same as:
.millust begin
_fsopen( filename, mode, SH_COMPAT );
.millust end
.do end
.el .do begin
.np
You should consult the technical documentation for the DOS system that
you are using for more detailed information about these sharing modes.
.do end
.if &'length(&wfunc.) ne 0 .do begin
.np
The &wfunc function is identical to &func except that it accepts
wide-character string arguments for
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
.seelist &function. _dos_open fclose fcloseall fdopen fopen freopen
.seelist &function. _fsopen _grow_handles _hdopen open _open_osfhandle
.seelist &function. _popen sopen
.see end
.exmp begin
#include <stdio.h>
#include <share.h>

void main()
  {
    FILE *fp;
.exmp break
    /*
      open a file and prevent others from writing to it
    */
    fp = _fsopen( "report.dat", "w", SH_DENYWR );
    if( fp != NULL ) {
      /* rest of code goes here */
      fclose( fp );
    }
  }
.exmp end
.class WATCOM
.system
