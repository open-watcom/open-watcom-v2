#include <fcntl.h>
#include <io.h>

void main()
  {
    int handle;

    handle = open( "file", O_RDONLY );
    if( handle != -1 ) {
      /* process file */
      close( handle );
    }
  }
