#include <stdio.h>
#include <jstring.h>

void main()
  {
    printf( "%d\n", jstrlen( "Howdy" ) );
    printf( "%d\n", jstrlen( "Hello world\n" ) );
    printf( "%d\n", jstrlen( "" ) );
  }
