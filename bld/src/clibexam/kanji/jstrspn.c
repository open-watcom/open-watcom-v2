#include <stdio.h>
#include <jstring.h>

void main()
  {
    printf( "%d\n", jstrspn( "out to lunch", "aeiou" ) );
    printf( "%d\n", jstrspn( "out to lunch", "xyz" ) );
  }
