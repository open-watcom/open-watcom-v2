#define __STDC_WANT_LIB_EXT1__  1
#include <stdio.h>
#include <string.h>
#include <errno.h>

void main()
{
    FILE *fp;
    char emsg[ 100 ];

    fp = fopen( "file.nam", "r" );
    if( fp == NULL ) {
        strerror_s( emsg, sizeof( emsg ), errno );
        printf( "Unable to open file: %s\n", emsg );
    }
}
