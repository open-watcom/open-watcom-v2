#include <stdio.h>
#include <sys/stat.h>

void main()
  {
    struct stat buf;

    if( stat( "file", &buf ) != -1 ) {
      printf( "File size = %d\n", buf.st_size );
    }
  }
