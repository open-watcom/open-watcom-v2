#define __STDC_WANT_LIB_EXT1__  1
#include <stdio.h>
#include <string.h>

void main()
{
    char    buffer[ 30 ] = "Hello world.";
    size_t  len;

    len = strnlen_s( buffer, sizeof( buffer ) );
    printf( "Length of text: %d\n", len );
    printf( "Text: %s\n", buffer );
}
