#include <stdio.h>
#include <string.h>

void main()
  {
    auto char buffer[80];

    memcpy( buffer, "Hello", 5 );
    buffer[5] = '\0';
    printf( "%s\n", buffer );
  }
