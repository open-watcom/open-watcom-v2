#include <stdio.h>
#include <sys/locking.h>
#include <share.h>
#include <fcntl.h>
#include <io.h>

void main()
  {
    int handle;
    unsigned nbytes;
    unsigned long offset;
    auto char buffer[512];

    nbytes = 512;
    offset = 1024;
    handle = sopen( "db.fil", O_RDWR, SH_DENYNO );
    if( handle != -1 ) {
      lseek( handle, offset, SEEK_SET );
      locking( handle, LK_LOCK, nbytes );
      read( handle, buffer, nbytes );
      /* update data in the buffer */
      lseek( handle, offset, SEEK_SET );
      write( handle, buffer, nbytes );
      lseek( handle, offset, SEEK_SET );
      locking( handle, LK_UNLCK, nbytes );
      close( handle );
    }
  }
