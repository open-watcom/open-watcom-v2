.func utime _utime _wutime
.if '&machsys' eq 'QNX' .do begin
#include <sys/types.h>
#include <utime.h>
.do end
.el .do begin
#include <sys/utime.h>
.do end
int utime( const char *path,
           const struct utimbuf *times );
.ixfunc2 '&OsIo' &func
.if &'length(&_func.) ne 0 .do begin
int _utime( const char *path,
           const struct utimbuf *times );
.ixfunc2 '&OsIo' &_func
.do end
.if &'length(&wfunc.) ne 0 .do begin
int _wutime( const wchar_t *path,
             const struct utimbuf *times );
.ixfunc2 '&OsIo' &wfunc
.ixfunc2 '&Wide' &func
.do end

struct utimbuf {
    time_t   actime;    /* access time */
    time_t   modtime;   /* modification time */
};
.funcend
.desc begin
The &func function records the access and modification times
for the file
.if '&machsys' eq 'QNX' .do begin
or directory
.do end
identified by
.arg path
.ct .li .
.if &'length(&_func.) ne 0 .do begin
.np
The &_func function is identical to &func..
Use &_func for ANSI naming conventions.
.do end
.np
If the
.arg times
argument is
.mono NULL
.ct , the access and modification times of the file or directory are set to
the current time.
.if '&machsys' eq 'QNX' .do begin
The effective user ID of the process must match the owner of the file
or directory, or the process must have write permission to the file or
directory, or appropriate privileges in order to use the &func
function in this way.
.do end
.el .do begin
Write access to this file must be permitted for the time to be
recorded.
.np
If the
.arg times
argument is not
.mono NULL
.ct , it is interpreted as a pointer to a
.kw utimbuf
structure and the access and modification times of the file or
directory are set to the values contained in the designated structure.
.*
.if '&machsys' eq 'QNX' .do begin
Only the owner of the file or directory and processes with appropriate
privileges are permitted to use the &func function in this way.
.do end
.*
The access and modification times are taken from the
.kw actime
and
.kw modtime
fields in this structure.
.do end
.if &'length(&wfunc.) ne 0 .do begin
.np
The &wfunc function is identical to &func except that
.arg path
points to a wide-character string.
.do end
.desc end
.return begin
The &func function returns zero when the time was successfully recorded.
A value of &minus.1 indicates an error occurred.
.return end
.error begin
.begterm 12
.termhd1 Constant
.termhd2 Meaning
.term EACCES
Search permission is denied for a component of
.arg path
or the
.arg times
argument is
.mono NULL
and the effective user ID of the process does not match the owner of
the file and write access is denied.
.if '&machsys' ne 'QNX' .do begin
.term EINVAL
The date is before 1980 (DOS only).
.term EMFILE
There are too many open files.
.do end
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
.term EPERM
The
.arg times
argument is not
.mono NULL
and the calling process's effective user ID has write access to the
file but does not match the owner of the file and the calling process
does not have the appropriate privileges.
.term EROFS
The named file resides on a read-only file system.
.do end
.endterm
.error end
.exmp begin
#include <stdio.h>
#include <sys/utime.h>

void main( int argc, char *argv[] )
  {
    if( (utime( argv[1], NULL ) != 0) && (argc > 1) ) {
       printf( "Unable to set time for %s\n", argv[1] );
    }
  }
.exmp end
.class POSIX 1003.1
.system
