/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2021 The Open Watcom Contributors. All Rights Reserved.
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
 * makeBiggerBitmap - make a bitmap 1 pixel wider and 1 higher than that
 *                    of the given bitmap
 */
static WPI_HBITMAP makeBiggerBitmap( WPI_HBITMAP hbitmap, WPI_POINT *pt )
{
    WPI_PRES    pres;
    WPI_PRES    srcpres;
    HDC         srcdc;
    WPI_PRES    dstpres;
    HDC         dstdc;
    WPI_HBITMAP new_hbitmap;
    WPI_HBITMAP oldsrc_hbitmap;
    WPI_HBITMAP olddst_hbitmap;
    int         bmwidth;
    int         bmheight;
    int         bmplanes;
    int         bmbitspixel;

    _wpi_getbitmapparms( hbitmap, &bmwidth, &bmheight, &bmplanes, NULL, &bmbitspixel );
    new_hbitmap = _wpi_createbitmap( bmwidth + 1, bmheight + 1, bmplanes, bmbitspixel, NULL );
    pres = _wpi_getpres( HWND_DESKTOP );
    srcpres = _wpi_createcompatiblepres( pres, Instance, &srcdc );
    dstpres = _wpi_createcompatiblepres( pres, Instance, &dstdc );
    _wpi_releasepres( HWND_DESKTOP, pres );
    _wpi_torgbmode( srcpres );
    _wpi_torgbmode( dstpres );

    olddst_hbitmap = _wpi_selectbitmap( dstpres, new_hbitmap );
    oldsrc_hbitmap = _wpi_selectbitmap( srcpres, hbitmap );

    if( _wpi_getpixel( srcpres, pt->x, pt->y ) == BLACK ) {
        _wpi_patblt( dstpres, 0, 0, bmwidth + 1, bmheight + 1, WHITENESS );
    } else {
        _wpi_patblt( dstpres, 0, 0, bmwidth + 1, bmheight + 1, BLACKNESS );
    }
    /*
     * put the old bitmap in the top left corner of our new bitmap
     * (bottom left for PM)
     */
    _wpi_bitblt( dstpres, 0, 0, bmwidth, bmheight, srcpres, 0, 0, SRCCOPY );
    _wpi_getoldbitmap( dstpres, olddst_hbitmap );
    _wpi_getoldbitmap( srcpres, oldsrc_hbitmap );
    _wpi_deletecompatiblepres( dstpres, dstdc );
    _wpi_deletecompatiblepres( srcpres, srcdc );

    return( new_hbitmap );

} /* makeBiggerBitmap */

/*
 * getFillCase - there are a number of different possible cases when filling an image
 *             - determine which case we are in
 */
static int getFillCase( fill_info_struct *fillinfo, img_node *node )
{
    WPI_PRES    pres;
    WPI_PRES    andpres;
    HDC         anddc;
    WPI_PRES    xorpres;
    HDC         xordc;
    WPI_HBITMAP old_hbitmap;
    COLORREF    andpixel;
    COLORREF    xorpixel;

    if( fillinfo->imgtype == BITMAP_IMG ) {
        return( NORMAL_FILL );
    }

    pres = _wpi_getpres( HWND_DESKTOP );
    andpres = _wpi_createcompatiblepres( pres, Instance, &anddc );
    _wpi_releasepres( HWND_DESKTOP, pres );
    _wpi_torgbmode( andpres );

    old_hbitmap = _wpi_selectbitmap( andpres, node->and_hbitmap );
    andpixel = _wpi_getpixel( andpres, fillinfo->pt.x, fillinfo->pt.y );
    _wpi_getoldbitmap( andpres, old_hbitmap );
    _wpi_deletecompatiblepres( andpres, anddc );

    if( andpixel == BLACK ) {
        pres = _wpi_getpres( HWND_DESKTOP );
        xorpres = _wpi_createcompatiblepres( pres, Instance, &xordc );
        _wpi_releasepres( HWND_DESKTOP, pres );
        _wpi_torgbmode( xorpres );

        old_hbitmap = _wpi_selectbitmap( xorpres, node->xor_hbitmap );
        xorpixel = _wpi_getpixel( xorpres, fillinfo->pt.x, fillinfo->pt.y );
        _wpi_getoldbitmap( xorpres, old_hbitmap );
        _wpi_deletecompatiblepres( xorpres, xordc );
        if( xorpixel == BLACK || xorpixel == WHITE ) {
            return( DIFFERENT_FILL );
        }
        if( fillinfo->colortype == NORMAL_CLR ) {
            return( NORMAL_FILL );
        } else {
            return( DIFFERENT_FILL );
        }
    } else {
        return( DIFFERENT_FILL );
    }

} /* getFillCase */

/*
 * fillNormal - do a normal fill on the XOR bitmap and leave the AND bitmap alone
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
    WPI_HBITMAP oldxor_hbitmap;
    WPI_HBITMAP old_hbitmap;
    WPI_HBITMAP big_hbitmap;

    pres = _wpi_getpres( HWND_DESKTOP );
    xormempres = _wpi_createcompatiblepres( pres, Instance, &xormemdc );
    dupmempres = _wpi_createcompatiblepres( pres, Instance, &dupmemdc );
    _wpi_releasepres( HWND_DESKTOP, pres );

    /*
     * Because ExtFloodFill seems to screw up on the right side of the
     * bitmap, I create a bitmap 1 bigger on the right, and fill with
     * another color (black or white) so that we can avoid the right
     * side screw up thing.
     */
    big_hbitmap = makeBiggerBitmap( node->xor_hbitmap, &fillinfo->pt );

    _wpi_torgbmode( dupmempres );
    _wpi_torgbmode( xormempres );
    hbrush = _wpi_createsolidbrush( fillinfo->xorcolor );
    oldbrush = _wpi_selectbrush( dupmempres, hbrush );
    old_hbitmap = _wpi_selectbitmap( dupmempres, big_hbitmap );

    _wpi_extfloodfill( dupmempres, fillinfo->pt.x, fillinfo->pt.y,
                       _wpi_getpixel( dupmempres, fillinfo->pt.x, fillinfo->pt.y ),
                       FLOODFILLSURFACE );

    oldxor_hbitmap = _wpi_selectbitmap( xormempres, node->xor_hbitmap );

    _wpi_bitblt( xormempres, 0, 0, node->width, node->height, dupmempres, 0, 0, SRCCOPY );

    _wpi_getoldbrush( dupmempres, oldbrush );
    _wpi_deletebrush( hbrush );
    _wpi_getoldbitmap( xormempres, oldxor_hbitmap );
    _wpi_deletecompatiblepres( xormempres, xormemdc );
    _wpi_getoldbitmap( dupmempres, old_hbitmap );
    _wpi_deletecompatiblepres( dupmempres, dupmemdc );
    _wpi_deletebitmap( big_hbitmap );

} /* fillNormal */

#ifdef __OS2_PM__

/*
 * fillScreenFirst - fill the screen bitmap first and find out which pixels have changed
 *                 - use these pixels to determine which should change in the
 *                   AND and XOR bitmaps
 *                 - PM version
 */
static void fillScreenFirst( fill_info_struct *fillinfo, img_node *node )
{
    WPI_PRES    pres;
    HDC         screendc;
    WPI_PRES    screenpres;
    HBRUSH      oldbrush;
    HBRUSH      hbrush;
    WPI_HBITMAP screen_hbitmap;
    WPI_HBITMAP screendup_hbitmap;
    WPI_HBITMAP oldscreen_hbitmap;
    WPI_HBITMAP big_hbitmap;      // bigger bitmap we actually fill
    COLORREF    fillcolor;
    bool        screenchanged;
    short       x, y;
    bitmap_bits *xorbits;
    bitmap_bits *screenbeforebits;
    bitmap_bits *screenbits;
    bitmap_bits *andbits;

    /*
     * Create a copy of the screen bitmap but make it one bigger so that
     * the fill won't screw up on the right hand side of the bitmap.
     */
    screen_hbitmap = CreateViewBitmap( node );
    big_hbitmap = makeBiggerBitmap( screen_hbitmap, &fillinfo->pt );
    _wpi_deletebitmap( screen_hbitmap );

    /*
     * First fill the screen bitmap with a color we know will force the
     * pixels to change (we know that either black or white will do).  We
     * also fill the XOR bitmap with the color since it may be a dithered
     * color.
     */
    screendup_hbitmap = DuplicateBitmap( big_hbitmap );

    pres = _wpi_getpres( HWND_DESKTOP );
    screenpres = _wpi_createcompatiblepres( pres, Instance, &screendc );
    _wpi_releasepres( HWND_DESKTOP, pres );

    _wpi_torgbmode( screenpres );

    oldscreen_hbitmap = _wpi_selectbitmap( screenpres, big_hbitmap );
    if( _wpi_getpixel( screenpres, fillinfo->pt.x, fillinfo->pt.y ) == BLACK ) {
        fillcolor = WHITE;
    } else {
        fillcolor = BLACK;
    }

    hbrush = _wpi_createsolidbrush( fillcolor );
    oldbrush = _wpi_selectbrush( screenpres, hbrush );
    _wpi_extfloodfill( screenpres, fillinfo->pt.x, fillinfo->pt.y,
                       _wpi_getpixel( screenpres, fillinfo->pt.x, fillinfo->pt.y ),
                       FLOODFILLSURFACE );
    _wpi_getoldbrush( screenpres, oldbrush );
    _wpi_deletebrush( hbrush );
    _wpi_getoldbitmap( screenpres, oldscreen_hbitmap );
    _wpi_deletecompatiblepres( screenpres, screendc );

    xorbits = GetTheBits( node->xor_hbitmap );
    screenbeforebits = GetTheBits( screendup );
    screenbits = GetTheBits( big_hbitmap );
    andbits = GetTheBits( node->and_hbitmap );

    /*
     * Now go through every pixel and see if it changed on the screen.  If
     * it did then it should change on the XOR and AND bitmaps
     */
    for( x = 0; x < node->width; x++ ) {
        for( y = 0; y < node->height; y++ ) {
            screenchanged = (MyGetPixel( screenbits, x, y ) !=
                             MyGetPixel( screenbeforebits, x, y ) );

            if( screenchanged ) {
                MySetPixel( xorbits, x, y, fillinfo->xorcolor );
                MySetPixel( andbits, x, y, fillinfo->andcolor );
            }
        }
    }
    FreeTheBits( andbits, node->and_hbitmap, true );
    FreeTheBits( xorbits, node->xor_hbitmap, true );
    FreeTheBits( screenbeforebits, screendup_hbitmap, false );
    FreeTheBits( screenbits, big_hbitmap, false );

    _wpi_deletebitmap( big_hbitmap );
    _wpi_deletebitmap( screendup_hbitmap );

} /* fillScreenFirst */

#else

/*
 * fillScreenFirst - fill the screen bitmap first and find out which pixels have changed
 *                 - use these pixels to determine which should change in the
 *                   AND and XOR bitmaps
 *                 - Windows version
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
    WPI_HBITMAP oldxor_hbitmap;
    WPI_HBITMAP oldand_hbitmap;
    WPI_HBITMAP xorbefore_hbitmap;
    WPI_HBITMAP oldxorbefore_hbitmap;
    WPI_HBITMAP screen_hbitmap;
    WPI_HBITMAP screendup_hbitmap;
    WPI_HBITMAP oldscreen_hbitmap;
    WPI_HBITMAP oldscreendup_hbitmap;
    WPI_HBITMAP big_hbitmap;      // bigger bitmap we actually fill
    COLORREF    xorcolor;
    COLORREF    fillcolor;
    bool        xorchanged;
    bool        screenchanged;
    short       x, y;

    /*
     * Create a copy of the screen bitmap but make it one bigger so that
     * the fill won't screw up on the right hand side of the bitmap.
     */
    screen_hbitmap = CreateViewBitmap( node );
    big_hbitmap = makeBiggerBitmap( screen_hbitmap, &fillinfo->pt );

    /*
     * First fill the screen bitmap with a color we know will force the
     * pixels to change (we know that either black or white will do).  We
     * also fill the XOR bitmap with the color since it may be a dithered
     * color.
     */
    screendup_hbitmap = DuplicateBitmap( big_hbitmap );

    hdc = GetDC( NULL );
    screendc = CreateCompatibleDC( hdc );
    xordc = CreateCompatibleDC( hdc );
    screendcbefore = CreateCompatibleDC( hdc );
    xordcbefore = CreateCompatibleDC( hdc );
    anddc = CreateCompatibleDC( hdc );
    ReleaseDC( NULL, hdc );

    oldscreen_hbitmap = SelectObject( screendc, big_hbitmap );
    if( GetPixel( screendc, fillinfo->pt.x, fillinfo->pt.y ) == BLACK ) {
        fillcolor = WHITE;
    } else {
        fillcolor = BLACK;
    }
    hbrush = CreateSolidBrush( fillcolor );
    oldbrush = SelectObject( screendc, hbrush );
    ExtFloodFill( screendc, fillinfo->pt.x, fillinfo->pt.y,
                  GetPixel( screendc, fillinfo->pt.x, fillinfo->pt.y ),
                  FLOODFILLSURFACE );
    SelectObject( screendc, oldbrush );
    DeleteObject( hbrush );

    xorbefore_hbitmap = DuplicateBitmap( node->xor_hbitmap );
    oldxor_hbitmap = SelectObject( xordc, node->xor_hbitmap );
    hbrush = CreateSolidBrush( fillinfo->xorcolor );
    oldbrush = SelectObject( xordc, hbrush );
    ExtFloodFill( xordc, fillinfo->pt.x, fillinfo->pt.y,
                  GetPixel( xordc, fillinfo->pt.x, fillinfo->pt.y ),
                  FLOODFILLSURFACE );
    SelectObject( xordc, oldbrush );
    DeleteObject( hbrush );

    oldxorbefore_hbitmap = SelectObject( xordcbefore, xorbefore_hbitmap );
    oldscreendup_hbitmap = SelectObject( screendcbefore, screendup_hbitmap );
    oldand_hbitmap = SelectObject( anddc, node->and_hbitmap );

    /*
     * Now go through every pixel and see if it changed on the screen.  If
     * it did not change, it should not have changed on the XOR bitmap.
     */
    for( x = 0; x < node->width; x++ ) {
        for( y = 0; y < node->height; y++ ) {
            xorcolor = GetPixel( xordcbefore, x, y );

            xorchanged = (GetPixel( xordc, x, y ) != xorcolor);
            screenchanged = (GetPixel( screendc, x, y ) != GetPixel( screendcbefore, x, y ));

            if( !screenchanged ) {
                if( xorchanged ) {
                    SetPixel( xordc, x, y, xorcolor );
                }
            } else {
                SetPixel( anddc, x, y, fillinfo->andcolor );
            }
        }
    }
    SelectObject( screendc, oldscreen_hbitmap );
    DeleteDC( screendc );

    SelectObject( screendcbefore, oldscreendup_hbitmap );
    DeleteDC( screendcbefore );

    SelectObject( xordcbefore, oldxorbefore_hbitmap );
    DeleteDC( xordcbefore );

    SelectObject( anddc, oldand_hbitmap );
    DeleteDC( anddc );

    SelectObject( xordc, oldxor_hbitmap );
    DeleteDC( xordc );

    DeleteObject( xorbefore_hbitmap );
    DeleteObject( screen_hbitmap );
    DeleteObject( big_hbitmap );
    DeleteObject( screendup_hbitmap );

} /* fillScreenFirst */

#endif

/*
 * Fill - handle the filling of the image
 *      - if the image is a bitmap, we simply do a flood fill on the XOR bitmap
 *      - for icons and cursors, we have to consider the AND bitmap and the
 *        potential of screen colors to screw us up
 */
void Fill( fill_info_struct *fillinfo, img_node *node )
{
    int         fillcase;
    HCURSOR     prev_cursor;

    prev_cursor = _wpi_setcursor( _wpi_getsyscursor( IDC_WAIT ) );
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
