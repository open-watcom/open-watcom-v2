#define __STDC_WANT_LIB_EXT1__  1
#include <stdio.h>
#include <string.h>

void main( void )
{
    char buffer[15];

    strncpy_s( buffer, sizeof( buffer ), "abcdefg", 10 );
    printf( "%s\n", buffer );

    strncpy_s( buffer, sizeof( buffer ), "1234567",  6 );
    printf( "%s\n", buffer );

    strncpy_s( buffer, sizeof( buffer ), "abcdefg",  3 );
    printf( "%s\n", buffer );

    strncpy_s( buffer, sizeof( buffer ), "*******",  0 );
    printf( "%s\n", buffer );
}
