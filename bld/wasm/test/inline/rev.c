#include "stdio.h"
#define MAX_SIZE 120
main( int argc, char *argv[] )
{
    FILE *fp;
    int len;
    int offset = -2;
    char *buffer;
    char *ptr;
    char *loc;
    char linebuffer[MAX_SIZE];

    fp = fopen( argv[1], "r" );
    fseek( fp, -1, SEEK_END );
    len = ftell( fp );
    fseek( fp, 0, SEEK_SET );

    buffer = malloc( len + 1 );
    buffer[len] = '\0';
    loc = buffer+len;

    for( ; ; ) {
        offset++;
        ptr = fgets( linebuffer, MAX_SIZE, fp );
        if( ptr == NULL ) break;
        loc -= strlen( ptr );
        strncpy( loc, ptr, strlen( ptr ) );
    }

    /* now dump it back out */

    fclose( fp );
    fp = fopen( argv[1], "w" );

    fputs( buffer + offset, fp );

    fclose( fp );
}
