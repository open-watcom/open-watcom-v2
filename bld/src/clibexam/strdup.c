#include <stdio.h>
#include <string.h>

void main()
  {
    char *dup;

    dup = strdup( "Make a copy" );
    printf( "%s\n", dup );
  }
