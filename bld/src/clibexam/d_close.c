#include <stdio.h>
#include <dos.h>
#include <fcntl.h>

void main()
  {
    int handle;

    /* Try to open "stdio.h" and then close it */
    if( _dos_open( "stdio.h", O_RDONLY, &handle ) != 0 ){
      printf( "Unable to open file\n" );
    } else {
      printf( "Open succeeded\n" );
      if( _dos_close( handle ) != 0 ) {
        printf( "Close failed\n" );
      } else {
        printf( "Close succeeded\n" );
      }
    }
  }
