/* Dumps the structure of an OS/2 IPF help file
*/

#include <malloc.h>
#include <stdlib.h>
#include "ipfcdump.h"

static int parseFile( char * );

int main( int argc, char **argv )
{
    if( argc < 2 ) {
        puts( "Usage: ipfcdump filename\n" );
        return( EXIT_FAILURE );
    }
    return parseFile( argv[1] );
}
/*****************************************************************************/
static int parseFile( char *filename )
{
    FILE *in = fopen( filename, "rb" );
    //FILE *out = stdout;
    FILE *out = fopen( "ipfcdump.txt", "w" );
    if( in ) {
        fprintf( out, "File name: %s\n", filename );
        readHeader( in, out );
        readExtFiles( in, out );
        readStrings( in, out );
        readNLS( in, out );
        readFonts( in, out );
        readControls( in, out );
        readTOC( in, out );
        readDictionary( in, out );
        readGNames( in, out );
        readPanels( in, out );
        readCells( in, out );
        readBitMaps( in, out );
        readIndex( in, out );
        readIcmdIndex( in, out );
        readFTS( in, out );
        readChildPages( in, out );
        if( Vocabulary != NULL ) {  //free Vocabulary strings
            unsigned int count;
            for( count = 0; count < Hdr.dictCount; count++ )
                free( Vocabulary[ count ] );
            free( Vocabulary );
        }
        fclose( out );
        fclose( in );
        return( EXIT_SUCCESS );
    }
    else {
        fprintf(stderr, "Cannot open %s\n\a", filename);
        return( EXIT_FAILURE );
    }
}
/*****************************************************************************/
size_t readDictString(FILE *in, wchar_t *buffer)
{
    char    temp[ STRING_MAX_LEN ];
    size_t  length = fgetc( in ) - 1;
    fread( temp, sizeof( char ), length, in );
    temp[ length ] = '\0';
    length = mbstowcs( buffer, temp, WSTRING_MAX_LEN );
    return( length + 1 );
}
