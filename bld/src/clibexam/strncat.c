#include <stdio.h>
#include <string.h>

char buffer[80];

void main()
  {
    strcpy( buffer, "Hello " );
    strncat( buffer, "world", 8 );
    printf( "%s\n", buffer );
    strncat( buffer, "*************", 4 );
    printf( "%s\n", buffer );
  }
