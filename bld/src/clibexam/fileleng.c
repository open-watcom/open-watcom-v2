#include <sys/types.h>
#include <fcntl.h>
#include <stdio.h>
#include <io.h>

void main()
  {
    int handle;

    /* open a file for input              */
    handle = open( "file", O_RDONLY | O_TEXT );
    if( handle != -1 ) {
      printf( "Size of file is %ld bytes\n",
              filelength( handle ) );
      close( handle );
    }
  }
