#include <stdio.h>
#include <malloc.h>

void main()
  {
    void *buffer;

    buffer = malloc( 999 );
    printf( "Size of block is %u bytes\n",
                _msize( buffer ) );
  }
