#include <stdio.h>
#include <dos.h>

void main()
  {
    int handle1, handle2;
    if( _dos_creat( "file", _A_NORMAL, &handle1 ) ){
      printf( "Unable to create file\n" );
    } else {
      printf( "Create succeeded\n" );
      if( _dos_creatnew( "file", _A_NORMAL, &handle2 ) ){
        printf( "Unable to create new file\n" );
      }
      _dos_close( handle1 );
    }
  }
