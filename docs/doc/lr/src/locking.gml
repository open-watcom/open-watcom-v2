.func locking _locking
.synop begin
#include <sys/locking.h>
int locking( int &fd, int mode, long nbyte );
.ixfunc2 '&OsIo' &func
.if &'length(&_func.) ne 0 .do begin
int _locking( int &fd, int mode, long nbyte );
.ixfunc2 '&OsIo' &_func
.do end
.synop end
.desc begin
The &func function locks or unlocks
.arg nbyte
bytes of the file specified by
.arg &fd
.ct .li .
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
.do end
.el .do begin
Locking a region of a file prevents other processes from reading or
writing the locked region until the region has been unlocked.
.do end
The locking and unlocking takes place at the current file position.
The argument
.arg mode
specifies the action to be performed.
The possible values for mode are:
.begterm 12
.termhd1 Mode
.termhd2 Meaning
.term _LK_LOCK, LK_LOCK
Locks the specified region.
The function will retry to lock the region after 1 second intervals
until successful or until 10 attempts have been made.
.term _LK_RLCK, LK_RLCK
Same action as
.kw _LK_LOCK
.ct .li .
.term _LK_NBLCK, LK_NBLCK
Non-blocking lock: makes only 1 attempt to lock the specified region.
.term _LK_NBRLCK, LK_NBRLCK
Same action as
.kw _LK_NBLCK
.ct .li .
.term _LK_UNLCK, LK_UNLCK
Unlocks the specified region. The region must have been previously locked.
.endterm
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
.if &'length(&_func.) ne 0 .do begin
.np
The &_func function is identical to &func..
Use &_func for ANSI/ISO naming conventions.
.do end
.desc end
.return begin
The &func function returns zero if successful.
Otherwise, it returns &minus.1 and
.kw errno
is set to indicate the error.
.return end
.error begin
.begterm 12
.termhd1 Constant
.termhd2 Meaning
.term EACCES
Indicates a locking violation (file already locked or unlocked).
.term EBADF
Indicates an invalid file &handle..
.term EDEADLOCK
Indicates a locking violation.
This error is returned when
.arg mode
is
.mono LK_LOCK
or
.mono LK_RLCK
and the file cannot be locked after 10 attempts.
.term EINVAL
Indicates that an invalid argument was given to the function.
.endterm
.error end
.see begin
.seelist locking creat _dos_creat _dos_open fcntl lock open sopen unlock
.see end
.exmp begin
#include <stdio.h>
#include <sys/locking.h>
#include <share.h>
#include <fcntl.h>
#include <&iohdr>

void main()
  {
    int &fd;
    unsigned nbytes;
    unsigned long offset;
    auto char buffer[512];
.exmp break
    nbytes = 512;
    offset = 1024;
    &fd = sopen( "db.fil", O_RDWR, SH_DENYNO );
    if( &fd != -1 ) {
      lseek( &fd, offset, SEEK_SET );
      locking( &fd, LK_LOCK, nbytes );
      read( &fd, buffer, nbytes );
      /* update data in the buffer */
      lseek( &fd, offset, SEEK_SET );
      write( &fd, buffer, nbytes );
      lseek( &fd, offset, SEEK_SET );
      locking( &fd, LK_UNLCK, nbytes );
      close( &fd );
    }
  }
.exmp end
.class begin WATCOM
.ansiname &_func
.class end
.system
