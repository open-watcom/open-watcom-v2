.func lock
.synop begin
#include <&iohdr>
int lock( int &fd,
          unsigned long offset,
          unsigned long nbytes );
.ixfunc2 '&OsIo' &funcb
.synop end
.desc begin
The
.id &funcb.
function locks
.arg nbytes
amount of data in the file designated by
.arg &fd
starting at byte
.arg offset
in the file.
.if '&machsys' eq 'QNX' .do begin
The file must be opened with write access to lock it.
.np
Locking is a protocol designed for updating a file shared among
concurrently running applications.
Locks are only advisory, that is, they do not prevent an errant or
poorly-designed application from overwriting a locked region of a
shared file.
An application should use locks to indicate regions of a file that are
to be updated by the application and it should respect the locks of
other applications.
.np
.do end
.el .do begin
This prevents other processes from reading or writing into the locked
region until an
.kw unlock
has been done for this locked region of the file.
.do end
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
The
.id &funcb.
function returns zero if successful, and &minus.1 when an error
occurs.
.im errnoref
.return end
.see begin
.seelist lock fcntl locking open sopen unlock
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
