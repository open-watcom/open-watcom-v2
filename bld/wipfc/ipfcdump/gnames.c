// Dump the global names table

#include <malloc.h>
#include "ipfcdump.h"

void readGNames( FILE *in, FILE *out )
{
    fputs( "\nGlobal Names\n", out );
    if( eHdr.gNameCount ) {
        uint16_t *name = calloc( eHdr.gNameCount, sizeof( uint16_t ) );
        if( name != NULL ) {
            uint16_t *tocnum = calloc( eHdr.gNameCount, sizeof( uint16_t ) );
            if (tocnum != NULL) {
                size_t count;
                fseek( in, eHdr.gNameOffset, SEEK_SET );
                fread( name, eHdr.gNameCount, sizeof( uint16_t ), in );
                fread( tocnum, eHdr.gNameCount, sizeof( uint16_t ), in );
                for( count = 0; count < eHdr.gNameCount; count++ )
                    fprintf(out, "%4.4x (%5hu) %ls -> %4.4x (%5hu)\n", name[ count ], name[ count ],
                    Vocabulary[ name[ count ] ], tocnum[ count ], tocnum[ count ] );
                free(tocnum);
            }
            free(name);
        }
    }
    else
        fputs("  No global names found\n", out);
}


