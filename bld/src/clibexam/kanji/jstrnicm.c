#include <stdio.h>
#include <jstring.h>

void main()
  {
    printf( "%d\n", jstrnicmp( "abcdef", "ABCXXX", 10 ) );
    printf( "%d\n", jstrnicmp( "abcdef", "ABCXXX",  6 ) );
    printf( "%d\n", jstrnicmp( "abcdef", "ABCXXX",  3 ) );
    printf( "%d\n", jstrnicmp( "abcdef", "ABCXXX",  0 ) );
  }
