#include <stdio.h>
#include <string.h>
#include <jstring.h>

void main()
  {
    JCHAR buffer[80];

    strcpy( buffer, "Hello " );
    jstrcat( buffer, "world" );
    printf( "%s\n", buffer );
  }
