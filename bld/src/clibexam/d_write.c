#include <stdio.h>
#include <dos.h>
#include <fcntl.h>

char buffer[] = "This is a test for _dos_write.";

void main()
  {
    unsigned len_written;
    int      handle;

    if( _dos_creat( "file", _A_NORMAL, &handle ) != 0 ) {
      printf( "Unable to create file\n" );
    } else {
      printf( "Create succeeded\n" );
      _dos_write( handle, buffer, sizeof(buffer),
                  &len_written );
      _dos_close( handle );
    }
  }
