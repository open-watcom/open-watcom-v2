
#include <stdio.h>
#include <sys\types.h>
#include <sys\stat.h>
#include <direct.h>

void main()
  {
    DIR *dirp;
    struct dirent *direntp;
    int handle;

    dirp = opendir( "\\watcom\\h\\*.*" );
    if( dirp != NULL ) {
      printf( "Old directory listing\n" );
      for(;;) {
          direntp = readdir( dirp );
          if( direntp == NULL ) break;
          printf( "%s\n", direntp->d_name );
      }

      handle = creat( "\\watcom\\h\\file.new",
                  S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP );
      close( handle );

      rewinddir( dirp );
      printf( "New directory listing\n" );
      for(;;) {
        direntp = readdir( dirp );
        if( direntp == NULL ) break;
        printf( "%s\n", direntp->d_name );
      }
      closedir( dirp );
    }
  }

