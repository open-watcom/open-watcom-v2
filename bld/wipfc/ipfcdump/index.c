// Dump the index data

#include <string.h>
#include <stdlib.h>
#include "ipfcdump.h"

static void processIndex( FILE *, FILE *, size_t );

void readIndex( FILE *in, FILE *out )
{
    fputs( "\nIndex\n", out );
    if( Hdr.indexCount ) {
        fseek( in, Hdr.indexOffset, SEEK_SET );
        processIndex( in, out, Hdr.indexCount );
    }
    else
        fputs( "  No index data is present\n", out );
}
/*****************************************************************************/
void readIcmdIndex( FILE *in, FILE *out) 
{
    fputs( "\nIcmd Index\n", out );
    if( Hdr.icmdCount ) {
        fseek(in, Hdr.icmdOffset, SEEK_SET );
        processIndex( in, out, Hdr.icmdCount );
    }
    else
        fputs( "  No Icmd index data is present\n", out );
}
/*****************************************************************************/
static void processIndex( FILE *in, FILE *out, size_t items )
{
    IndexItem   idx;
    size_t      count1;
    size_t      count2;
    char        buffer[ 256 ];
    wchar_t     text[ WSTRING_MAX_LEN ];

    for( count1 = 0; count1 < items; count1++ ) {
        fread( &idx, sizeof(IndexItem), 1, in );
        fprintf( out, "  Index Item #%u\n", count1 );
        fprintf( out, "    IndexItem.size:          %4.2x (%u)\n", idx.size, idx.size );
        fprintf( out, "    IndexItem.primary:       %4.4s\n", idx.primary ? "yes" : "no" );
        fprintf( out, "    IndexItem.secondary:     %4.4s\n", idx.secondary ? "yes" : "no" );
        fprintf( out, "    IndexItem.unknown:       %4.1x (%u)\n", idx.unknown, idx.unknown );
        fprintf( out, "    IndexItem.global:        %4.4s\n", idx.global ? "yes" : "no" );
        fprintf( out, "    IndexItem.sortKey:       %4.4s\n", idx.sortKey ? "yes" : "no" );
        fprintf( out, "    IndexItem.synonymCount:  %4.2x (%u)\n", idx.synonymCount, idx.synonymCount );
        fprintf( out, "    IndexItem.tocPanelIndex: %4.4x (%hu)\n", idx.tocPanelIndex, idx.tocPanelIndex );
        if( idx.sortKey ) {
            memset(text, 0, 256 * sizeof( wchar_t ) );
            readDictString( in, text );
            fprintf( out, "    IndexItem.sortKeyText:     %ls\n", text );
        }
        memset( buffer, 0, sizeof( buffer ) );
        memset( text, 0, sizeof( text ) );
        fread( buffer, sizeof( uint8_t ), idx.size, in );
        mbstowcs( text, buffer, WSTRING_MAX_LEN );
        fprintf( out, "    IndexItem.text:          %ls\n", text );
        for( count2 = 0; count2 < idx.synonymCount; count2++ ) {
            uint32_t    offset;
            uint32_t     position;
            uint16_t   synonymSize;
            uint16_t   total = 0;
            fread( &offset, sizeof(unsigned long int), 1, in );
            fprintf( out, "    IndexItem.synonyms[%u]: %8.8x (%lu)\n", count2, offset, offset );
            position = ftell( in );
            fseek( in, offset, SEEK_SET );
            fread( &synonymSize, sizeof( uint16_t ), 1, in );
            while( total < synonymSize ) {
                total += readDictString( in, text );
                fprintf( out, "      %ls\n", text);
            }
            fseek(in, position, SEEK_SET);
        }
    }
}

