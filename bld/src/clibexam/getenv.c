#include <stdio.h>
#include <stdlib.h>

void main()
  {
    char *path;

    path = getenv( "INCLUDE" );
    if( path != NULL )
      printf( "INCLUDE=%s\n", path );
  }
