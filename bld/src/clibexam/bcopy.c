#include <stdio.h>
#include <strings.h>

void main()
  {
    auto char buffer[80];

    bcopy( "Hello ", buffer,     6 );
    bcopy( "world",  &buffer[6], 6 );
    printf( "%s\n", buffer );
  }
