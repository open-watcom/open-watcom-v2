#include <stdio.h>
#include <fcntl.h>
#include <io.h>

void main()
  {
    int handle;
    char buffer[20];

    handle = open( "file", O_RDWR | O_TEXT );
    if( handle != -1 ) {
      if( lock( handle, 0L, 20L ) ) {
        printf( "Lock failed\n" );
      } else {
        read( handle, buffer, 20 );
        /* update the buffer here */
        lseek( handle, 0L, SEEK_SET );
        write( handle, buffer, 20 );
        unlock( handle, 0L, 20L );
      }
      close( handle );
    }
  }
