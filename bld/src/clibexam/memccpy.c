#include <stdio.h>
#include <string.h>

char *msg = "This is the string: not copied";

void main()
  {
    auto char buffer[80];

    memset( buffer, '\0', 80 );
    memccpy( buffer, msg, ':', 80 );
    printf( "%s\n", buffer );
  }
