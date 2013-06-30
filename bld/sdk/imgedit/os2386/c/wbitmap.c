/****************************************************************************
*
*                            Open Watcom Project
*
*    Portions Copyright (c) 1983-2002 Sybase, Inc. All Rights Reserved.
*
*  ========================================================================
*
*    This file contains Original Code and/or Modifications of Original
*    Code as defined in and that are subject to the Sybase Open Watcom
*    Public License version 1.0 (the 'License'). You may not use this file
*    except in compliance with the License. BY USING THIS FILE YOU AGREE TO
*    ALL TERMS AND CONDITIONS OF THE LICENSE. A copy of the License is
*    provided with the Original Code and Modifications, and is also
*    available at www.sybase.com/developer/opensource.
*
*    The Original Code and all software distributed under the License are
*    distributed on an 'AS IS' basis, WITHOUT WARRANTY OF ANY KIND, EITHER
*    EXPRESS OR IMPLIED, AND SYBASE AND ALL CONTRIBUTORS HEREBY DISCLAIM
*    ALL SUCH WARRANTIES, INCLUDING WITHOUT LIMITATION, ANY WARRANTIES OF
*    MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, QUIET ENJOYMENT OR
*    NON-INFRINGEMENT. Please see the License for the specific language
*    governing rights and limitations under the License.
*
*  ========================================================================
*
* Description:  WHEN YOU FIGURE OUT WHAT THIS FILE DOES, PLEASE
*               DESCRIBE IT HERE!
*
****************************************************************************/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#include <dos.h>

#define INCL_GPI
#define INCL_WIN
#define INCL_GPILOGCOLORTABLE
#include <os2.h>

#include "wbitmap.h"
#include "palette.h"
#include "mem.h"

#define CHUNK_SIZE      (48 * 1024)

/*
 * readInPieces - reads in the bitmap bits in chunks
 */
static void readInPieces( BYTE *dst, FILE *fp, ULONG size )
{
    BYTE                *buffer;
    LONG                chunk_size;

    chunk_size = CHUNK_SIZE;
    while( ( buffer = MemAlloc( chunk_size ) ) == NULL ) {
        chunk_size >>= 1;
    }
    while( size > chunk_size ) {
        fread( buffer, chunk_size, 1, fp );
        memcpy( dst, buffer, chunk_size );
        dst += chunk_size;
        size -= chunk_size;
    }
    fread( buffer, size, 1, fp );
    memcpy( dst, buffer, size );
    MemFree( buffer );

} /* readInPieces */
/*
 * convertToBmp2 - converts an old form BITMAPINFO structure to a
 *                   BITMAPINFO2 structure (all extra fields are 0).
 */
static BITMAPINFO2 *convertToBmp2( BITMAPINFO *bmi )
{
    BITMAPINFO2         *bmi2;
    LONG                info_size;
    USHORT              colour_count;
    USHORT              i;

    info_size = BMPINFO2_SIZE( bmi->cBitCount );
    bmi2 = MemAlloc( info_size );
    if (!bmi2) return(NULL);

    memset(bmi2, 0, sizeof(info_size) );
    bmi2->cbFix = sizeof(BITMAPINFO2) - sizeof(RGB2);
    bmi2->cx = bmi->cx;
    bmi2->cy = bmi->cy;
    bmi2->cPlanes = bmi->cPlanes;
    bmi2->cBitCount = bmi->cBitCount;
    colour_count = (1<<(bmi->cBitCount));
    for (i=0; i < colour_count; ++i) {
        bmi2->argbColor[i].bBlue = bmi->argbColor[i].bBlue;
        bmi2->argbColor[i].bGreen = bmi->argbColor[i].bGreen;
        bmi2->argbColor[i].bRed = bmi->argbColor[i].bRed;
    }
    return( bmi2 );
} /* convertToBmp2 */

/*
 * readBitmapInfo - returns the bitmap info
 */
static BITMAPINFO2 *readBitmapInfo( FILE *fp, BITMAPINFOHEADER2 *infoheader,
                                                BOOL is_bmp2, ULONG offset )
{
    ULONG               tablesize;
    BITMAPINFO2         *bmi2;
    BITMAPINFO          *bmi;

    /*
     * offset is to the colour table
     */
    if ( is_bmp2 ) {
        bmi2 = MemAlloc( BMPINFO2_SIZE(infoheader->cBitCount) );
        if( !bmi2 ) return( NULL );

        tablesize = BMPINFO2_SIZE(infoheader->cBitCount) -
                                                sizeof(BITMAPINFOHEADER2 );
        memcpy( bmi2, infoheader, sizeof(BITMAPINFOHEADER2) );
        fseek( fp, offset, SEEK_SET );
        fread( &(bmi2->argbColor), tablesize, 1, fp );
    } else {
        bmi = MemAlloc( BMPINFO_SIZE(infoheader->cBitCount) );
        if ( !bmi ) return (NULL);

        tablesize = BMPINFO_SIZE(infoheader->cBitCount) -
                                                sizeof(BITMAPINFOHEADER);
        bmi->cbFix = sizeof( BITMAPINFOHEADER );
        bmi->cx = (USHORT)infoheader->cx;
        bmi->cy = (USHORT)infoheader->cy;
        bmi->cPlanes = infoheader->cPlanes;
        bmi->cBitCount = infoheader->cBitCount;
        fseek( fp, offset, SEEK_SET );
        fread( &(bmi->argbColor), tablesize, 1, fp );

        bmi2 = convertToBmp2( bmi );
        MemFree( bmi );
    }
    return( bmi2 );
} /* readBitmapInfo */

/*
 * readBitmap - reads in the bitmap.
 */
static HBITMAP readBitmap(HWND hwnd, FILE *fp, ULONG off_bits, ULONG offset,
                                BOOL is_bmp2, BITMAPINFOHEADER2 *bmih2 )
{
    ULONG               size;
    HBITMAP             hbitmap;
    BYTE                *bits;
    HPS                 hps;
    BITMAPINFO2         *info;
    HPAL                newpalette;
    HPAL                oldpalette;
    ULONG               num;

    hbitmap = (HBITMAP)0;

    info = readBitmapInfo( fp, bmih2, is_bmp2, offset );
    if (!info) return(hbitmap);

    size = BITS_TO_BYTES( info->cx * info->cBitCount, info->cy );
    fseek( fp, off_bits, SEEK_SET );
    bits = MemAlloc( size );
    if( bits != NULL ) {
        readInPieces( bits, fp, size );
        newpalette = CreateDIBPalette( info );
        if (newpalette) {
            hps = WinGetPS( hwnd );
            GpiCreateLogColorTable(hps, 0, LCOLF_RGB, 0, 0, NULL);
            oldpalette = GpiSelectPalette( hps, newpalette );
            WinRealizePalette( hwnd, hps, &num );
            hbitmap = GpiCreateBitmap( hps, bmih2, CBM_INIT, bits, info );
            GpiSelectPalette( hps, oldpalette );
            WinReleasePS( hps );
            GpiDeletePalette( newpalette );
        }
        MemFree( bits );
    }
    MemFree( info );
    return( hbitmap );
} /* readBitmap */

/*
 * getFileHeaderFromArray - returns the BITMAPFILEHEADER2 structure upon
 *                          reading the BITMAPARRAYFILEHEADER structure from
 *                          the given offset.
 */
static BITMAPFILEHEADER2 *getFileHeaderFromArray( FILE *fp, ULONG offset, BOOL *is_bmp2 )
{
    BITMAPARRAYFILEHEADER       filearray;
    BITMAPARRAYFILEHEADER2      filearray2;
    BITMAPFILEHEADER2           *fileheader2;
    BITMAPFILEHEADER2           *f;

    fileheader2 = MemAlloc( sizeof(BITMAPFILEHEADER2) );

    fseek( fp, offset, SEEK_SET );
    fread( &filearray2, sizeof( BITMAPARRAYFILEHEADER2 ), 1, fp );
    if ( filearray2.cbSize != sizeof(BITMAPARRAYFILEHEADER2) ) {
        memcpy( &filearray, &filearray2, sizeof(BITMAPARRAYFILEHEADER) );
        memset( &(filearray2.bfh2.bmp2), 0, sizeof(BITMAPINFOHEADER2) );
        f = &(filearray2.bfh2);
        f->cbSize = sizeof(BITMAPFILEHEADER2);
        f->bmp2.cbFix = sizeof( BITMAPINFOHEADER2 );
        f->bmp2.cx = filearray.bfh.bmp.cx;
        f->bmp2.cy = filearray.bfh.bmp.cy;
        f->bmp2.cPlanes = filearray.bfh.bmp.cPlanes;
        f->bmp2.cBitCount = filearray.bfh.bmp.cBitCount;
        *is_bmp2 = FALSE;
        memcpy( fileheader2, f, sizeof(BITMAPFILEHEADER2) );
    } else {
        memcpy( fileheader2, &(filearray2.bfh2), sizeof(BITMAPFILEHEADER2) );
    }
    return( fileheader2 );
} /* getFileHeaderFromArray */

/*
 * readFileHeader - reads the bitmap file header (converts to
 *                  BITMAPFILEHEADER2 type) from the given offset.
 */
static BITMAPFILEHEADER2 *readFileHeader( FILE *fp, ULONG offset, BOOL *is_bmp2 )
{
    BITMAPFILEHEADER    *bmfh;
    BITMAPFILEHEADER2   *bmfh2;

    fseek( fp, offset, SEEK_SET );
    bmfh = MemAlloc( sizeof(BITMAPFILEHEADER) );
    bmfh2 = MemAlloc( sizeof(BITMAPFILEHEADER2) );

    /*
     * We'll have to read the bitmap file header and then determine
     * whether we read the BITMAPFILEHEADER or BITMAPFILEHEADER2 structure.
     */
    fread( bmfh2, sizeof( BITMAPFILEHEADER2 ), 1, fp );
    if ( bmfh2->cbSize != sizeof(BITMAPFILEHEADER2) ) {
        memcpy( bmfh, bmfh2, sizeof(BITMAPFILEHEADER) );
        memset( &(bmfh2->bmp2), 0, sizeof(BITMAPINFOHEADER2) );
        bmfh2->cbSize = sizeof( BITMAPFILEHEADER2 );
        bmfh2->bmp2.cbFix = sizeof( BITMAPINFOHEADER2 );
        bmfh2->bmp2.cx = bmfh->bmp.cx;
        bmfh2->bmp2.cy = bmfh->bmp.cy;
        bmfh2->bmp2.cPlanes = bmfh->bmp.cPlanes;
        bmfh2->bmp2.cBitCount = bmfh->bmp.cBitCount;
        *is_bmp2 = FALSE;
        MemFree( bmfh );
    } else {
        *is_bmp2 = TRUE;
    }
    return( bmfh2 );
} /* readFileHeader */

/*
 * ReadPMBitmapFile - reads in a pm format bitmap file and returns a handle
 *                    to the bitmap
 */
HBITMAP ReadPMBitmapFile( HWND hwnd, char *fname, BITMAPINFOHEADER2 *info )
{
    FILE                *fp;
    HBITMAP             hbitmap;
    BITMAPFILEHEADER2   *file_header2;
    BOOL                is_bmp2;
    USHORT              filetype;
    ULONG               offset;

    hbitmap = (HBITMAP)0;
    fp = fopen( fname, "rb" );
    if( fp == NULL ) return( hbitmap );

    fread( &filetype, sizeof(USHORT), 1, fp );
    switch( filetype ) {
    case BFT_BMAP:
        file_header2 = readFileHeader( fp, 0L, &is_bmp2 );
        if ( is_bmp2 ) {
            offset = sizeof( BITMAPFILEHEADER2 );
        } else {
            offset = sizeof( BITMAPFILEHEADER );
        }
        hbitmap = readBitmap( hwnd, fp, file_header2->offBits, offset,
                                        is_bmp2, &(file_header2->bmp2) );
        break;

    case BFT_BITMAPARRAY:
        /*
         * For now, we just return the handle to the first bitmap we
         * read (the file could possibly contain more than 1)
         */
        file_header2 = getFileHeaderFromArray( fp, 0L, &is_bmp2 );
        if ( is_bmp2 ) {
            offset = sizeof( BITMAPARRAYFILEHEADER2 );
        } else {
            offset = sizeof( BITMAPARRAYFILEHEADER );
        }
        hbitmap = readBitmap( hwnd, fp, file_header2->offBits, offset,
                                        is_bmp2, &(file_header2->bmp2) );
        break;

    default:
        fclose( fp );
        return( hbitmap );
    }

    memcpy( info, &(file_header2->bmp2), sizeof(BITMAPINFOHEADER2) );

    fclose( fp );
    MemFree( file_header2 );
    return( hbitmap );
} /* ReadPMBitmapFile */
