#include <stdio.h>
#include <string.h>
#include <jstring.h>

void main()
  {
    JCHAR buffer[80];
    JSTRING p;

    strcpy( buffer, "Hello world" );
    p = jstradv( buffer, 6 );
    printf( "%s\n", p );
  }
