#include <stdio.h>
#include <string.h>
#include <jstring.h>

void main()
  {
    JSTRING p;
    JSTRING buffer;
    JSTRING delims = { " .," };

    buffer = strdup( "Find words, all of them." );
    printf( "%s\n", buffer );
    p = jstrtok( buffer, delims );
    while( p != NULL ) {
      printf( "word: %s\n", p );
      p = jstrtok( NULL, delims );
    }
    printf( "%s\n", buffer );
  }
