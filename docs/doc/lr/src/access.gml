.func access _access _waccess
.synop begin
#include <&iohdr>
int access( const char *path, int mode );
.ixfunc2 '&FileOp' &func
.if &'length(&_func.) ne 0 .do begin
int _access( const char *path, int mode );
.ixfunc2 '&FileOp' &_func
.do end
.if &'length(&wfunc.) ne 0 .do begin
int _waccess( const wchar_t *path, int mode );
.ixfunc2 '&FileOp' &wfunc
.ixfunc2 '&Wide' &wfunc
.do end
.synop end
.desc begin
The &func function determines if the file or directory specified by
.arg path
exists and if it can be accessed with the file permission given by
.arg mode
.ct .li .
.if &'length(&_func.) ne 0 .do begin
.np
The &_func function is identical to &func..
Use &_func for ANSI naming conventions.
.do end
.np
When the value of
.arg mode
is zero, only the existence of the file is verified.
The read and/or write permission for the file can be determined when
.arg mode
is a combination of the bits:
.begterm 10
.termhd1 Bit
.termhd2 Meaning
.term R_OK
test for read permission
.term W_OK
test for write permission
.term X_OK
test for execute permission
.term F_OK
test for existence of file
:cmt. .term ACCESS_WR
:cmt. check for write permission
:cmt. .term ACCESS_RD
:cmt. check for read permission
.endterm
.if '&machsys' ne 'QNX' .do begin
.pc
With DOS, all files have read permission; it is a good idea to test
for read permission anyway, since a later version of DOS may support
write-only files.
.do end
.if &'length(&wfunc.) ne 0 .do begin
.np
The &wfunc function is identical to &func except that it accepts a
wide-character string argument for
.arg path
.ct .li .
.do end
.desc end
.return begin
The &func function returns zero if the file or directory exists and can be
accessed with the specified mode.
Otherwise, &minus.1 is returned and
.kw errno
is set to indicate the error.
.return end
.error begin
.begterm 12
.termhd1 Constant
.termhd2 Meaning
.term EACCES
Access denied because the file's permission does not allow the
specified access.
.term ENOENT
Path or file not found.
.endterm
.error end
.see begin
.seelist access chmod fstat open sopen stat
.see end
.exmp begin
#include <stdio.h>
#include <stdlib.h>
#include <&iohdr>
.exmp break
void main( int argc, char *argv[] )
  {
    if( argc != 2 ) {
      fprintf( stderr, "Use: check <filename>\n" );
      exit( 1 );
    }
.exmp break
    if( access( argv[1], F_OK ) == 0 ) {
      printf( "%s exists\n", argv[1] );
    } else {
      printf( "%s does not exist\n", argv[1] );
      exit( EXIT_FAILURE );
    }
    if( access( argv[1], R_OK ) == 0 ) {
      printf( "%s is readable\n", argv[1] );
    }
    if( access( argv[1], W_OK ) == 0 ) {
      printf( "%s is writeable\n", argv[1] );
    }
    if( access( argv[1], X_OK ) == 0 ) {
      printf( "%s is executable\n", argv[1] );
    }
    exit( EXIT_SUCCESS );
  }
.exmp end
.class POSIX 1003.1
.system
