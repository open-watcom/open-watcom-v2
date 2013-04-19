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
#include <malloc.h>
#include <math.h>

/*
 * CreateInverseBitmap - This routine creates the bitmap which represents
 *                       how screen colours should be used.  Images in
 *                       PM use a convention of 3 bitmaps instead of 2.
 *                       the AND and XOR bitmaps here refer to the
 *                       windows bitmaps, not the PM ones.
 */
HBITMAP CreateInverseBitmap( HBITMAP andbitmap, HBITMAP xorbitmap )
{
    WPI_PRES    pres;
    WPI_PRES    srcpres;
    HDC         srcdc;
    WPI_PRES    destpres;
    HDC         destdc;
    HBITMAP     new_bitmap;
    IMAGEBUNDLE p;
    int         width;
    int         height;
    HBITMAP     oldsrc;
    HBITMAP     olddest;

    pres = _wpi_getpres( HWND_DESKTOP );
    srcpres = _wpi_createcompatiblepres( pres, Instance, &srcdc );
    destpres = _wpi_createcompatiblepres( pres, Instance, &destdc );
    _wpi_releasepres( HWND_DESKTOP, pres );
    _wpi_torgbmode( srcpres );
    _wpi_torgbmode( destpres );

    p.lBackColor = WHITE;
    p.lColor = BLACK;
    GpiSetAttrs(srcpres, PRIM_IMAGE, IBB_COLOR | IBB_BACK_COLOR, 0, &p);
    GpiSetAttrs(destpres, PRIM_IMAGE, IBB_COLOR | IBB_BACK_COLOR, 0, &p);

    _wpi_getbitmapdim( xorbitmap, &width, &height );
    new_bitmap = _wpi_createbitmap( width, height, 1, 1, NULL );

    oldsrc = _wpi_selectobject( srcpres, xorbitmap );
    olddest = _wpi_selectobject( destpres, new_bitmap );

    _wpi_bitblt( destpres, 0, 0, width, height, srcpres, 0, 0, SRCCOPY );
    _wpi_selectobject( srcpres, oldsrc );
    oldsrc = _wpi_selectobject( srcpres, andbitmap );

    _wpi_bitblt( destpres, 0, 0, width, height, srcpres, 0, 0, SRCAND );
    _wpi_selectobject( srcpres, oldsrc );
    _wpi_selectobject( destpres, olddest );

    _wpi_deletecompatiblepres( srcpres, srcdc );
    _wpi_deletecompatiblepres( destpres, destdc );

    return( new_bitmap );
} /* CreateInverseBitmap */

/*
 * CreateColourBitmap - This routine creates the colour bitmap mask that
 *                      is saved for icons and cursors.  It's different
 *                      than the xor mask that i store since it has 0's
 *                      anywhere where the AND bitmap has 1's.
 */
HBITMAP CreateColourBitmap( HBITMAP andbitmap, HBITMAP xorbitmap )
{
    WPI_PRES    pres;
    WPI_PRES    srcpres;
    HDC         srcdc;
    WPI_PRES    destpres;
    HDC         destdc;
    HBITMAP     new_bitmap;
    IMAGEBUNDLE p;
    int         width;
    int         height;
    int         planes;
    int         bitspixel;
    int         notused;
    HBITMAP     oldsrc;
    HBITMAP     olddest;

#if __OS2_PM__
    notused = 0;
#endif

    pres = _wpi_getpres( HWND_DESKTOP );
    srcpres = _wpi_createcompatiblepres( pres, Instance, &srcdc );
    destpres = _wpi_createcompatiblepres( pres, Instance, &destdc );
    _wpi_releasepres( HWND_DESKTOP, pres );
    _wpi_torgbmode( srcpres );
    _wpi_torgbmode( destpres );

    p.lBackColor = RGB_WHITE;
    p.lColor = RGB_BLACK;
    GpiSetAttrs(destpres, PRIM_IMAGE, IBB_COLOR | IBB_BACK_COLOR, 0, &p);

    _wpi_getbitmapparms(xorbitmap, &width, &height, &planes, NULL, &bitspixel);
    new_bitmap = _wpi_createbitmap( width, height, planes, bitspixel, NULL );
    oldsrc = _wpi_selectobject( srcpres, andbitmap );
    olddest = _wpi_selectobject( destpres, new_bitmap );

    _wpi_bitblt( destpres, 0, 0, width, height, srcpres, 0, 0, SRCCOPY );
    _wpi_selectobject( srcpres, oldsrc );
    oldsrc = _wpi_selectobject( srcpres, xorbitmap );

    _wpi_bitblt( destpres, 0, 0, width, height, srcpres, 0, 0, SRCAND );
    _wpi_selectobject( srcpres, oldsrc );
    _wpi_selectobject( destpres, olddest );

    _wpi_deletecompatiblepres( srcpres, srcdc );
    _wpi_deletecompatiblepres( destpres, destdc );

    return( new_bitmap );
} /* CreateColourBitmap */

/*
 * InitXorAndBitmaps - initializes the XOR and AND bitmaps
 */
void InitXorAndBitmaps( img_node *node )
{
    WPI_PRES            pres;
    WPI_PRES            mempres;
    HDC                 hdc;
    HBITMAP             oldbitmap;
    BITMAPINFOHEADER2   bmih;

    memset( &bmih, 0, sizeof(BITMAPINFOHEADER2) );
    bmih.cbFix = sizeof( BITMAPINFOHEADER2 );
    bmih.cx = node->width;
    bmih.cy = node->height;
    bmih.cPlanes = (USHORT)1;
    bmih.cBitCount = (USHORT)node->bitcount;

    pres = _wpi_getpres( HWND_DESKTOP );
    node->hxorbitmap = _wpi_createbitmap( node->width, node->height, 1,
                                        node->bitcount, NULL );
    mempres = _wpi_createcompatiblepres( pres, Instance, &hdc );

    _wpi_torgbmode( mempres );

    oldbitmap = _wpi_selectobject( mempres, node->hxorbitmap );
    _wpi_patblt( mempres, 0, 0, node->width, node->height, WHITENESS );
    _wpi_selectobject( mempres, oldbitmap );

    bmih.cBitCount = (USHORT)1;
    node->handbitmap = _wpi_createbitmap( node->width, node->height, 1, 1, NULL );
    _wpi_releasepres( HWND_DESKTOP, pres );

    oldbitmap = _wpi_selectobject( mempres, node->handbitmap );
    _wpi_patblt( mempres, 0, 0, node->width, node->height, BLACKNESS );
    _wpi_selectobject( mempres, oldbitmap );

    _wpi_deletecompatiblepres( mempres, hdc );
} /* InitXorAndBitmaps */

/*
 * MakeWPIBitmap - Converts an HBITMAP to a WPI_HANDLE
 */
WPI_HANDLE MakeWPIBitmap( HBITMAP hbitmap )
{
    WPI_OBJECT      *obj;

    obj = MemAlloc( sizeof(WPI_OBJECT) );
    obj->type = WPI_BITMAP_OBJ;
    obj->bitmap = hbitmap;

    return( (WPI_HANDLE)obj );
} /* MakeWPIBitmap */
