// Dump the Child Pages table data

#include "ipfcdump.h"

void readChildPages( FILE *in, FILE *out )
{
    fputs( "\nChild Pages Table\n", out );
    if (eHdr.childPagesSize) {
        ChildPages cp;
        size_t bytes = 0;
        size_t count;
        uint16_t data[ 128 ];
        fseek( in, eHdr.childPagesOffset, SEEK_SET );
        while (bytes < eHdr.childPagesSize) {
            fread( &cp, sizeof( ChildPages ), 1, in );
            fread( data, sizeof( uint16_t ), ( cp.size - sizeof( ChildPages ) ) / sizeof( uint16_t ), in);
            bytes += cp.size;
            fprintf(out, "  Children of TOC entry %4.4x (%hu):\n", cp.parent, cp.parent );
            for (count = 0; count < ( cp.size - sizeof( ChildPages ) ) / sizeof( uint16_t ); count++ ) {
                fprintf(out, "%4.4x (%hu) ", data[ count ], data[ count ]);
                if( !( ( count + 1 ) & 7 ) )
                    fputc('\n', out);
            }
            fputc('\n', out);
        }
    }
    else
        fputs("  There are no child page records\n", out);
}

