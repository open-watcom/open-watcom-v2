#include <stdio.h>
#include <jstring.h>

JCHAR source[] = { "A mixed-case STRING" };

void main()
  {
    printf( "%s\n", source );
    printf( "%s\n", jstrupr( source ) );
    printf( "%s\n", source );
  }
