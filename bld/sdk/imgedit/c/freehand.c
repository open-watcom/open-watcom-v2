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

static WPI_PRES     presWindow = NULL;
static WPI_PRES     xorMempres = NULL;
static HDC          xorMemdc;
static WPI_PRES     andMempres = NULL;
static HDC          andMemdc;
static WPI_HBITMAP  oldxor_hbitmap;
static WPI_HBITMAP  oldand_hbitmap;

/*
 * BeginFreeHand - creates the device contexts for drawing
 *               - to create a DC with each pixel drawn proved to be too slow with
 *                 free hand drawing, so these routines were needed
 *               - this is called when a mouse button down is detected
 */
void BeginFreeHand( HWND hwnd )
{
    img_node    *node;

    node = SelectImage( hwnd );
    if( node == NULL ) {
        return;
    }

    presWindow = _wpi_getpres( node->viewhwnd );
    xorMempres = _wpi_createcompatiblepres( presWindow, Instance, &xorMemdc );
    andMempres = _wpi_createcompatiblepres( presWindow, Instance, &andMemdc );

    _wpi_torgbmode( xorMempres );
    _wpi_torgbmode( andMempres );
    _wpi_torgbmode( presWindow );
    oldxor_hbitmap = _wpi_selectbitmap( xorMempres, node->xor_hbitmap );
    oldand_hbitmap = _wpi_selectbitmap( andMempres, node->and_hbitmap );

} /* BeginFreeHand */

/*
 * DrawThePoints - actually set the points on the device contexts
 */
void DrawThePoints( COLORREF color, COLORREF xorcolor, COLORREF andcolor, WPI_POINT *pt )
{
    _wpi_setpixel( xorMempres, pt->x, pt->y, xorcolor );
    _wpi_setpixel( andMempres, pt->x, pt->y, andcolor );
    _wpi_setpixel( presWindow, pt->x + BORDER_WIDTH, pt->y + BORDER_WIDTH, color );

} /* DrawThePoints */

/*
 * BrushThePoints - using the brush, draw the points on the view window
 */
void BrushThePoints( COLORREF color, COLORREF xorcolor, COLORREF andcolor,
                     WPI_POINT *pt, int brushsize )
{
    HBRUSH      hbrush;
    HBRUSH      oldbrush;

    hbrush = _wpi_createsolidbrush( xorcolor );
    oldbrush = _wpi_selectbrush( xorMempres, hbrush );
    _wpi_patblt( xorMempres, pt->x, pt->y, brushsize, brushsize, PATCOPY );
    _wpi_getoldbrush( xorMempres, oldbrush );
    _wpi_deletebrush( hbrush );

    hbrush = _wpi_createsolidbrush( andcolor );
    oldbrush = _wpi_selectbrush( andMempres, hbrush );
    _wpi_patblt( andMempres, pt->x, pt->y, brushsize, brushsize, PATCOPY );
    _wpi_getoldbrush( andMempres, oldbrush );
    _wpi_deletebrush( hbrush );

    hbrush = _wpi_createsolidbrush( color );
    oldbrush = _wpi_selectbrush( presWindow, hbrush );
    _wpi_patblt( presWindow, BORDER_WIDTH + pt->x, BORDER_WIDTH + pt->y,
                 brushsize, brushsize, PATCOPY );
    _wpi_getoldbrush( presWindow, oldbrush );
    _wpi_deletebrush( hbrush );

} /* BrushThePoints */

/*
 * EndFreeHand - releases the device contexts and the bitmaps
 *             - this is called when a mouse button up is detected
 */
void EndFreeHand( HWND hwnd )
{
    img_node    *node;

    node = SelectImage( hwnd );

    _wpi_releasepres( node->viewhwnd, presWindow );
    _wpi_getoldbitmap( xorMempres, oldxor_hbitmap );
    _wpi_getoldbitmap( andMempres, oldand_hbitmap );
    _wpi_deletecompatiblepres( xorMempres, xorMemdc );
    _wpi_deletecompatiblepres( andMempres, andMemdc );
    presWindow = (WPI_PRES)NULL;
    xorMempres = (WPI_PRES)NULL;
    andMempres = (WPI_PRES)NULL;

} /* EndFreeHand */

/*
 * GetFreeHandPresentationSpace - the purpose of this function is to provide routines
 *                                that have a need to access the AND and XOR bitmaps
 *                                while free hand drawing is taking place
 *                              - this is because these bitmaps are selected into the
 *                                presentation spaces of this module and thus cannot be
 *                                selected into any other presentation spaces
 */
bool GetFreeHandPresentationSpaces( WPI_PRES *win, WPI_PRES *and, WPI_PRES *xor )
{
    if( win != NULL ) {
        *win = presWindow;
    }
    if( and != NULL ) {
        *and = andMempres;
    }
    if( xor != NULL ) {
        *xor = xorMempres;
    }

    return( presWindow != (WPI_PRES)NULL && xorMempres != (WPI_PRES)NULL &&
            andMempres != (WPI_PRES)NULL );

} /* GetFreeHandPresentationSpace */
