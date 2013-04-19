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


#include "imgedit.h"

static BYTE             powersOf2[8] = { 128, 64, 32, 16, 8, 4, 2, 1 };
static BYTE             inverseOf2[8] = { 127, 191, 223, 239, 247, 251, 253, 254} ;
static COLORREF         colorPalette[16] = {
                            0x000000, 0x000080, 0x008000, 0x008080,
                            0x800000, 0x800080, 0x808000, 0x808080,
                            0xC0C0C0, 0x0000FF, 0x00FF00, 0x00FFFF,
                            0xFF0000, 0xFF00FF, 0xFFFF00, 0xFFFFFF };

#ifdef __OS2_PM__

/*
 * getTheBitmapBits - different for PM and Windows
 */
static void getTheBitmapBits( HBITMAP bitmap, long bcount, BYTE *bits )
{
    _wpi_getbitmapbits( bitmap, bcount, bits );

} /* getTheBitmapBits */

#else

/*
 * getTheBitmapBits - different for PM and Windows
 */
static void getTheBitmapBits( HBITMAP bitmap, long bcount, BYTE *bits )
{
    BITMAP              bm;
    BITMAPINFO          *bmi;
    BITMAPINFOHEADER    *h;
    HDC                 hdc;
    HDC                 memdc;
    int                 size;
    int                 bc;
    RGBQUAD             *rgb;

    bcount = bcount;
    GetObject( bitmap, sizeof( BITMAP ), &bm );
    bc = bm.bmPlanes * bm.bmBitsPixel;
    size = DIB_INFO_SIZE( bc );
    bmi = MemAlloc( size );

    hdc = GetDC( NULL );
    memdc = CreateCompatibleDC( hdc );
    ReleaseDC( NULL, hdc );

    h = &bmi->bmiHeader;
    h->biSize = sizeof( BITMAPINFOHEADER );
    h->biWidth = bm.bmWidth;
    h->biHeight = bm.bmHeight;
    h->biPlanes = 1;
    h->biBitCount = bc;
    h->biCompression = 0;
    h->biSizeImage = 0;
    h->biXPelsPerMeter = 0;
    h->biYPelsPerMeter = 0;
    h->biClrUsed = 0;
    h->biClrImportant = 0;

    rgb = MemAlloc( RGBQ_SIZE( h->biBitCount ) );
    SetRGBValues( rgb, 1 << h->biBitCount );
    memcpy( bmi->bmiColors, rgb, RGBQ_SIZE( h->biBitCount ) );
    MemFree( rgb );

    GetDIBits( memdc, bitmap, 0, bm.bmHeight, NULL, bmi, DIB_RGB_COLORS );
    if( bmi->bmiHeader.biSizeImage == 0 ) {
        if( bm.bmWidth > 32 && FALSE ) {
            bmi->bmiHeader.biSizeImage = BITS_INTO_BYTES( bm.bmWidth * bc, bm.bmHeight );
        } else {
            bmi->bmiHeader.biSizeImage = BITS_TO_BYTES( bm.bmWidth * bc, bm.bmHeight );
        }
    }
    GetDIBits( memdc, bitmap, 0, bm.bmHeight, bits, bmi, DIB_RGB_COLORS );

    DeleteDC( memdc );
    MemFree( bmi );

} /* getTheBitmapBits */

#endif

/*
 * GetTheBits - gets the bits for the bitmap
 */
bitmap_bits *GetTheBits( HBITMAP bitmap )
{
    int         width;
    int         height;
    int         bitspixel;
    int         planes;
    bitmap_bits *info;

    _wpi_getbitmapparms( bitmap, &width, &height, &planes, NULL, &bitspixel );

    info = MemAlloc( sizeof( bitmap_bits ) );
    info->bitcount = planes * bitspixel;
    if( width > 32 && FALSE ) {
        info->byte_count = BITS_INTO_BYTES( width * info->bitcount, height );
    } else {
        info->byte_count = BITS_TO_BYTES( width * info->bitcount, height );
    }
    info->width = ((width + 7) / 8) * 8;
    info->bits = MemAlloc( info->byte_count );

//    ret = _wpi_getbitmapbits( bitmap, info->byte_count, info->bits );
    getTheBitmapBits( bitmap, info->byte_count, info->bits );
    return( info );

} /* GetTheBits */

/*
 * getBWPixel - returns the color of a pixel in a black and white bitmap
 */
static COLORREF getBWPixel( bitmap_bits *bits, int x, int y )
{
    BYTE        *new_bit;
    short       i;

    new_bit = bits->bits + (y * (bits->width / 8)) + x / 8;
    i = x % 8;
    if( *new_bit & powersOf2[i] ) {
        return( 0xFFFFFF );
    } else {
        return( 0x000000 );
    }

} /* getBWPixel */

/*
 * MyGetPixel - returns the color of the pixel at the given coordinates
 *              for the bitmap whose bits are in bits.
 */
COLORREF MyGetPixel( bitmap_bits *bits, int x, int y )
{
    int         index;
    BYTE        *new_bit;

    if( bits->bitcount == 1 ) {
        return( getBWPixel( bits, x, y ) );
    } else if( bits->bitcount == 8 ) {
        new_bit = bits->bits + ((y * bits->width) + x);
        return( colorPalette[*new_bit] );
    } else {
        new_bit = bits->bits + (y * (bits->width / 2)) + (x / 2);
        if( x % 2 != 0 ) {
            index = *new_bit & 0x0F;
            return( colorPalette[index] );
        } else {
            index = *new_bit >> 4;
            return( colorPalette[index] );
        }
    }

} /* MyGetPixel */

/*
 * setBWPixel - sets the pixel for a black and white bitmap.
 */
static void setBWPixel( bitmap_bits *bits, int x, int y, COLORREF color )
{
    short       i;
    BYTE        clr;
    BYTE        *new_bit;

    new_bit = bits->bits + (y * (bits->width / 8)) + x / 8;
    i = x % 8;
    if( color == 0xFFFFFF ) {
        clr = 0xFF;
    } else {
        clr = 0x00;
    }

    *new_bit = (*new_bit & inverseOf2[i]) | (powersOf2[i] & clr);

} /* setBWPixel */

/*
 * MySetPixel - sets the pixel
 */
void MySetPixel( bitmap_bits *bits, int x, int y, COLORREF color )
{
    BYTE        *new_bit;
    unsigned    i;

    if( bits->bitcount == 1 ) {
        setBWPixel( bits, x, y, color );
        return;
    } else if( bits->bitcount == 8 ) {
        new_bit = bits->bits + (y * bits->width) + x;
        for( i = 0; i <= 255; i++ ) {
            if( color == colorPalette[i] ) {
                *new_bit = (BYTE)i;
                break;
            }
        }
    } else {
        new_bit = bits->bits + (y * (bits->width / 2)) + x / 2;
        for( i = 0; i < 16; i++ ) {
            if( color == colorPalette[i] ) {
                if( x % 2 != 0 ) {
                    *new_bit = (*new_bit & 0xF0) | i;
                } else {
                    *new_bit = (*new_bit & 0x0F) | (i << 4);
                }
                break;
            }
        }
    }

} /* MySetPixel */

#ifdef __OS2_PM__

/*
 * setTheBitmapBits - set bits for PM
 */
static void setTheBitmapBits( HBITMAP bitmap, long bcount, BYTE *bits )
{
    _wpi_setbitmapbits( bitmap, bcount, bits );

} /* _wpi_setTheBitmapBits */

#else

/*
 * setTheBitmapBits - set bits for Windows
 */
static void setTheBitmapBits( HBITMAP bitmap, long bcount, BYTE *bits )
{
    BITMAP              bm;
    BITMAPINFO          *bmi;
    BITMAPINFOHEADER    *h;
    HDC                 hdc;
    HDC                 memdc;
    int                 size;
    RGBQUAD             *rgb;
    int                 bc;

    bcount = bcount;
    GetObject( bitmap, sizeof( BITMAP ), &bm );
    bc = bm.bmPlanes * bm.bmBitsPixel;
    size = DIB_INFO_SIZE( bc );
    bmi = MemAlloc( size );

    hdc = GetDC( NULL );
    memdc = CreateCompatibleDC( hdc );
    ReleaseDC( NULL, hdc );

    h = &bmi->bmiHeader;
    h->biSize = sizeof( BITMAPINFOHEADER );
    h->biWidth = bm.bmWidth;
    h->biHeight = bm.bmHeight;
    h->biPlanes = 1;
    h->biBitCount = bc;
    h->biCompression = 0;
    h->biSizeImage = 0;
    h->biXPelsPerMeter = 0;
    h->biYPelsPerMeter = 0;
    h->biClrUsed = 0;
    h->biClrImportant = 0;

    rgb = MemAlloc( RGBQ_SIZE( h->biBitCount ) );
    SetRGBValues( rgb, 1 << h->biBitCount );
    memcpy( bmi->bmiColors, rgb, RGBQ_SIZE( h->biBitCount ) );
    MemFree( rgb );

    GetDIBits( memdc, bitmap, 0, bm.bmHeight, NULL, bmi, DIB_RGB_COLORS );
    if( bmi->bmiHeader.biSizeImage == 0 ) {
        if( bm.bmWidth > 32 && FALSE ) {
            bmi->bmiHeader.biSizeImage = BITS_INTO_BYTES( bm.bmWidth * bc, bm.bmHeight );
        } else {
            bmi->bmiHeader.biSizeImage = BITS_TO_BYTES( bm.bmWidth * bc, bm.bmHeight );
        }
    }
    SetDIBits( memdc, bitmap, 0, bm.bmHeight, bits, bmi, DIB_RGB_COLORS );

    DeleteDC( memdc );
    MemFree( bmi );

} /* _wpi_setTheBitmapBits */

#endif

/*
 * FreeTheBits - frees the bits associated with bits and sets the bitmap
 *               bits if requested
 */
void FreeTheBits( bitmap_bits *info, HBITMAP bitmap, BOOL setbits )
{
    if( setbits ) {
        setTheBitmapBits( bitmap, info->byte_count, info->bits );
//      _wpi_setbitmapbits( bitmap, info->byte_count, info->bits );
    }
    MemFree( info->bits );
    MemFree( info );

} /* FreeTheBits */
