.func rmdir _rmdir _wrmdir
.synop begin
#include <sys/types.h>
.if '&machsys' eq 'QNX' .do begin
#include <unistd.h>
.do end
.el .do begin
#include <direct.h>
.do end
int rmdir( const char *path );
.ixfunc2 '&Direct' &func
.if &'length(&_func.) ne 0 .do begin
int _rmdir( const char *path );
.ixfunc2 '&Direct' &_func
.do end
.if &'length(&wfunc.) ne 0 .do begin
int _wrmdir( const wchar_t *path );
.ixfunc2 '&Direct' &wfunc
.ixfunc2 '&Wide' &wfunc
.do end
.synop end
.*
.desc begin
The
.id &func.
function removes (deletes) the specified directory.
The directory must not contain any files or directories.
The
.arg path
can be either relative to the current working directory or it can be
an absolute path name.
.if '&machsys' eq 'QNX' .do begin
.pp
If the directory is the root directory or the current working
directory of any process, the effect of this function is
implementation-defined.
.pp
If the directory's link count becomes zero and no process has the
directory open, the space occupied by the directory is freed and the
directory is no longer accessible.
If one or more processes have the directory open when the last link
is removed, the dot and dot-dot entries, if present, are removed before
.id &func.
returns and no new entries may be created in the directory, but
the directory is not removed until all references to the directory
have been closed.
.pp
Upon successful completion, the
.id &func.
function will mark for update
the
.us st_ctime
and
.us st_mtime
fields of the parent directory.
.do end
.if &'length(&_func.) ne 0 .do begin
.np
The
.id &_func.
function is identical to &func..
Use
.id &_func.
for ANSI/ISO naming conventions.
.do end
.if &'length(&wfunc.) ne 0 .do begin
.np
The
.id &wfunc.
function is identical to
.id &func.
except that it accepts a
wide-character string argument.
.do end
.desc end
.*
.return begin
The
.id &func.
function returns zero if successful and -1 otherwise.
.return end
.*
.error begin
.if '&machsys' eq 'QNX' .do begin
.begterm 12
.termhd1 Constant
.termhd2 Meaning
.term EACCES
Search permission is denied for a component of
.arg path
or write permission is denied on the parent directory of the
directory to be removed.
.if '&machsys' eq 'QNX' .do begin
.term EBUSY
The directory named by the
.arg path
argument cannot be removed
because it is being used by another process and the implementation
considers this to be an error.
.do end
.term EEXIST
The
.arg path
argument names a directory that is not an empty directory.
.if '&machsys' eq 'QNX' .do begin
.term ENAMETOOLONG
The argument
.arg path
exceeds {PATH_MAX} in length, or a pathname component is longer than
{NAME_MAX}.
.do end
.term ENOENT
The specified
.arg path
does not exist or
.arg path
is an empty string.
.if '&machsys' eq 'QNX' .do begin
.term ENOTDIR
A component of
.arg path
is not a directory.
.term ENOTEMPTY
The
.arg path
argument names a directory that is not an empty directory.
.term EROFS
The directory entry to be removed resides on a read-only file system.
.do end
.endterm
.do end
.error end
.*
.see begin
.seelist chdir chmod getcwd mkdir mknod rmdir stat umask
.see end
.*
.exmp begin
.blktext begin
To remove the directory called
.if '&machsys' eq 'QNX' .do begin
.filename /home/terry
.do end
.el .do begin
.filename \watcom
on drive
.filename C:
.do end
.blktext end
.blkcode begin
.if '&machsys' eq 'QNX' .do begin
#include <sys/types.h>
#include <sys/stat.h>
.do end
.el .do begin
#include <sys/types.h>
#include <direct.h>
.do end

void main( void )
{
.if '&machsys' eq 'QNX' .do begin
    rmdir( "/home/terry" );
.do end
.el .do begin
    rmdir( "c:\\watcom" );
.do end
}
.blkcode end
.im dblslash
.exmp end
.class begin POSIX 1003.1
.ansiname &_func
.class end
.system
