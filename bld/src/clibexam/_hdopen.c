#include <stdio.h>
#include <dos.h>
#include <fcntl.h>
#include <io.h>

void main()
  {
    int dos_handle;
    int handle;
    if( _dos_open( "file", O_RDONLY, &dos_handle ) != 0 ) {
      printf( "Unable to open file\n" );
    } else {
      handle = _hdopen( dos_handle, O_RDONLY );
      if( handle != -1 ) {
        write( handle, "hello\n", 6 );
        close( handle );
      }
    }
  }
