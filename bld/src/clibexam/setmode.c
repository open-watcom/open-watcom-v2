#include <stdio.h>
#include <fcntl.h>
#include <io.h>

void main()
  {
    FILE *fp;
    long count;

    fp = fopen( "file", "rb" );
    if( fp != NULL ) {
      setmode( fileno( fp ), O_BINARY );
      count = 0L;
      while( fgetc( fp ) != EOF ) ++count;
      printf( "File contains %lu characters\n",
              count );
      fclose( fp );
    }
  }
