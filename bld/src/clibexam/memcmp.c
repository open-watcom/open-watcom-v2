#include <stdio.h>
#include <string.h>

void main()
  {
    auto char buffer[80];

    strcpy( buffer, "world" );
    if( memcmp( buffer, "Hello ", 6 ) < 0 ) {
      printf( "Less than\n" );
    }
  }
