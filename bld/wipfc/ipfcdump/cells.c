// Dump the cell data

#include <malloc.h>
#include <string.h>
#include "ipfcdump.h"

static void readCell( FILE *, FILE * );
static unsigned int readEscape( FILE *, uint8_t *, uint8_t, uint8_t, size_t );
static void parseAlign( FILE *, uint8_t );
static char * parseColor( uint8_t );

void readCells(FILE *in, FILE *out)
{
    uint32_t *cellOffsets = calloc( Hdr.cellCount, sizeof( uint32_t ) );
    if( cellOffsets != NULL ) {
        unsigned int count;
        fseek( in, Hdr.cellOffsetOffset, SEEK_SET );
        fread( cellOffsets, sizeof( uint32_t ), Hdr.cellCount, in );
        fputs( "\nCells\n", out );
        for( count = 0; count < Hdr.cellCount; count++ ) {
            fseek( in, cellOffsets[ count ], SEEK_SET );
            fprintf( out, "  Cell #%u at offset %8.8x\n", count, cellOffsets[ count ] );
            readCell( in, out );
            }
        }
    free( cellOffsets );
    }
/*****************************************************************************/
static void readCell( FILE *in, FILE *out )
{
    Cell cell;
    uint8_t *text = NULL;
    uint16_t dict[ 256 ];
    uint8_t size;
    uint8_t type;
    uint16_t offset;
    fread( &cell, sizeof( Cell ), 1, in );
    fprintf(out, "    Flags:      %8.2x (%u)\n", cell.zero, cell.zero);
    fprintf(out, "    Dictionary: %8.8x (%lu)\n", cell.dictOffset, cell.dictOffset);
    fprintf(out, "    Dict size:  %8.2x (%u)\n", cell.dictCount, cell.dictCount);
    fprintf(out, "    Text size:  %8.4x (%u)\n", cell.textCount, cell.textCount);
    text = calloc( cell.textCount, sizeof( uint8_t ) );
    if( text != NULL ) {
        size_t count1;
        size_t count2 = 1;
        fread( text, cell.textCount, sizeof( uint8_t ), in );
        fseek( in, cell.dictOffset, SEEK_SET );
        memset( dict, 0, 256 * sizeof( uint16_t ) );
        fread( dict, cell.dictCount, sizeof( uint16_t ), in );
        for(count1 = 0; count1 < cell.textCount; count1++, count2++) {
            switch( text[ count1 ] ) {
            case 0xFA:
                fprintf(out, "[(fa) </p>]");
                break;
            case 0xFB:
                fprintf(out, "[(fb) <center>]");
                break;
            case 0xFC:
                fprintf(out, "[(fc) <~spacing>]");
                break;
            case 0xFD:
                fprintf(out, "[(fd) <br />]");
                break;
            case 0xFE:
                fprintf(out, "[(fe) \" \"]");
                break;
            case 0xFF:
                size = text[ ++count1 ];
                type = text[ ++count1 ];
                fprintf(out, "[(ff) <ESC: ");
                count1 = readEscape( out, text, type, size, count1 );
                break;
            default:
                offset = dict[text[count1]];
    #ifdef DICT_TRACE
                fprintf(out, "[(%2.2x -> %4.4x) %ls]", text[count1], dict[text[count1]], Vocabulary[dict[text[count1]]]);
    #else
                fprintf(out, "[%ls]", Vocabulary[dict[text[count1]]]);
    #endif
            }
            if (!(count2 & 7))
                fputc('\n', out);
        }
        if ((count2 - 1) & 7)
            fputc('\n', out);
    }
    free(text);
}
/*****************************************************************************/
static unsigned int readEscape(FILE *out, uint8_t *text, uint8_t type, uint8_t size, size_t count)
{
    uint32_t larg;
    uint16_t sarg;
    uint8_t carg;
    uint8_t subcode;
    uint8_t buffer[ 256 ];
    switch( type ) {
    case 0x02:
        carg = text[ ++count ];
        fprintf( out, "02: set left margin of current line to %2.2x (%u)>]", carg, carg );
        break;
    case 0x03:
        carg = text[ ++count ];
        fprintf( out, "03: set right margin to %2.2x (%u)>]", carg, carg );
        break;
    case 0x04:
        carg = text[ ++count ];
        fprintf( out, "04: set style to %2.2x ", carg );
        if( carg == 0)
            fputs( "(plain text)>]", out );
        else if( carg == 1 )
            fputs( "(italic)>]", out );
        else if( carg == 2 )
            fputs( "(bold)>]", out );
        else if( carg == 3 )
            fputs( "(bold italic)>]", out );
        else if( carg == 4 )
            fputs( "(underscored)>]", out );
        else if( carg == 5 )
            fputs( "(italic underscored)>]", out );
        else if( carg == 6 )
            fputs( "(bold underscored)>]", out );
        else if( carg == 7 )
            fputs( "(bold italic underscored)>]", out );
        break;
    case 0x05:
        sarg = (uint16_t)text[ ++count ];
        sarg |= ( (uint16_t)text[ ++count ] ) << 8;
        fprintf( out, "05: begin cross-reference to TOC index %4.4x (%hu)", sarg, sarg );
        if( size > 4 ) {
            uint8_t carg2 = 0;
            carg = text[ ++count ];
            fprintf( out, ", flag byte1 %2.2x", carg );
            if( size > 5 ) {
                carg2 = text[ ++count ];
                fprintf( out, ", flag byte2 %2.2x", carg );
            }
            if( carg & 0x01 ) {
                PanelOrigin p;
                memcpy( &p, &text[ count + 1 ], sizeof( PanelOrigin ) );
                count += sizeof( PanelOrigin );
                fprintf( out, ", target position supplied (" );
                if( p.xPosType == DYNAMIC)
                    fprintf( out, "%s %s, ", getDPosString( p.xpos ), getPosString( p.xPosType ) );
                else
                    fprintf( out, "%hu %s,", p.xpos, getPosString(p.xPosType));
                if( p.yPosType == DYNAMIC)
                    fprintf( out, "%s %s)", getDPosString( p.ypos ), getPosString( p.yPosType ) );
                else
                    fprintf( out, "%hu %s)", p.ypos, getPosString( p.yPosType ) );
            }
            if( carg & 0x02 ) {
                PanelSize p;
                memcpy(&p, &text[ count + 1 ], sizeof( PanelSize ) );
                count += sizeof( PanelSize );
                fprintf( out, ", target size supplied (" );
                if( p.widthType == DYNAMIC )
                    fprintf( out, "%s %s, ", getDPosString( p.width ), getPosString( p.widthType ) );
                else
                    fprintf( out, "%hu %s,", p.width, getPosString( p.widthType ) );
                if( p.heightType == DYNAMIC )
                    fprintf( out, "%s %s)", getDPosString( p.height ), getPosString( p.heightType ) );
                else
                    fprintf( out, "%hu %s)", p.height, getPosString( p.heightType ) );
            }
            if( carg & 0x04 )
                fprintf( out, ", viewport" );
            if( carg & 0x08 ) {
                PanelStyle style;
                memcpy( &style.word, &text[ count + 1 ], sizeof( PanelStyle ) );
                count += sizeof( PanelStyle );
                fprintf( out, ", window style specified (%4.4x)", style.word );
            }
            if( carg & 0x40)
                fprintf( out, ", autolink" );
            if( carg & 0x80)
                fprintf( out, ", split window" );
            if( carg2 & 0x02)
                fprintf( out, ", dependent" );
            if( carg2 & 0x04) {
                Group grp;
                memcpy( &grp, &text[ count + 1 ], sizeof( Group ) );
                count += sizeof( Group );
                fprintf( out, ", group supplied (%4.4x)", grp );
            }
        }
        fprintf( out, ">]" );
        break;
    case 0x07:
        sarg = ( uint16_t )text[ ++count ];
        sarg |= ( ( uint16_t )text[ ++count ] ) << 8;
        fprintf( out, "07: begin link to footnote, TOC index %4.4x (%hu)>]", sarg, sarg );
        break;
    case 0x08:
        fprintf( out, "08: end of link>]" );
        break;
    case 0x0B:
        fprintf( out, "0B: begin monospaced>]" );
        break;
    case 0x0C:
        fprintf( out, "0C: end monospaced>]" );
        break;
    case 0x0D:
        carg = text[ ++count ];
        fprintf( out, "0D: set special color %2.2x (%u)>]", carg, carg );
        break;
    case 0x0E:
        carg = text[ ++count ];
        larg = ( uint32_t )text[ ++count ];
        larg |= ( ( uint32_t )text[ ++count ] ) << 8;
        larg |= ( ( uint32_t )text[ ++count ] ) << 16;
        larg |= ( ( uint32_t )text[ ++count ] ) << 24;
        fprintf( out, "0E: bitmap image" );
        parseAlign( out, carg );
        fprintf( out, ", from offset %8.8x (%lu)>]", larg, larg );
        break;
    case 0x0F:
        carg = text[ ++count ];
        fprintf( out, "0F: image map");
        subcode = text[ ++count ];
        fprintf( out, " subcode %2.2x (%u):", subcode, subcode);
        switch (subcode) {
        case 0:
            carg = text[ ++count ];
            larg = ( uint32_t )text[ ++count ];
            larg |= ( ( uint32_t )text[ ++count ] ) << 8;
            larg |= ( ( uint32_t )text[ ++count ] ) << 16;
            larg |= ( ( uint32_t )text[ ++count ] ) << 24;
            fprintf( out, " define" );
            parseAlign( out, carg );
            fprintf( out, ", from offset %8.8x (%lu)>]", larg, larg );
            break;
        case 1:
        case 2:
        case 3:
            fprintf( out, " define partial bitmap to %s", subcode == 3 ? "?" : ( subcode == 1 ? "panel" : "footnote" ) );
            sarg = ( uint16_t )text[ ++count ];
            sarg |= ( ( uint16_t )text[ ++count ] ) << 8;
            fprintf( out, " id %4.4x (%hu)", sarg, sarg);
            sarg = ( uint16_t )text[ ++count ];
            sarg |= ( ( uint16_t )text[ ++count ] ) << 8;
            fprintf( out, " (xorg = %4.4x (%hu);", sarg, sarg );
            sarg = ( uint16_t )text[ ++count ];
            sarg |= ( ( uint16_t )text[ ++count ] ) << 8;
            fprintf( out, " yorg = %4.4x (%hu);", sarg, sarg );
            sarg = ( uint16_t )text[ ++count ];
            sarg |= ( ( uint16_t )text[ ++count ] ) << 8;
            fprintf( out, " width = %4.4x (%hu);", sarg, sarg );
            sarg = ( uint16_t )text[ ++count ];
            sarg |= ( ( uint16_t )text[ ++count ] ) << 8;
            fprintf( out, " height = %4.4x (%hu))>]", sarg, sarg );
            break;
        case 4:
        case 5:
        case 6:
            fprintf( out, " define full bitmap to %s", subcode == 6 ? "?" : ( subcode == 4 ? "panel" : "footnote" ) );
            sarg = ( uint16_t )text[ ++count ];
            sarg |= ( ( uint16_t )text[ ++count ] ) << 8;
            fprintf( out, " id %4.4x (%hu)", sarg, sarg );
            break;
        case 7:
            carg = text[ ++count ];
            memset( buffer, 0, 256) ;
            ++count;
            memcpy( buffer, &text[ count ], size - 5 );
            count += size - 4;
            fprintf( out, " define full bitmap to application %s>]", buffer );
            break;
        case 8:
            carg = text[ ++count ];
            fprintf( out, " define full bitmap to application" );
            sarg = ( uint16_t )text[ ++count ];
            sarg |= ( ( uint16_t )text[ ++count ] ) << 8;
            fprintf( out, " (xorg = %4.4x (%hu);", sarg, sarg );
            sarg = ( uint16_t )text[ ++count ];
            sarg |= ( ( uint16_t )text[ ++count ] ) << 8;
            fprintf( out, " yorg = %4.4x (%hu);", sarg, sarg );
            sarg = ( uint16_t )text[ ++count ];
            sarg |= ( ( uint16_t )text[ ++count ] ) << 8;
            fprintf( out, " width = %4.4x (%hu);", sarg, sarg );
            sarg = ( uint16_t )text[ ++count ];
            sarg |= ( ( uint16_t )text[ ++count ] ) << 8;
            fprintf( out, " height = %4.4x (%hu))", sarg, sarg );
            memset( buffer, 0, 256 );
            ++count;
            memcpy( buffer, &text[ count ], size - 12 );
            count += size - 11;
            fprintf( out, " %s>]", buffer );
            break;
        case 9:
            sarg = ( uint16_t )text[ ++count ];
            sarg |= ( ( uint16_t )text[ ++count ] ) << 8;
            fprintf( out, " define full bitmap, send message to application id %4.4x (%hu)>]", sarg, sarg );
            break;
        case 10:
            sarg = ( uint16_t )text[ ++count ];
            sarg |= ( ( uint16_t )text[ ++count ] ) << 8;
            fprintf( out, " define partial bitmap, send message to application id %4.4x (%hu)", sarg, sarg );
            sarg = ( uint16_t )text[ ++count ];
            sarg |= ( ( uint16_t )text[ ++count ] ) << 8;
            fprintf( out, " (xorg = %4.4x (%hu);", sarg, sarg );
            sarg = ( uint16_t )text[ ++count ];
            sarg |= ( ( uint16_t )text[ ++count ] ) << 8;
            fprintf( out, " yorg = %4.4x (%hu);", sarg, sarg );
            sarg = ( uint16_t )text[ ++count ];
            sarg |= ( ( uint16_t )text[ ++count ] ) << 8;
            fprintf( out, " width = %4.4x (%hu);", sarg, sarg );
            sarg = ( uint16_t )text[ ++count ];
            sarg |= ( ( uint16_t )text[ ++count ] ) << 8;
            fprintf( out, " height = %4.4x (%hu))>]", sarg, sarg );
            break;
        case 12:
        case 13:
            sarg = ( uint16_t )text[ ++count ];
            sarg |= ( ( uint16_t )text[ ++count ] ) << 8;
            fprintf( out, " define partial bitmap to external id %4.4x (%hu)", sarg, sarg );
            sarg = ( uint16_t )text[ ++count ];
            sarg |= ( ( uint16_t )text[ ++count ] ) << 8;
            fprintf( out, " (xorg = %4.4x (%hu);", sarg, sarg );
            sarg = ( uint16_t )text[ ++count ];
            sarg |= ( ( uint16_t )text[ ++count ] ) << 8;
            fprintf( out, " yorg = %4.4x (%hu);", sarg, sarg );
            sarg = ( uint16_t )text[ ++count ];
            sarg |= ( ( uint16_t )text[ ++count ] ) << 8;
            fprintf( out, " width = %4.4x (%hu);", sarg, sarg );
            sarg = ( uint16_t )text[ ++count ];
            sarg |= ( ( uint16_t )text[ ++count ] ) << 8;
            fprintf( out, " height = %4.4x (%hu))>]", sarg, sarg );
            break;
        case 14:
        case 15:
            sarg = ( uint16_t )text[ ++count ];
            sarg |= ( ( uint16_t )text[ ++count ] ) << 8;
            fprintf( out, " define full bitmap to id %4.4x (%hu)>]", sarg, sarg );
            break;
        case 16:
            carg = text[ ++count ];
            fprintf( out, " define full bitmap to id %2.2x (%u)", carg, carg );
            carg = text[ ++count ];
            ++count;
            memset( buffer, 0, 256 );
            ++count;
            memcpy( buffer, &text[ count ], carg );
            count += carg - 1;
            fprintf( out, " in external file %s>]", buffer );
            break;
        case 17:
            carg = text[ ++count ];
            fprintf( out, " define partial bitmap to external file %2.2x (%u)", carg, carg );
            carg = text[ ++count ];
            sarg = ( uint16_t )text[ ++count ];
            sarg |= ( ( uint16_t )text[ ++count ] ) << 8;
            fprintf( out, " (xorg = %4.4x (%hu);", sarg, sarg );
            sarg = ( uint16_t )text[ ++count ];
            sarg |= ( ( uint16_t )text[ ++count ] ) << 8;
            fprintf( out, " yorg = %4.4x (%hu);", sarg, sarg );
            sarg = ( uint16_t )text[ ++count ];
            sarg |= ( ( uint16_t )text[ ++count ] ) << 8;
            fprintf( out, " width = %4.4x (%hu);", sarg, sarg );
            sarg = ( uint16_t )text[ ++count ];
            sarg |= ( ( uint16_t )text[ ++count ] ) << 8;
            fprintf( out, " height = %4.4x (%hu))", sarg, sarg );
            memset( buffer, 0, 256 );
            ++count;
            memcpy( buffer, &text[ count ], carg );
            count += carg - 1;
            fprintf( out, " id %s>]", buffer );
            break;
        default:
            fprintf( out, " unknown subcode>]" );
            break;
        }
        break;
    case 0x10:
        carg = text[ ++count ];
        memset( buffer, 0, 256 );
        ++count;
        memcpy( buffer, &text[ count ], size - 3 );
        count += size - 2;
        fprintf( out, "10: begin link (launch application %s)>]", buffer );
        break;
    case 0x11:
        carg = text[ ++count ];
        fprintf( out, "11: set left margin to %2.2x (%u), start new line>]", carg, carg );
        break;
    case 0x12:
        carg = text[ ++count ];
        fprintf( out, "12: set left margin (fit) to %2.2x (%u), start new line>]", carg, carg );
        break;
    case 0x13:
        carg = text[ ++count ];
        fprintf( out, "13: set foreground color to %2.2x (%s)>]", carg, parseColor( carg ) );
        break;
    case 0x14:
        carg = text[ ++count ];
        fprintf( out, "14: set background color to %2.2x (%s)>]", carg, parseColor( carg ) );
        break;
    case 0x15:
        memset( buffer, 0, 256 );
        ++count;
        memcpy( buffer, &text[ count ], size - 2 );
        count += size - 3;
        fprintf( out, "15: tutorial %s>]", buffer );
        break;
    case 0x16:
        sarg = ( uint16_t )text[ ++count ];
        sarg |= ( ( uint16_t )text[ ++count ] ) << 8;
        fprintf( out, "16: send message to id %4.4x (%hu)>]", sarg, sarg );
        break;
    case 0x17:
        memset( buffer, 0, 256 );
        ++count;
        memcpy( buffer, &text[ count ], size - 2 );
        count += size - 3;
        fprintf( out, "17: hide text, key: %s>]", buffer );
        break;
    case 0x18:
        fprintf( out, "18: end hide text>]" );
        break;
    case 0x19:
        carg = text[ ++count ];
        fprintf( out, "19: change font to %2.2x (%u)>]", carg, carg );
        break;
    case 0x1A:
        carg = text[ ++count ];
        fprintf( out, "1A: begin \"lines\" sequence, flags %2.2x, align ", carg );
        if( carg & 0x01 )
            fprintf( out, "left>]" );
        if( carg & 0x02 )
            fprintf( out, "right>]" );
        if( carg & 0x04 )
            fprintf( out, "center>]" );
        break;
    case 0x1B:
        fprintf( out, "1B: end \"lines\" sequence>]" );
        break;
    case 0x1C:
        fprintf( out, "1C: set left margin to current position>]" );
        break;
    case 0x1D:
        sarg = ( uint16_t )text[ ++count ];
        sarg |= ( ( uint16_t )text[ ++count ] ) << 8;
        fprintf( out, "1D: start link to external document id %4.4x (%hu)>]", sarg, sarg );
        break;
    case 0x1F:
        carg = text[ ++count ];
        fprintf( out, "1F: start link to external file id %2.2x (%u)", carg, carg );
        memset( buffer, 0, 256 );
        carg = text[ ++count ];
        memcpy( buffer, &text[ ++count ], carg );
        count += carg - 1;
        fprintf( out, " id %s>]", buffer );
        break;
    case 0x20:
        sarg = ( uint16_t )text[ ++count ];
        sarg |= ( ( uint16_t )text[ ++count ] ) << 8;
        fprintf( out, "20: begin ddf, resource id %4.4x (%hu)>]", sarg, sarg );
        break;
    case 0x21:
        carg = text[ ++count ];
        fprintf( out, "21: acviewport, reserved %2.2x (%hu)", carg, carg );
        carg = text[ ++count ];
        if( carg ) {
            size_t dataBytes = carg;
            fprintf( out, ", data length %2.2x (%hu)", carg, carg );
            sarg = ( uint16_t )text[ ++count ];
            sarg |= ( ( uint16_t )text[ ++count ] ) << 8;
            fprintf( out, ", objectid %4.4x (%hu)", sarg, sarg );
            carg = text[ ++count ];
            memset( buffer, 0, 256 );
            memcpy( buffer, &text[ ++count ], carg - 1 );
            count += carg - 2;
            fprintf( out, ", objectname \"%s\"", buffer );
            carg = text[ ++count ];
            if( carg > 1 ) {
                memset( buffer, 0, 256 );
                memcpy( buffer, &text[ ++count ], carg - 1 );
                count += carg - 2;
                fprintf( out, ", dll \"%s\"", buffer );
            }
            carg = text[ ++count ];
            if( carg > 1 ) {
                memset( buffer, 0, 256 );
                memcpy( buffer, &text[ ++count ], carg - 1 );
                count += carg - 2;
                fprintf( out, ", objectinfo \"%s\"", buffer );
            }
            if( size - 6 - dataBytes ) {
                uint8_t carg2;
                carg = text[ ++count ];
                carg2 = text[ ++count ];
                fprintf( out, ", flag bytes %2.2x %2.2x", carg, carg2 );
                if( carg & 0x01 ) {
                    PanelOrigin p;
                    memcpy( &p, &text[ count + 1 ], sizeof( PanelOrigin ) );
                    count += sizeof( PanelOrigin );
                    fprintf( out, ", position (" );
                    if( p.xPosType == DYNAMIC)
                        fprintf( out, "%s %s, ", getDPosString( p.xpos ), getPosString( p.xPosType ) );
                    else
                        fprintf( out, "%hu %s,", p.xpos, getPosString(p.xPosType));
                    if( p.yPosType == DYNAMIC)
                        fprintf( out, "%s %s)", getDPosString( p.ypos ), getPosString( p.yPosType ) );
                    else
                        fprintf( out, "%hu %s)", p.ypos, getPosString( p.yPosType ) );
                }
                if( carg & 0x02 ) {
                    PanelSize p;
                    memcpy(&p, &text[ count + 1 ], sizeof( PanelSize ) );
                    count += sizeof( PanelSize );
                    fprintf( out, ", size (" );
                    if( p.widthType == DYNAMIC )
                        fprintf( out, "%s %s, ", getDPosString( p.width ), getPosString( p.widthType ) );
                    else
                        fprintf( out, "%hu %s,", p.width, getPosString( p.widthType ) );
                    if( p.heightType == DYNAMIC )
                        fprintf( out, "%s %s)", getDPosString( p.height ), getPosString( p.heightType ) );
                    else
                        fprintf( out, "%hu %s)", p.height, getPosString( p.heightType ) );
                }
            }
        }
        fprintf( out, ">]" );
        break;
    default:
        fprintf( out, "%2.2x: unknown type of size %2.2x (%u) bytes:", type, size, size );
        while( --size > 1 )
            fprintf( out, " %2.2x", text[ ++count ] );
        fprintf( out, ">]" );
        break;
    }
    return count;
}
/*****************************************************************************/
static void parseAlign( FILE *out, uint8_t carg )
{
    if( carg & 0x01 )
        fprintf( out, ", left align" );
    if( carg & 0x02 )
        fprintf( out, ", right align" );
    if( carg & 0x04 )
        fprintf( out, ", center align" );
    if( carg & 0x08 )
        fprintf( out, ", scale to window" );
    if( carg & 0x10 )
        fprintf( out, ", run-in" );
}
/*****************************************************************************/
static char * parseColor( uint8_t carg )
{
static char *colors[] = {"default", "blue", "red", "pink", "green", "cyan", "yellow", "neutral",
    "dark gray", "dark blue", "dark red", "dark pink", "dark green", "dark cyan", "black", "pale gray" };
return( colors[ carg ] );
}
