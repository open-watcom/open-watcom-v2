#include <stdio.h>
#include <stdlib.h>

void main( int argc, char *argv[] )
  {
    FILE *fp;

    if( argc <= 1 ) {
      fprintf( stderr, "Missing argument\n" );
      exit( EXIT_FAILURE );
    }

    fp = fopen( argv[1], "r" );
    if( fp == NULL ) {
      fprintf( stderr, "Unable to open '%s'\n", argv[1] );
      _exit( EXIT_FAILURE );
    }
    fclose( fp );
    _exit( EXIT_SUCCESS );
  }
