#include <stdio.h>
#include <stdlib.h>

void main( int argc, char *argv[] )
  {
    int i;
    char buff[ PATH_MAX ];

    for( i = 1; i < argc; ++i ) {
      puts( argv[i] );
      if( _fullpath( buff, argv[i], PATH_MAX ) ) {
        puts( buff );
      } else {
        puts( "FAIL!" );
      }
    }
  }
