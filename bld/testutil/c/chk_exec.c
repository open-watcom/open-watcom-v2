#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "bool.h"


#define PREFIX1     "PASS compiling "
#define PREFIX2     "PASS executing "
#define PREFIX2_d   "PASS decoding "

static char buff[2048];

int main( int argc, char **argv )
{
    FILE        *fp;
    char        *chk;
    unsigned    line;
    unsigned    flip;
    int         rc;
    int         i;
    bool        decoding_format;

    if( argc < 2 ) {
        puts( "usage: chk_exec <options> <file>" );
        puts( "options: -d WASM or INLINE tests decoding format" );
        puts( "FAIL" );
        return( EXIT_FAILURE );
    }
    decoding_format = false;
    i = 1;
    while( i < argc ) {
        if( argv[i][0] != '-' ) {
            break;
        }
        if( argv[i][1] == 'd' ) {
            decoding_format = true;
        } else {
            puts( "unknown option, skip it" );
        }
        i++;
    }
    if( i == argc ) {
        puts( "missing input file" );
        puts( "" );
        puts( "usage: chk_exec <options> <input file>" );
        puts( "options: -d WASM or INLINE tests decoding format" );
        puts( "FAIL" );
        return( EXIT_FAILURE );
    }
    fp = fopen( argv[i], "r" );
    if( !fp ) {
        puts( "cannot open input file" );
        puts( "FAIL" );
        return( EXIT_FAILURE );
    }
    line = 0;
    flip = 0;
    for( ;; ) {
        chk = fgets( buff, sizeof( buff ), fp );
        if( chk == NULL )
            break;
        ++line;
        if( flip == 0 ) {
            rc = memcmp( buff, PREFIX1, sizeof( PREFIX1 ) - 1 );
        } else if( decoding_format ) {
            rc = memcmp( buff, PREFIX2_d, sizeof( PREFIX2_d ) - 1 );
        } else {
            rc = memcmp( buff, PREFIX2, sizeof( PREFIX2 ) - 1 );
        }
        if( rc ) {
            printf( "detected on line %u\n", line );
            printf( "contents: %s", buff );
            puts( "FAIL" );
            return( EXIT_FAILURE );
        }
        flip ^= 1;
    }
    if( line <= 2 || line & 1 ) {
        puts( "incorrect # of lines!" );
        puts( "FAIL" );
        return( EXIT_FAILURE );
    }
    fclose( fp );
    puts( "PASS" );
    return( EXIT_SUCCESS );
}
