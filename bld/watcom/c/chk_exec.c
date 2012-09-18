#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define PREFIX1 "PASS compiling "

#if defined( WASM ) || defined( INLINETEST )
#define PREFIX2 "PASS decoding "
#else
#define PREFIX2 "PASS executing "
#endif

char buff[2048];

int main( int argc, char **argv )
{
    FILE        *fp;
    char        *chk;
    unsigned    line;
    unsigned    flip;
    int         rc;

    if( argc != 2 ) {
        puts( "usage: chk_exec <file>" );
        puts( "FAIL" );
        return( EXIT_FAILURE );
    }
    fp = fopen( argv[1], "r" );
    if( !fp ) {
        puts( "cannot open input file" );
        puts( "FAIL" );
        return( EXIT_FAILURE );
    }
    line = 0;
    flip = 0;
    for( ; ; ) {
        chk = fgets( buff, sizeof( buff ), fp );
        if( chk == NULL )
            break;
        ++line;
        if( flip == 0 ) {
            rc = memcmp( buff, PREFIX1, sizeof( PREFIX1 ) - 1 );
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
