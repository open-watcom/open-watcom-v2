#include <stdio.h>
#include <stdlib.h>
#include <io.h>

void main( int argc, char *argv[] )
  {
    if( argc != 2 ) {
      fprintf( stderr, "Use: check <filename>\n" );
      exit( 1 );
    }

    if( access( argv[1], F_OK ) == 0 ) {
      printf( "%s exists\n", argv[1] );
    } else {
      printf( "%s does not exist\n", argv[1] );
      exit( EXIT_FAILURE );
    }
    if( access( argv[1], R_OK ) == 0 ) {
      printf( "%s is readable\n", argv[1] );
    }
    if( access( argv[1], W_OK ) == 0 ) {
      printf( "%s is writeable\n", argv[1] );
    }
    if( access( argv[1], X_OK ) == 0 ) {
      printf( "%s is executable\n", argv[1] );
    }
    exit( EXIT_SUCCESS );
  }
