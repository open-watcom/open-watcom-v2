#include <stdio.h>
#include <sys/stat.h>
#include <io.h>
#include <fcntl.h>

char buffer[]
        = { "A text record to be written" };

void main()
  {
    int handle;
    int size_written;

    /* open a file for output             */
    /* replace existing file if it exists */
    handle = open( "file",
                O_WRONLY | O_CREAT | O_TRUNC | O_TEXT,
                S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP );

    if( handle != -1 ) {

      /* print file position */
      printf( "%ld\n", tell( handle ) );

      /* write the text */
      size_written = write( handle, buffer,
                            sizeof( buffer ) );

      /* print file position */
      printf( "%ld\n", tell( handle ) );

      /* close the file */
      close( handle );
    }
  }
