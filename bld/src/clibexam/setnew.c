#include <stdio.h>
#include <new.h>

#if defined(__386__)
const size_t MemBlock = 8192;
#else
const size_t MemBlock = 2048;
#endif

/*
    Pre-allocate a memory block for demonstration
    purposes. The out-of-memory handler will return
    it to the system so that "new" can use it.
*/

long *failsafe = new long[MemBlock];

/*
    Declare a customized function to handle memory
    allocation failure.
*/

int out_of_memory_handler( unsigned size )
  {
    printf( "Allocation failed, " );
    printf( "%u bytes not available.\n", size );
    /* Release pre-allocated memory if we can */
    if( failsafe == NULL ) {
      printf( "Halting allocation.\n" );
      /* Tell new to stop allocation attempts */
      return( 0 );
    } else {
      delete failsafe;
      failsafe = NULL;
      printf( "Retrying allocation.\n" );
      /* Tell new to retry allocation attempt */
      return( 1 );
    }
  }

void main( void )
  {
    int i;

    /* Register existence of a new memory handler */
    _set_new_handler( out_of_memory_handler );
    long *pmemdump = new long[MemBlock];
    for( i=1 ; pmemdump != NULL; i++ ) {
      pmemdump = new long[MemBlock];
      if( pmemdump != NULL )
        printf( "Another block allocated %d\n", i );
    }
  }
