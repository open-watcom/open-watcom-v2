#include <stdio.h>
#include <string.h>

void main()
  {
    char buffer[80];

    if( memicmp( buffer, "Hello", 5 ) < 0 ) {
      printf( "Less than\n" );
    }
  }
