#include <stdio.h>

void main()
  {
    char buffer[80];

    while( gets( buffer ) != NULL )
      puts( buffer );
  }
