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

/*
 * makeBiggerBitmap - makes a bitmap 1 pixel wider and 1 higher than that
 *                    of the given bitmap
 */
static HBITMAP makeBiggerBitmap( HBITMAP hbitmap, WPI_POINT *pt )
{
    WPI_PRES    pres;
    WPI_PRES    srcpres;
    HDC         srcdc;
    WPI_PRES    destpres;
    HDC         destdc;
    HBITMAP     newbitmap;
    HBITMAP     oldsrcbmp;
    HBITMAP     olddestbmp;
    int         bmwidth;
    int         bmheight;
    int         bmplanes;
    int         bmbitspixel;

    _wpi_getbitmapparms( hbitmap, &bmwidth, &bmheight, &bmplanes, NULL,
                                                        &bmbitspixel );
    newbitmap = _wpi_createbitmap(bmwidth+1, bmheight+1, bmplanes, bmbitspixel,
                                                                        NULL );
    pres = _wpi_getpres( HWND_DESKTOP );
    srcpres = _wpi_createcompatiblepres( pres, Instance, &srcdc );
    destpres = _wpi_createcompatiblepres( pres, Instance, &destdc );
    _wpi_releasepres( HWND_DESKTOP, pres );
    _wpi_torgbmode( srcpres );
    _wpi_torgbmode( destpres );

    olddestbmp = _wpi_selectobject( destpres, newbitmap );
    oldsrcbmp = _wpi_selectobject( srcpres, hbitmap );

    if ( _wpi_getpixel(srcpres, pt->x, pt->y) == BLACK ) {
        _wpi_patblt( destpres, 0, 0, bmwidth+1, bmheight+1, WHITENESS );
    } else {
        _wpi_patblt( destpres, 0, 0, bmwidth+1, bmheight+1, BLACKNESS );
    }
    /*
     * put the old bitmap in the top left corner of our new bitmap
     * (bottom left for PM)
     */
    _wpi_bitblt( destpres, 0, 0, bmwidth, bmheight, srcpres, 0, 0, SRCCOPY );
    _wpi_selectobject( destpres, olddestbmp );
    _wpi_selectobject( srcpres, oldsrcbmp );
    _wpi_deletecompatiblepres( destpres, destdc );
    _wpi_deletecompatiblepres( srcpres, srcdc );

    return( newbitmap );
} /* makeBiggerBitmap */

/*
 * getFillCase - There are a number of different possible cases when filling
 *               an image.  This routine determines which case we are in.
 */
static int getFillCase( fill_info_struct *fillinfo, img_node *node )
{
    WPI_PRES    pres;
    WPI_PRES    andpres;
    HDC         anddc;
    WPI_PRES    xorpres;
    HDC         xordc;
    HBITMAP     oldbitmap;
    COLORREF    andpixel;
    COLORREF    xorpixel;

    if (fillinfo->img_type == BITMAP_IMG) {
        return( NORMAL_FILL );
    }

    pres = _wpi_getpres( HWND_DESKTOP );
    andpres = _wpi_createcompatiblepres( pres, Instance, &anddc );
    _wpi_releasepres( HWND_DESKTOP, pres );
    _wpi_torgbmode( andpres );

    oldbitmap = _wpi_selectobject( andpres, node->handbitmap );
    andpixel = _wpi_getpixel( andpres, fillinfo->pt.x, fillinfo->pt.y );
    _wpi_selectobject( andpres, oldbitmap );
    _wpi_deletecompatiblepres( andpres, anddc );

    if (andpixel == BLACK) {
        pres = _wpi_getpres( HWND_DESKTOP );
        xorpres = _wpi_createcompatiblepres( pres, Instance, &xordc );
        _wpi_releasepres( HWND_DESKTOP, pres );
        _wpi_torgbmode( xorpres );

        oldbitmap = _wpi_selectobject( xorpres, node->hxorbitmap );
        xorpixel = _wpi_getpixel( xorpres, fillinfo->pt.x, fillinfo->pt.y );
        _wpi_selectobject( xorpres, oldbitmap );
        _wpi_deletecompatiblepres( xorpres, xordc );
        if (xorpixel == BLACK || xorpixel == WHITE) {
            return( DIFFERENT_FILL );
        }
        if ( fillinfo->colourtype == NORMAL_CLR ) {
            return( NORMAL_FILL );
        } else {
            return (DIFFERENT_FILL);
        }
    } else {
        return( DIFFERENT_FILL );
    }
} /* getFillCase */

/*
 * fillNormal - This routine just does a normal fill on the xorbitmap and
 *              leaves the and bitmap alone.
 */
static void fillNormal( fill_info_struct *fillinfo, img_node *node )
{
    WPI_PRES    pres;
    WPI_PRES    xormempres;
    HDC         xormemdc;
    WPI_PRES    dupmempres;
    HDC         dupmemdc;
    HBRUSH      oldbrush;
    HBRUSH      hbrush;
    HBITMAP     oldxorbmp;
    HBITMAP     oldbitmap;
    HBITMAP     bigbitmap;

    pres = _wpi_getpres( HWND_DESKTOP );
    xormempres = _wpi_createcompatiblepres( pres, Instance, &xormemdc );
    dupmempres = _wpi_createcompatiblepres( pres, Instance, &dupmemdc );
    _wpi_releasepres( HWND_DESKTOP, pres );

    /*
     * Because ExtFloodFill seems to screw up on the right side of the
     * bitmap, I create a bitmap 1 bigger on the right, and fill with
     * another colour (black or white) so that we can avoid the right
     * side screw up thing.
     */
    bigbitmap = makeBiggerBitmap( node->hxorbitmap, &(fillinfo->pt) );

    _wpi_torgbmode( dupmempres );
    _wpi_torgbmode( xormempres );
    hbrush = _wpi_createsolidbrush( fillinfo->xorcolour );
    oldbrush = _wpi_selectobject( dupmempres, hbrush );
    oldbitmap = _wpi_selectobject( dupmempres, bigbitmap );

    _wpi_extfloodfill( dupmempres, fillinfo->pt.x, fillinfo->pt.y,
                        _wpi_getpixel(dupmempres, fillinfo->pt.x,
                        fillinfo->pt.y), FLOODFILLSURFACE );

    oldxorbmp = _wpi_selectobject( xormempres, node->hxorbitmap );

    _wpi_bitblt( xormempres, 0, 0, node->width, node->height, dupmempres,
                                                            0, 0, SRCCOPY );

    _wpi_selectobject( dupmempres, oldbrush );
    _wpi_deleteobject( hbrush );
    _wpi_selectobject( xormempres, oldxorbmp );
    _wpi_deletecompatiblepres( xormempres, xormemdc );
    _wpi_selectobject( dupmempres, oldbitmap );
    _wpi_deletecompatiblepres( dupmempres, dupmemdc );
    _wpi_deletebitmap( bigbitmap );
} /* fillNormal */

#ifdef __OS2_PM__
/*
 * fillScreenFirst - We fill the screen bitmap first and find out which
 *                   pixels have changed.  We use these pixels to determine
 *                   which should change in the AND and XOR bitmaps.
 *                   PM version
 */
static void fillScreenFirst( fill_info_struct *fillinfo, img_node *node )
{
    WPI_PRES    pres;
    HDC         screendc;
    WPI_PRES    screenpres;
    HBRUSH      oldbrush;
    HBRUSH      hbrush;
    HBITMAP     screenbitmap;
    HBITMAP     screendup;
    HBITMAP     oldscreen;
    HBITMAP     bigbitmap;      // bigger bitmap we actually fill
    COLORREF    fillcolour;
    BOOL        screenchanged;
    short       x,y;
    bitmap_bits *xorbits;
    bitmap_bits *screenbeforebits;
    bitmap_bits *screenbits;
    bitmap_bits *andbits;

    /*
     * create a copy of the screen bitmap but make it one bigger so that
     * the fill won't screw up on the right hand side of the bitmap.
     */
    screenbitmap = CreateViewBitmap( node );
    bigbitmap = makeBiggerBitmap( screenbitmap, &(fillinfo->pt) );
    _wpi_deletebitmap( screenbitmap );

    /*
     * First fill the screen bitmap with a colour we know will force the
     * pixels to change (we know that either black or white will do).  We
     * also fill the xor bitmap with the colour since it may be a dithered
     * colour.
     */
    screendup = DuplicateBitmap( bigbitmap );

    pres = _wpi_getpres( HWND_DESKTOP );
    screenpres = _wpi_createcompatiblepres( pres, Instance, &screendc );
    _wpi_releasepres( HWND_DESKTOP, pres );

    _wpi_torgbmode( screenpres );

    oldscreen = _wpi_selectobject( screenpres, bigbitmap );
    if ( _wpi_getpixel(screenpres, fillinfo->pt.x, fillinfo->pt.y) == BLACK ) {
        fillcolour = WHITE;
    } else {
        fillcolour = BLACK;
    }

    hbrush = _wpi_createsolidbrush( fillcolour );
    oldbrush = _wpi_selectobject( screenpres, hbrush );
    _wpi_extfloodfill( screenpres, fillinfo->pt.x, fillinfo->pt.y,
                        _wpi_getpixel(screenpres, fillinfo->pt.x,
                        fillinfo->pt.y), FLOODFILLSURFACE );
    _wpi_selectobject( screenpres, oldbrush );
    _wpi_deleteobject( hbrush );
    _wpi_selectobject( screenpres, oldscreen );
    _wpi_deletecompatiblepres( screenpres, screendc );

    xorbits = GetTheBits( node->hxorbitmap );
    screenbeforebits = GetTheBits( screendup );
    screenbits = GetTheBits( bigbitmap );
    andbits = GetTheBits( node->handbitmap );

    /*
     * Now go through every pixel and see if it changed on the screen.  If
     * it did then it should change on the XOR and AND bitmaps
     */
    for (x=0; x < node->width; ++x) {
        for (y=0; y < node->height; ++y) {
            screenchanged = (MyGetPixel(screenbits, x, y) !=
                                        MyGetPixel(screenbeforebits, x, y));

            if (screenchanged) {
                MySetPixel( xorbits, x, y, fillinfo->xorcolour );
                MySetPixel( andbits, x, y, fillinfo->andcolour );
            }
        }
    }
    FreeTheBits( andbits, node->handbitmap, TRUE );
    FreeTheBits( xorbits, node->hxorbitmap, TRUE );
    FreeTheBits( screenbeforebits, screendup, FALSE );
    FreeTheBits( screenbits, bigbitmap, FALSE );

    _wpi_deletebitmap( bigbitmap );
    _wpi_deletebitmap( screendup );
} /* fillScreenFirst */

#else

/*
 * fillScreenFirst - We fill the screen bitmap first and find out which
 *                   pixels have changed.  We use these pixels to determine
 *                   which should change in the AND and XOR bitmaps.
 *                   Windows version
 */
static void fillScreenFirst( fill_info_struct *fillinfo, img_node *node )
{
    HDC         hdc;
    HDC         xordc;
    HDC         xordcbefore;
    HDC         screendc;
    HDC         screendcbefore;
    HDC         anddc;
    HBRUSH      oldbrush;
    HBRUSH      hbrush;
    HBITMAP     oldxorbmp;
    HBITMAP     oldandbmp;
    HBITMAP     xorbmpbefore;
    HBITMAP     oldxorbmpbefore;
    HBITMAP     screenbitmap;
    HBITMAP     screendup;
    HBITMAP     oldscreen;
    HBITMAP     oldscreendup;
    HBITMAP     bigbitmap;      // bigger bitmap we actually fill
    COLORREF    xorcolour;
    COLORREF    fillcolour;
    BOOL        xorchanged;
    BOOL        screenchanged;
    short       x,y;

    /*
     * create a copy of the screen bitmap but make it one bigger so that
     * the fill won't screw up on the right hand side of the bitmap.
     */
    screenbitmap = CreateViewBitmap( node );
    bigbitmap = makeBiggerBitmap( screenbitmap, &(fillinfo->pt) );

    /*
     * First fill the screen bitmap with a colour we know will force the
     * pixels to change (we know that either black or white will do).  We
     * also fill the xor bitmap with the colour since it may be a dithered
     * colour.
     */
    screendup = DuplicateBitmap( bigbitmap );

    hdc = GetDC( NULL );
    screendc = CreateCompatibleDC( hdc );
    xordc = CreateCompatibleDC( hdc );
    screendcbefore = CreateCompatibleDC( hdc );
    xordcbefore = CreateCompatibleDC( hdc );
    anddc = CreateCompatibleDC( hdc );
    ReleaseDC( NULL, hdc );

    oldscreen = SelectObject( screendc, bigbitmap );
    if (GetPixel(screendc, fillinfo->pt.x, fillinfo->pt.y) == BLACK) {
        fillcolour = WHITE;
    } else {
        fillcolour = BLACK;
    }
    hbrush = CreateSolidBrush( fillcolour );
    oldbrush = SelectObject( screendc, hbrush );
    ExtFloodFill( screendc, fillinfo->pt.x, fillinfo->pt.y,
                        GetPixel(screendc, fillinfo->pt.x, fillinfo->pt.y),
                        FLOODFILLSURFACE );
    SelectObject( screendc, oldbrush );
    DeleteObject( hbrush );

    xorbmpbefore = DuplicateBitmap( node->hxorbitmap );
    oldxorbmp = SelectObject( xordc, node->hxorbitmap );
    hbrush = CreateSolidBrush( fillinfo->xorcolour );
    oldbrush = SelectObject( xordc, hbrush );
    ExtFloodFill( xordc, fillinfo->pt.x, fillinfo->pt.y,
                        GetPixel(xordc, fillinfo->pt.x, fillinfo->pt.y),
                        FLOODFILLSURFACE );
    SelectObject( xordc, oldbrush );
    DeleteObject( hbrush );

    oldxorbmpbefore = SelectObject( xordcbefore, xorbmpbefore );
    oldscreendup = SelectObject( screendcbefore, screendup );
    oldandbmp = SelectObject( anddc, node->handbitmap );

    /*
     * Now go through every pixel and see if it changed on the screen.  If
     * it did not change, it should not have changed on the xor bitmap.
     */
    for (x=0; x < node->width; ++x) {
        for (y=0; y < node->height; ++y) {
            xorcolour = GetPixel(xordcbefore, x, y);

            xorchanged = (GetPixel(xordc, x, y) != xorcolour);
            screenchanged = (GetPixel(screendc, x, y) != GetPixel(screendcbefore, x, y));

            if (!screenchanged) {
                if (xorchanged) {
                    SetPixel( xordc, x, y, xorcolour );
                }
            } else {
                SetPixel( anddc, x, y, fillinfo->andcolour );
            }
        }
    }
    SelectObject( screendc, oldscreen);
    DeleteDC( screendc );

    SelectObject( screendcbefore, oldscreendup);
    DeleteDC( screendcbefore );

    SelectObject( xordcbefore, oldxorbmpbefore );
    DeleteDC( xordcbefore );

    SelectObject( anddc, oldandbmp );
    DeleteDC( anddc );

    SelectObject( xordc, oldxorbmp );
    DeleteDC( xordc );

    DeleteObject( xorbmpbefore );
    DeleteObject( screenbitmap );
    DeleteObject( bigbitmap );
    DeleteObject( screendup );

} /* fillScreenFirst */

#endif

/*
 * Fill - This routine handles the filling of the image.  If the image is
 * a bitmap, we simply do a flood fill on the xorbitmap.  For icons and
 * cursors we have to consider the AND bitmap and the potential of screen
 * colours to screw us up.
 */
void Fill( fill_info_struct *fillinfo, img_node *node )
{
    int         fillcase;
    HCURSOR     prev_cursor;

    prev_cursor = _wpi_setcursor( _wpi_getsyscursor(IDC_WAIT) );
    fillcase = getFillCase( fillinfo, node );

    switch( fillcase ) {
    case NORMAL_FILL:
        fillNormal( fillinfo, node );
        break;

    case DIFFERENT_FILL:
        fillScreenFirst( fillinfo, node );
        break;

    default:
        WImgEditError( WIE_ERR_BAD_FILLCASE, WIE_INTERNAL_003 );
        break;
    }
    _wpi_setcursor( prev_cursor );

} /* Fill */

