#define __STDC_WANT_LIB_EXT1__  1
#include <stdio.h>
#include <string.h>

char buffer[80];

void main( void )
{
    strcpy( buffer, "Hello " );
    strncat_s( buffer, sizeof( buffer ), "world", 8 );
    printf( "%s\n", buffer );
    strncat( buffer, "*************", 4 );
    printf( "%s\n", buffer );
}
