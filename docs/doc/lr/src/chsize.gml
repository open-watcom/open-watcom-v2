.func chsize _chsize
#include <&iohdr>
int chsize( int &fd, long size );
.ixfunc2 '&DosFunc' &func
.if &'length(&_func.) ne 0 .do begin
int _chsize( int &fd, long size );
.ixfunc2 '&DosFunc' &_func
.do end
.funcend
.desc begin
The &func function changes the size of the file associated with
.arg &fd
by extending or truncating the file to the length specified by
.arg size
.ct .li .
If the file needs to be extended, the file is padded with NULL ('\0')
characters.
.if &'length(&_func.) ne 0 .do begin
.np
The &_func function is identical to &func..
Use &_func for ANSI naming conventions.
.do end
.if '&machsys' eq 'QNX' .do begin
.np
Note that the &func function call ignores advisory locks which may
have been set by the
.kw fcntl
.ct ,
.kw lock
.ct , or
.kw locking
functions.
.do end
.desc end
.return begin
The &func function returns zero if successful.
A return value of -1 indicates an error, and
.kw errno
is set to indicate the error.
.return end
.error begin
.if '&machsys' ne 'PP' .do begin
.begterm 12 $compact
.termhd1 Constant
.termhd2 Meaning
.term EACCES
The specified file is locked against access.
.term EBADF
.if '&machsys' eq 'QNX' .do begin
Invalid file &handle.. or file not opened for write.
.do end
.el .do begin
Invalid file &handle..
.do end
.term ENOSPC
Not enough space left on the device to extend the file.
.endterm
.do end
.error end
.see begin
.seelist chsize close creat open
.see end
.exmp begin
#include <stdio.h>
#include <&iohdr>
#include <fcntl.h>
#include <sys/stat.h>

void main()
  {
    int  &fd;
.exmp break
    &fd = open( "file", O_RDWR | O_CREAT,
                S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP );
    if( &fd != -1 ) {
      if( chsize( &fd, 32 * 1024L ) != 0 ) {
          printf( "Error extending file\n" );
      }
      close( &fd );
    }
  }
.exmp end
.class WATCOM
.system
