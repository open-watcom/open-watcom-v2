#include <stdio.h>
#include <jstring.h>

JCHAR source[] = { "A sample STRING" };

void main()
  {
    printf( "%s\n", source );
    printf( "%s\n", jstrset( source, '=' ) );
    printf( "%s\n", jstrset( source, '*' ) );
  }
