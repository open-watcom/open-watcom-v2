#include <stdio.h>
#include <stdlib.h>

void main()
  {
    int base;
    char buffer[18];

    for( base = 2; base <= 16; base = base + 2 )
      printf( "%2d %s\n", base,
              utoa( (unsigned) 12765, buffer, base ) );
  }
