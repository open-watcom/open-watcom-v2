#include <stdio.h>
#include <stdlib.h>

void print_value( long value )
  {
    int base;
    char buffer[33];

    for( base = 2; base <= 16; base = base + 2 )
      printf( "%2d %s\n", base,
              ltoa( value, buffer, base ) );
  }

void main()
  {
    print_value( 12765L );
  }
