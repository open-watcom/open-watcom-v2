// Dump the TOC data

#include <stdlib.h>
#include "ipfcdump.h"
static void readTOCEntry( FILE *, FILE *, uint32_t, size_t, size_t * );


void readTOC( FILE *in, FILE *out )
{
    uint32_t *tocOffsets = calloc( Hdr.tocCount, sizeof( uint32_t ) );
    if( tocOffsets != NULL ) {
        size_t count1;
        size_t visCount = 0;
        fseek( in, Hdr.tocOffsetOffset, SEEK_SET );
        fread( tocOffsets, sizeof( uint32_t ), Hdr.tocCount, in );
        fputs( "\nTable of Contents\n", out );
        fseek( in, Hdr.tocOffset, SEEK_SET );
        for( count1 = 0; count1 < Hdr.tocCount; count1++ )
            readTOCEntry(in, out, tocOffsets[ count1 ], count1, &visCount );
        fprintf( out,"There are %u visible pages\n", visCount);
    }
    free(tocOffsets);
}
/*****************************************************************************/
static void readTOCEntry( FILE *in, FILE *out, uint32_t offset, size_t count1, size_t *visCount )
{
    TocEntry toc;
    size_t count2;
    size_t textSize;
    uint16_t cellIndex;
    char text[ 256 ];
    wchar_t wtext[ WSTRING_MAX_LEN ];
    //long int nextPos = ftell(in);
    fread( &toc, sizeof(TocEntry), 1, in );
    fprintf( out, "  TOC Entry #%u at offset %8.8x (%lu)\n", count1, offset, offset );
    fprintf( out, "    TocEntry.size:        %2.2x (%u)\n", toc.size, toc.size );
    //nextPos += toc.size;
    fprintf( out, "    TocEntry.level:       %u\n", toc.nestLevel, toc.nestLevel );
    fprintf( out, "    TocEntry.unknown:     %s\n", toc.unknown ? "set" : "clear" );
    fprintf( out, "    TocEntry.extended:    %s\n", toc.extended ? "yes" : "no" );
    fprintf( out, "    TocEntry.hidden:      %s\n", toc.hidden ? "yes" : "no" );
    if( !toc.hidden)
        ++*visCount;
    fprintf( out, "    TocEntry.hasChildren: %s\n", toc.hasChildren ? "yes" : "no" );
    fprintf( out, "    TocEntry.cellCount:   %2.2x (%u)\n", toc.cellCount, toc.cellCount );
    textSize = toc.size - 3 - toc.cellCount * sizeof( uint16_t );
    if( toc.extended ) {
        ExtTocEntry etoc;
        textSize -= sizeof( ExtTocEntry );
        fread(&etoc, sizeof( ExtTocEntry ), 1, in );
        fprintf( out, "    Extended TOC Entry\n");
        fprintf( out, "      ExtTocEntry.setPos:   %s\n", etoc.setPos ? "yes" : "no" );
        fprintf( out, "      ExtTocEntry.setSize:  %s\n", etoc.setSize ? "yes" : "no" );
        fprintf( out, "      ExtTocEntry.setView:  %s\n", etoc.setView ? "yes" : "no" );
        fprintf( out, "      ExtTocEntry.setStyle: %s\n", etoc.setStyle ? "yes" : "no" );
        fprintf( out, "      ExtTocEntry.noSearch: %s\n", etoc.noSearch ? "yes" : "no" );
        fprintf( out, "      ExtTocEntry.noPrint:  %s\n", etoc.noPrint ? "yes" : "no" );
        fprintf( out, "      ExtTocEntry.setCtrl:  %s\n", etoc.setCtrl ? "yes" : "no" );
        fprintf( out, "      ExtTocEntry.setTutor: %s\n", etoc.setTutor ? "yes" : "no" );
        fprintf( out, "      ExtTocEntry.clear:    %s\n", etoc.clear ? "clear" : "set" );
        fprintf( out, "      ExtTocEntry.unknown1: %1.1x\n", etoc.unknown1);
        fprintf( out, "      ExtTocEntry.setGroup: %s\n", etoc.setGroup ? "yes" : "no");
        fprintf( out, "      ExtTocEntry.isParent: %s\n", etoc.isParent ? "yes" : "no");
        fprintf( out, "      ExtTocEntry.unknown2: %1.1x\n", etoc.unknown2);
        if( etoc.setPos ) {
            PanelOrigin p;
            textSize -= sizeof( PanelOrigin );
            fread( &p, sizeof( PanelOrigin ), 1, in );
            fputs( "    Set panel position\n", out );
            fprintf( out, "      PanelOrigin.xPosType: %4.2x (%s)\n", p.xPosType, getPosString( p.xPosType ) );
            fprintf( out, "      PanelOrigin.yPosType: %4.2x (%s)\n", p.yPosType, getPosString( p.yPosType ) );
            if( p.xPosType == DYNAMIC )
                fprintf( out, "      PanelOrigin.xPos:     %s\n", getDPosString( p.xpos ) );
            else
                fprintf( out, "      PanelOrigin.xPos:     %4.4x (%hu)\n", p.xpos, p.xpos );
            if( p.yPosType == DYNAMIC )
                fprintf( out, "      PanelOrigin.yPos:     %s\n", getDPosString( p.ypos ) );
            else
                fprintf( out, "      PanelOrigin.yPos:     %4.4x (%hu)\n", p.ypos, p.ypos );
        }
        if( etoc.setSize ) {
            PanelSize p;
            textSize -= sizeof( PanelSize );
            fread( &p, sizeof( PanelSize ), 1, in);
            fputs( "    Set panel size\n", out);
            fprintf( out, "      PanelSize.widthType:  %4.2x (%s)\n", p.widthType, getPosString( p.widthType ) );
            fprintf( out, "      PanelSize.heightType: %4.2x (%s)\n", p.heightType, getPosString( p.heightType ) );
            if( p.widthType == DYNAMIC )
                fprintf( out, "      PanelSize.width:      %s\n", getDPosString( p.width ) );
            else
                fprintf( out, "      PanelSize.width:      %4.4x (%hu)\n", p.width, p.width );
            if( p.widthType == DYNAMIC)
                fprintf( out, "      PanelSize.height:     %s\n", getDPosString( p.height ) );
            else
                fprintf( out, "      PanelSize.height:     %4.4x (%hu)\n", p.height, p.height );
        }
        if( etoc.setStyle ) {
            PanelStyle p;
            textSize -= sizeof( PanelStyle );
            fread( &p, sizeof( PanelStyle), 1, in );
            fputs( "    Set panel style\n", out );
            fprintf( out, "      PanelStyle.style: %4.4x (%hu)\n", p.word, p.word );
        }
        if( etoc.setGroup)
            {
            Group g;
            textSize -= sizeof( Group );
            fread( &g, sizeof( Group ), 1, in );
            fputs( "    Set panel group\n", out );
            fprintf( out, "      Group.id: %4.4x (%hu)\n", g.id, g.id );
            }
        if( etoc.setCtrl)
            {
            PanelControls p;
            textSize -= sizeof( PanelControls );
            fread( &p, sizeof( PanelControls ), 1, in );
            fputs( "    Set panel controls\n", out);
            fprintf( out, "      PanelControls.word: %4.4x (%hu)\n", p.word, p.word );
            }
        }
    fputs( "    Cell indexes:         ", out);
    for( count2 = 0; count2 < toc.cellCount; count2++ ) {
        fread( &cellIndex, sizeof( uint16_t ), 1, in );
        fprintf( out, "%4.4x (%hu) ", cellIndex, cellIndex );
    }
    fputc( '\n', out);
    fread( text, sizeof( char ), textSize, in );
    text[ textSize ] = '\0';
    mbstowcs( wtext, text, WSTRING_MAX_LEN );
    fprintf( out, "    TocEntry.title:       %ls\n", wtext );
    fflush( out );
    //fseek(in, nextPos, SEEK_SET);
}
/*****************************************************************************/
char *getPosString( uint8_t type )
{
    static char *Pos[] = {"Absolute Character", "Relative Percent", "Absolute Pixel", "Absolute Points", "Dynamic"};
    return( Pos[ type ] );
}
/*****************************************************************************/
char *getDPosString( uint8_t type )
{
static char *dPos[] = {"Left", "Right", "Top", "Bottom", "Center"};
char *value;
if( type & DYNAMIC_LEFT)
    value = dPos[ 0 ];
if( type & DYNAMIC_RIGHT)
    value = dPos[ 1 ];
if( type & DYNAMIC_TOP)
    value = dPos[ 2 ];
if( type & DYNAMIC_BOTTOM)
    value = dPos[ 3 ];
if( type & DYNAMIC_CENTER)
    value = dPos[ 4 ];
return( value );
}

