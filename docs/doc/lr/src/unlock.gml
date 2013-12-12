.func unlock
#include <&iohdr>
int unlock( int &fd,
            unsigned long offset,
            unsigned long nbytes );
.ixfunc2 '&OsIo' &func
.synop end
.desc begin
The &func function unlocks
.arg nbytes
amount of previously locked data in the file designated by
.arg &fd
starting at byte
.arg offset
in the file.
This allows other processes to lock this region of the file.
.np
Multiple regions of a file can be locked, but no overlapping regions
are allowed.
You cannot unlock multiple regions in the same call, even if the
regions are contiguous.
All locked regions of a file should be unlocked before closing a file
or exiting the program.
.if '&machsys' ne 'QNX' .do begin
.np
With DOS, locking is supported by version 3.0 or later.
Note that
.mono SHARE.COM
or
.mono SHARE.EXE
must be installed.
.do end
.desc end
.return begin
The &func function returns zero if successful, and &minus.1 when an
error occurs.
.im errnoref
.return end
.see begin
.seelist unlock lock locking open sopen
.see end
.exmp begin
#include <stdio.h>
#include <fcntl.h>
#include <&iohdr>

void main()
  {
    int &fd;
    char buffer[20];
.exmp break
.if '&machsys' eq 'QNX' .do begin
    &fd = open( "file", O_RDWR );
.do end
.el .do begin
    &fd = open( "file", O_RDWR | O_TEXT );
.do end
    if( &fd != -1 ) {
      if( lock( &fd, 0L, 20L ) ) {
        printf( "Lock failed\n" );
      } else {
        read( &fd, buffer, 20 );
        /* update the buffer here */
        lseek( &fd, 0L, SEEK_SET );
        write( &fd, buffer, 20 );
        unlock( &fd, 0L, 20L );
      }
      close( &fd );
    }
  }
.exmp end
.class WATCOM
.system
