#include <stdio.h>
#include <stdlib.h>

/* Format output into a buffer after determining its size */

void main()
  {
    int     bufsize;
    char    *buffer;

    bufsize = snprintf( NULL, 0, "%3d %P", 42, 42 );
    buffer  = malloc( bufsize + 1 );
    snprintf( buffer, bufsize + 1, "%3d %P", 42, 42 );
    free( buffer );
  }
