// Read the header and extended header of the IPF file

#include <stdlib.h>
#include <string.h>
#include "ipfcdump.h"

IpfHeader Hdr;
IpfExtHeader eHdr;

void readHeader( FILE *in, FILE *out )
{
    wchar_t wtext[ WSTRING_MAX_LEN ];

    fread( &Hdr, sizeof( IpfHeader ),1, in );
    if ( ( Hdr.id[0] != 'H' && Hdr.id[0] != 'I' ) || Hdr.id[1] != 'S' || Hdr.id[2] != 'P' ) {
        fputs( "This is not an OS/2 help file\n\a", out );
        exit( EXIT_FAILURE );
    }
    fputs( "Header Section\n", out);
    fprintf( out, "  IpfHeader.id:               %c%c%c\n", Hdr.id[0], Hdr.id[1], Hdr.id[2]);
    fprintf( out, "  IpfHeader.flags:            %8.2x (%s format)\n", Hdr.flags, ( Hdr.flags & 0x01 ) ? "INF" : "HLP" );
    fprintf( out, "  IpfHeader.size:             %8.4x (%hu)\n", Hdr.size, Hdr.size );
    fprintf( out, "  IpfHeader.version:          %d.%d\n", Hdr.version_hi, Hdr.version_lo );
    fprintf( out, "  IpfHeader.tocCount:         %8.4x (%hu)\n", Hdr.tocCount, Hdr.tocCount );
    fprintf( out, "  IpfHeader.tocOffset:        %8.8x (%lu)\n", Hdr.tocOffset, Hdr.tocOffset );
    fprintf( out, "  IpfHeader.tocSize:          %8.8x (%lu)\n", Hdr.tocSize, Hdr.tocSize );
    fprintf( out, "  IpfHeader.tocOffsetOffset:  %8.8x (%lu)\n", Hdr.tocOffsetOffset, Hdr.tocOffsetOffset );
    fprintf( out, "  IpfHeader.panelCount:       %8.4x (%hu)\n", Hdr.panelCount, Hdr.panelCount );
    fprintf( out, "  IpfHeader.panelOffset:      %8.8x (%lu)\n", Hdr.panelOffset, Hdr.panelOffset );
    fprintf( out, "  IpfHeader.nameCount:        %8.4x (%hu)\n", Hdr.nameCount, Hdr.nameCount );
    fprintf( out, "  IpfHeader.nameOffset:       %8.8x (%lu)\n", Hdr.nameOffset, Hdr.nameOffset );
    fprintf( out, "  IpfHeader.indexCount:       %8.4x (%hu)\n", Hdr.indexCount, Hdr.indexCount );
    fprintf( out, "  IpfHeader.indexOffset:      %8.8x (%lu)\n", Hdr.indexOffset, Hdr.indexOffset );
    fprintf( out, "  IpfHeader.indexSize:        %8.8x (%lu)\n", Hdr.indexSize, Hdr.indexSize );
    fprintf( out, "  IpfHeader.icmdCount:        %8.4x (%hu)\n", Hdr.icmdCount, Hdr.icmdCount );
    fprintf( out, "  IpfHeader.icmdOffset:       %8.8x (%lu)\n", Hdr.icmdOffset, Hdr.icmdOffset );
    fprintf( out, "  IpfHeader.icmdSize:         %8.8x (%lu)\n", Hdr.icmdSize, Hdr.icmdSize );
    fprintf( out, "  IpfHeader.searchOffset:     %8.8x (%lu)\n", Hdr.searchOffset, Hdr.searchOffset );
    fprintf( out, "            Size of search record is %s-bit\n", ( Hdr.recSize ? "16" : "8" ) );
    fprintf( out, "  IpfHeader.searchSize:       %8.8x (%lu)\n", Hdr.searchSize, Hdr.searchSize );
    fprintf( out, "  IpfHeader.cellCount:        %8.4x (%hu)\n", Hdr.cellCount, Hdr.cellCount );
    fprintf( out, "  IpfHeader.cellOffsetOffset: %8.8x (%lu)\n", Hdr.cellOffsetOffset, Hdr.cellOffsetOffset );
    fprintf( out, "  IpfHeader.dictSize:         %8.8x (%lu)\n", Hdr.dictSize, Hdr.dictSize );
    fprintf( out, "  IpfHeader.dictCount:        %8.4x (%hu)\n", Hdr.dictCount, Hdr.dictCount );
    fprintf( out, "  IpfHeader.dictOffset:       %8.8x (%lu)\n", Hdr.dictOffset, Hdr.dictOffset );
    fprintf( out, "  IpfHeader.imageOffset:      %8.8x (%lu)\n", Hdr.imageOffset, Hdr.imageOffset );
    fprintf( out, "  IpfHeader.maxCVTIndex:      %8.2x (%hu)\n", Hdr.maxCVTIndex, Hdr.maxCVTIndex) ;
    fprintf( out, "  IpfHeader.nlsOffset:        %8.8x (%lu)\n", Hdr.nlsOffset, Hdr.nlsOffset );
    fprintf( out, "  IpfHeader.nlsSize:          %8.8x (%lu)\n", Hdr.nlsSize, Hdr.nlsSize );
    fprintf( out, "  IpfHeader.extOffset:        %8.8x (%lu)\n", Hdr.extOffset, Hdr.extOffset );
    fprintf( out, "  IpfHeader.reserved: %2.2x %2.2x %2.2x %2.2x %2.2x %2.2x %2.2x %2.2x %2.2x %2.2x %2.2x %2.2x\n",
        Hdr.reserved[0], Hdr.reserved[1], Hdr.reserved[2], Hdr.reserved[3], Hdr.reserved[4], Hdr.reserved[5], Hdr.reserved[6],
        Hdr.reserved[7], Hdr.reserved[8], Hdr.reserved[9], Hdr.reserved[10], Hdr.reserved[11] );
    mbstowcs( wtext, (char *)Hdr.title, WSTRING_MAX_LEN );
    fprintf( out, "  IpfHeader.title:            %ls\n", wtext );
    fseek( in, Hdr.extOffset, SEEK_SET );
    fread( &eHdr, sizeof( IpfExtHeader ), 1, in );
    fputs( "\nExtended Header Section\n", out );
    fprintf( out, "  IpfExtHeader.fontCount:        %8.4x (%hu)\n", eHdr.fontCount, eHdr.fontCount );
    fprintf( out, "  IpfExtHeader.fontOffset:       %8.8x (%lu)\n", eHdr.fontOffset, eHdr.fontOffset );
    fprintf( out, "  IpfExtHeader.dbCount:          %8.4x (%hu)\n", eHdr.dbCount, eHdr.dbCount );
    fprintf( out, "  IpfExtHeader.dbOffset:         %8.8x (%lu)\n", eHdr.dbOffset, eHdr.dbOffset );
    fprintf( out, "  IpfExtHeader.dbSize:           %8.8x (%lu)\n", eHdr.dbSize, eHdr.dbSize );
    fprintf( out, "  IpfExtHeader.gNameCount:       %8.4x (%hu)\n", eHdr.gNameCount, eHdr.gNameCount );
    fprintf( out, "  IpfExtHeader.gNameOffset:      %8.8x (%lu)\n", eHdr.gNameOffset, eHdr.gNameOffset );
    fprintf( out, "  IpfExtHeader.stringsOffset:    %8.8x (%lu)\n", eHdr.stringsOffset, eHdr.stringsOffset );
    fprintf( out, "  IpfExtHeader.stringsSize:      %8.4x (%hu)\n", eHdr.stringsSize, eHdr.stringsSize );
    fprintf( out, "  IpfExtHeader.childPagesOffset: %8.8x (%lu)\n", eHdr.childPagesOffset, eHdr.childPagesOffset );
    fprintf( out, "  IpfExtHeader.childPagesSize:   %8.8x (%lu)\n", eHdr.childPagesSize, eHdr.childPagesSize );
    fprintf( out, "  IpfExtHeader.gIndexCount:      %8.8x (%lu)\n", eHdr.gIndexCount, eHdr.gIndexCount );
    fprintf( out, "  IpfExtHeader.ctrlOffset:       %8.8x (%lu)\n", eHdr.ctrlOffset, eHdr.ctrlOffset );
    fprintf( out, "  IpfExtHeader.ctrlSize:         %8.8x (%lu)\n", eHdr.ctrlSize, eHdr.ctrlSize );
    fprintf( out, "  IpfExtHeader.reserved:         %8.8x %8.8x %8.8x %8.8x\n",
        eHdr.reserved[0], eHdr.reserved[1], eHdr.reserved[2], eHdr.reserved[3] );
}
