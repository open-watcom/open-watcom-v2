// Dump the dictionary

#include <malloc.h>
#include <string.h>
#include <stdlib.h>
#include "ipfcdump.h"

wchar_t **Vocabulary;

void readDictionary( FILE *in, FILE *out )
{
Vocabulary = calloc( Hdr.dictCount, sizeof( wchar_t * ) );
if( Vocabulary != NULL ) {
    size_t count1;
    size_t size;
    wchar_t text[ WSTRING_MAX_LEN ];
    fputs( "\nDictionary (vocabulary list)\n", out );
    fseek( in, Hdr.dictOffset, SEEK_SET );
    for( count1 = 0; count1 < Hdr.dictCount; count1++ ) {
        size = readDictString( in, text );
        fprintf( out, "  %4.4x (%5d): (%2.2x, %2.2hu) [%ls]\n", count1, count1, size, size, text );
        Vocabulary[ count1 ] = calloc( size, sizeof( wchar_t ) );
        if (Vocabulary[ count1 ] == NULL)
            exit( EXIT_FAILURE );
        memcpy( Vocabulary[ count1 ], text, ( size - 1 ) * sizeof( wchar_t ) );
        }
    }
}
