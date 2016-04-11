// Dump bitmap information

#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include "ipfcdump.h"

#define FILEBITMAP
#ifdef FILEBITMAP

#define CLEAR_TABLE 256                 /* flush the string table */
#define TERMINATOR  257                 /* marks EOF */
#define FIRST_CODE  258                 /* First available code for code_value table */
#define INIT_BITS   9
#define MAX_BITS    12
#define TABLE_SIZE  4096
#define MAXVAL(n)   ((1 << ( n )) - 1)  /* max_value formula macro */

typedef struct {
    uint16_t    offset;
    uint16_t    length;
} codepos_t;

static unsigned expand( uint8_t *src, uint8_t *dst, size_t length )
/*****************************************************************/
{
    uint8_t     *dstStart;      /* Saved start of output buffer */
    uint8_t     *srcStart;      /* Saved start of compressed data */
    uint8_t     *tmpChar;
    codepos_t   *codePos;       /* Position of previous strings in output */
    uint16_t    numBits;
    uint16_t    maxCode;
    uint16_t    nextCode;
    uint16_t    newCode;
    uint16_t    oldCode = 0;
    uint16_t    code;
    uint8_t     character = 0;
    uint8_t     clearFlg = 1;

    uint16_t    bitCount  = 0;
    uint32_t    bitBuffer = 0;

    srcStart    = src;
    dstStart    = dst;
    nextCode    = FIRST_CODE;
    numBits     = INIT_BITS;
    maxCode     = MAXVAL( numBits );

    codePos     = calloc( 1, TABLE_SIZE * sizeof( codepos_t ) );
    if( codePos == NULL ) {
        printf( "Error: failed to allocate decompressor table.\n" );
        return( 0 );
    }

    for( ;; ) {
        /* Check for source buffer overrun */
        if( src - srcStart > length ) {
            printf( "Error: src - start = %d, length = %d\n", src - srcStart, length );
            break;
        }
        while( bitCount <= 24 ) {
            bitBuffer |= (uint32_t)*src++ << ( 24 - bitCount );
            bitCount  += 8;
        }

        newCode = (uint16_t)( bitBuffer >> ( 32 - numBits ) );
        if( newCode == TERMINATOR )
            break;

        /* Check for decompressor table overflow */
        if( newCode >= TABLE_SIZE ) {
            printf( "Error: newCode = %d\n", newCode );
            break;
        } else {
//            printf( "newCode = %d\n", newCode );
        }

        bitBuffer <<= numBits;
        bitCount   -= numBits;

        if( clearFlg ) {    /* Clear the decompressor's tables */
            clearFlg = 0;
            codePos[ newCode ].offset = (uint16_t)( dst - dstStart );
            codePos[ newCode ].length = 1;
            oldCode = newCode;
            *dst++ = character = (uint8_t)newCode;
        } else if( newCode == CLEAR_TABLE ) {
            clearFlg = 1;
            nextCode = FIRST_CODE;
            numBits  = INIT_BITS;
            maxCode  = MAXVAL( numBits );
        } else {
            if( newCode < CLEAR_TABLE ) {
                codePos[ newCode ].offset = (uint16_t)( ( tmpChar = dst ) - dstStart );
                codePos[ newCode ].length = 1;
                *dst++ = character = (uint8_t)newCode;
            } else {
                code = newCode;
                if( code >= nextCode )
                    code = oldCode;     /* Special handling for string/char/string */

                /* Copy previously decompressed string to output buffer */
                tmpChar = memcpy( dst, dstStart + codePos[ code ].offset, codePos[ code ].length );

                dst += codePos[ code ].length;
                if( newCode >= nextCode )
                    *dst++ = character;

                character = *tmpChar;
            }

            /* Check for decompressor table overflow */
            if( nextCode >= TABLE_SIZE ) {
                printf( "Error: nextCode = %d\n", nextCode );
                break;
            }

            codePos[ nextCode ].offset = (uint16_t)( tmpChar - dstStart ) - codePos[ oldCode ].length;
            codePos[ nextCode ].length = codePos[ oldCode ].length + 1;
            ++nextCode;

            if( nextCode == maxCode && numBits < 12 )
                maxCode = MAXVAL( ++numBits );

            oldCode = newCode;
        }
    }

    free( codePos );
    return( dst - dstStart );
}

static void blockToHex( uint8_t *data, size_t size, FILE* out )
/*************************************************************/
{
    size_t bytes = 0;
    while( bytes < size ) {
        unsigned int count1;
        for( count1 = 0; count1 < 16; count1++ ) {
            if (bytes >= size)
                break;
            fprintf( out, "%2.2X ", data[ bytes ] );
            ++bytes;
        }
        fputc('\n', out);
    }
}

#endif

void readBitMaps( FILE *in, FILE *out )
{
    fputs( "\nBitmap data\n", out );
    if( Hdr.imageOffset ) {
        BitMap bm;
        BitMapBlock bmb;
        RGB rgb[ 256] ;
        uint32_t size;
        uint32_t nextMap = Hdr.imageOffset;
        unsigned int count1 = 0;
        uint16_t blockSize;
#ifdef FILEBITMAP
        uint8_t *compressed = ( uint8_t * )malloc( 65504 * sizeof( uint8_t ) );
        uint8_t *expanded = ( uint8_t * )malloc( 65504 * sizeof( uint8_t ) );
        FILE *btmp;
        FILE *lzw;
        int32_t bytesLeft;
        char name[ 13 ];
#endif
        fseek( in, Hdr.imageOffset, SEEK_SET );
        fread( &bm, sizeof( BitMap ), 1, in );
        while( ( bm.hdr.type[0] == 'b' && bm.hdr.type[1] == 'M' ) ||
               ( bm.hdr.type[0] == 'm' && bm.hdr.type[1] == 'F' ) ) {
            uint32_t totalSize = 0;
            unsigned int count2 = 0;
            fprintf( out, "  Bitmap #%u at offset %8.8x\n", count1, nextMap );
            fprintf( out, "    Type:                 %c%c\n", bm.hdr.type[0], bm.hdr.type[1] );
            fprintf( out, "    Size:           %8.8x (%lu)\n", bm.hdr.size, bm.hdr.size );
            fprintf( out, "    xHotspot:       %8.4x (%hu)\n", bm.hdr.xHotspot, bm.hdr.xHotspot );
            fprintf( out, "    yHotspot:       %8.4x (%hu)\n", bm.hdr.yHotspot, bm.hdr.yHotspot );
            fprintf( out, "    Bits offset:    %8.8x (%lu)\n", bm.hdr.bitsOffset, bm.hdr.bitsOffset );
            fprintf( out, "    Size:           %8.8x (%lu)\n", bm.hdr.info.size, bm.hdr.info.size );
            fprintf( out, "    Width:          %8.4x (%hu)\n", bm.hdr.info.width, bm.hdr.info.width );
            fprintf( out, "    Height:         %8.4x (%hu)\n", bm.hdr.info.height, bm.hdr.info.height );
            fprintf( out, "    Planes:         %8.4x (%hu)\n", bm.hdr.info.planes, bm.hdr.info.planes );
            fprintf( out, "    Bits per Pixel: %8.4x (%hu)\n", bm.hdr.info.bitsPerPixel, bm.hdr.info.bitsPerPixel );
#ifdef FILEBITMAP
            snprintf( name, sizeof(name) / sizeof(char), "btmp%4.4u.bmp", count1 );
            btmp = fopen( name, "wb" );
            snprintf( name, sizeof(name) / sizeof(char), "btmp%4.4u.lzw", count1 );
            lzw = fopen( name, "w" );
            if( bm.hdr.size && bm.hdr.size > bm.hdr.bitsOffset )
                bytesLeft = bm.hdr.size - bm.hdr.bitsOffset /* sizeof(BitMap) */;
            else {
                bm.hdr.size = sizeof( BitMap );
                if (bm.hdr.info.bitsPerPixel == 1 ) {
                    bytesLeft = bm.hdr.info.width / 8;
                    bytesLeft = bytesLeft & 3 ? ( bytesLeft & ~3 ) + 4 : bytesLeft;
                    bytesLeft *= bm.hdr.info.height;
                    bm.hdr.size += bytesLeft + 2 * sizeof( RGB );
                }
                else if( bm.hdr.info.bitsPerPixel == 4 ) {
                    bytesLeft = bm.hdr.info.width / 2;
                    bytesLeft = bytesLeft & 3 ? ( bytesLeft & ~3 ) + 4 : bytesLeft;
                    bytesLeft *= bm.hdr.info.height;
                    bm.hdr.size += bytesLeft + 16 * sizeof( RGB );
                }
                else if( bm.hdr.info.bitsPerPixel == 8 ) {
                    bytesLeft = bm.hdr.info.width & 3 ? ( bm.hdr.info.width & ~3 ) + 4 : bm.hdr.info.width;
                    bytesLeft *= bm.hdr.info.height;
                    bm.hdr.size += bytesLeft + 256 * sizeof( RGB );
                }
                else if( bm.hdr.info.bitsPerPixel == 15 || bm.hdr.info.bitsPerPixel == 16 ) {
                    bytesLeft = bm.hdr.info.width * 2;
                    bytesLeft = bytesLeft & 3 ? ( bytesLeft & ~3 ) + 4 : bytesLeft;
                    bytesLeft *= bm.hdr.info.height;
                    bm.hdr.size += bytesLeft;
                }
                else if( bm.hdr.info.bitsPerPixel == 24 ) {
                    bytesLeft = bm.hdr.info.width * 3;
                    bytesLeft = bytesLeft & 3 ? ( bytesLeft & ~3 ) + 4 : bytesLeft;
                    bytesLeft *= bm.hdr.info.height;
                    bm.hdr.size += bytesLeft;
                }
                else if( bm.hdr.info.bitsPerPixel == 32 ) {
                    bytesLeft = bm.hdr.info.width * 4 * bm.hdr.info.height;
                    bm.hdr.size += bytesLeft;
                }
            }
            bm.hdr.type[ 0 ] = toupper( bm.hdr.type[ 0 ] );
            bm.hdr.bitsOffset = sizeof( BitMap );
            if( bm.hdr.info.bitsPerPixel == 1 )
                bm.hdr.bitsOffset += 2 * sizeof( RGB );
            else if( bm.hdr.info.bitsPerPixel == 4 )
                bm.hdr.bitsOffset += 16 * sizeof( RGB );
            else if( bm.hdr.info.bitsPerPixel == 8 )
                bm.hdr.bitsOffset += 256 * sizeof( RGB );
            bm.hdr.size = bytesLeft + bm.hdr.bitsOffset;
            fwrite( &bm, sizeof( BitMap ), 1, btmp );
            fprintf( lzw, "Bitmap #%u at offset %8.8x\n", count1, nextMap );
            fprintf( lzw, "  Type:                 %c%c\n", bm.hdr.type[0], bm.hdr.type[1] );
            fprintf( lzw, "  Size:           %8.8x (%lu)\n", bm.hdr.size, bm.hdr.size );
            fprintf( lzw, "  xHotspot:       %8.4x (%hu)\n", bm.hdr.xHotspot, bm.hdr.xHotspot );
            fprintf( lzw, "  yHotspot:       %8.4x (%hu)\n", bm.hdr.yHotspot, bm.hdr.yHotspot );
            fprintf( lzw, "  Bits offset:    %8.8x (%lu)\n", bm.hdr.bitsOffset, bm.hdr.bitsOffset );
            fprintf( lzw, "  Size:           %8.8x (%lu)\n", bm.hdr.info.size, bm.hdr.info.size );
            fprintf( lzw, "  Width:          %8.4x (%hu)\n", bm.hdr.info.width, bm.hdr.info.width );
            fprintf( lzw, "  Height:         %8.4x (%hu)\n", bm.hdr.info.height, bm.hdr.info.height );
            fprintf( lzw, "  Planes:         %8.4x (%hu)\n", bm.hdr.info.planes, bm.hdr.info.planes );
            fprintf( lzw, "  Bits per Pixel: %8.4x (%hu)\n", bm.hdr.info.bitsPerPixel, bm.hdr.info.bitsPerPixel );
#endif
            memset( &rgb, 0, 256 * sizeof( RGB ) );
            if( bm.hdr.info.bitsPerPixel == 1 ) {
#ifdef COLOR_PAL
                unsigned int count;
                fread( rgb, sizeof( RGB ), 2, in );
                fputs( "    Color Palette\n", out );
                for( count = 0; count < 2; count++ )
                    fprintf( out, "      %2d [%2.2x %2.2x %2.2x]\n", count, rgb[count].blue, rgb[count].green, rgb[count].red );
#else
                fread( rgb, sizeof( RGB ), 2, in );
                fputs( "    Skipping color 2 palette entries...\n", out );
#endif
#ifdef FILEBITMAP
                fwrite( rgb, sizeof(RGB), 2, btmp );
                fputs( "Color Palette\n", lzw );
                blockToHex( ( unsigned char * )&rgb[ 0 ], 2 * sizeof( RGB ), lzw );
#endif
            }
            else if( bm.hdr.info.bitsPerPixel == 4 ) {
#ifdef COLOR_PAL
                unsigned int count;
                fread( rgb, sizeof( RGB ), 16, in );
                fputs( "    Color Palette\n", out );
                for( count = 0; count < 16; count++ )
                    fprintf( out, "      %2d [%2.2x %2.2x %2.2x]\n", count, rgb[count].blue, rgb[count].green, rgb[count].red );
#else
                fread( rgb, sizeof( RGB ), 16, in );
                fputs( "    Skipping color 16 palette entries...\n", out );
#endif
#ifdef FILEBITMAP
                fwrite( rgb, sizeof(RGB), 16, btmp );
                fputs( "Color Palette\n", lzw );
                blockToHex( ( unsigned char * )&rgb[ 0 ], 16 * sizeof( RGB ), lzw );
#endif
            }
            else if( bm.hdr.info.bitsPerPixel == 8 ) {
#ifdef COLOR_PAL
                unsigned int count;
                fread( rgb, sizeof( RGB ), 256, in );
                fputs( "    Color Palette\n", out );
                for  (count = 0; count < 256; count++ )
                    fprintf(out, "      %3d [%2.2x %2.2x %2.2x]\n", count, rgb[count].blue, rgb[count].green, rgb[count].red );
#else
                fread( rgb, sizeof( RGB ), 256, in );
                fputs( "    Skipping color 256 palette entries...\n", out );
#endif
#ifdef FILEBITMAP
                fwrite(rgb, sizeof(RGB), 256, btmp);
                fputs("Color Palette\n", lzw);
                blockToHex((unsigned char*)&rgb[0], 256 * sizeof(RGB), lzw);
#endif
            }
            else
                fputs("    No color palette for this bit depth\n", out);
            fread( &size, sizeof( uint32_t ), 1, in );
            nextMap = ftell( in ) + size;
            fread( &blockSize, sizeof( uint16_t ), 1, in );
            fprintf( out, "    Data size:      %8.8x (%lu)\n", size, size );
            fprintf( out, "    Block size:     %8.4x (%hu)\n", blockSize, blockSize );
            fread( &bmb, sizeof( BitMapBlock ), 1, in );
#ifdef FILEBITMAP
            fprintf( lzw, "  Data size:      %8.8x (%lu)\n", size, size );
            fprintf( lzw, "  Block size:     %8.4x (%hu)\n", blockSize, blockSize );
#endif
            totalSize += bmb.size;
            while( totalSize < size ) {
                fprintf(out, "    Bitmap #%u Block #%u\n", count1, count2);
                fprintf(out, "      Size:             %4.4x (%hu)\n", bmb.size, bmb.size);
                fprintf(out, "      Compression type: %4.2x (%u)\n", bmb.type, bmb.type);
#ifdef FILEBITMAP
                fprintf(lzw, "  Block #%u\n", count2);
                fprintf(lzw, "    Size:             %4.4x (%hu)\n", bmb.size, bmb.size);
                fprintf(lzw, "    Compression type: %4.2x (%u)\n", bmb.type, bmb.type);
                /* NB: The compressor does not emit trailing zeros, if any, but they are
                 * needed by the decompressor. The source buffer must be zeroed out.
                 */
                memset( compressed, 0, 65504 * sizeof( uint8_t ) );
                fread( compressed, sizeof( uint8_t ), bmb.size - 1, in );
                blockToHex( compressed, sizeof( uint8_t ) * ( bmb.size - 1 ), lzw );
                if( bmb.type ) {
                    uint8_t filler = 0;
                    int32_t len;
                    size_t bytes = expand( compressed, expanded, bmb.size );
                    if( bytes < blockSize ) {
                        memset(expanded + bytes, filler, blockSize - bytes);
                        //fprintf(out, "Adding %u bytes of filler %2.2x\n", blockSize - bytes, filler);
                    }
                    len = bytesLeft;
                    if( len > blockSize )
                        len = blockSize;
                    fwrite( expanded, sizeof( uint8_t ), len, btmp );
                    bytesLeft -= len;
                }
                else
                    fwrite( compressed, sizeof( uint8_t ), bmb.size - 1, btmp );
#else
                fseek( in, bmb.size - 1, SEEK_CUR );
#endif
                fread( &bmb, sizeof( BitMapBlock ), 1, in );
                totalSize += bmb.size;
                ++count2;
            }
#ifdef FILEBITMAP
            fclose(btmp);
            fclose(lzw);
#endif
            ++count1;
            fseek(in, nextMap, SEEK_SET);
            fread(&bm, sizeof(BitMap), 1, in);
        }
#ifdef FILEBITMAP
        free(compressed);
        free(expanded);
#endif
    }
    else
        fputs("  There are no bitmaps\n", out);
}
