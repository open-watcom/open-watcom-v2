#include <stdio.h>
#include <jstring.h>

void main()
  {
    printf( "%d\n", jstrncmp( "abcdef", "abcDEF", 10 ) );
    printf( "%d\n", jstrncmp( "abcdef", "abcDEF",  6 ) );
    printf( "%d\n", jstrncmp( "abcdef", "abcDEF",  3 ) );
    printf( "%d\n", jstrncmp( "abcdef", "abcDEF",  0 ) );
  }
