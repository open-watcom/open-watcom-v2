#include <stdio.h>
#include <stdlib.h>

void main()
  {
    char buffer[20];
    int base;

    for( base = 2; base <= 16; base = base + 2 )
      printf( "%2d %s\n", base,
              itoa( 12765, buffer, base ) );
  }
