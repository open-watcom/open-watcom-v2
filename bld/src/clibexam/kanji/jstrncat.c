#include <stdio.h>
#include <string.h>
#include <jstring.h>

JCHAR buffer[80];

void main()
  {
    strcpy( buffer, "Hello " );
    jstrncat( buffer, "world", 8 );
    printf( "%s\n", buffer );
    jstrncat( buffer, "*************", 4 );
    printf( "%s\n", buffer );
  }
