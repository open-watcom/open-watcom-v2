#include <stdio.h>
#include <string.h>
#include <io.h>

#define TMPLTE "_tXXXXXX"

void main()
  {
    char name[sizeof(TMPLTE)];
    char *mknm;
    int i;
    FILE *fp;

    for( i = 0; i < 30; i++ ) {
      strcpy( name, TMPLTE );
      mknm = _mktemp( name );
      if( mknm == NULL )
        printf( "Name is badly formed\n" );
      else {
        printf( "Name is %s\n", mknm );
        fp = fopen( mknm, "w" );
        if( fp != NULL ) {
          fprintf( fp, "Name is %s\n", mknm );
          fclose( fp );
        }
      }
    }
  }
