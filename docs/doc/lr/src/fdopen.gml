.func fdopen _fdopen _wfdopen
.synop begin
#include <stdio.h>
FILE *fdopen( int &fd, const char *mode );
.ixfunc2 '&StrIo' &funcb
.if &'length(&_func.) ne 0 .do begin
FILE *_fdopen( int &fd, const char *mode );
.ixfunc2 '&OsIo' &_func
.do end
.if &'length(&wfunc.) ne 0 .do begin
FILE *_wfdopen( int &fd, const wchar_t *mode );
.ixfunc2 '&StrIo' &wfunc
.ixfunc2 '&Wide' &wfunc
.do end
.synop end
.desc begin
The
.id &funcb.
function associates a stream with the file &handle
.arg &fd
which represents an opened file or device.
The &handle was returned by one of
.kw creat
.ct ,
.kw dup
.ct ,
.kw dup2
.ct ,
.if '&machsys' eq 'QNX' .do begin
.kw fcntl
.ct ,
.do end
.kw open
.ct ,
.if '&machsys' eq 'QNX' .do begin
.kw pipe
.ct ,
.do end
or
.kw sopen
.ct .li .
The open mode
.arg mode
must match the mode with which the file or device was originally
opened.
.np
The argument
.arg mode
is described in the description of the
.kw fopen
function.
.im ansiconf
.if &'length(&wfunc.) ne 0 .do begin
.np
The
.id &wfunc.
function is identical to
.id &funcb.
except that it accepts a
wide character string for the second argument.
.do end
.desc end
.return begin
The
.id &funcb.
function returns a pointer to the object controlling the
stream.
This pointer must be passed as a parameter to subsequent functions for
performing operations on the file.
If the open operation fails,
.id &funcb.
returns a NULL pointer.
.im errnoref
.return end
.see begin
.seelist creat _dos_open dup dup2 fcntl fdopen fopen
.seelist freopen _fsopen _grow_handles _hdopen open
.seelist _open_osfhandle pipe _popen sopen
.see end
.exmp begin
#include <stdio.h>
#include <fcntl.h>
#include <&iohdr>

void main()
  {
    int &fd;
    FILE *fp;
.exmp break
.if '&machsys' eq 'QNX' .do begin
    &fd = open( "file", O_RDONLY );
.do end
.el .do begin
    &fd = open( "file", O_RDONLY | O_TEXT );
.do end
    if( &fd != -1 ) {
      fp = fdopen( &fd, "r" );
      if( fp != NULL ) {
        /*
            process the stream
        */
        fclose( fp );
      } else {
        close( &fd );
      }
    }
  }
.exmp end
.class POSIX 1003.1
.system
