.func creat _creat _wcreat
.synop begin
#include <sys/types.h>
#include <sys/stat.h>
.ixfunc2 '&OsIo' &func
.if '&machsys' eq 'QNX' .do begin
#include <fcntl.h>
int creat( const char *path, mode_t mode );
.if &'length(&wfunc.) ne 0 .do begin
int _wcreat( const wchar_t *path, mode_t mode );
.ixfunc2 '&OsIo' &wfunc
.do end
.do end
.el .do begin
#include <&iohdr>
int creat( const char *path, int mode );
.if &'length(&_func.) ne 0 .do begin
int _creat( const char *path, int mode );
.ixfunc2 '&OsIo' &_func
.do end
.if &'length(&wfunc.) ne 0 .do begin
int _wcreat( const wchar_t *path, int mode );
.ixfunc2 '&OsIo' &wfunc
.ixfunc2 '&Wide' &wfunc
.do end
.do end
.synop end
.desc begin
The
.id &func.
function creates (and opens) a file at the operating system
level.
It is equivalent to:
.millust begin
  open( path, O_WRONLY | O_CREAT | O_TRUNC, mode );
.millust end
.if &'length(&_func.) ne 0 .do begin
.np
The
.id &_func.
function is identical to
.id &func.
.
Use
id &_func.
for ANSI naming conventions.
.do end
.if &'length(&wfunc.) ne 0 .do begin
.np
The
.id &wfunc.
function is identical to
.id &func.
except that it accepts a
wide character string argument.
.do end
.np
The name of the file to be created is given by
.arg path
.ct .li .
When the file exists (it must be writeable), it is truncated to
contain no data and the preceding
.arg mode
setting is unchanged.
.np
When the file does not exist, it is created with access permissions
given by the
.arg mode
argument.
.im openperv
.if '&machsys' ne 'QNX' .do begin
.np
All files are readable with DOS; however, it is a good idea to set
.mono S_IREAD
when read permission is intended for the file.
.do end
.desc end
.return begin
If successful,
.id &func.
returns a &handle for the file.
When an error occurs while opening the file, &minus.1 is returned, and
.kw errno
is set to indicate the error.
.return end
.error begin
.begterm 12
.termhd1 Constant
.termhd2 Meaning
.if '&machsys' eq 'QNX' .do begin
.term EACCES
Search permission is denied on a component of the path prefix, or the
file exists and the permissions specified by
.arg mode
are denied, or the file does not exist and write permission is denied
for the parent directory of the file to be created.
.term EBADFSYS
While attempting to open the named file, either the file itself or a
component of the path prefix was found to be corrupted.
A system failure -- from which no automatic recovery is possible --
occurred while the file was being written to or while the directory
was being updated.
It will be necessary to invoke appropriate systems administrative
procedures to correct this situation before proceeding.
.term EBUSY
The file named by
.arg path
is a block special device which is already open for writing, or
.arg path
names a file which is on a file system mounted on a block special
device which is already open for writing.
.term EINTR
The
.id &func.
operation was interrupted by a signal.
.term EISDIR
The named file is a directory and the file creation flags
specify write-only or read/write access.
.term EMFILE
Too many file descriptors are currently in use by this process.
.term ENAMETOOLONG
The length of the
.arg path
string exceeds {PATH_MAX}, or a pathname component is longer than
{NAME_MAX}.
.term ENFILE
Too many files are currently open in the system.
.term ENOENT
Either the path prefix does not exist or the
.arg path
argument points to an empty string.
.term ENOSPC
The directory or file system which would contain the new file cannot
be extended.
.term ENOTDIR
A component of the path prefix is not a directory.
.term EROFS
The named file resides on a read-only file system and either
.kw O_WRONLY
.ct ,
.kw O_RDWR
.ct ,
.kw O_CREAT
(if the file does not exist), or
.kw O_TRUNC
is set.
.do end
.el .do begin
.term EACCES
Access denied because
.arg path
specifies a directory or a volume ID, or a read-only file.
.term EMFILE
No more &handle.s available (too many open files).
.term ENOENT
The specified
.arg path
does not exist or
.arg path
is an empty string.
.do end
.endterm
.error end
.see begin
.im seeioos creat
.see end
.exmp begin
#include <sys/types.h>
#include <sys/stat.h>
.if '&machsys' eq 'QNX' .do begin
#include <fcntl.h>
.do end
.el .do begin
#include <&iohdr>
.do end

void main()
  {
    int &fd;
.exmp break
.if '&machsys' eq 'QNX' .do begin
    &fd = creat( "file",
                 S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP );
.do end
.el .do begin
    &fd = creat( "file", S_IWRITE | S_IREAD );
.do end
    if( &fd != -1 ) {

      /* process file */

      close( &fd );
    }
  }
.exmp end
.class POSIX 1003.1
.system
