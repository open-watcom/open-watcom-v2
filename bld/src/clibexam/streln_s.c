#define __STDC_WANT_LIB_EXT1__  1
#include <stdio.h>
#include <string.h>
#include <errno.h>

void main()
{
    FILE    *fp;
    char    emsg[ 100 ];
    size_t  emsglen;

    fp = fopen( "file.nam", "r" );
    if( fp == NULL ) {
        emsglen = strerrorlen_s( errno );
        printf( "Length of errormessage: %d\n", emsglen );
        strerror_s( emsg, sizeof( emsg ), errno );
        printf( "Unable to open file: %s\n", emsg );
    }
}
