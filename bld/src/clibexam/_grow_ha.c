#include <stdio.h>

FILE *fp[ 50 ];

void main()
  {
    int hndl_count;
    int i;

    hndl_count = _NFILES;
    if( hndl_count < 50 ) {
        hndl_count = _grow_handles( 50 );
    }
    for( i = 0; i < hndl_count; i++ ) {
      fp[ i ] = tmpfile();
      if( fp[ i ] == NULL ) break;
      printf( "File %d successfully opened\n", i );
    }
    printf( "%d files were successfully opened\n", i );
  }
