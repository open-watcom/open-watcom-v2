#include <stdio.h>
#include <string.h>

void main()
  {
    char buffer[80];

    strcpy( buffer, "Hello " );
    strcat( buffer, "world" );
    printf( "%s\n", buffer );
  }
