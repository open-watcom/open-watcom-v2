#include <stdio.h>
#include <malloc.h>

void main()
  {
    char *p;

    p = malloc( 200*1024 );
    if( p != NULL ) free( p );
    _heapenable( 0 );
    /*
      allocate memory from a pool that
      has been capped at 200K
    */
  }
