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

static WPI_PRES presWindow = NULL;
static WPI_PRES xorMempres = NULL;
static HDC      xorMemdc;
static WPI_PRES andMempres = NULL;
static HDC      andMemdc;
static HBITMAP  oldXor;
static HBITMAP  oldAnd;

/*
 * BeginFreeHand - Creates the DCs for drawing.  To create a DC with each
 *                 pixel drawn proved to be too slow with free hand drawing
 *                 so these routines were needed.  This is called when a
 *                 mouse button down is detected.
 */
void BeginFreeHand( HWND hwnd )
{
    img_node    *node;

    node = SelectImage( hwnd );
    if (!node) return;

    presWindow = _wpi_getpres( node->viewhwnd );
    xorMempres = _wpi_createcompatiblepres( presWindow, Instance, &xorMemdc );
    andMempres = _wpi_createcompatiblepres( presWindow, Instance, &andMemdc );

    _wpi_torgbmode( xorMempres );
    _wpi_torgbmode( andMempres );
    _wpi_torgbmode( presWindow );
    oldXor = _wpi_selectbitmap( xorMempres, node->hxorbitmap );
    oldAnd = _wpi_selectbitmap( andMempres, node->handbitmap );
} /* BeginFreeHand */

/*
 * DrawThePoints - Actually sets the points on the DCs.
 */
void DrawThePoints( COLORREF colour, COLORREF xorcolour, COLORREF andcolour,
                                                            WPI_POINT *pt )
{
    _wpi_setpixel( xorMempres, pt->x, pt->y, xorcolour );
    _wpi_setpixel( andMempres, pt->x, pt->y, andcolour );
    _wpi_setpixel( presWindow, pt->x + BORDER_WIDTH, pt->y + BORDER_WIDTH,
                                                                colour );
} /* DrawThePoints */

/*
 * BrushThePoints - Using the brush, draw the points on the view window.
 */
void BrushThePoints( COLORREF colour, COLORREF xorcolour, COLORREF andcolour,
                                                WPI_POINT *pt, int brushsize )
{
    HBRUSH      hbrush;
    HBRUSH      oldbrush;

    hbrush = _wpi_createsolidbrush( xorcolour );
    oldbrush = _wpi_selectobject( xorMempres, hbrush );
    _wpi_patblt( xorMempres, pt->x, pt->y, brushsize, brushsize, PATCOPY );
    _wpi_selectobject( xorMempres, oldbrush );
    _wpi_deleteobject( hbrush );

    hbrush = _wpi_createsolidbrush( andcolour );
    oldbrush = _wpi_selectobject( andMempres, hbrush );
    _wpi_patblt( andMempres, pt->x, pt->y, brushsize, brushsize, PATCOPY );
    _wpi_selectobject( andMempres, oldbrush );
    _wpi_deleteobject( hbrush );

    hbrush = _wpi_createsolidbrush( colour );
    oldbrush = _wpi_selectobject( presWindow, hbrush );
    _wpi_patblt( presWindow, BORDER_WIDTH+pt->x, BORDER_WIDTH+pt->y,
                                        brushsize, brushsize, PATCOPY );
    _wpi_selectobject( presWindow, oldbrush );
    _wpi_deleteobject( hbrush );
} /* BrushThePoints */

/*
 * EndFreeHand - Releases the DCs  and the bitmaps.  This is called when a
 *               mouse button up is detected.
 */
void EndFreeHand( HWND hwnd )
{
    img_node    *node;

    node = SelectImage(hwnd);

    _wpi_releasepres( node->viewhwnd, presWindow );
    _wpi_getoldbitmap( xorMempres, oldXor );
    _wpi_getoldbitmap( andMempres, oldAnd );
    _wpi_deletecompatiblepres( xorMempres, xorMemdc );
    _wpi_deletecompatiblepres( andMempres, andMemdc );
    presWindow = (WPI_PRES)NULL;
    xorMempres = (WPI_PRES)NULL;
    andMempres = (WPI_PRES)NULL;
} /* EndFreeHand */

// The purpose of this function is to provide routines that have a
// need to access the 'and' and 'xor' bitmaps while free hand drawing
// is taking place. This is 'cause these bitmaps are selected into the
// presentation spaces of this module and thus cannot be selected into
// any other presentation spaces.
BOOL GetFreeHandPresentationSpaces( WPI_PRES *win, WPI_PRES *and,
                                    WPI_PRES *xor )
{
    if( win ) {
        *win = presWindow;
    }
    if( and ) {
        *and = andMempres;
    }
    if( xor ) {
        *xor = xorMempres;
    }

    return( ( presWindow != (WPI_PRES)NULL ) &&
            ( xorMempres != (WPI_PRES)NULL ) &&
            ( andMempres != (WPI_PRES)NULL ) );
}

