#define __STDC_WANT_LIB_EXT1__ 1
#include <string.h>
#include <stdio.h>

void main( void )
{
    char buffer[80] = "0123456789";

    memmove_s( buffer + 1, sizeof( buffer ), buffer, 79 );
    buffer[0] = '*';
    printf( buffer );
}
