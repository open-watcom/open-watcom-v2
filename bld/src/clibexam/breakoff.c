#include <stdio.h>
#include <stdlib.h>

void main()
  {
    long i;
    FILE *tmpf;

    tmpf = tmpfile();
    if( tmpf != NULL ) {
      printf( "Start\n" );
      break_off();
      for( i = 1; i < 100000; i++ )
        fprintf( tmpf, "%ld\n", i );
      break_on();
      printf( "Finish\n" );
    }
  }
