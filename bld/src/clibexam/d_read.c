#include <stdio.h>
#include <dos.h>
#include <fcntl.h>

void main()
  {
    unsigned len_read;
    int      handle;
    auto char buffer[80];

    if( _dos_open( "file", O_RDONLY, &handle ) != 0 ) {
      printf( "Unable to open file\n" );
    } else {
      printf( "Open succeeded\n" );
      _dos_read( handle, buffer, 80, &len_read );
      _dos_close( handle );
    }
  }
