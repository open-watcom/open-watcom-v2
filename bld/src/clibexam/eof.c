#include <stdio.h>
#include <fcntl.h>
#include <io.h>

void main()
  {
    int handle, len;
    char buffer[100];

    handle = open( "file", O_RDONLY );
    if( handle != -1 ) {
      while( ! eof( handle ) ) {
        len = read( handle, buffer, sizeof(buffer) - 1 );
        buffer[ len ] = '\0';
        printf( "%s", buffer );
      }
      close( handle );
    }
  }
