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


#include "precomp.h"
#include <commdlg.h>
#include <stdio.h>
#include <string.h>
#include <dos.h>
#include <stdlib.h>
#include <malloc.h>
#include "imgedit.h"
#include "iemem.h"

/*
 * getXorBits - retrieve the bits for the XOR bitmap (only for icons and cursors)
 */
static BOOL getXorBits( BITMAPINFO *bmi, BYTE *bits, img_node *node )
{
    HDC         hdc;
    HDC         memdc;

    hdc = GetDC( node->viewhwnd );
    memdc = CreateCompatibleDC( hdc );
    ReleaseDC( node->viewhwnd, hdc );

    GetDIBits( memdc, node->hxorbitmap, 0, node->height, NULL, bmi, DIB_RGB_COLORS );
    if( bmi->bmiHeader.biSizeImage == 0 ) {
        if( node->width > 32 && FALSE ) {
            bmi->bmiHeader.biSizeImage = BITS_INTO_BYTES( node->width * node->bitcount,
                                                          node->height );
        } else {
            bmi->bmiHeader.biSizeImage = BITS_TO_BYTES( node->width*node->bitcount,
                                                        node->height );
        }
    }
    GetDIBits( memdc, node->hxorbitmap, 0, node->height, bits, bmi, DIB_RGB_COLORS );
    DeleteDC( memdc );
    return( TRUE );

} /* getXorBits */

/*
 * getBitmapInfo - retrieve the bitmap information structure
 */
static void getBitmapInfo( BITMAPINFO *bmi, img_node *node )
{
    BITMAPINFOHEADER    bmih;
    RGBQUAD             *aRgbq;

    GetBitmapInfoHeader( &bmih, node );

    memcpy( &bmi->bmiHeader, &bmih, sizeof( BITMAPINFOHEADER ) );

    if( node->bitcount < 9 ) {
        aRgbq = MemAlloc( RGBQ_SIZE( node->bitcount ) );
        SetRGBValues( aRgbq, 1 << node->bitcount );
        memcpy( bmi->bmiColors, aRgbq, RGBQ_SIZE( node->bitcount ) );
        MemFree( aRgbq );
    }

} /* getBitmapInfo */

/*
 * getAndBits - retrieve the bits for the AND bitmap
 */
static void getAndBits( BYTE *bits, img_node *node )
{
    HDC                 hdc;
    HDC                 memdc;
    HBITMAP             oldbitmap;
    BITMAPINFO          *bmi;
    RGBQUAD             *rgbq;
    BITMAPINFOHEADER    *h;

    hdc = GetDC( node->viewhwnd );
    memdc = CreateCompatibleDC( hdc );
    ReleaseDC( node->viewhwnd, hdc );

    bmi = MemAlloc( DIB_INFO_SIZE( 1 ) );

    rgbq = MemAlloc( RGBQ_SIZE( 1 ) );
    SetRGBValues( rgbq, 2 );
    memcpy( bmi->bmiColors, rgbq, RGBQ_SIZE( 1 ) );
    MemFree( rgbq );

    h = &bmi->bmiHeader;

    h->biSize = sizeof( BITMAPINFOHEADER );
    h->biWidth = node->width;
    h->biHeight = node->height;
    h->biPlanes = 1;
    h->biBitCount = 1;
    h->biCompression = BI_RGB;
    h->biSizeImage = 0;
    h->biXPelsPerMeter = 0;
    h->biYPelsPerMeter = 0;
    h->biClrUsed = 0;
    h->biClrImportant = 0;

    oldbitmap = SelectObject( memdc, node->handbitmap );
    GetDIBits( memdc, node->handbitmap, 0, node->height, bits, bmi, DIB_RGB_COLORS );
    SelectObject( memdc, oldbitmap );
    DeleteDC( memdc );

} /* getAndBits */

/*
 * GetBitmapInfoHeader - gets the bitmap information header structure from the
 *                       bitmap information structure
 */
void GetBitmapInfoHeader( BITMAPINFOHEADER *bmih, img_node *node )
{
    bmih->biSize = sizeof( BITMAPINFOHEADER );
    bmih->biWidth = node->width;
    bmih->biHeight = node->height;
    bmih->biPlanes = 1;
    bmih->biBitCount = node->bitcount;
    bmih->biCompression = 0;
//    bmih->biSizeImage = BITS_TO_BYTES( node->bitcount * node->width,
//                                       node->height );
    bmih->biSizeImage = 0;
    bmih->biXPelsPerMeter = 0;
    bmih->biYPelsPerMeter = 0;
    bmih->biClrUsed = 0;
    bmih->biClrImportant = 0;

} /* GetBitmapInfoHeader */

/*
 * FillImageResource - fill the "an_img_resource" structure from the image
 *                     information structure
 */
void FillImageResource( an_img_resource *img_res, img_node *node )
{
    img_res->width = node->width;
    img_res->height = node->height;
    img_res->color_count = 1 << node->bitcount;
    img_res->reserved = 0;
    img_res->xhotspot = node->hotspot.x;
    img_res->yhotspot = node->hotspot.y;

} /* FillImageResource */

/*
 * GetImageData - get icon/cursor information for saving the icon/cursor
 */
void GetImageData( an_img *img, img_node *node )
{
    RGBQUAD             *aRgbq;
    BITMAPINFOHEADER    *h;

    h = &img->bm->bmiHeader;

    if( h->biBitCount < 9 ) {
        aRgbq = MemAlloc( RGBQ_SIZE( h->biBitCount ) );
        SetRGBValues( aRgbq, 1 << h->biBitCount );
        memcpy( img->bm->bmiColors, aRgbq, RGBQ_SIZE( h->biBitCount ) );
        MemFree( aRgbq );
    }

    getXorBits( img->bm, img->xor_mask, node );

    /*
     * Get the AND bitmap information ...
     */
    getAndBits( img->and_mask, node );

} /* GetImageData */

/*
 * GetDIBitmapInfo - return a pointer to a bitmap information structure
 *                 - memory should be freed with FreeDIBitmapInfo
 */
BITMAPINFO *GetDIBitmapInfo( img_node *node )
{
    long        size;
    BITMAPINFO  *bmi;

    size = DIB_INFO_SIZE( node->bitcount );
    bmi = MemAlloc( size );
    getBitmapInfo( bmi, node );
    return( bmi );

} /* GetDIBitmapInfo */

/*
 * FreeDIBitmapInfo - free the memory allocated by GetDIBitmapInfo
 */
void FreeDIBitmapInfo( BITMAPINFO *bmi )
{
    MemFree( bmi );

} /* FreeDIBitmapInfo */
