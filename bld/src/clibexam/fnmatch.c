#include <stdio.h>
#include <fnmatch.h>
#include <stdlib.h>
#include <limits.h>

int main( int argc, char **argv )
{
    int     i;
    char    buffer[PATH_MAX+1];

    while( gets( buffer ) ) {
        for( i = 1; i < argc; i++ ) {
            if( fnmatch( argv[i], buffer, 0 ) == 0 ) {
                printf( "'%s' matches pattern '%s'\n",
                        buffer, argv[i] );
                break;
            }
        }
    }
    return( EXIT_SUCCESS );
}
