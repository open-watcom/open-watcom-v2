#include <stdio.h>
#include <io.h>

void main()
  {
    int handle;
    FILE *fp;

    fp = fopen( "file", "r" );
    if( fp != NULL ) {
      handle = _os_handle( fileno( fp ) );
      fclose( fp );
    }
  }
