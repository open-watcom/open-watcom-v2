#include <stdio.h>
#include <malloc.h>

void main()
  {
    int heap_status;
    char *buffer;

    buffer = (char *)malloc( 80 );
    malloc( 1024 );
    free( buffer );
    heap_status = _heapset( 0xff );
    switch( heap_status ) {
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
