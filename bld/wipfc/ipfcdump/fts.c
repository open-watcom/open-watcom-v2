// Dump Full Text Search data

#define DECODE_RLE

#include "ipfcdump.h"

#define getU8(i)    fgetc( i )

typedef uint16_t    word;
typedef uint8_t     byte;

static word getU16( FILE *in )
/****************************/
{
    word    data;

    fread( &data, sizeof( data ), 1, in );
    return( data );
}

static void processFTS( FILE *in, FILE *out )
/*******************************************/
{
    int         big;
    long        nextPos;
    unsigned    fts_size;
    unsigned    count1 = 0;
    unsigned    count2 = 0;
    unsigned    items = 1;
    unsigned    data_size;
    unsigned    fts_compr;
    unsigned    dataw;

    big = isBigFTS( &Hdr );
    for( nextPos = dataOffsetFTS( &Hdr ); nextPos < dataOffsetFTS( &Hdr ) + Hdr.searchSize; nextPos += fts_size ) {
        fseek( in, nextPos, SEEK_SET );
        fprintf( out, "  Record #%u ([%ls])\n", count1, Vocabulary[count1] );
        ++count1;
        // read FTS header fixed part
        fts_size = ( big ) ? getU16( in ) : getU8( in );
        fts_compr = getU8( in );
        if( big ) {
            fprintf( out, "    FTS16Data.size:        %4.4x (%u)\n", fts_size, fts_size );
            fprintf( out, "    FTS16Data.compression: %4.4x (%u)\n", fts_compr, fts_compr );
        } else {
            fprintf( out, "    FTS8Data.size:        %2.2x (%u)\n", fts_size, fts_size );
            fprintf( out, "    FTS8Data.compression: %2.2x (%u)\n", fts_compr, fts_compr );
        }
        // rest of data, independent on FTS size
        data_size = fts_size - (( big ) ? sizeof( word ) : sizeof( byte )) - sizeof( byte );
        switch( fts_compr ) {
        case NONE:
            fputs( "    Is present on no page\n", out );
            break;
        case ALL:
            fputs( "    Is present on all pages\n", out );
            break;
        case RLE:
#ifdef DECODE_RLE
            fprintf( out, "    Is RLE-encoded using method %d, and present on the following pages:\n", getU8( in ) );
            count2 = 0;
            items = 1;
            while( count2 < data_size - sizeof( byte ) ) {
                unsigned bytes;
                unsigned count3;
                byte rle_attr = getU8( in );
                ++count2;
                if( rle_attr == 0x80 ) {
                    const char *bits = bstring( getU8( in ) );
                    bytes = getU16( in );
                    count2 += sizeof( byte ) + sizeof( word );
                    for( count3 = 0; count3 < bytes; count3++, items++ ) {
                        fprintf( out, "%s ", bits );
                        if( !(items & 7) ) {
                            fputc( '\n', out );
                        }
                    }
                } else if( rle_attr & 0x80 ) {
                    bytes = ( rle_attr & ~0x80 ) + 1;
                    for( count3 = 0; count3 < bytes; count3++, items++ ) {
                        fprintf( out, "%s ", bstring( getU8( in ) ) );
                        if( !(items & 7) ) {
                            fputc( '\n', out );
                        }
                    }
                    count2 += bytes * sizeof( byte );
                } else {    // rle_attr < 0x80
                    const char *bits = bstring( getU8( in ) );
                    count2 += sizeof( byte );
                    bytes = rle_attr + 1;
                    for( count3 = 0; count3 < bytes; count3++, items++ ) {
                        fprintf( out, "%s ", bits );
                        if( !(items & 7) ) {
                            fputc( '\n', out );
                        }
                    }
                }
            }
            if( ( items - 1 ) & 7 )
                fputc( '\n', out );
#else
            fputs( "    Is RLL-encoded. Data are:\n", out );
            for( count2 = 0, items = 1; count2 < data_size; count2 += sizeof( byte ), items++ ) {
                fprintf( out, "%2.2x ", getU8( in ) );
                if( !(items & 7) ) {
                    fputc( '\n', out );
                }
            }
            if( ( items - 1 ) & 7 )
                fputc( '\n', out );
#endif
            break;
        case PRESENT:
            fputs( "    Is present on the following pages:\n", out );
            for( count2 = 0, items = 1; count2 < data_size; count2 += sizeof( word ), items++ ) {
                dataw = getU16( in );
                fprintf( out, "%4.4x (%u) ", dataw, dataw );
                if( !(items & 7) ) {
                    fputc( '\n', out );
                }
            }
            if( ( items - 1 ) & 7 )
                fputc( '\n', out );
            break;
        case NOT_PRESENT:
            fputs("    Is not present on the following pages:\n", out);
            for( count2 = 0, items = 1; count2 < data_size; count2 += sizeof( word ), items++ ) {
                dataw = getU16( in );
                fprintf( out, "%4.4x (%u) ", dataw, dataw );
                if( !(items & 7 ) ) {
                    fputc( '\n', out );
                }
            }
            if( ( items - 1) & 7 )
                fputc( '\n', out );
            break;
        case TRUNC:
            fputs( "    Is present on the following pages:\n", out );
            fputs( "    Bitstring:\n", out );
            for( count2 = 0, items = 1; count2 < data_size; count2 += sizeof( byte ), items++ ) {
                fprintf( out, "%s ", bstring( getU8( in ) ) );
                if( !(items & 7) ) {
                    fputc('\n', out );
                }
            }
            if( ( items - 1 ) & 7 )
                fputc('\n', out);
            break;
        case DBL_TRUNC:
            dataw = getU16( in );
            fprintf( out, "    Is present on the following pages (offset is %u pages):\n", dataw * 8 );
            fputs( "    Bitstring:\n", out );
            for( count2 = 0, items = 1; count2 < data_size - sizeof( word ); count2 += sizeof( byte ), items++ ) {
                fprintf( out, "%s ", bstring( getU8( in ) ) );
                if( !(items & 7) ) {
                    fputc( '\n', out );
                }
            }
            if( ( items - 1 ) & 7 )
                fputc( '\n', out );
            break;
        default:
            fputs( "    Unknown compression code\n", out );
        }
    }
}

const char * bstring( uint8_t datab )
/***********************************/
{
    static char text[9] = {'0', '0', '0', '0', '0', '0', '0', '0', '\0'};
    uint8_t mask = 0x80;
    int count;
    for( count = 0; count < 8; count++ ) {
        if( datab & mask ) {
            text[count] = '1';
        } else {
            text[count] = '0';
        }
        mask >>= 1;
    }
    return( text );
}

void readFTS( FILE *in, FILE *out )
/*********************************/
{
    fputs( "\nFull Text Search\n", out );
    if( Hdr.searchSize ) {
        processFTS( in, out );
    } else {
        fputs( "  No full text search data found\n", out );
    }
}
