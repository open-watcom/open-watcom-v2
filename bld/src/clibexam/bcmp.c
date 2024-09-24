#include <stdio.h>
#include <strings.h>

void main()
  {
    if( bcmp( "Hello there", "Hello world", 6 ) ) {
      printf( "Not equal\n" );
    } else {
      printf( "Equal\n" );
    }
  }
