#include <string.h>

void main()
  {
    char buffer[80];

    memmove( buffer+1, buffer, 79 );
    buffer[0] = '*';
  }
