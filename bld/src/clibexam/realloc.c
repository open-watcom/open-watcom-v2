#include <stdlib.h>

void main()
  {
    char *buffer;
    char *new_buffer;

    buffer = (char *) malloc( 80 );
    new_buffer = (char *) realloc( buffer, 100 );
    if( new_buffer == NULL ) {

      /* not able to allocate larger buffer */

    } else {
      buffer = new_buffer;
    }
  }
