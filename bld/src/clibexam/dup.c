#include <fcntl.h>
#include <io.h>

void main()
  {
    int handle, dup_handle;

    handle = open( "file",
                O_WRONLY | O_CREAT | O_TRUNC | O_TEXT,
                S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP );
    if( handle != -1 ) {
      dup_handle = dup( handle );
      if( dup_handle != -1 ) {

        /* process file */

        close( dup_handle );
      }
      close( handle );
    }
  }
