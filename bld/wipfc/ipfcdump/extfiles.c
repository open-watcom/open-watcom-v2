// Dumps the names of external database (help) files referenced by this file

#include "ipfcdump.h"

void readExtFiles( FILE *in, FILE *out )
{
    fputs( "\nExternal File References\n", out );
    if( eHdr.dbCount ) {
        size_t  count;
        wchar_t name[ WSTRING_MAX_LEN ];
        fseek( in, eHdr.dbOffset, SEEK_SET );
        for( count = 0; count < eHdr.dbCount; count++ ) {
            readDictString( in, name );
            fprintf( out, "  File #%u: %ls\n", count, name );
        }
    }
    else
        fputs("  No external file references found\n", out);
}
