#include <stdlib.h>
#include <string.h>
int main( int argc, char **argv )
{
    char        name[_MAX_FNAME];

    _splitpath( argv[0], NULL, NULL, name, NULL );
    strlwr( name );
    printf( "%s command is NYI\n", name );
    return( 0 );
}
