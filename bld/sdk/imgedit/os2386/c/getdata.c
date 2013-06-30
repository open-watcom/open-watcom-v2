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
#include "..\h\wbitmap.h"

/*
 * GetBitmapInfoHeader - Gets the bitmap info header structure from the
 *                       bitmap info structure.
 */
void GetBitmapInfoHeader( WPI_BITMAPINFOHEADER *bmih, img_node *node )
{
    memset( bmih, 0, sizeof(WPI_BITMAPINFOHEADER) );

    bmih->cbFix = sizeof( WPI_BITMAPINFOHEADER );
    bmih->cx = node->width;
    bmih->cy = node->height;
    bmih->cPlanes = 1;
    bmih->cBitCount = node->bitcount;
    bmih->cbImage = BITS_TO_BYTES( node->bitcount * node->width,
                                                        node->height );
    bmih->cclrUsed = 1<<(node->bitcount);
} /* GetBitmapInfoHeader */

/*
 * GetXorBitmapInfo - Returns a pointer to a bitmap info structure ... memory
 *                   should be freed with FreeDIBitmapInfo. this is for
 *                   the xor part.
 */
BITMAPINFO2 *GetXorBitmapInfo( img_node *node )
{
    long                        size;
    WPI_BITMAPINFO              *bmi;
    WPI_BITMAPINFOHEADER        bmih;
    WPI_PRES                    pres;
    WPI_PRES                    mempres;
    HDC                         memdc;
    HBITMAP                     oldbitmap;

    pres = _wpi_getpres( HWND_DESKTOP );
    mempres = _wpi_createcompatiblepres( pres, Instance, &memdc );
    _wpi_releasepres( HWND_DESKTOP, pres );

    size = BMPINFO2_SIZE( node->bitcount );
    bmi = MemAlloc( size );

    GetBitmapInfoHeader( &bmih, node );
    memcpy( bmi, &bmih, sizeof(WPI_BITMAPINFOHEADER) );
    oldbitmap = _wpi_selectobject( mempres, node->hxorbitmap );

    GpiQueryBitmapBits( mempres, 0, node->height, NULL, bmi );

    _wpi_selectobject( mempres, oldbitmap );
    _wpi_deletecompatiblepres( mempres, memdc );
    return( bmi );
} /* GetXorBitmapInfo */

/*
 * GetAndBitmapInfo - Returns a pointer to a bitmap info structure ... memory
 *                   should be freed with FreeDIBitmapInfo. this is for
 *                   the and part.
 */
BITMAPINFO2 *GetAndBitmapInfo( img_node *node )
{
    long                        size;
    WPI_BITMAPINFO              *bmi;
    WPI_BITMAPINFOHEADER        bmih;
    WPI_PRES                    pres;
    WPI_PRES                    mempres;
    HDC                         memdc;
    HBITMAP                     oldbitmap;

    pres = _wpi_getpres( HWND_DESKTOP );
    mempres = _wpi_createcompatiblepres( pres, Instance, &memdc );
    _wpi_releasepres( HWND_DESKTOP, pres );

    size = BMPINFO2_SIZE( 1 );
    bmi = MemAlloc( size );

    GetBitmapInfoHeader( &bmih, node );
    // Adjustments for the and mask
    bmih.cBitCount = 1;
    bmih.cbImage = BITS_TO_BYTES( node->bitcount, node->height );
    bmih.cclrUsed = 2;
    memcpy( bmi, &bmih, sizeof(WPI_BITMAPINFOHEADER) );

    oldbitmap = _wpi_selectobject( mempres, node->handbitmap );
    GpiQueryBitmapBits( mempres, 0, node->height, NULL, bmi );
    _wpi_selectobject( mempres, oldbitmap );
    _wpi_deletecompatiblepres( mempres, memdc );

    bmi->cbImage = BITS_TO_BYTES( node->bitcount, 2*node->height );
    bmi->cy = node->height * 2;
    return( bmi );
} /* GetAndBitmapInfo */

/*
 * FreeDIBitmapInfo - Frees the memory allocated by GetDIBitmapInfo
 */
void FreeDIBitmapInfo( BITMAPINFO2 *bmi )
{
    free (bmi);
} /* FreeDIBitmapInfo */

