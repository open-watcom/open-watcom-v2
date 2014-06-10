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


#include <wwindows.h>
#include <limits.h>
#include <string.h>
#include <malloc.h>
#include <dos.h>
#include <stdlib.h>
#include "wrglbl.h"
#include "wrmemi.h"
#include "wr_wres.h"
#include "palette.h"

/****************************************************************************/
/* macro definitions                                                        */
/****************************************************************************/
#define START_OF_HEADER         sizeof( BITMAPFILEHEADER )
#define HUGE_SHIFT              8
#define CHUNK_SIZE              (48 * 1024)
#define RGBQ_SIZE( bc )         (sizeof( RGBQUAD ) * ((size_t)1 << (bc)))
#define SCANLINE_SIZE           32
#define MAX_CHUNK               32768

/****************************************************************************/
/* external function prototypes                                             */
/****************************************************************************/

/****************************************************************************/
/* type definitions                                                         */
/****************************************************************************/

/****************************************************************************/
/* static function prototypes                                               */
/****************************************************************************/
static BITMAPINFO       *WRReadDIBInfo( BYTE **data );
static BITMAPCOREINFO   *WRReadCoreInfo( BYTE **data );
static HBITMAP          WRReadBitmap( BYTE *data, long offset, BOOL core, bitmap_info *info );

/****************************************************************************/
/* static variables                                                         */
/****************************************************************************/
static WResID   *BitmapName = NULL;

#if defined( _M_I86 )

#define _HUGE __huge
#define __halloc halloc
#define __hfree hfree
static void HugeMemCopy( void __far *dst, void __far *src, unsigned bytes )
{
    long                offset, selector;
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
#ifdef _HUGE
    #undef _HUGE
#endif
#define _HUGE
#define __halloc( a, b ) malloc( a )
#define __hfree free

#endif

static BITMAPINFO *WRReadDIBInfo( BYTE **data )
{
    BITMAPINFO          *bm;
    BITMAPINFOHEADER    *header;
    long                bitmap_size;
    int                 pos;

    if( data == NULL || *data == NULL ) {
        return( NULL );
    }

    pos = START_OF_HEADER;
    header = (BITMAPINFOHEADER *)((*data) + pos);
    bitmap_size = DIB_INFO_SIZE( header->biBitCount );
    bm = MemAlloc( bitmap_size );
    if( bm != NULL ) {
        memcpy( bm, header, bitmap_size );
        *data += pos + bitmap_size;
    }

    return( bm );
}

static BITMAPCOREINFO *WRReadCoreInfo( BYTE **data )
{
    BITMAPCOREINFO      *bm_core;
    BITMAPCOREHEADER    *header;
    long                bitmap_size;
    int                 pos;

    if( data == NULL || *data == NULL ) {
        return( NULL );
    }

    pos = START_OF_HEADER;
    header = (BITMAPCOREHEADER *)(*data + pos);
    bitmap_size = CORE_INFO_SIZE( header->bcBitCount );
    bm_core = MemAlloc( bitmap_size );
    if( bm_core == NULL ) {
        return( NULL );
    }
    memcpy( bm_core, *data + pos, bitmap_size );
    *data += pos + bitmap_size;

    return( bm_core );
}

static void WRReadInPieces( BYTE _HUGE *dst, BYTE *data, DWORD size )
{
    BYTE                *buffer = NULL;
    WORD                chunk_size;

    chunk_size = CHUNK_SIZE;
    while( chunk_size != 0 && (buffer = MemAlloc( chunk_size )) == NULL ) {
        chunk_size >>= 1;
    }
    if( buffer == NULL ) {
        return;
    }
    while( size > chunk_size ) {
        memcpy( buffer, data, chunk_size );
        HugeMemCopy( dst, buffer, chunk_size );
        dst += chunk_size;
        size -= chunk_size;
        data += chunk_size;
    }
    memcpy( buffer, data, size );
    HugeMemCopy( dst, buffer, size );
    MemFree( buffer );
}

static HBITMAP WRReadBitmap( BYTE *data, long offset, BOOL core, bitmap_info *info )
{
    DWORD               size;           /* generic size - used repeatedly */
    BYTE _HUGE          *mask_ptr;      /* pointer to bit array in memory */
    HDC                 hdc;
    HPALETTE            new_palette, old_palette;
    BITMAPINFO          *bm_info = NULL;
    BITMAPCOREINFO      *bm_core = NULL;
    HBITMAP             bitmap_handle;
    int                 pos;

    bitmap_handle = (HBITMAP)NULL;

    if( core ) {
        bm_core = WRReadCoreInfo( &data );
        if( bm_core == NULL ) {
            return( bitmap_handle );
        }
        size = BITS_TO_BYTES( bm_core->bmciHeader.bcWidth * bm_core->bmciHeader.bcBitCount,
                              bm_core->bmciHeader.bcHeight );
    } else {
        bm_info = WRReadDIBInfo( &data );
        if( bm_info == NULL ) {
            return( bitmap_handle );
        }
        size = BITS_TO_BYTES( bm_info->bmiHeader.biWidth * bm_info->bmiHeader.biBitCount,
                              bm_info->bmiHeader.biHeight );
    }

    pos = offset;
    mask_ptr = __halloc( size, 1 );
    if( mask_ptr != NULL ) {
        WRReadInPieces( mask_ptr, data, size );
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
                if( new_palette != NULL ) {
                    hdc = GetDC( (HWND)NULL );
                    old_palette = SelectPalette( hdc, new_palette, FALSE );
                    RealizePalette( hdc );
                    bitmap_handle = CreateDIBitmap( hdc, &bm_info->bmiHeader, CBM_INIT,
                                                    mask_ptr, bm_info, DIB_RGB_COLORS );
                    SelectPalette( hdc, old_palette, FALSE );
                    DeleteObject( new_palette );
                    ReleaseDC( (HWND)NULL, hdc );
                }
            } else {
                /* Bitmap with no palette*/
                hdc = GetDC( (HWND)NULL );
                bitmap_handle = CreateDIBitmap( hdc, &bm_info->bmiHeader, CBM_INIT,
                                                mask_ptr, bm_info, DIB_RGB_COLORS );
                ReleaseDC( (HWND)NULL, hdc );
            }
        }
        __hfree( mask_ptr );
    }
    if( core ) {
        if( info != NULL ) {
            info->u.bm_core = bm_core;
        } else {
            MemFree( bm_core );
        }
    } else {
        if( info != NULL ) {
            info->u.bm_info = bm_info;
        } else {
            MemFree( bm_info );
        }
    }
    return( bitmap_handle );
}

/*
 * Creates a device independant bitmap from the data <data> and
 * returns a handle to a newly created BITMAP.
 */
HBITMAP WRAPI WRBitmapFromData( BYTE *data, bitmap_info *info )
{
    HBITMAP             bitmap_handle;
    BITMAPFILEHEADER    *file_header;
    BOOL                core;
    DWORD               *size;
    int                 pos;
    long                offset;

    if( data == NULL ) {
        return( (HBITMAP)NULL );
    }

    bitmap_handle = (HBITMAP)NULL;
    file_header = (BITMAPFILEHEADER *)data;

    if( file_header->bfType != BITMAP_TYPE ) {
        return( bitmap_handle );
    }

    pos = sizeof( BITMAPFILEHEADER );
    size = (DWORD *)(data + pos);

    core = (*size == sizeof( BITMAPCOREHEADER ));
    if( !core ) {
        offset = file_header->bfOffBits;
        bitmap_handle = WRReadBitmap( data, offset, core, info );
    }

    if( info != NULL ) {
        info->is_core = core;
    }

    return( bitmap_handle );
}

void WRGetBitmapInfoHeader( BITMAPINFOHEADER *bmih, BITMAP *bm )
{
    bmih->biSize = sizeof( BITMAPINFOHEADER );
    bmih->biWidth = bm->bmWidth;
    bmih->biHeight = bm->bmHeight;
    bmih->biPlanes = 1;
    bmih->biBitCount = bm->bmPlanes;
    bmih->biCompression = 0;
    bmih->biSizeImage = 0;
    bmih->biXPelsPerMeter = 0;
    bmih->biYPelsPerMeter = 0;
    bmih->biClrUsed = 0;
    bmih->biClrImportant = 0;
}

int WRSetRGBValues( RGBQUAD *argbvals, int upperlimit )
{
    int                 i;
    PALETTEENTRY        *pe;
    int                 num;
    HDC                 hdc;

    if( argbvals == NULL ) {
        return( FALSE );
    }

    pe = (PALETTEENTRY *)MemAlloc( upperlimit * sizeof( PALETTEENTRY ) );
    if( pe == NULL ) {
        return( FALSE );
    }

    hdc = GetDC( HWND_DESKTOP );
    num = GetSystemPaletteEntries( hdc, 0, upperlimit, pe );
    ReleaseDC( HWND_DESKTOP, hdc );

    for( i = 0; i < min( upperlimit, num ); i++ ) {
        argbvals[i].rgbBlue = pe[i].peBlue;
        argbvals[i].rgbGreen = pe[i].peGreen;
        argbvals[i].rgbRed = pe[i].peRed;
        argbvals[i].rgbReserved = 0;
    }

    MemFree( pe );

    return( TRUE );
}

int WRGetBitmapInfo( BITMAPINFO *bmi, BITMAP *bm )
{
    RGBQUAD     *rgb_quad;
    int         ret;

    if( bmi == NULL || bm == NULL ) {
        return( FALSE );
    }

    rgb_quad = MemAlloc( RGBQ_SIZE( bm->bmPlanes ) );
    if( rgb_quad == NULL ) {
        return( FALSE );
    }

    WRGetBitmapInfoHeader( &bmi->bmiHeader, bm );
    ret = WRSetRGBValues( rgb_quad, 1 << bm->bmPlanes );
    if( ret ) {
        memcpy( bmi->bmiColors, rgb_quad, RGBQ_SIZE( bm->bmPlanes ) );
    }

    MemFree( rgb_quad );

    return( ret );
}

BITMAPINFO *WRGetDIBitmapInfo( HBITMAP hbitmap )
{
    long        size;
    BITMAPINFO  *bmi;
    BITMAP      bm;

    if( hbitmap == (HBITMAP)NULL ) {
        return( NULL );
    }
    GetObject( hbitmap, sizeof( BITMAP ), &bm );
    size = DIB_INFO_SIZE( bm.bmPlanes );
    bmi = (BITMAPINFO *)MemAlloc( size );
    if( bmi != NULL ) {
        if( !WRGetBitmapInfo( bmi, &bm ) ) {
            MemFree( bmi );
            bmi = NULL;
        }
    }

    return( bmi );
}

int WRWriteDataInPiecesData( BITMAPINFO *bmi, BYTE **data, uint_32 *size,
                             HBITMAP hbitmap )
{
    HDC         hdc;
    HDC         memdc;
    int         scanline_count;
    int         one_scanline_size;
    long        chunk_size;
    int         start;
    int         num_lines;
    long        byte_count;

    if( data == NULL || *data == NULL || size == NULL ) {
        return( FALSE );
    }

    hdc = GetDC( (HWND)NULL );
    memdc = CreateCompatibleDC( hdc );
    ReleaseDC( (HWND)NULL, hdc );

    byte_count = bmi->bmiHeader.biSizeImage;
    start = 0;
    num_lines = SCANLINE_SIZE;
    one_scanline_size = BITS_TO_BYTES( bmi->bmiHeader.biWidth *
                                       bmi->bmiHeader.biBitCount, 1 );
    scanline_count = bmi->bmiHeader.biHeight;
    chunk_size = one_scanline_size * num_lines;
    while( chunk_size > MAX_CHUNK ) {
        chunk_size >>= 1;
        num_lines = chunk_size / one_scanline_size;
    }

    while( scanline_count > num_lines ) {
        GetDIBits( memdc, hbitmap, start, num_lines, *data + *size, bmi, DIB_RGB_COLORS );
        *size += chunk_size;
        scanline_count -= num_lines;
        start += num_lines;
        byte_count -= chunk_size;
    }
    GetDIBits( memdc, hbitmap, start, scanline_count, *data + *size, bmi, DIB_RGB_COLORS );
    *size += scanline_count * one_scanline_size;

    DeleteDC( memdc );

    return( TRUE );
}

int WRAPI WRWriteBitmapToData( HBITMAP hbitmap, BYTE **data, uint_32 *size )
{
    BITMAPFILEHEADER    bmfh;
    BITMAPINFO          *bmi;
    long                bitmap_size;
    long                number_of_bytes;
    HDC                 hdc;

    if( hbitmap == (HBITMAP)NULL || data == NULL || size == NULL ) {
        return( FALSE );
    }

    bmi = WRGetDIBitmapInfo( hbitmap );
    if( bmi == NULL ) {
        return( FALSE );
    }

    number_of_bytes = BITS_TO_BYTES( bmi->bmiHeader.biBitCount * bmi->bmiHeader.biWidth,
                                     bmi->bmiHeader.biHeight );

    bitmap_size = DIB_INFO_SIZE( bmi->bmiHeader.biBitCount );

    hdc = GetDC( (HWND)NULL );
    GetDIBits( hdc, hbitmap, 0, bmi->bmiHeader.biHeight, NULL, bmi, DIB_RGB_COLORS );
    ReleaseDC( (HWND)NULL, hdc );

    if( bmi->bmiHeader.biSizeImage == 0 ) {
        bmi->bmiHeader.biSizeImage = number_of_bytes;
    } else {
        number_of_bytes = bmi->bmiHeader.biSizeImage;
    }

    bmfh.bfType = BITMAP_TYPE;
    bmfh.bfSize = sizeof( BITMAPFILEHEADER ) + bitmap_size + number_of_bytes;
    bmfh.bfReserved1 = 0;
    bmfh.bfReserved2 = 0;
    bmfh.bfOffBits = sizeof( BITMAPFILEHEADER ) + bitmap_size;

    // make sure the bitmap can actually be malloc'd!!
    if( bmfh.bfSize > INT_MAX ) {
        MemFree( bmi );
        return( FALSE );
    }

    *data = MemAlloc( bmfh.bfSize );
    if( *data == NULL ) {
        MemFree( bmi );
        return( FALSE );
    }
    *size = 0;

    memcpy( *data + *size, &bmfh, sizeof( BITMAPFILEHEADER ) );
    *size += sizeof( BITMAPFILEHEADER );

    memcpy( *data + *size, bmi, bitmap_size );
    *size += bitmap_size;

    if( !WRWriteDataInPiecesData( bmi, data, size, hbitmap ) ) {
        MemFree( bmi );
        return( FALSE );
    }

    MemFree( bmi );

    return( TRUE );
}

int WRAPI WRAddBitmapFileHeader( BYTE **data, uint_32 *size )
{
    BITMAPFILEHEADER    *bmfh;
    BITMAPINFO          *bmi;
    BITMAPCOREINFO      *bmci;
    int                 hsize;
    int                 is_core;

    if( data == NULL || size == NULL ) {
        return( FALSE );
    }

    is_core = (*(DWORD *)*data == sizeof( BITMAPCOREHEADER ));

    hsize = sizeof( BITMAPFILEHEADER );
    *data = MemRealloc( *data, *size + hsize );
    if( *data == NULL ) {
        return( FALSE );
    }
    memmove( *data + hsize, *data, *size );
    memset( *data, 0, hsize );
    *size += hsize;

    bmfh = (BITMAPFILEHEADER *)*data;
    bmfh->bfType = BITMAP_TYPE;
    bmfh->bfSize = *size;
    bmfh->bfOffBits = hsize;

    if( is_core ) {
        bmci = (BITMAPCOREINFO *)(*data + hsize);
        bmfh->bfOffBits += CORE_INFO_SIZE( bmci->bmciHeader.bcBitCount );
    } else {
        bmi = (BITMAPINFO *)(*data + hsize);
        bmfh->bfOffBits += DIB_INFO_SIZE( bmi->bmiHeader.biBitCount );
    }

    return( TRUE );
}

int WRAPI WRStripBitmapFileHeader( BYTE **data, uint_32 *size )
{
    int         bfhsize;
    if( data != NULL && size != NULL ) {
        bfhsize = sizeof( BITMAPFILEHEADER );
        memmove( *data, *data + bfhsize, *size - bfhsize );
        *size -= bfhsize;
        return( TRUE );
    }

    return( FALSE );
}

void WRAPI WRForgetBitmapName( void )
{
    if( BitmapName != NULL ) {
        MemFree( BitmapName );
        BitmapName = NULL;
    }
}

void WRAPI WRRememberBitmapName( WResID *name )
{
    WRForgetBitmapName();
    BitmapName = WRCopyWResID( name );
}

WResID * WRAPI WRRecallBitmapName( void )
{
    return( WRCopyWResID( BitmapName ) );
}
