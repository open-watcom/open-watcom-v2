#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

/* Demonstrate error routine in which negative */
/* arguments to "sqrt" are treated as positive */

void main()
  {
    printf( "%e\n", sqrt( -5e0 ) );
    exit( 0 );
  }

int matherr( struct _exception *err )
  {
    if( strcmp( err->name, "sqrt" ) == 0 ) {
      if( err->type == DOMAIN ) {
        err->retval = sqrt( -(err->arg1) );
        return( 1 );
      } else
        return( 0 );
    } else
      return( 0 );
  }
