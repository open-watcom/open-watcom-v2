#include <stdio.h>
#include <io.h>
#include <fcntl.h>
#include <sys\stat.h>

void main()
  {
    int  handle;

    handle = open( "file", O_RDWR | O_CREAT,
                S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP );
    if( handle != -1 ) {
      if( chsize( handle, 32 * 1024L ) != 0 ) {
          printf( "Error extending file\n" );
      }
      close( handle );
    }
  }
