#include <stdio.h>
#include <sys/utime.h>

void main( int argc, char *argv[] )
  {
    if( (utime( argv[1], NULL ) != 0) && (argc > 1) ) {
       printf( "Unable to set time for %s\n", argv[1] );
    }
  }
