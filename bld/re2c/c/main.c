#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "globals.h"
#include "parser.h"
#include "dfa.h"

const char  *fileName = NULL;
bool        sFlag = false;
bool        bFlag = false;
bool        iFlag = false;
bool        bUsedYYAccept = false;
uint        oline = 1;

static void usage( void )
{
    fprintf( stderr,
        "usage: re2c [-bdfis] file\n"
        "\n"
        "-b      Implies -s. Use bit vectors as well in the attempt to\n"
        "        coax better code out of the compiler. Most useful for\n"
        "        specifications with more than a few keywords (e.g. for\n"
        "        most programming languages).\n"
        "-i      Do not generate '#line' info (usefull for versioning).\n"
        "-s      Generate nested ifs for some switches. Many compilers\n"
        "        need this assist to generate better code.\n" );
}

static char *fname_normalize( const char *name )
{
    char    *p;
    char    *dst;

    for( dst = p = strdup( name ); *p != '\0'; p++ ) {
        if( *p != '\\' )
            continue;
        *p = '/';
    }
    return( dst );
}

int main( int argc, char *argv[] )
{
    FILE    *fi;
    char    *p;

    fileName = NULL;

    if( argc == 1 ) {
        usage();
        return 2;
    }

    while( --argc > 1 ){
        p = *++argv;
        while( *++p != '\0' ) {
            switch( *p ) {
            case 'b':
                sFlag = true;
                bFlag = true;
                break;
            case 's':
                sFlag = true;
                break;
            case 'i':
                iFlag = true;
                break;
            default:
                usage();
                return 2;
            }
        }
    }
    fileName = *++argv;
    if( fileName[0] == '-' && fileName[1] == '\0' ) {
        fileName = "<stdin>";
        fi = stdin;
    } else {
        if( (fi = fopen( fileName, "r" )) == NULL ) {
            fprintf( stderr, "can't open %s\n", fileName );
            return 1;
        }
    }
    fileName = fname_normalize( fileName );
    parse( fi, stdout );
    free( (void *)fileName );
    return 0;
}
