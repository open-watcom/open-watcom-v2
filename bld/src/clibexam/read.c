#include <stdio.h>
#include <fcntl.h>
#include <io.h>

void main()
  {
    int  handle;
    int  size_read;
    char buffer[80];

    /* open a file for input              */
    handle = open( "file", O_RDONLY | O_TEXT );
    if( handle != -1 ) {

      /* read the text                      */
      size_read = read( handle, buffer,
                        sizeof( buffer ) );

      /* test for error                     */
      if( size_read == -1 ) {
          printf( "Error reading file\n" );
      }

      /* close the file                     */
      close( handle );
    }
  }
