#include <stdio.h>
#include <unistd.h>

int main( int argc, char **argv )
{
    int     c;
    char    *ifile;
    char    *ofile;

    while( (c = getopt( argc, argv, ":abf:o:" )) != -1 ) {
        switch( c ) {
        case 'a':
            printf( "option a is set\n" );
            break;
        case 'b':
            printf( "option b is set\n" );
            break;
        case 'f':
            ifile = optarg;
            printf( "input filename is '%s'\n", ifile );
            break;
        case 'o':
            ofile = optarg;
            printf( "output filename is '%s'\n", ofile );
            break;
        case ':':
            printf( "-%c without filename\n", optopt );
            break;
        case '?':
            printf( "usage: %s -ab -f <filename> -o <filename>\n", argv[0] );
            break;
        }
    }
    return( 0 );
}
