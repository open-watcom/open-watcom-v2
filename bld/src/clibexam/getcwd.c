#include <stdio.h>
#include <stdlib.h>
#include <direct.h>

void main()
  {
    char *cwd;

    cwd = getcwd( NULL, 0 );
    if( cwd != NULL ) {
      printf( "My working directory is %s\n", cwd );
      free( cwd );
    }
  }
