/*
 *      Write a file and make sure it is on disk.
 */
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <io.h>

char buf[512];

void main()
{
    int handle;
    int i;

    handle = creat( "file", S_IWRITE | S_IREAD );
    if( handle == -1 ) {
      perror( "Error creating file" );
      exit( EXIT_FAILURE );
    }

    for( i = 0; i < 255; ++i ) {
      memset( buf, i, sizeof( buf ) );
      if( write( handle, buf, sizeof(buf) ) != sizeof(buf) ) {
        perror( "Error writing file" );
        exit( EXIT_FAILURE );
      }
    }

    if( fsync( handle ) == -1 ) {
      perror( "Error sync'ing file" );
      exit( EXIT_FAILURE );
    }

    close( handle );
    exit( EXIT_SUCCESS );
}
