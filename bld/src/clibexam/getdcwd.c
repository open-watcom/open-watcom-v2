#include <stdio.h>
#include <stdlib.h>
#include <direct.h>

void main( void )
{
    char    *cwd;

    cwd = _getdcwd( 3, NULL, 0 );
    if( cwd != NULL ) {
        printf( "The current directory on drive C is %s\n",
                cwd );
        free( cwd );
    }
}
