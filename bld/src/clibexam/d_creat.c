#include <stdio.h>
#include <dos.h>

void main()
  {
    int handle;

    if( _dos_creat( "file", _A_NORMAL, &handle ) != 0 ){
      printf( "Unable to create file\n" );
    } else {
      printf( "Create succeeded\n" );
      _dos_close( handle );
    }
  }
