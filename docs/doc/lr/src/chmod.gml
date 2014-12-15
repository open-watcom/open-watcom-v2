.func chmod _chmod _wchmod
.synop begin
#include <sys/types.h>
#include <sys/stat.h>
.if '&machsys' eq 'QNX' .do begin
int chmod( const char *path, mode_t permission );
.do end
.el .do begin
#include <&iohdr>
int chmod( const char *path, int permission );
.if &'length(&_func.) ne 0 .do begin
int _chmod( const char *path, int permission );
.ixfunc2 '&FileOp' &_func
.do end
.do end
.ixfunc2 '&FileOp' &funcb
.if &'length(&wfunc.) ne 0 .do begin
int _wchmod( const wchar_t *path, int permission );
.ixfunc2 '&FileOp' &wfunc
.ixfunc2 '&Wide' &wfunc
.do end
.synop end
.desc begin
The
.id &funcb.
function changes the permissions for a file specified by
.arg path
to be the settings in the mode given by
.arg permission
.ct .li .
.im openperv
.if '&machsys' eq 'QNX' .do begin
.np
The following bits may also be specified in
.arg permission
.ct .li .
.begterm 12 $compact
.termhd1 Permission
.termhd2 Meaning
.term S_ISUID
set user id on execution
.term S_ISGID
set group id on execution
.endterm
.np
If the calling process does not have appropriate privileges,
and if the group ID of the file does not match the effective
group ID or one of the supplementary group IDs, and if the file
is a regular file, bit
.mono S_ISGID
(set group ID on execution)
in the file's mode shall be cleared upon successful return from the
.id &funcb.
function.
.do end
.np
Upon successful completion, the
.id &funcb.
function will mark for update
the
.us st_ctime
field of the file.
.if &'length(&_func.) ne 0 .do begin
.np
The
.id &_func.
function is identical to &funcb..
Use
.id &_func.
for ANSI naming conventions.
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
.return begin
The &funcb
returns zero if the new settings are successfully made; otherwise,
&minus.1 is returned and
.kw errno
is set to indicate the error.
.return end
.error begin
.begterm 12
.termhd1 Constant
.termhd2 Meaning
.term EACCES
Search permission is denied for a component of
.arg path
.ct .li .
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
The effective user ID does not match the owner of the file and the
calling process does not have the appropriate privileges.
.term EROFS
The named file resides on a read-only file system.
.do end
.endterm
.error end
.see begin
.if '&machsys' eq 'QNX' .do begin
.seelist chmod chown mkdir mkfifo stat
.do end
.el .do begin
.seelist chmod fstat open sopen stat
.do end
.see end
.exmp begin
/*
 * change the permissions of a list of files
 * to be read/write by the owner only
 */
.exmp break
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
.if '&machsys' ne 'QNX' .do begin
#include <&iohdr>
.do end
.exmp break
void main( int argc, char *argv[] )
  {
    int i;
    int ecode = 0;
.exmp break
    for( i = 1; i < argc; i++ ) {
      if( chmod( argv[i], S_IRUSR | S_IWUSR ) == -1 ) {
        perror( argv[i] );
        ecode++;
      }
    }
    exit( ecode );
  }
.exmp end
.class POSIX 1003.1
.system
