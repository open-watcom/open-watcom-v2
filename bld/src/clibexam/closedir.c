
#include <stdio.h>
#include <direct.h>

void main()
  {
    DIR *dirp;
    struct dirent *direntp;

    dirp = opendir( "\\watcom\\h" );
    if( dirp != NULL ) {

      for(;;) {
        direntp = readdir( dirp );
        if( direntp == NULL ) break;
        printf( "%s\n", direntp->d_name );
      }

      closedir( dirp );
    }
  }

