// Dump Full Text Search data

#include "ipfcdump.h"
#define DECODE_RLE

static void processFTS8( FILE *, FILE * );
static void processFTS16( FILE *, FILE * );
const char * bstring( uint8_t );

void readFTS( FILE *in, FILE *out )
{
    fputs( "\nFull Text Serach\n", out );
    if( Hdr.searchSize ) {
        if( Hdr.recSize )
            processFTS16( in, out );
        else
            processFTS8( in, out );
    }
    else
        fputs( "  No full text serach data found\n", out );
}
/*****************************************************************************/
static void processFTS8( FILE *in, FILE *out )
{
    FTS8Data    d;
    uint32_t    nextPos = Hdr.searchOffset;
    size_t      count1 = 0;
    size_t      count2 = 0;
    size_t      items = 1;
    uint16_t    word;
    while( nextPos < Hdr.searchOffset + Hdr.searchSize ) {
        fseek( in, nextPos, SEEK_SET );
        fread( &d, sizeof( FTS8Data ), 1, in );
        nextPos += d.size;
        fprintf( out, "  Record #%u ([%ls])\n",count1, Vocabulary[ count1 ] );
        ++count1;
        fprintf( out, "    FTS8Data.size:        %2.2x (%u)\n", d.size, d.size );
        fprintf( out, "    FTS8Data.compression: %2.2x (%u)\n", d.compression, d.compression );
        switch( d.compression ) {
        case NONE:
            fputs( "    Is present on no page\n", out );
            break;
        case ALL:
            fputs( "    Is present on all pages\n", out );
            break;
        case RLE:
#ifdef DECODE_RLE
            fprintf( out, "    Is RLE-encoded using method %d, and present on the following pages:\n", fgetc( in ) );
            count2 = 0;
            items = 1;
            while( count2 < d.size - sizeof( uint8_t ) - sizeof( FTS8Data ) ) {
                uint8_t byte = fgetc( in );
                ++count2;
                if( byte == 0x80 ) {
                    uint16_t bytes;
                    uint16_t count3;
                    byte = fgetc( in );
                    fread(&bytes, sizeof( uint16_t ), 1, in );
                    count2 += sizeof( uint8_t ) + sizeof( uint16_t );
                    for( count3 = 0; count3 < bytes; count3++, items++ ) {
                        fprintf( out, "%s ", bstring( byte ) );
                        if( !( items & 7 ) )
                            fputc( '\n', out );
                    }
                }
                else if( byte & 0x80 ) {
                    uint8_t count3;
                    uint8_t bytes = ( byte & ~0x80 ) + 1;
                    for( count3 = 0; count3 < bytes; count3++, items++ ) {
                        fprintf( out, "%s ", bstring( fgetc ( in ) ) );
                        if( !( items & 7 ) )
                            fputc( '\n', out );
                    }
                    count2 += bytes * sizeof( uint8_t );
                }
                else {
                    uint8_t count3;
                    uint8_t bytes = byte + 1;
                    byte = fgetc( in );
                    count2 += sizeof( uint8_t );
                    for( count3 = 0; count3 < bytes; count3++, items++ ) {
                        fprintf( out, "%s ", bstring( byte ) );
                        if( !( items & 7 ) )
                            fputc( '\n', out );
                    }
                }
            }
            if( ( items - 1 ) & 7 )
                fputc( '\n', out );
#else
            fputs( "    Is RLL-encoded. Data are:\n", out );
            for( count2 = 0, items = 1; count2 < d.size - sizeof( FTS8Data ); count2 += sizeof( uint8_t ), items++ ) {
                fprintf( out, "%2.2x ", fgetc( in ) );
                if( !(items & 7) )
                    fputc( '\n', out );
            }
            if( ( items - 1 ) & 7 )
                fputc( '\n', out );
#endif
            break;
        case PRESENT:
            fputs( "    Is present on the following pages:\n", out );
            for( count2 = 0, items = 1; count2 < d.size - sizeof( FTS8Data ); count2 += sizeof( uint16_t ), items++ ) {
                fread( &word, sizeof( uint16_t ), 1, in );
                fprintf( out, "%4.4x (%hu) ", word, word );
                if( !( items & 7 ) )
                    fputc( '\n', out );
            }
            if( ( items - 1 ) & 7 )
                fputc( '\n', out );
            break;
        case NOT_PRESENT:
            fputs("    Is not present on the following pages:\n", out);
            for( count2 = 0, items = 1; count2 < d.size - sizeof( FTS8Data); count2 += sizeof( uint16_t ), items++ ) {
                fread( &word, sizeof( uint16_t ), 1, in );
                fprintf( out, "%4.4x (%hu) ", word, word );
                if( !(items & 7 ) )
                    fputc( '\n', out );
            }
            if( ( items - 1) & 7 )
                fputc( '\n', out );
            break;
        case TRUNC:
            fputs( "    Is present on the following pages:\n", out );
            fputs( "    Bitstring:\n", out );
            for( count2 = 0, items = 1; count2 < d.size - sizeof( FTS8Data ); count2 += sizeof( uint8_t ), items++ ) {
                fprintf( out, "%s ", bstring( fgetc( in ) ) );
                if( !( items & 7 ) )
                    fputc('\n', out );
            }
            if( ( items - 1 ) & 7 )
                fputc('\n', out);
            break;
        case DBL_TRUNC:
            fread( &word, sizeof( uint16_t ), 1, in );
            fprintf( out, "    Is present on the following pages (offset is %hu pages):\n", word * 8 );
            fputs("    Bitstring:\n", out );
            for( count2 = 0, items = 1; count2 < d.size - sizeof( FTS8Data ) - sizeof( uint16_t ); count2 += sizeof( uint8_t ), items++ ) {
                fprintf( out, "%s ", bstring( fgetc( in ) ) );
                if( !( items & 7 ) )
                    fputc( '\n', out );
            }
            if( ( items - 1 ) & 7 )
                fputc( '\n', out );
            break;
        default:
            fputs( "    Unknown compression code\n", out );
        }
    }
}
/*****************************************************************************/
static void processFTS16( FILE *in, FILE *out )
{
    FTS16Data   d;
    uint32_t    nextPos = Hdr.searchOffset;
    size_t      count1 = 0;
    size_t      count2 = 0;
    size_t      items = 1;
    uint16_t    word;
    while( nextPos < Hdr.searchOffset + Hdr.searchSize ) {
        fseek( in, nextPos, SEEK_SET );
        fread( &d, sizeof( FTS16Data ), 1, in );
        nextPos += d.size;
        fprintf( out, "  Record #%u ([%ls])\n",count1, Vocabulary[ count1 ] );
        ++count1;
        fprintf( out, "    FTS16Data.size:        %4.4x (%u)\n", d.size, d.size );
        fprintf( out, "    FTS16Data.compression: %4.4x (%u)\n", d.compression, d.compression );
        switch( d.compression ) {
        case NONE:
            fputs( "    Is present on no page\n", out );
            break;
        case ALL:
            fputs( "    Is present on all pages\n", out );
            break;
        case RLE:
#ifdef DECODE_RLE
            fprintf( out, "    Is RLE-encoded using method %d, and present on the following pages:\n", fgetc( in ) );
            count2 = 0;
            items = 1;
            while( count2 < d.size - sizeof( uint8_t ) - sizeof( FTS16Data ) ) {
                uint8_t byte = fgetc( in );
                ++count2;
                if( byte == 0x80 ) {
                    uint16_t bytes;
                    uint16_t count3;
                    byte = fgetc( in );
                    fread( &bytes, sizeof( uint16_t ), 1, in );
                    count2 += sizeof( uint8_t ) + sizeof( uint16_t );
                    for( count3 = 0; count3 < bytes; count3++, items++ ) {
                        fprintf( out, "%s ", bstring( byte ) );
                        if( !( items & 7 ) )
                            fputc( '\n', out );
                    }
                }
                else if( byte & 0x80 ) {
                    uint8_t count3;
                    uint8_t bytes = ( byte & ~0x80 ) + 1;
                    for( count3 = 0; count3 < bytes; count3++, items++ ) {
                        fprintf( out, "%s ", bstring( fgetc( in ) ) );
                        if( !( items & 7 ) )
                            fputc( '\n', out );
                    }
                    count2 += bytes * sizeof( uint8_t );
                }
                else {
                    uint8_t count3;
                    uint8_t bytes = byte + 1;
                    byte = fgetc( in );
                    count2 += sizeof( uint8_t );
                    for( count3 = 0; count3 < bytes; count3++, items++ ) {
                        fprintf( out, "%s ", bstring( byte ) );
                        if( !( items & 7 ) )
                            fputc( '\n', out );
                    }
                }
            }
            if( ( items - 1 ) & 7 )
                fputc( '\n', out );
#else
            fputs( "    Is RLL-encoded. Data are:\n", out );
            for( count2 = 0, items = 1; count2 < d.size - sizeof( FTS16Data ); count2 += sizeof( uint8_t ), items++ ) {
                fprintf( out, "%2.2x ", fgetc( in ) );
                if( !( items & 7) )
                    fputc( '\n', out );
            }
            if( ( items - 1 ) & 7 )
                fputc( '\n', out );
#endif
            break;
        case PRESENT:
            fputs( "    Is present on the following pages:\n", out );
            for( count2 = 0, items = 1; count2 < d.size - sizeof( FTS16Data ); count2 += sizeof( uint16_t ), items++ ) {
                fread( &word, sizeof( uint16_t), 1, in );
                fprintf( out, "%4.4x (%hu) ", word, word );
                if( !( items & 7 ) )
                    fputc( '\n', out );
            }
            if( ( items - 1 ) & 7 )
                fputc( '\n', out );
            break;
        case NOT_PRESENT:
            fputs( "    Is not present on the following pages:\n", out );
            for( count2 = 0, items = 1; count2 < d.size - sizeof( FTS16Data ); count2 += sizeof( uint16_t ), items++ ) {
                fread( &word, sizeof( uint16_t), 1, in );
                fprintf( out, "%4.4x (%hu) ", word, word );
                if( !( items & 7 ) )
                    fputc( '\n', out );
            }
            if( ( items - 1 ) & 7 )
                fputc( '\n', out );
            break;
        case TRUNC:
            fputs( "    Is present on the following pages:\n", out );
            fputs( "    Bitstring:\n", out );
            for( count2 = 0, items = 1; count2 < d.size - sizeof( FTS16Data ); count2 += sizeof( uint8_t ), items++ ) {
                fprintf( out, "%s ", bstring(fgetc( in ) ) );
                if( !( items & 7 ) )
                    fputc( '\n', out );
            }
            if( ( items - 1 ) & 7 )
                fputc( '\n', out );
            break;
        case DBL_TRUNC:
            fread( &word, sizeof( uint16_t), 1, in);
            fprintf( out, "    Is present on the following pages (offset is %hu pages):\n", word * 8);
            fputs( "    Bitstring:\n", out );
            for( count2 = 0, items = 1; count2 < d.size - sizeof( FTS16Data ) - sizeof( uint16_t ); count2 += sizeof( uint8_t ), items++ ) {
                fprintf( out, "%s ", bstring( fgetc( in ) ) );
                if( !( items & 7 ) )
                    fputc( '\n', out );
            }
            if( ( items - 1 ) & 7 )
                fputc( '\n', out );
            break;
        default:
            fputs( "    Unknown compression code\n", out );
        }
    }
}
/*****************************************************************************/
const char * bstring( uint8_t byte )
{
    static char text[ 9 ] = {'0', '0', '0', '0', '0', '0', '0', '0', '\0'};
    uint8_t mask = 0x80;
    size_t count;
    for( count = 0; count < 8; count++ ) {
        if( byte & mask )
            text[ count ] = '1';
        else
            text[ count ] = '0';
        mask >>= 1;
    }
    return( text );
}
