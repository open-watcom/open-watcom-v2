#include <stdio.h>
#include <stdlib.h>

void print_value( unsigned long int value )
  {
    int base;
    char buffer[33];

    for( base = 2; base <= 16; base = base + 2 )
      printf( "%2d %s\n", base,
              ultoa( value, buffer, base ) );
  }

void main()
  {
    print_value( (unsigned) 12765L );
  }
