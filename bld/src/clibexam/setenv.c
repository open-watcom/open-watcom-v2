
#include <stdio.h>
#include <stdlib.h>
#include <env.h>

void main()
  {
    char *path;

    if( setenv( "INCLUDE", "D:\\WATCOM\\H", 1 ) == 0 )
      if( (path = getenv( "INCLUDE" )) != NULL )
        printf( "INCLUDE=%s\n", path );
  }

