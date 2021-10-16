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
#include "iconinfo.h"
#include "pathgrp2.h"

#include "clibext.h"


/* Local Window callback functions prototypes */
WINEXPORT BOOL CALLBACK GetPosProc( HWND hwnd, LONG lparam );

static WPI_POINT        *windowCoords;
static int              windowIndex;    // used to figure out window coordinates
#ifdef __OS2_PM__
static int              imageMax;
#endif

/*
 * CreateViewBitmap - create the bitmap on the screen (with the background
 *                    color as it should be, etc.)
 *                  - the caller is responsible for deleting the bitmap
 */
WPI_HBITMAP CreateViewBitmap( img_node *mdi_node )
{
    WPI_PRES    pres;
    WPI_PRES    xorandpres;
    WPI_PRES    mempres;
    WPI_PRES    freehandpres;
    HDC         xoranddc;
    HDC         memdc;
    WPI_HBITMAP new_hbitmap;
    WPI_HBITMAP oldxorand_hbitmap;
    WPI_HBITMAP old_hbitmap;
    HBRUSH      brush;
    HBRUSH      oldbrush;
    img_node    *node;
    COLORREF    bkcolor;

    if( mdi_node != NULL ) {
        node = mdi_node;
    } else {
        node = GetCurrentNode();
        if( node == NULL ) {
            return( NULL );
        }
    }

    pres = _wpi_getpres( HWND_DESKTOP );
    xorandpres = _wpi_createcompatiblepres( pres, Instance, &xoranddc );
    mempres = _wpi_createcompatiblepres( pres, Instance, &memdc );
    new_hbitmap = _wpi_createcompatiblebitmap( pres, node->width, node->height );
    _wpi_releasepres( HWND_DESKTOP, pres );

    _wpi_torgbmode( mempres );
    _wpi_torgbmode( xorandpres );
    bkcolor = GetViewBkColor();

#ifdef __OS2_PM__
    _wpi_preparemono( mempres, BLACK, bkcolor );
#endif
    old_hbitmap = _wpi_selectbitmap( mempres, new_hbitmap );

    brush = _wpi_createsolidbrush( bkcolor );
    oldbrush = _wpi_selectbrush( mempres, brush );

    _wpi_patblt( mempres, 0, 0, node->width, node->height, PATCOPY );
    _wpi_selectbrush( mempres, oldbrush );
    _wpi_deletebrush( brush );

    GetFreeHandPresentationSpaces( NULL, &freehandpres, NULL );
    if( freehandpres == (WPI_PRES)NULL ) {
        oldxorand_hbitmap = _wpi_selectbitmap( xorandpres, node->and_hbitmap );
        _wpi_bitblt( mempres, 0, 0, node->width, node->height,
                     xorandpres, 0, 0, SRCAND );
        _wpi_selectbitmap( xorandpres, oldxorand_hbitmap );
    } else {
        _wpi_bitblt( mempres, 0, 0, node->width, node->height,
                     freehandpres, 0, 0, SRCAND );
    }

    GetFreeHandPresentationSpaces( NULL, NULL, &freehandpres );
    if( freehandpres == (WPI_PRES)NULL ) {
        oldxorand_hbitmap = _wpi_selectbitmap( xorandpres, node->xor_hbitmap );
        _wpi_bitblt( mempres, 0, 0, node->width, node->height,
                     xorandpres, 0, 0, SRCINVERT );
        _wpi_selectbitmap( xorandpres, oldxorand_hbitmap );
    } else {
        _wpi_bitblt( mempres, 0, 0, node->width, node->height,
                     freehandpres, 0, 0, SRCINVERT );
    }

    _wpi_deletecompatiblepres( xorandpres, xoranddc );
    _wpi_selectbitmap( mempres, old_hbitmap );
    _wpi_deletecompatiblepres( mempres, memdc );

    return( new_hbitmap );

} /* CreateViewBitmap */

/*
 * DuplicateBitmap - produces a duplicate of the bitmap
 */
WPI_HBITMAP DuplicateBitmap( WPI_HBITMAP hbitmap )
{
    HDC         srcdc;
    WPI_PRES    srcpres;
    HDC         dstdc;
    WPI_PRES    dstpres;
    WPI_PRES    pres;
    int         width;
    int         height;
    int         planes;
    int         bitspixel;
    WPI_HBITMAP new_hbitmap;
    WPI_HBITMAP old_hbitmap;
    WPI_HBITMAP oldnew_hbitmap;

    _wpi_getbitmapparms( hbitmap, &width, &height, &planes, NULL, &bitspixel );
    pres = _wpi_getpres( HWND_DESKTOP );
    srcpres = _wpi_createcompatiblepres( pres, Instance, &srcdc );
    dstpres = _wpi_createcompatiblepres( pres, Instance, &dstdc );
    _wpi_releasepres( HWND_DESKTOP, pres );

    new_hbitmap = _wpi_createbitmap( width, height, planes, bitspixel, NULL );

    old_hbitmap = _wpi_selectbitmap( srcpres, hbitmap );
    oldnew_hbitmap = _wpi_selectbitmap( dstpres, new_hbitmap );

    _wpi_bitblt( dstpres, 0, 0, width, height, srcpres, 0, 0, SRCCOPY );

    _wpi_selectbitmap( srcpres, old_hbitmap );
    _wpi_selectbitmap( dstpres, oldnew_hbitmap );

    _wpi_deletecompatiblepres( srcpres, srcdc );
    _wpi_deletecompatiblepres( dstpres, dstdc );
    return( new_hbitmap );

} /* DuplicateBitmap */

/*
 * IEStretchBlt
 */
static BOOL IEStretchBlt( WPI_PRES hdcDst, int nXOriginDst, int nYOriginDst,
                                     int nWidthDst, int nHeightDst,
                   WPI_PRES hdcSrc, int nXOriginSrc, int nYOriginSrc,
                                    int nWidthSrc, int nHeightSrc,
                   DWORD fdwRop, int bitcount )
{
    POINT               slines;
    POINT               dlines;
    POINT               num_strips;
    unsigned long       linesize;
    int                 x, y;
    int                 sw, sh, dw, dh;
    WPI_PRES            srcpres;
    HDC                 srcdc;
    WPI_HBITMAP         old_hbitmap;
    WPI_HBITMAP         new_hbitmap;

    num_strips.x = nWidthDst / 256;
    num_strips.x++;
#if 0
    if( nWidthDst > 32 ) {
        /* use the version that returns exact bytes needed for bits */
        linesize = BITS_INTO_BYTES( (unsigned long)(nWidthDst * bitcount), 1 );
    } else {
        /* use the version that rounds up to 32 bits */
        linesize = BITS_TO_BYTES( (unsigned long)(nWidthDst * bitcount), 1 );
    }
#else
    /* use the version that rounds up to 32 bits */
    linesize = BITS_TO_BYTES( (unsigned long)(nWidthDst * bitcount), 1 );
#endif
    num_strips.y = ((unsigned long)nHeightDst * linesize) / (16 * 1024);
    num_strips.y++;

    if( num_strips.x > nWidthSrc ) {
        num_strips.x = nWidthSrc;
    } else if( num_strips.x < nWidthSrc ) {
        num_strips.x += num_strips.x % 2;
    }

    if( num_strips.y > nHeightSrc ) {
        num_strips.y = nHeightSrc;
    } else if( num_strips.y < nHeightSrc ) {
        num_strips.y += num_strips.y % 2;
    }

    slines.x = nWidthSrc / num_strips.x;
    dlines.x = ((unsigned long)slines.x * (unsigned long)nWidthDst) /
               (unsigned long)nWidthSrc;

    slines.y = nHeightSrc / num_strips.y;
    dlines.y = ((unsigned long)slines.y * (unsigned long)nHeightDst) /
               (unsigned long)nHeightSrc;

    srcpres = _wpi_createcompatiblepres( hdcDst, Instance, &srcdc );
    new_hbitmap = _wpi_createcompatiblebitmap( hdcDst, dlines.x, dlines.y );
    old_hbitmap = _wpi_selectbitmap( srcpres, new_hbitmap );

    sw = slines.x;
    dw = dlines.x;
    for( x = 0; slines.x * x <= nWidthSrc; x++ ) {
        if( slines.x * x + sw > nWidthSrc ) {
            sw = nWidthSrc - x * slines.x;
            dw = nWidthDst - x * dlines.x;
        }
        sh = slines.y;
        dh = dlines.y;
        for( y = 0; slines.y * y <= nHeightSrc; y++ ) {
            if( slines.y * y + sh > nHeightSrc ) {
                sh = nHeightSrc - y * slines.y;
                dh = nHeightDst - y * dlines.y;
            }
            _wpi_stretchblt( srcpres, 0, 0, dw, dh, hdcSrc,
                             nXOriginSrc + slines.x * x, nYOriginSrc + slines.y * y,
                             sw, sh, fdwRop );
            _wpi_bitblt( hdcDst, nXOriginDst + dlines.x * x,
                         nYOriginDst + dlines.y * y, dw, dh, srcpres, 0, 0, SRCCOPY );
        }
    }

    _wpi_selectbitmap( srcpres, old_hbitmap );
    _wpi_deletebitmap( new_hbitmap );
    _wpi_deletecompatiblepres( srcpres, srcdc );

    return( TRUE );

} /* IEStretchBlt */

/*
 * EnlargeImage - take an MDI window handle and enlarge the view bitmap
 *                that goes with it
 *              - returns a handle to the bitmap
 *              - the bitmap must be deleted by the calling routine
 */
WPI_HBITMAP EnlargeImage( HWND hwnd )
{
    WPI_PRES    pres;
    WPI_PRES    srcpres;
    WPI_PRES    dstpres;
    HDC         srcdc;
    HDC         dstdc;
    WPI_HBITMAP old_hbitmap;
    WPI_HBITMAP new_hbitmap;
    WPI_HBITMAP olddst_hbitmap;
    WPI_HBITMAP view_hbitmap;
    img_node    *node;
    WPI_RECT    rc;
    short       width;
    short       height;
    int         window_width;
    int         window_height;
    BITMAP      bm;

    node = SelectImage( hwnd );
    if( node == NULL ) {
        return( NULL );
    }

    view_hbitmap = CreateViewBitmap( node );
    _wpi_getclientrect( hwnd, &rc );
    /*
     * I add this so that if the window's client rect doesn't fit on the
     * screen, it will still enlarge to the right size.
     */
    window_width = _wpi_getwidthrect( rc );
    window_height = _wpi_getheightrect( rc );
    if( window_width < node->width ) {
        window_width = node->width;
    }
    if( window_height < node->height ) {
        window_height = node->height;
    }

    pres = _wpi_getpres( HWND_DESKTOP );
    srcpres = _wpi_createcompatiblepres( pres, Instance, &srcdc );
    dstpres = _wpi_createcompatiblepres( pres, Instance, &dstdc );
    new_hbitmap = _wpi_createcompatiblebitmap( pres, _wpi_getwidthrect( rc ),
                                                   _wpi_getheightrect( rc ) );
    _wpi_releasepres( HWND_DESKTOP, pres );
    GetObject( new_hbitmap, sizeof( BITMAP ), &bm );

    _wpi_torgbmode( dstpres );
    _wpi_torgbmode( srcpres );
    olddst_hbitmap = _wpi_selectbitmap( dstpres, new_hbitmap );
    old_hbitmap = _wpi_selectbitmap( srcpres, view_hbitmap );
    height = node->height;
    width = node->width;

    IEStretchBlt( dstpres, 0, 0, _wpi_getwidthrect( rc ),
                  _wpi_getheightrect( rc ), srcpres, 0, 0, width, height,
                  SRCCOPY, bm.bmBitsPixel );

    _wpi_selectbitmap( srcpres, old_hbitmap );
    _wpi_deletecompatiblepres( srcpres, srcdc );
    _wpi_deletebitmap( view_hbitmap );

    _wpi_selectbitmap( dstpres, olddst_hbitmap );
    _wpi_deletecompatiblepres( dstpres, dstdc );

    return( new_hbitmap );

} /* EnlargeImage */

/*
 * SetIsSaved - set whether the given MDI child has been saved
 */
void SetIsSaved( HWND hwnd, bool issaved )
{
    img_node    *node;
    img_node    *next_icon;
    pgroup2     pg;
    char        title[_MAX_EXT + _MAX_FNAME + 2];
    char        *main_title;

    node = GetImageNode( hwnd );
    if( node == NULL ) {
        return;
    }

    // We do not check whether or not the image needs to have its
    // title bars updated here because sometimes we will save images
    // that are not modified to different files.
#if 0
    if( issaved == node->issaved ) {
        return;
    }
#endif

    next_icon = node;
    while( next_icon != NULL ) {
        next_icon->issaved = issaved;
        next_icon = next_icon->nexticon;
    }

    if( issaved ) {
        _wpi_getwindowtext( _wpi_getframe( node->hwnd ), title, sizeof( title ) );
        if( strnicmp( title, IEImageUntitled, strlen( IEImageUntitled ) ) == 0 &&
            strnicmp( node->fname, IEImageUntitled, strlen( IEImageUntitled ) ) == 0 ) {
            return;
        }
        _splitpath2( node->fname, pg.buffer, &pg.drive, &pg.dir, &pg.fname, &pg.ext );
        _makepath( title, NULL, NULL, pg.fname, pg.ext );
        strupr( title );
        _wpi_setwindowtext( _wpi_getframe( node->hwnd ), (LPSTR)title );

        main_title = (char *)MemAlloc( strlen( IEAppTitle ) + strlen( title ) + 3 + 1 );
        if( main_title != NULL ) {
            strcpy( main_title, IEAppTitle );
            strcat( main_title, " - " );
            strcat( main_title, title );
            _wpi_setwindowtext( HMainWindow, main_title );
            MemFree( main_title );
        }
    } else {
        _wpi_getwindowtext( _wpi_getframe( node->hwnd ), title, sizeof( title ) );
        if( strnicmp( title, IEImageUntitled, strlen( IEImageUntitled ) ) == 0 ) {
            return;
        } else if( title[strlen( title ) - 1] == '*' ) {
            return;
        } else {
            strcat( title, "*" );
            _wpi_setwindowtext( _wpi_getframe( node->hwnd ), (LPSTR)title );
        }
    }

} /* SetIsSaved */

/*
 * OutlineRectangle - outline a rectangle with the XOR pen
 */
void OutlineRectangle( bool firsttime, WPI_PRES pres, WPI_RECT *prevrc, WPI_RECT *newrc )
{
    int         prevrop2;
    HBRUSH      holdbrush;
    HBRUSH      nullbrush;
    HPEN        holdpen;
    HPEN        whitepen;
    int         left, top, right, bottom;

    _wpi_torgbmode( pres );

    nullbrush = _wpi_createnullbrush();
    whitepen = _wpi_createpen( PS_SOLID, 0, WHITE );
    holdbrush = _wpi_selectbrush( pres, nullbrush );
    holdpen = _wpi_selectpen( pres, whitepen );

    prevrop2 = _wpi_setrop2( pres, R2_XORPEN );
    if( !firsttime ) {
        _wpi_getintrectvalues( *prevrc, &left, &top, &right, &bottom );
        /*
         * In this case don't call _wpi_convertheight because of the
         * way the rectangle values are set in iedraw.c
         */
        _wpi_rectangle( pres, left, top, right, bottom );
    }

    _wpi_getintrectvalues( *newrc, &left, &top, &right, &bottom );
    _wpi_rectangle( pres, left, top, right, bottom );

    _wpi_selectbrush( pres, holdbrush );
    _wpi_selectpen( pres, holdpen );
    _wpi_setrop2( pres, prevrop2 );
    _wpi_deletepen( whitepen );
    _wpi_deletenullbrush( nullbrush );

} /* OutlineRectangle */

/*
 * GetFnameFromPath - given a full pathname, return just the file name
 */
void GetFnameFromPath( const char *fullpath, char *fname )
{
    pgroup2     pg;

    if( strnicmp( fullpath, IEImageUntitled, strlen( IEImageUntitled ) ) == 0 ) {
        strcpy( fname, fullpath );
        return;
    }
    _splitpath2( fullpath, pg.buffer, NULL, NULL, &pg.fname, &pg.ext );
    _makepath( fname, NULL, NULL, pg.fname, pg.ext );

} /* GetFnameFromPath */

/*
 * GrayEditOptions - gray the edit options (at the beginning and when an
 *                   image is closed)
 */
void GrayEditOptions( void )
{
    HMENU       hmenu;

    if( HMainWindow == NULL ) {
        return;
    }
    hmenu = _wpi_getmenu( _wpi_getframe( HMainWindow ) );

    _wpi_enablemenuitem( hmenu, IMGED_UNDO, FALSE, FALSE );
    _wpi_enablemenuitem( hmenu, IMGED_REDO, FALSE, FALSE );
    _wpi_enablemenuitem( hmenu, IMGED_REST, FALSE, FALSE );
    _wpi_enablemenuitem( hmenu, IMGED_CUT, FALSE, FALSE );
    _wpi_enablemenuitem( hmenu, IMGED_COPY, FALSE, FALSE );
    _wpi_enablemenuitem( hmenu, IMGED_PASTE, FALSE, FALSE );
    _wpi_enablemenuitem( hmenu, IMGED_SNAP, FALSE, FALSE );
    _wpi_enablemenuitem( hmenu, IMGED_CLEAR, FALSE, FALSE );
    _wpi_enablemenuitem( hmenu, IMGED_NEWIMG, FALSE, FALSE );
    _wpi_enablemenuitem( hmenu, IMGED_SELIMG, FALSE, FALSE );
    _wpi_enablemenuitem( hmenu, IMGED_DELIMG, FALSE, FALSE );
    _wpi_enablemenuitem( hmenu, IMGED_RIGHT, FALSE, FALSE );
    _wpi_enablemenuitem( hmenu, IMGED_LEFT, FALSE, FALSE );
    _wpi_enablemenuitem( hmenu, IMGED_UP, FALSE, FALSE );
    _wpi_enablemenuitem( hmenu, IMGED_DOWN, FALSE, FALSE );
    _wpi_enablemenuitem( hmenu, IMGED_FLIPHORZ, FALSE, FALSE );
    _wpi_enablemenuitem( hmenu, IMGED_FLIPVERT, FALSE, FALSE );
    _wpi_enablemenuitem( hmenu, IMGED_ROTATECC, FALSE, FALSE );
    _wpi_enablemenuitem( hmenu, IMGED_ROTATECL, FALSE, FALSE );

} /* GrayEditOptions */

/*
 * GetPosProc - get the position of all the child windows
 */
BOOL CALLBACK GetPosProc( HWND hwnd, LONG lparam )
{
    WPI_RECT    windowrect;
    WPI_POINT   topleft;        // this ends up being bottom left for PM
    WPI_RECTDIM left;
    WPI_RECTDIM top;

    lparam = lparam;

#ifdef __OS2_PM__
    if( windowIndex >= imageMax ) {
        return( 0 );
    }
#endif
    if( _wpi_getowner( hwnd ) != NULL ) {
        return( 1 );
    }

    if( _wpi_isiconic( hwnd ) ) {
        windowCoords[windowIndex].x = -1;
        windowCoords[windowIndex].y = -1;
        windowIndex++;
        return( 1 );
    }

    _wpi_getwindowrect( hwnd, &windowrect );
    _wpi_getrectvalues( windowrect, &left, &top, NULL, NULL );
    topleft.x = left;
    topleft.y = top;

    _wpi_screentoclient( ClientWindow, &topleft );
    if( topleft.x < 0 ) {
        topleft.x = 0;
    }
#ifndef __OS2_PM__
    if( topleft.y < 0 ) {
        topleft.y = 0;
    }
#endif
    windowCoords[windowIndex] = topleft;
    windowIndex++;
    return( 1 );

} /* GetPosProc */

/*
 * FindOrigin - "cascade" the windows to find the placement of the new MDI child
 *            - origin is expected to come in as (0, 0) or the equivalent for PM
 */
void FindOrigin( WPI_POINT *new_origin )
{
    WPI_ENUMPROC    enumproc;
    int             image_count;
    int             i, j;
    WPI_POINT       temp;
    WPI_RECT        proposed;
    WPI_RECTDIM     width;
    int             base;

    image_count = DoImagesExist();

    if( image_count == 0 ) {
        return;
    }

#ifdef __OS2_PM__
    imageMax = image_count;
#endif
    windowCoords = MemAlloc( image_count * sizeof( WPI_POINT ) );
    windowIndex = 0;

    enumproc = _wpi_makeenumprocinstance( GetPosProc, Instance );
    _wpi_enumchildwindows( ClientWindow, enumproc, 0L );
    _wpi_freeenumprocinstance( enumproc );

    /*
     * I'm just using a simple bubble sort ... we're using small amounts of data
     */
    for( i = 0; i < image_count; i++ ) {
        for( j = 0; j < image_count - i - 1; j++ ) {
            if( windowCoords[j].x > windowCoords[j + 1].x ) {
                temp = windowCoords[j];
                windowCoords[j] = windowCoords[j + 1];
                windowCoords[j + 1] = temp;
            }
        }
    }

    /*
     * Minimized windows will have coordinates set to negative, and we only
     * want non-minimized windows.
     */
    for( base = 0; base < image_count; base++ ) {
        if( windowCoords[base].x >= 0 ) {
            break;
        }
    }
    if( base >= image_count ) {
        MemFree( windowCoords );
        return;
    }

    width = _wpi_getsystemmetrics( SM_CYCAPTION );
#ifndef __OS2_PM__
    _wpi_setintrectvalues( &proposed, new_origin->x, new_origin->y,
                           new_origin->x + width, new_origin->y + width );
#else
    _wpi_setrectvalues( &proposed, new_origin->x, new_origin->y - 1,
                        new_origin->x + width, new_origin->y + width - 1 );
#endif

    /*
     * Try to place at the origin passed in if we can.
     */
    if( !_wpi_ptinrect( &proposed, windowCoords[base] ) ) {
        MemFree( windowCoords );
        return;
    }

    for( i = base; i < image_count - 1; i++ ) {
        if( windowCoords[i + 1].x - windowCoords[i].x > 2 * width ) {
            break;
        }
    }

    temp.x = windowCoords[i].x + width;
#ifndef __OS2_PM__
    temp.y = windowCoords[i].x + width;
#else
    temp.y = windowCoords[i].y - width;
#endif

    _wpi_getclientrect( ClientWindow, &proposed );
#ifndef __OS2_PM__
    if( !_wpi_ptinrect( &proposed, temp ) ) {
        return;
    } else {
        new_origin->x = temp.x;
        new_origin->y = temp.y;
    }
#else
    new_origin->x = temp.x;
    new_origin->y = temp.y;
#endif

    MemFree( windowCoords );

} /* FindOrigin */

/*
 * SetMenus - set the menu options for the new image
 */
void SetMenus( img_node *node )
{
    HMENU       hmenu;

    hmenu = _wpi_getmenu( _wpi_getframe( HMainWindow ) );
    _wpi_enablemenuitem( hmenu, IMGED_SNAP, TRUE, FALSE );
    _wpi_enablemenuitem( hmenu, IMGED_CUT, TRUE, FALSE );
    _wpi_enablemenuitem( hmenu, IMGED_COPY, TRUE, FALSE );
    _wpi_enablemenuitem( hmenu, IMGED_CLEAR, TRUE, FALSE );
#ifndef __OS2_PM__
    SetColorMenus( node );
#endif

    if( node->imgtype == BITMAP_IMG ) {
        DisplayScreenClrs( false );
        AddHotSpotTool( false );
        _wpi_enablemenuitem( hmenu, IMGED_NEWIMG, FALSE, FALSE );
        _wpi_enablemenuitem( hmenu, IMGED_SELIMG, FALSE, FALSE );
        _wpi_enablemenuitem( hmenu, IMGED_DELIMG, FALSE, FALSE );
        _wpi_enablemenuitem( hmenu, IMGED_SIZE, TRUE, FALSE );
    } else if( node->imgtype == ICON_IMG ) {
        DisplayScreenClrs( true );
        AddHotSpotTool( false );
        _wpi_enablemenuitem( hmenu, IMGED_SELIMG, TRUE, FALSE );
        if( node->num_of_images < NUM_OF_ICONS ) {
            _wpi_enablemenuitem( hmenu, IMGED_NEWIMG, TRUE, FALSE );
        } else {
            _wpi_enablemenuitem( hmenu, IMGED_NEWIMG, FALSE, FALSE );
        }
        if( node->num_of_images > 1 ) {
            _wpi_enablemenuitem( hmenu, IMGED_DELIMG, TRUE, FALSE );
            _wpi_enablemenuitem( hmenu, IMGED_SELIMG, TRUE, FALSE );
        } else {
            _wpi_enablemenuitem( hmenu, IMGED_DELIMG, FALSE, FALSE );
            _wpi_enablemenuitem( hmenu, IMGED_SELIMG, FALSE, FALSE );
        }
        _wpi_enablemenuitem( hmenu, IMGED_SIZE, FALSE, FALSE );
        SetIconInfo( node );
    } else if( node->imgtype == CURSOR_IMG ) {
        DisplayScreenClrs( true );
        AddHotSpotTool( true );
        _wpi_enablemenuitem( hmenu, IMGED_NEWIMG, FALSE, FALSE );
        _wpi_enablemenuitem( hmenu, IMGED_SELIMG, FALSE, FALSE );
        _wpi_enablemenuitem( hmenu, IMGED_DELIMG, FALSE, FALSE );
        _wpi_enablemenuitem( hmenu, IMGED_SIZE, FALSE, FALSE );
    }

} /* SetMenus */

#if 0

/*
 * ConvertToDIBitmap - convert the device dependent bitmap to a DI bitmap
 */
void ConvertToDIBitmap( WPI_HBITMAP hbitmap )
{
    HDC         hdc;
    BITMAPINFO  *bmi;
    BYTE        *bits;
    BITMAP      bm;
    img_node    *node;

    /*
     * Fill the info structure with information about the current image
     * we are editing.
     */
    node = GetCurrentNode();
    bmi = GetDIBitmapInfo( node );

    GetObject( hbitmap, sizeof( BITMAP ), &bm );
    /*
     * Replace the fields on the info header with values for *this* bitmap.
     */
    bmi->bmiHeader.biWidth = bm.bmWidth;
    bmi->bmiHeader.biHeight = bm.bmHeight;
#if 0
    if( bmi->bmiHeader.bmWidth > 32 ) {
        bmi->bmiHeader.biSizeImage = BITS_INTO_BYTES( bmi->bmiHeader.biBitCount * bm.bmWidth, bm.bmHeight );
    } else {
        bmi->bmiHeader.biSizeImage = BITS_TO_BYTES( bmi->bmiHeader.biBitCount * bm.bmWidth, bm.bmHeight );
    }
#else
    bmi->bmiHeader.biSizeImage = BITS_TO_BYTES( bmi->bmiHeader.biBitCount * bm.bmWidth, bm.bmHeight );
#endif
    bits = MemAlloc( bmi->bmiHeader.biSizeImage );

    hdc = GetDC( NULL );
    GetDIBits( hdc, hbitmap, 0, bmi->bmiHeader.biHeight, bits, bmi, DIB_RGB_COLORS );
    SetDIBits( hdc, hbitmap, 0, bmi->bmiHeader.biHeight, bits, bmi, DIB_RGB_COLORS );

    ReleaseDC( NULL, hdc );

    FreeDIBitmapInfo( bmi );
    MemFree( bits );

} /* ConvertToDIBitmap */

#endif
