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
* Description:  Helpers for .BMP file loading.
*
****************************************************************************/

#include "precomp.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#include <dos.h>
#include "palette.h"
#include "bitmap.h"
#include "mem.h"

#ifndef _WCI86HUGE
    #if !defined( __386__ ) && defined( _M_IX86 )
        #define _WCI86HUGE __huge
    #else
        #define _WCI86HUGE
    #endif
#endif

#define HUGE_SHIFT      8
#define START_OF_HEADER sizeof( BITMAPFILEHEADER )

/*
 * readDIBInfo - read BITMAPINFO structure from a bitmap file
 *
 * NOTE: Assume fp is positioned at the start of the bitmap information.
 *       We first read in the BITMAPINFOHEADER to get information about the
 *       number of quads needed, then we reposition ourselves and read in
 *       the entire BITMAPINFOHEADER structure.
 */
static BITMAPINFO *readDIBInfo( FILE *fp )
{
    BITMAPINFO          *bm;
    BITMAPINFOHEADER    *header;
    long                bitmap_size;

    header = MemAlloc( sizeof( BITMAPINFOHEADER ) );
    if( header == NULL ) {
        return( NULL );
    }
    fseek( fp, START_OF_HEADER, SEEK_SET );
    fread( header, sizeof( BITMAPINFOHEADER ), 1, fp );
    if( header->biBitCount < 9 ) {
        /* Bitmap has palette, read it */
        fseek( fp, START_OF_HEADER, SEEK_SET );
        bitmap_size = DIB_INFO_SIZE( header->biBitCount );
        bm = MemReAlloc( header, bitmap_size );
        if( bm == NULL ) {
            return( NULL );
        }
        fread( bm, bitmap_size, 1, fp );
    }
    else {
        return( (BITMAPINFO*) header );
    }

    return( bm );

} /* readDIBInfo */

/*
 * readCoreInfo - read BITMAPCOREINFO structure from a bitmap file
 */
static BITMAPCOREINFO *readCoreInfo( FILE *fp )
{
    BITMAPCOREINFO      *bm_core;
    BITMAPCOREHEADER    *header;
    long                bitmap_size;

    header = MemAlloc( sizeof( BITMAPCOREHEADER ) );
    if( header == NULL ) {
        return( NULL );
    }
    fseek( fp, START_OF_HEADER, SEEK_SET );
    fread( header, sizeof( BITMAPCOREHEADER ), 1, fp );
    fseek( fp, START_OF_HEADER, SEEK_SET );
    bitmap_size = CORE_INFO_SIZE( header->bcBitCount );
    bm_core = MemReAlloc( header, bitmap_size );
    if( bm_core == NULL ) {
        return( NULL );
    }
    fread( bm_core, bitmap_size, 1, fp );
    return( bm_core );

} /* readCoreInfo */

#if defined( _M_I86 )
#define __halloc halloc
#define __hfree hfree
static void HugeMemCopy( void __far *dst, void __far *src, unsigned bytes )
{
    long                offset;
    long                selector;
    long                bytes_before_segment_end;

    offset = FP_OFF( dst );
    selector = FP_SEG( dst );
    bytes_before_segment_end = 0x10000L - offset;
    if( bytes_before_segment_end < bytes ) {
        _fmemcpy( dst, src, bytes_before_segment_end );
        bytes -= bytes_before_segment_end;
        selector += HUGE_SHIFT;
        dst = MK_FP( selector, 0 );
        src = (char *)src + bytes_before_segment_end;
    }
    _fmemcpy( dst, src, bytes );
}
#else
#define HugeMemCopy( a, b, c ) memcpy( a, b, c )
#define __halloc( a, b ) MemAlloc( a )
#define __hfree MemFree
#endif

/* This is the amount of memory we read in at once. */
#define CHUNK_SIZE      (48 * 1024)

/*
 * readInPieces - read a specified amount of data from a file in chunks
 */
static void readInPieces( BYTE _WCI86HUGE *dst, FILE *fp, DWORD size )
{
    BYTE                *buffer;
    WORD                chunk_size;

    chunk_size = CHUNK_SIZE;
    while( chunk_size && (buffer = MemAlloc( chunk_size )) == NULL ) {
        chunk_size >>= 1;
    }
    if( buffer == NULL ) {
        return;
    }
    while( size > chunk_size ) {
        fread( buffer, chunk_size, 1, fp );
        HugeMemCopy( dst, buffer, chunk_size );
        dst += chunk_size;
        size -= chunk_size;
    }
    fread( buffer, size, 1, fp );
    HugeMemCopy( dst, buffer, size );
    MemFree( buffer );

} /* readInPieces */

/*
 * readBitmap - read a bitmap from a file
 */
static HBITMAP readBitmap( HWND hwnd, FILE *fp, long offset, BOOL core,
                           bitmap_info *info )
{
    DWORD               size;           /* generic size - used repeatedly */
    BYTE _WCI86HUGE     *mask_ptr;      /* pointer to bit array in memory */
    HDC                 hdc;
    HPALETTE            new_palette;
    HPALETTE            old_palette;
    BITMAPINFO          *bm_info;
    BITMAPCOREINFO      *bm_core;
    HBITMAP             bitmap_handle;

    bitmap_handle = (HBITMAP)0;
    if( core ) {
        bm_core = readCoreInfo( fp );
        if( bm_core == NULL ) {
            return( bitmap_handle );
        }
        size = BITS_TO_BYTES( bm_core->bmciHeader.bcWidth * bm_core->bmciHeader.bcBitCount,
                              bm_core->bmciHeader.bcHeight );
    } else {
        bm_info = readDIBInfo( fp );
        if( bm_info == NULL ) {
            return( bitmap_handle );
        }
        size = BITS_TO_BYTES( bm_info->bmiHeader.biWidth * bm_info->bmiHeader.biBitCount,
                              bm_info->bmiHeader.biHeight );
    }
    fseek( fp, offset, SEEK_SET );
    mask_ptr = __halloc( size, 1 );
    if( mask_ptr != NULL ) {
        readInPieces( mask_ptr, fp, size );
        if( core ) {
            BITMAPCOREHEADER    *h;

            h = &bm_core->bmciHeader;

            /*
             * This will cause a GP Fault!
             */
            bitmap_handle = CreateBitmap( h->bcWidth, h->bcHeight, h->bcPlanes,
                                          h->bcBitCount, mask_ptr );
        } else {
            if( bm_info->bmiHeader.biBitCount < 9 ) {
                /* Bitmap has palette, create it */
                new_palette = CreateDIBPalette( bm_info );
                if( new_palette ) {
                    hdc = GetDC( hwnd );
                    old_palette = SelectPalette( hdc, new_palette, FALSE );
                    RealizePalette( hdc );
                    bitmap_handle = CreateDIBitmap( hdc, &bm_info->bmiHeader, CBM_INIT,
                                                    mask_ptr, bm_info, DIB_RGB_COLORS );
                    SelectPalette( hdc, old_palette, FALSE );
                    DeleteObject( new_palette );
                    ReleaseDC( hwnd, hdc );
                }
            } else {
                /* Bitmap with no palette */
                hdc = GetDC( hwnd );
                bitmap_handle = CreateDIBitmap( hdc, &bm_info->bmiHeader, CBM_INIT,
                                                mask_ptr, bm_info, DIB_RGB_COLORS );
                ReleaseDC( hwnd, hdc );
            }
        }
        __hfree( mask_ptr );
    }
    if( core ) {
        if( info != NULL ) {
            info->bm_core = bm_core;
        } else {
            MemFree( bm_core );
        }
    } else {
        if( info != NULL ) {
            info->bm_info = bm_info;
        } else {
            MemFree( bm_info );
        }
    }
    return( bitmap_handle );

} /* readBitmap */

/*
 * ReadBitmapFile - load a device independant bitmap from the file <file_name> and
 *                  return a handle to a newly created bitmap
 */

HBITMAP ReadBitmapFile( HWND hwnd, char *file_name, bitmap_info *info )
{
    FILE                *fp;
    HBITMAP             bitmap_handle;
    BITMAPFILEHEADER    file_header;
    BOOL                core;
    DWORD               size;

    bitmap_handle = (HBITMAP)0;
    fp = fopen( file_name, "rb" );
    if( fp == NULL ) {
        return( bitmap_handle );
    }
    fread( &file_header, sizeof( BITMAPFILEHEADER ), 1, fp );
    if( file_header.bfType != BITMAP_TYPE ) {
        fclose( fp );
        return( bitmap_handle );
    }
    fread( &size, sizeof( size ), 1, fp );
    core = (size == sizeof( BITMAPCOREHEADER ));
    if( !core ) {
        bitmap_handle = readBitmap( hwnd, fp, file_header.bfOffBits, core, info );
    }
    if( info != NULL ) {
        info->is_core = core;
    }

    fclose( fp );
    return( bitmap_handle );

} /* ReadBitmapFile */
