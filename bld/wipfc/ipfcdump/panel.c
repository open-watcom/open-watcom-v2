// Dump the panel numbers and names

#include <malloc.h>
#include "ipfcdump.h"

static void readPanelNumbers( FILE *in, FILE *out );
static void readPanelNames( FILE *in, FILE *out );

void readPanels( FILE *in, FILE *out )
{
    readPanelNumbers(in, out);
    readPanelNames(in, out);
}
/*****************************************************************************/
static void readPanelNumbers( FILE *in, FILE *out )
{
    fputs( "\nPanel resource number to TOC index map\n", out );
    if( Hdr.panelCount)
        {
        uint16_t *resnum = calloc( Hdr.panelCount, sizeof( uint16_t ) );
        if( resnum != NULL ) {
            uint16_t *tocnum = calloc( Hdr.panelCount, sizeof( uint16_t ) );
            if( tocnum != NULL ) {
                size_t count;
                fseek( in, Hdr.panelOffset, SEEK_SET );
                fread( resnum, Hdr.panelCount, sizeof( uint16_t ), in );
                fread( tocnum, Hdr.panelCount, sizeof( uint16_t ), in );
                for( count = 0; count < Hdr.panelCount; count++)
                    fprintf(out, "%4.4x (%5hu) -> %4.4x (%5hu)\n", resnum[ count ],
                    resnum[ count ], tocnum[ count ], tocnum[ count ] );
                free( tocnum );
            }
            free( resnum );
        }
    }
    else
        fputs( "  There are no numbered panels\n", out );
}
/*****************************************************************************/
static void readPanelNames( FILE *in, FILE *out )
{
    fputs( "\nPanel name to TOC index map\n", out );
    if( Hdr.nameCount)
        {
        uint16_t *name = calloc(Hdr.nameCount, sizeof( uint16_t ) );
        if( name != NULL ) {
            uint16_t *tocnum = calloc( Hdr.nameCount, sizeof( uint16_t ) );
            if( tocnum != NULL ) {
                size_t count;
                fseek( in, Hdr.nameOffset, SEEK_SET );
                fread( name, Hdr.nameCount, sizeof( uint16_t ), in );
                fread( tocnum, Hdr.nameCount, sizeof( uint16_t ), in );
                for( count = 0; count < Hdr.nameCount; count++ )
                    fprintf(out, "%4.4x (%5hu) %ls -> %4.4x (%5hu)\n", name[ count ],
                    name[ count ], Vocabulary[ name[ count ] ], tocnum[ count ], tocnum[ count ] );
                free( tocnum );
            }
            free( name );
        }
    }
    else
        fputs( "  There are no named panels\n", out );
}
