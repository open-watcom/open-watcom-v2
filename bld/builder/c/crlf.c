#include <stdio.h>
#include "bool.h"


#define CR  13
#define LF  10

int main( int argc, char **argv )
{
    bool    to_crlf;
    FILE    *fi;
    FILE    *fo;
    int     c;
    int     prev;

    argc = argc;
#ifdef __UNIX__
    to_crlf = false;
#else
    to_crlf = true;
#endif
    ++argv;
    while( *argv != NULL && **argv == '-' ) {
        if( argv[0][1] == 'd' || argv[0][1] == 'D' ) {
            to_crlf = true;
        } else if( argv[0][1] == 'u' || argv[0][1] == 'U' ) {
            to_crlf = false;
        } else {
            printf( "Option '%s' not recognized.\n", *argv );
            return( 1 );
        }
        ++argv;
    }
    if( *argv == NULL ) {
        printf( "Missing input file name.\n" );
        return( 2 );
    }
    fi = fopen( *argv, "rb" );
    if( fi == NULL ) {
        printf( "Cannot open input file '%s'.\n", *argv );
        return( 3 );
    }
    ++argv;
    if( *argv == NULL ) {
        fclose( fi );
        printf( "Missing output file name.\n" );
        return( 4 );
    }
    fo = fopen( *argv, "wb" );
    if( fo == NULL ) {
        printf( "Cannot open output file '%s'.\n", *argv );
        fclose( fi );
        return( 5 );
    }
    prev = '\0';
    while( (c = fgetc( fi )) != EOF ) {
        if( c == CR ) {
            if( prev == CR ) {
                fputc( CR, fo );
            }
        } else if( c == LF ) {
            if( to_crlf ) {
                fputc( CR, fo );
                fputc( LF, fo );
            } else {
                fputc( LF, fo );
            }
        } else {
            fputc( c, fo );
        }
        prev = c;
    }
    fclose( fi );
    fclose( fo );
    return( 0 );
}
