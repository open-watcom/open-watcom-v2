.func mkdir _mkdir _wmkdir
.synop begin
.if '&machsys' eq 'QNX' .do begin
#include <sys/types.h>
#include <sys/stat.h>
int mkdir( const char *path, mode_t mode );
.ixfunc2 '&Direct' &funcb
.if &'length(&_func.) ne 0 .do begin
int _mkdir( const char *path, mode_t mode );
.ixfunc2 '&Direct' &_func
.do end
.do end
.el .do begin
#include <sys/types.h>
#include <direct.h>
int mkdir( const char *path );
.ixfunc2 '&Direct' &funcb
.if &'length(&_func.) ne 0 .do begin
int _mkdir( const char *path );
.ixfunc2 '&Direct' &_func
.do end
.do end
.if &'length(&wfunc.) ne 0 .do begin
int _wmkdir( const wchar_t *path );
.ixfunc2 '&Direct' &wfunc
.ixfunc2 '&Wide' &wfunc
.do end
.synop end
.*
.desc begin
The
.id &funcb.
function creates a new subdirectory with name
.arg path
.ct .li .
The
.arg path
can be either relative to the current working directory or it can be
an absolute path name.
.if '&machsys' eq 'QNX' .do begin
.np
The file permission bits of the new directory are initialized from
.arg mode
.ct .li .
The file permission bits of the
.arg mode
argument are modified by the process's file creation mask (see
.kw umask
.ct ).
.im openperv
.np
The directory's owner ID is set to the process's effective user ID.
The directory's group ID is set to the group ID of the directory in
which the directory is being created or to the process's effective
group ID.
.np
The newly created directory will be empty.
.np
Upon successful completion, the
.id &funcb.
function will mark for update
the
.us st_atime, st_ctime,
and
.us st_mtime
fields of the directory.
Also, the
.us st_ctime
and
.us st_mtime
fields of the directory that contains the new entry are marked for
update.
.do end
.if &'length(&_func.) ne 0 .do begin
.np
The
.id &_func.
function is identical to &funcb..
Use
.id &_func.
for ISO C naming conventions.
.do end
.if &'length(&wfunc.) ne 0 .do begin
.np
The
.id &wfunc.
function is identical to
.id &funcb.
except that it accepts a
wide-character string argument.
.do end
.desc end
.*
.return begin
The
.id &funcb.
function returns zero if successful, and a non-zero value
otherwise.
.return end
.*
.error begin
.begterm 12
.termhd1 Constant
.termhd2 Meaning
.term EACCES
Search permission is denied for a component of
.arg path
or write permission is denied on the parent directory of the
directory to be created.
.term EEXIST
The named file exists.
.if '&machsys' eq 'QNX' .do begin
.term EMLINK
The link count of the parent directory would exceed {LINK_MAX}.
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
.term ENOSPC
The file system does not contain enough space to hold the contents of
the new directory or to extend the parent directory of the new
directory.
.term ENOSYS
This function is not supported for this path.
.term ENOTDIR
A component of
.arg path
is not a directory.
.term EROFS
The parent directory of the directory being created resides on a
read-only file system.
.do end
.endterm
.error end
.*
.see begin
.seelist chdir chmod getcwd mkdir mknod rmdir stat umask
.see end
.*
.exmp begin
.blktext begin
To make a new directory called
.if '&machsys' eq 'QNX' .do begin
.filename /watcom
on node 2
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
    mkdir( "//2/hd/watcom",
           S_IRWXU |
           S_IRGRP | S_IXGRP |
           S_IROTH | S_IXOTH );
.do end
.el .do begin
    mkdir( "c:\\watcom" );
.do end
}
.blkcode end
.im dblslash
.exmp end
.ansiname &_func
.class POSIX 1003.1
.system
