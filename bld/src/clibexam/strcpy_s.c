#define __STDC_WANT_LIB_EXT1__  1
#include <stdio.h>
#include <string.h>

void main()
{
    auto char buffer[80];

    strcpy_s( buffer, sizeof( buffer ), "Hello " );
    strcat_s( buffer, sizeof( buffer ), "world" );
    printf( "%s\n", buffer );
}
