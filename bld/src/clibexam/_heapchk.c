#include <stdio.h>
#include <malloc.h>

void main()
  {
    char *buffer;

    buffer = (char *)malloc( 80 );
    malloc( 1024 );
    free( buffer );
    switch( _heapchk() ) {
    case _HEAPOK:
      printf( "OK - heap is good\n" );
      break;
    case _HEAPEMPTY:
      printf( "OK - heap is empty\n" );
      break;
    case _HEAPBADBEGIN:
      printf( "ERROR - heap is damaged\n" );
      break;
    case _HEAPBADNODE:
      printf( "ERROR - bad node in heap\n" );
      break;
    }
  }
