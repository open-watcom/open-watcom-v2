#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char    buff[2048];
char    prefix[] = "PASS executing ";
size_t  plen;

int main( int argc, char **argv ) {
    FILE *fp;
    char *chk;
    unsigned line;

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
    plen = strlen( prefix );
    line = 0;
    for(;;) {
        chk = fgets( buff, sizeof( buff ), fp );
        if( chk == NULL )
            break;
        ++line;
        if( memcmp( buff, prefix, plen ) ) {
            printf( "detected on line %u\n", line );
            printf( "contents: %s", buff );
            puts( "FAIL" );
            return( EXIT_FAILURE );
        }
    }
    fclose( fp );
    puts( "PASS" );
    return( EXIT_SUCCESS );
}
