/*
 * change the permissions of a list of files
 * to be read/write by the owner only
 */

#include <stdio.h>
#include <stdlib.h>
#include <sys\types.h>
#include <sys\stat.h>
#include <io.h>

void main( int argc, char *argv[] )
  {
    int i;
    int ecode = 0;

    for( i = 1; i < argc; i++ ) {
      if( chmod( argv[i], S_IRUSR | S_IWUSR ) == -1 ) {
        perror( argv[i] );
        ecode++;
      }
    }
    exit( ecode );
  }
