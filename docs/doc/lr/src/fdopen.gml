.func fdopen _fdopen _wfdopen _ufdopen
#include <stdio.h>
FILE *fdopen( int &fd, const char *mode );
.ixfunc2 '&StrIo' &func
.if &'length(&_func.) ne 0 .do begin
FILE *_fdopen( int &fd, const char *mode );
.ixfunc2 '&OsIo' &_func
.do end
.if &'length(&wfunc.) ne 0 .do begin
FILE *_wfdopen( int &fd, const wchar_t *mode );
.ixfunc2 '&StrIo' &wfunc
.ixfunc2 '&Wide' &wfunc
.do end
.if &'length(&ufunc.) ne 0 .do begin
FILE *_ufdopen( int &fd, const wchar_t *mode );
.ixfunc2 '&StrIo' &ufunc
.do end
.funcend
.desc begin
The &func function associates a stream with the file &handle
.arg &fd
which represents an opened file or device.
The &handle was returned by one of
.if '&machsys' eq 'PP' .do begin
.kw creat
or
.kw open
.ct .li .
.do end
.el .do begin
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
.do end
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
.if &'length(&_func.) ne 0 .do begin
.np
The &_func function is identical to &func..
Use &_func for ANSI/ISO naming conventions.
.do end
.if &'length(&wfunc.) ne 0 .do begin
.np
The &wfunc function is identical to &func except that it accepts a
wide character string for the second argument.
.do end
.if &'length(&ufunc.) ne 0 .do begin
.np
The &ufunc Unicode function is identical to &func except that it
accepts a Unicode string for the second argument.
.do end
.desc end
.return begin
The &func function returns a pointer to the object controlling the
stream.
This pointer must be passed as a parameter to subsequent functions for
performing operations on the file.
If the open operation fails, &func returns a NULL pointer.
.im errnoref
.return end
.see begin
.seelist &function. creat _dos_open dup dup2 fcntl fdopen fopen
.seelist &function. freopen _fsopen _grow_handles _hdopen open
.seelist &function. _open_osfhandle pipe _popen sopen
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
.if '&machsys' eq 'PP' .do begin
    &fd = open( "file", O_RDONLY );
.do end
.el .if '&machsys' eq 'QNX' .do begin
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
