#include <stdio.h>
#include <fcntl.h>
#include <io.h>

void main()
  {
    int handle;
    FILE *fp;

    handle = open( "file", O_RDONLY | O_TEXT );
    if( handle != -1 ) {
      fp = fdopen( handle, "r" );
      if( fp != NULL ) {
        /*
            process the stream
        */
        fclose( fp );
      } else {
        close( handle );
      }
    }
  }
