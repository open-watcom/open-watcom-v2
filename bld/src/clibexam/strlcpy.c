#include <stdio.h>
#include <string.h>

void main( void )
{
    char    buffer[10];

    printf( "%d:'%s'\n", strlcpy( buffer,
        "Buffer overflow", sizeof( buffer ) ), buffer );
}
