.func fsync
#include <&iohdr>
int fsync( int fd );
.funcend
.desc begin
The &func function writes to disk all the currently queued data for
the open file specified by
.arg fd
.ct .li .
All necessary file system information required to retrieve the data is
also written to disk.
The file access times are also updated.
.np
The &func function is used when you wish to ensure that both the file
data and file system information required to recover the complete file
have been written to the disk.
.np
The &func function does not return until the transfer is completed.
.desc end
.return begin
The &func function returns zero if successful.
Otherwise, it returns &minus.1 and
.kw errno
is set to indicate the error.
If the &func function fails, outstanding i/o operations are not
guaranteed to have been completed.
.return end
.error begin
.begterm 12
.termhd1 Constant
.termhd2 Meaning
.term EBADF
The
.arg fd
argument is not a valid file &handle..
.term EINVAL
Synchronized i/o is not supported for this file.
.term EIO
A physical I/O error occurred (e.g., a bad block). The precise meaning
is device dependent.
.term ENOSYS
The &func function is not supported.
.endterm
.error end
.see begin
.seelist &function. fdatasync fstat open stat write
.see end
.exmp begin
/*
 *      Write a file and make sure it is on disk.
 */
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <&iohdr>
.exmp break
char buf[512];
.exmp break
void main()
{
    int &fd;
    int i;
.exmp break
.if '&machsys' eq 'PP' .do begin
    &fd = creat( "file",
                 S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP );
.do end
.el .if '&machsys' eq 'QNX' .do begin
    &fd = creat( "file",
                 S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP );
.do end
.el .do begin
    &fd = creat( "file", S_IWRITE | S_IREAD );
.do end
    if( &fd == -1 ) {
      perror( "Error creating file" );
      exit( EXIT_FAILURE );
    }
.exmp break
    for( i = 0; i < 255; ++i ) {
      memset( buf, i, sizeof( buf ) );
      if( write( &fd, buf, sizeof(buf) ) != sizeof(buf) ) {
        perror( "Error writing file" );
        exit( EXIT_FAILURE );
      }
    }
.exmp break
    if( fsync( &fd ) == -1 ) {
      perror( "Error sync'ing file" );
      exit( EXIT_FAILURE );
    }
.exmp break
    close( &fd );
    exit( EXIT_SUCCESS );
}
.exmp end
.class POSIX 1003.4
.system
