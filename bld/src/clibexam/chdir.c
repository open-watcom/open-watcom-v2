#include <stdio.h>
#include <stdlib.h>
#include <direct.h>

void main( int argc, char *argv[] )
  {
    if( argc != 2 ) {
      fprintf( stderr, "Use: cd <directory>\n" );
      exit( 1 );
    }

    if( chdir( argv[1] ) == 0 ) {
      printf( "Directory changed to %s\n", argv[1] );
      exit( 0 );
    } else {
      perror( argv[1] );
      exit( 1 );
    }
  }
