// Dump the String table data

#include "ipfcdump.h"

void readStrings( FILE *in, FILE *out )
{
    fputs( "\nStrings Data\n", out );
    if( eHdr.stringsSize ) {
        size_t  bytes = 0;
        wchar_t text[ WSTRING_MAX_LEN ];
        fseek( in, eHdr.stringsOffset, SEEK_SET );
        while( bytes < eHdr.stringsSize ) {
            bytes += readDictString( in, text );
            fprintf( out, "  %ls\n", text );
        }
    }
    else
        fputs( "  There are no strings\n", out );
}

