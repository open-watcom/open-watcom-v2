#include <stdio.h>
#include <sys\types.h>
#include <sys\stat.h>
#include <fcntl.h>
#include <io.h>

void main()
  {
    int handle, rc;
    struct stat buf;

    handle = open( "file", O_RDONLY );
    if( handle != -1 ) {
      rc = fstat( handle, &buf );
      if( rc != -1 )
        printf( "File size = %d\n", buf.st_size );
      close( handle );
    }
  }
