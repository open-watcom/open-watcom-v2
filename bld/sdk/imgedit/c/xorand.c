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

static img_node         *activeImage = NULL;

/*
 * MakeBitmap - make the bitmap
 */
void MakeBitmap( img_node *node, BOOL isnew )
{
    HDC                 hdc;
    WPI_PRES            pres;
    WPI_PRES            mempres;
    HBITMAP             oldbitmap;

    if( isnew ) {
        InitXorAndBitmaps( node );
    } else {
        /*
         * The AND bitmap won't really get used, but it should be around
         * since some functions are generic (i.e. for all image types) and
         * hence assume an AND bitmap exists.
         */
        pres = _wpi_getpres( HWND_DESKTOP );
        mempres = _wpi_createcompatiblepres( pres, Instance, &hdc );
        node->handbitmap = _wpi_createbitmap( node->width, node->height, 1, 1, NULL );
        _wpi_releasepres( HWND_DESKTOP, pres );

        _wpi_torgbmode( mempres );
        oldbitmap = _wpi_selectobject( mempres, node->handbitmap );
        _wpi_patblt( mempres, 0, 0, node->width, node->height, BLACKNESS );

        _wpi_selectobject( mempres, oldbitmap );
        _wpi_deletecompatiblepres( mempres, hdc );
    }

    DisplayImageText( node );

} /* MakeBitmap */

/*
 * MakeIcon - create the AND and XOR bitmaps and draw the icon (or cursor)
 */
void MakeIcon( img_node *node, BOOL isnew )
{
    if( !isnew ) {
        activeImage = node;
    } else {
        InitXorAndBitmaps( node );
    }
    DisplayImageText( node );

} /* MakeIcon */

/*
 * LineXorAnd - draw the line on the XOR and the AND bitmaps
 */
void LineXorAnd( COLORREF xorcolor, COLORREF andcolor,
                 WPI_POINT *startpt, WPI_POINT *endpt )
{
    HPEN        oldpen;
    HPEN        hpen;
    HDC         memdc;
    WPI_PRES    pres;
    WPI_PRES    mempres;
    HBITMAP     oldbitmap;
//    HDC         anddc;

//    anddc = anddc;
    pres = _wpi_getpres( HWND_DESKTOP );
    mempres = _wpi_createcompatiblepres( pres, Instance, &memdc );
    _wpi_releasepres( HWND_DESKTOP, pres );

    _wpi_torgbmode( mempres );
    hpen = _wpi_createpen( PS_SOLID, 0, xorcolor );
    oldpen = _wpi_selectobject( mempres, hpen );
    oldbitmap = _wpi_selectobject( mempres, activeImage->hxorbitmap );

    _wpi_movetoex( mempres, startpt, NULL );
    _wpi_lineto( mempres, endpt );
    _wpi_setpixel( mempres, endpt->x, endpt->y, xorcolor );

    _wpi_selectobject( mempres, oldpen );
    _wpi_deleteobject( hpen );
    _wpi_selectobject( mempres, oldbitmap );

    if( activeImage->imgtype == BITMAP_IMG ) {
        _wpi_deletecompatiblepres( mempres, memdc );
        return;
    }

    hpen = _wpi_createpen( PS_SOLID, 0, andcolor );
    oldpen = _wpi_selectobject( mempres, hpen );
    oldbitmap = _wpi_selectobject( mempres, activeImage->handbitmap );

    _wpi_movetoex( mempres, startpt, NULL );
    _wpi_lineto( mempres, endpt );
    _wpi_setpixel( mempres, endpt->x, endpt->y, andcolor );

    _wpi_selectobject( mempres, oldpen );
    _wpi_deleteobject( hpen );
    _wpi_selectobject( mempres, oldbitmap );
//    _wpi_deletecompatiblepres( mempres, anddc );

} /* LineXorAnd */

/*
 * RegionXorAnd - draw a filled or framed region (ellipse or rectangle) in
 *                the view window
 */
void RegionXorAnd( COLORREF xorcolor, COLORREF andcolor,
                   BOOL fFillRgn, WPI_RECT *r, BOOL is_rect )
{
    HBRUSH      oldbrush;
    HBRUSH      hbrush;
    HPEN        oldpen;
    HPEN        hpen;
    WPI_PRES    mempres;
    HDC         memdc;
    WPI_PRES    pres;
    HBITMAP     oldbitmap;
    int         left;
    int         top;
    int         right;
    int         bottom;

    pres = _wpi_getpres( HWND_DESKTOP );
    mempres = _wpi_createcompatiblepres( pres, Instance, &memdc );
    _wpi_releasepres( HWND_DESKTOP, pres );

    _wpi_torgbmode( mempres );

    hpen = _wpi_createpen( PS_SOLID, 0, xorcolor );
    oldpen = _wpi_selectobject( mempres, hpen );
    oldbitmap = _wpi_selectobject( mempres, activeImage->hxorbitmap );

    if( fFillRgn ) {
        hbrush = _wpi_createsolidbrush( xorcolor );
    } else {
        hbrush = _wpi_createnullbrush();
    }

    oldbrush = _wpi_selectobject( mempres, hbrush );
    _wpi_getintrectvalues( *r, &left, &top, &right, &bottom );
    if( is_rect ) {
        _wpi_rectangle( mempres, left, top, right, bottom );
    } else {
        _wpi_ellipse( mempres, left, top, right, bottom );
    }

    _wpi_selectobject( mempres, oldpen );
    _wpi_deletepen( hpen );
    _wpi_selectobject( mempres, oldbrush );
    _wpi_selectobject( mempres, oldbitmap );
    if( fFillRgn ) {
        _wpi_deleteobject( hbrush );
    } else {
        _wpi_deletenullbrush( hbrush );
    }

    if( activeImage->imgtype == BITMAP_IMG ) {
        _wpi_deletecompatiblepres( mempres, memdc );
        return;
    }

    hpen = _wpi_createpen( PS_SOLID, 0, andcolor );
    oldpen = _wpi_selectobject( mempres, hpen );
    oldbitmap = _wpi_selectobject( mempres, activeImage->handbitmap );

    if( fFillRgn ) {
        hbrush = _wpi_createsolidbrush( andcolor );
    } else {
        hbrush = _wpi_createnullbrush();
    }
    oldbrush = _wpi_selectobject( mempres, hbrush );

    if( is_rect ) {
        _wpi_rectangle( mempres, left, top, right, bottom );
    } else {
        _wpi_ellipse( mempres, left, top, right, bottom );
    }
    _wpi_selectobject( mempres, oldpen );
    _wpi_deletepen( hpen );
    _wpi_selectobject( mempres, oldbrush );
    _wpi_selectobject( mempres, oldbitmap );
    if( fFillRgn ) {
        _wpi_deleteobject( hbrush );
    } else {
        _wpi_deletenullbrush( hbrush );
    }

    _wpi_deletecompatiblepres( mempres, memdc );

} /* RegionXorAnd */

/*
 * FillXorAnd - fill the area in the XOR and the AND bitmaps (with a call to fill)
 */
void FillXorAnd( COLORREF brushcolor, WPI_POINT *pt, wie_clrtype colortype )
{
    fill_info_struct    fillinfo;

    fillinfo.img_type = activeImage->imgtype;
    fillinfo.colortype = colortype;
    fillinfo.pt = *pt;

    if( colortype == SCREEN_CLR ) {
        fillinfo.xorcolor = BLACK;
        fillinfo.andcolor = WHITE;
    } else if( colortype == INVERSE_CLR ) {
        fillinfo.xorcolor = WHITE;
        fillinfo.andcolor = WHITE;
    } else {
        fillinfo.xorcolor = brushcolor;
        fillinfo.andcolor = BLACK;
    }
    Fill( &fillinfo, activeImage );

} /* FillXorAnd */

/*
 * SetNewHotSpot - set the value of the hot spot for the cursor
 */
void SetNewHotSpot( WPI_POINT *pt )
{
    activeImage->hotspot = *pt;
    SetHotSpot( activeImage );

} /* SetNewHotSpot */

/*
 * FocusOnImage - select one of the MDI children
 */
void FocusOnImage( HWND hwnd )
{
    char        current_file[_MAX_PATH];
    char        *text;

    if( activeImage != NULL  ) {
        RedrawPrevClip( activeImage->hwnd );
        SetRectExists( FALSE );
    }

    activeImage = SelectImage( hwnd );
    if( activeImage == NULL ) {
        WImgEditError( WIE_ERR_BAD_HWND, WIE_INTERNAL_001 );
        return;
    }
    ResetViewWindow( activeImage->viewhwnd );
    CreateDrawnImage( activeImage );

    SetMenus( activeImage );
    SetNumColors( 1 << activeImage->bitcount );

    SetHotSpot( activeImage );
    DisplayImageText( activeImage );
    CheckForUndo( activeImage );

    GetFnameFromPath( activeImage->fname, current_file );

    text = (char *)MemAlloc( strlen( IEAppTitle ) + strlen( current_file ) + 3 + 1 );
    if( text != NULL ) {
        strcpy( text, IEAppTitle );
        strcat( text, " - " );
        strcat( text, current_file );
        _wpi_setwindowtext( _wpi_getframe( HMainWindow ), text );
        MemFree( text );
    }

#ifndef __OS2_PM__
    RedrawWindow( hwnd, NULL, NULL, RDW_UPDATENOW );
#endif

} /* FocusOnImage */

/*
 * GetCurrentNode - return the current image node being edited
 */
img_node *GetCurrentNode( void )
{
    return( activeImage );

} /* GetCurrentNode */

/*
 * SelectIcon - set the current icon
 */
void SelectIcon( short index )
{
    img_node    *node;

    if( activeImage->imgtype != ICON_IMG ) {
        WImgEditError( WIE_ERR_BAD_IMAGE_TYPE, NULL );
        return;
    }

    node = GetNthIcon( activeImage->hwnd, index );

    if( node != NULL ) {
        SelIconUndoStack( activeImage->hwnd, index );
        ResetDrawArea( node );
        RePositionViewWnd( node );

        SetNumColors( 1 << node->bitcount );
        SetMenus( node );
    } else {
        WImgEditError( WIE_ERR_BAD_SELECTION, NULL );
        return;
    }

    activeImage = node;
    DisplayImageText( activeImage );

} /* SelectIcon */

/*
 * DeleteActiveImage - set the active image to null
 */
void DeleteActiveImage( void )
{
    activeImage = NULL;
    _wpi_setwindowtext( _wpi_getframe( HMainWindow ), IEAppTitle );

} /* DeleteActiveImage */
