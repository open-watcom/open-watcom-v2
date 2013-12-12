.func close _close
.synop begin
#include <&iohdr>
int close( int &fd );
.ixfunc2 '&OsIo' &func
.if &'length(&_func.) ne 0 .do begin
int _close( int &fd );
.ixfunc2 '&OsIo' &_func
.do end
.synop end
.desc begin
The
.id &func.
function closes a file at the operating system level.
The
.arg &fd
value is the file &handle returned by a successful execution of one of
the
.if '&machsys' eq 'QNX' .do begin
.kw creat
.ct,
.kw dup
.ct,
.kw dup2
.ct,
.kw fcntl
.ct,
.kw open
or
.kw sopen
.do end
.el .do begin
.kw creat
.ct,
.kw dup
.ct,
.kw dup2
.ct,
.kw open
or
.kw sopen
.do end
functions.
.if &'length(&_func.) ne 0 .do begin
.np
The
.id &_func.
function is identical to
.id &func.
.
Use
id &_func.
for ANSI/ISO naming conventions.
.do end
.desc end
.return begin
The
.id &func.
function returns zero if successful.
Otherwise, it returns &minus.1 and
.kw errno
is set to indicate the error.
.return end
.error begin
.begterm 12
.termhd1 Constant
.termhd2 Meaning
.term EBADF
The
.arg &fd
argument is not a valid file &handle..
.if '&machsys' eq 'QNX' .do begin
.term EINTR
The
.id &func.
function was interrupted by a signal.
.term EIO
An i/o error occurred while updating the directory information.
.term ENOSPC
A previous buffered write call has failed.
.do end
.endterm
.error end
.see begin
.if '&machsys' eq 'QNX' .do begin
.seelist close creat dup dup2 fcntl open sopen
.do end
.el .do begin
.seelist close creat dup dup2 open sopen
.do end
.see end
.exmp begin
#include <fcntl.h>
#include <&iohdr>

void main()
  {
    int &fd;
.exmp break
    &fd = open( "file", O_RDONLY );
    if( &fd != -1 ) {
      /* process file */
      close( &fd );
    }
  }
.exmp end
.class begin POSIX 1003.1
.ansiname &_func
.class end
.system
