#include <stdio.h>
#include <malloc.h>

void main()
  {
    long int __huge *big_buffer;

    big_buffer = (long int __huge *)
                  halloc( 1024L, sizeof(long) );
    if( big_buffer == NULL ) {
      printf( "Unable to allocate memory\n" );
    } else {

      /* rest of code goes here */

      hfree( big_buffer );  /* deallocate */
    }
  }
