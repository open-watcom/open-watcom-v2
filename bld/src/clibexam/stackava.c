#include <stdio.h>
#include <string.h>
#include <malloc.h>
#include <fcntl.h>
#include <io.h>

long char_count( FILE *fp )
  {
     char *buffer;
     size_t bufsiz;
     long count;

     /* allocate half of stack for temp buffer */
     bufsiz = stackavail() >> 1;
     buffer = (char *) alloca( bufsiz );
     setvbuf( fp, buffer, _IOFBF, bufsiz );
     count = 0L;
     while( fgetc( fp ) != EOF ) ++count;
     fclose( fp );
     return( count );
  }

void main()
  {
    FILE *fp;

    fp = fopen( "file", "rb" );
    if( fp != NULL ) {
      setmode( fileno( fp ), O_BINARY );
      printf( "File contains %lu characters\n",
          char_count( fp ) );
      fclose( fp );
    }
  }
