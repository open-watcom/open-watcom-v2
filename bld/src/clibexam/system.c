#include <stdlib.h>
#include <stdio.h>

void main()
  {
    int rc;

    rc = system( "dir" );
    if( rc != 0 ) {
      printf( "shell could not be run\n" );
    }
  }
