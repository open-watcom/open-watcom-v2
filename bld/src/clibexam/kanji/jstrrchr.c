#include <stdio.h>
#include <jstring.h>

void main()
  {
    printf( "%s\n", jstrrchr( "abcdeabcde", 'a' ) );
    if( jstrrchr( "abcdeabcde", 'x' ) == NULL )
        printf( "NULL\n" );
  }
