#include <stdio.h>
#include <dos.h>
#include <fcntl.h>
#include <share.h>

void main()
  {
    int handle;

    if( _dos_open( "file", O_RDONLY, &handle ) != 0 ) {
        printf( "Unable to open file\n" );
    } else {
        printf( "Open succeeded\n" );
        _dos_close( handle );
    }
  }
