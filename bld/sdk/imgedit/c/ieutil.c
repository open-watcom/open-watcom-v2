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
#include "iconinfo.h"
#include "iemem.h"

static WPI_POINT        *windowCoords;
static int              windowIndex;    // used to figure out window coordinates
#ifdef __OS2_PM__
static int              imageMax;
#endif
/*
 * CreateViewBitmap - creates the bitmap on the screen (with the background
 *                      colour as it should be etc...).  Function caller is
 *                      responsible for deleting the bitmap.
 */
HBITMAP CreateViewBitmap( img_node *mdi_node )
{
    WPI_PRES    pres;
    WPI_PRES    xorandpres;
    WPI_PRES    mempres;
    WPI_PRES    freehandpres;
    HDC         xoranddc;
    HDC         memdc;
    HBITMAP     newbitmap;
    HBITMAP     oldxorandbitmap;
    HBITMAP     oldbitmap;
    HBRUSH      brush;
    HBRUSH      oldbrush;
    img_node    *node;
    COLORREF    bkcolour;

    if (mdi_node) {
        node = mdi_node;
    } else {
        node = GetCurrentNode();
        if (!node) return(NULL);
    }

    pres = _wpi_getpres( HWND_DESKTOP );
    xorandpres = _wpi_createcompatiblepres( pres, Instance, &xoranddc );
    mempres = _wpi_createcompatiblepres( pres, Instance, &memdc );
    newbitmap = _wpi_createcompatiblebitmap(pres, node->width, node->height );
    _wpi_releasepres( HWND_DESKTOP, pres );

    _wpi_torgbmode( mempres );
    _wpi_torgbmode( xorandpres );
    bkcolour = GetBkColour();

#ifdef __OS2_PM__
    _wpi_preparemono( mempres, BLACK, bkcolour );
#endif
    oldbitmap = _wpi_selectobject( mempres, newbitmap );

    brush = _wpi_createsolidbrush( bkcolour );
    oldbrush = _wpi_selectobject( mempres, brush );

    _wpi_patblt( mempres, 0, 0, node->width, node->height, PATCOPY );
    _wpi_selectobject( mempres, oldbrush );
    _wpi_deletebrush( brush );

    GetFreeHandPresentationSpaces( NULL, &freehandpres, NULL );
    if( freehandpres == (WPI_PRES)NULL ) {
        oldxorandbitmap = _wpi_selectobject( xorandpres, node->handbitmap );
        _wpi_bitblt( mempres, 0, 0, node->width, node->height,
                     xorandpres, 0, 0, SRCAND);
        _wpi_selectobject( xorandpres, oldxorandbitmap );
    } else {
        _wpi_bitblt( mempres, 0, 0, node->width, node->height,
                     freehandpres, 0, 0, SRCAND);
    }

    GetFreeHandPresentationSpaces( NULL, NULL, &freehandpres );
    if( freehandpres == (WPI_PRES)NULL ) {
        oldxorandbitmap = _wpi_selectobject( xorandpres, node->hxorbitmap );
        _wpi_bitblt( mempres, 0, 0, node->width, node->height,
                     xorandpres, 0, 0, SRCINVERT);
        _wpi_selectobject( xorandpres, oldxorandbitmap );
    } else {
        _wpi_bitblt( mempres, 0, 0, node->width, node->height,
                     freehandpres, 0, 0, SRCINVERT);
    }

    _wpi_deletecompatiblepres( xorandpres, xoranddc );
    _wpi_selectobject( mempres, oldbitmap );
    _wpi_deletecompatiblepres( mempres, memdc );

    return( newbitmap );
} /* CreateViewBitmap */

/*
 * DuplicateBitmap - produces a duplicate of the bitmap.
 */
HBITMAP DuplicateBitmap( HBITMAP hbitmap)
{
    HDC         srcdc;
    WPI_PRES    srcpres;
    HDC         destdc;
    WPI_PRES    destpres;
    WPI_PRES    pres;
    int         width;
    int         height;
    int         planes;
    int         bitspixel;
    HBITMAP     newbitmap;
    HBITMAP     oldbitmap;
    HBITMAP     oldnewbitmap;

    _wpi_getbitmapparms( hbitmap, &width, &height, &planes, NULL, &bitspixel );
    pres = _wpi_getpres( HWND_DESKTOP );
    srcpres = _wpi_createcompatiblepres( pres, Instance, &srcdc );
    destpres = _wpi_createcompatiblepres( pres, Instance, &destdc );
    _wpi_releasepres( HWND_DESKTOP, pres );

    newbitmap = _wpi_createbitmap(width, height, planes, bitspixel, NULL );

    oldbitmap = _wpi_selectobject( srcpres, hbitmap );
    oldnewbitmap = _wpi_selectobject( destpres, newbitmap );

    _wpi_bitblt(destpres, 0, 0, width, height, srcpres, 0, 0, SRCCOPY);

    _wpi_selectobject( srcpres, oldbitmap );
    _wpi_selectobject( destpres, oldnewbitmap );

    _wpi_deletecompatiblepres( srcpres, srcdc );
    _wpi_deletecompatiblepres( destpres, destdc );
    return( newbitmap );
} /* DuplicateBitmap */

BOOL IEStretchBlt( WPI_PRES hdcDest, int nXOriginDest, int nYOriginDest,
                                     int nWidthDest, int nHeightDest,
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
    HBITMAP             oldbitmap;
    HBITMAP             newbitmap;

    num_strips.x = nWidthDest / 256;
    num_strips.x++;
    if (nWidthDest > 32 && FALSE) {
        /* use the version that returns exact bytes needed for bits */
        linesize = BITS_INTO_BYTES( (unsigned long)( nWidthDest * bitcount), 1 );
    } else {
        /* use the version that rounds up to 32 bits */
        linesize = BITS_TO_BYTES( (unsigned long)( nWidthDest * bitcount), 1 );
    }
    num_strips.y = ( (unsigned long)nHeightDest * linesize ) / ( 16 * 1024 );
    num_strips.y++;

    if( num_strips.x > nWidthSrc ) {
        num_strips.x = nWidthSrc;
    } else if( num_strips.x < nWidthSrc ) {
        num_strips.x += ( num_strips.x % 2 );
    }

    if( num_strips.y > nHeightSrc ) {
        num_strips.y = nHeightSrc;
    } else if( num_strips.y < nHeightSrc ) {
        num_strips.y += ( num_strips.y % 2 );
    }

    slines.x = nWidthSrc / num_strips.x;
    dlines.x = ( (unsigned long)slines.x * (unsigned long)nWidthDest ) /
                   (unsigned long)nWidthSrc;

    slines.y = nHeightSrc / num_strips.y;
    dlines.y = ( (unsigned long)slines.y * (unsigned long)nHeightDest ) /
                   (unsigned long)nHeightSrc;

    srcpres = _wpi_createcompatiblepres( hdcDest, Instance, &srcdc );
    newbitmap = _wpi_createcompatiblebitmap( hdcDest, dlines.x, dlines.y );
    oldbitmap = _wpi_selectobject( srcpres, newbitmap );

    sw = slines.x;
    dw = dlines.x;
    for( x=0; slines.x * x <= nWidthSrc; x++ ) {
        if( ( slines.x * x + sw ) > nWidthSrc ) {
            sw = nWidthSrc - x * slines.x;
            dw = nWidthDest - x * dlines.x;
        }
        sh = slines.y;
        dh = dlines.y;
        for( y=0; slines.y * y <= nHeightSrc; y++ ) {
            if( ( slines.y * y + sh ) > nHeightSrc ) {
                sh = nHeightSrc - y * slines.y;
                dh = nHeightDest - y * dlines.y;
            }
            _wpi_stretchblt( srcpres, 0, 0, dw, dh, hdcSrc,
                             nXOriginSrc + slines.x*x, nYOriginSrc + slines.y*y,
                             sw, sh, fdwRop );
            _wpi_bitblt( hdcDest, nXOriginDest + dlines.x*x,
                         nYOriginDest + dlines.y*y, dw, dh, srcpres, 0, 0,
                         SRCCOPY );
        }
    }

    _wpi_selectobject( srcpres, oldbitmap );
    _wpi_deleteobject( newbitmap );
    _wpi_deletecompatiblepres( srcpres, srcdc );

    return( TRUE );
}

/*
 * EnlargeImage - takes an mdi window handle and enlarges the view bitmap
 *                that goes with it.  It returns a handle to the bitmap.
 *                The bitmap must be deleted by the calling routine.
 */
HBITMAP EnlargeImage( HWND hwnd )
{
    WPI_PRES    pres;
    WPI_PRES    srcpres;
    WPI_PRES    destpres;
    HDC         srcdc;
    HDC         destdc;
    HBITMAP     oldbitmap;
    HBITMAP     newbitmap;
    HBITMAP     olddestbitmap;
    HBITMAP     viewbitmap;
    img_node    *node;
    WPI_RECT    rc;
    short       width;
    short       height;
    int         window_width;
    int         window_height;
    BITMAP      bm;

    node = SelectImage( hwnd );
    if (!node) return (NULL);

    viewbitmap = CreateViewBitmap( node );
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
    destpres = _wpi_createcompatiblepres( pres, Instance, &destdc );
    newbitmap = _wpi_createcompatiblebitmap( pres, _wpi_getwidthrect(rc),
                                                        _wpi_getheightrect(rc));
    _wpi_releasepres( HWND_DESKTOP, pres );
    GetObject( newbitmap, sizeof(BITMAP), &bm );

    _wpi_torgbmode( destpres );
    _wpi_torgbmode( srcpres );
    olddestbitmap = _wpi_selectobject( destpres, newbitmap );
    oldbitmap = _wpi_selectobject( srcpres, viewbitmap );
    height = node->height;
    width = node->width;

    IEStretchBlt( destpres, 0, 0, _wpi_getwidthrect(rc),
            _wpi_getheightrect(rc), srcpres, 0, 0, width, height,
            SRCCOPY, bm.bmBitsPixel );

    _wpi_selectobject( srcpres, oldbitmap );
    _wpi_deletecompatiblepres( srcpres, srcdc );
    _wpi_deleteobject( viewbitmap );

    _wpi_selectobject( destpres, olddestbitmap );
    _wpi_deletecompatiblepres( destpres, destdc );

    return( newbitmap );
} /* EnlargeImage */

/*
 * SetIsSaved - Sets whether the given mdi child has been saved.
 */
void SetIsSaved( HWND hwnd, BOOL fissaved )
{
    img_node    *node;
    img_node    *next_icon;
    char        fname[ _MAX_FNAME ];
    char        dir[ _MAX_DIR ];
    char        ext[ _MAX_EXT ];
    char        drive[ _MAX_DRIVE ];
    char        title[ _MAX_EXT + _MAX_FNAME + 2];
    char        *main_title;

    node = GetImageNode( hwnd );
    if (!node) return;

    // We do not check whether or not the image needs to have its
    // title bars updated here because sometimes we will save images
    // that are not modified to different files.
    #if 0
    if (fissaved == node->issaved) {
        return;
    }
    #endif

    next_icon = node;
    while (next_icon) {
        next_icon->issaved = fissaved;
        next_icon = next_icon->nexticon;
    }

    if( fissaved ) {
        _wpi_getwindowtext( _wpi_getframe(node->hwnd), title, 14 );
        if( ( strnicmp( title, IEImageUntitled, strlen( IEImageUntitled ) ) == 0 ) &&
            ( strnicmp( node->fname, IEImageUntitled, strlen( IEImageUntitled ) ) == 0 ) ) {
            return;
        }
        _splitpath( node->fname, drive, dir, fname, ext );

        strcpy( title, strupr(fname) );
        strcat( title, strupr(ext) );
        _wpi_setwindowtext( _wpi_getframe(node->hwnd), (LPSTR)title );

        main_title = (char *)
            MemAlloc( strlen( IEAppTitle ) + strlen( title ) + 3 + 1 );
        if( main_title ) {
            strcpy( main_title, IEAppTitle );
            strcat( main_title, " - " );
            strcat( main_title, title );
            _wpi_setwindowtext( HMainWindow, main_title );
            MemFree( main_title );
        }
    } else {
        _wpi_getwindowtext( _wpi_getframe(node->hwnd), title, 14 );
        if( strnicmp( title, IEImageUntitled, strlen( IEImageUntitled ) ) == 0 ) {
            return;
        } else if (title[strlen(title)-1] == '*') {
            return;
        } else {
            strcat( title, "*" );
            _wpi_setwindowtext( _wpi_getframe(node->hwnd), (LPSTR)title );
        }
    }
} /* SetIsSaved */

/*
 * OutlineRectangle - This routine outlines a rectangle with the xor pen.
 */
void OutlineRectangle( BOOL firsttime, WPI_PRES pres, WPI_RECT *prevrc,
                                                            WPI_RECT *newrc )
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
    holdbrush = _wpi_selectobject( pres, nullbrush );
    holdpen = _wpi_selectobject( pres, whitepen );

    prevrop2 = _wpi_setrop2( pres, R2_XORPEN );
    if (!firsttime) {
        _wpi_getintrectvalues( *prevrc, &left, &top, &right, &bottom );
        /*
         * In this case don't call _wpi_convertheight because of the
         * way the rectangle values are set in iedraw.c
         */
        _wpi_rectangle( pres, left, top, right, bottom );
    }

    _wpi_getintrectvalues( *newrc, &left, &top, &right, &bottom );
    _wpi_rectangle( pres, left, top, right, bottom );

    _wpi_selectobject( pres, holdbrush );
    _wpi_selectobject( pres, holdpen );
    _wpi_setrop2( pres, prevrop2 );
    _wpi_deleteobject( whitepen );
    _wpi_deletenullbrush( nullbrush );
} /* OutlineRectangle */

/*
 * GetFnameFromPath - given a full pathname, return just the file name.
 */
void GetFnameFromPath( char *fullpath, char *fname )
{
    char        filename[ _MAX_FNAME ];
    char        ext[ _MAX_EXT ];

    if( strnicmp( fullpath, IEImageUntitled, strlen( IEImageUntitled ) ) == 0 ) {
        strcpy( fname, fullpath );
        return;
    }
    _splitpath( fullpath, NULL, NULL, filename, ext );

    strcpy( fname, strupr(filename) );
    strcat( fname, strupr(ext) );

} /* GetFnameFromPath */

/*
 * GrayEditOptions - grays the edit options (at the beginning and when an
 *                   image is closed).
 */
void GrayEditOptions( void )
{
    HMENU       hmenu;

    if (!HMainWindow) return;
    hmenu = _wpi_getmenu( _wpi_getframe(HMainWindow) );

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
 * GetPosProc - used to get the position of all the child windows
 */
BOOL CALLBACK GetPosProc( HWND hwnd, LONG lparam )
{
    WPI_RECT    windowrect;
    WPI_POINT   topleft;        // this ends up being bottom left for PM
    WPI_RECTDIM left, top;

    lparam = lparam;

#ifdef __OS2_PM__
    if( windowIndex >= imageMax ) {
        return 0;
    }
#endif
    if ( _wpi_getowner(hwnd) ) {
        return 1;
    }

    if ( _wpi_isiconic(hwnd) ) {
        windowCoords[windowIndex].x = -1;
        windowCoords[windowIndex].y = -1;
        ++windowIndex;
        return 1;
    }

    _wpi_getwindowrect( hwnd, &windowrect );
    _wpi_getrectvalues( windowrect, &left, &top, NULL, NULL );
    topleft.x = left;
    topleft.y = top;

    _wpi_screentoclient( ClientWindow, &topleft );
    if (topleft.x < 0) {
        topleft.x = 0;
    }
#ifndef __OS2_PM__
    if (topleft.y < 0) {
        topleft.y = 0;
    }
#endif
    windowCoords[windowIndex] = topleft;
    ++windowIndex;
    return 1;
} /* GetPosProc */

/*
 * FindOrigin - This function "cascades" the windows to find the placement
 *              of the new mdi child.  Origin is expected to come in as 0,0
 *              or the equivalent for PM.
 */
void FindOrigin( WPI_POINT *new_origin )
{
    WPI_ENUMPROC        fp;
    int         image_count;
    int         i,j;
    WPI_POINT   temp;
    WPI_RECT    proposed;
    int         width;
    int         base;

    image_count = DoImagesExist();

    if (!image_count) return;

#ifdef __OS2_PM__
    imageMax = image_count;
#endif
    windowCoords = MemAlloc( image_count * sizeof(WPI_POINT) );
    windowIndex = 0;

    fp = _wpi_makeenumprocinstance( GetPosProc, Instance );
    _wpi_enumchildwindows( ClientWindow, fp, 0L );
    _wpi_freeprocinstance( fp );

    /*
     * I'm just using a simple bubble sort ... we're using small amounts of data
     */
    for (i=0; i < image_count; ++i) {
        for (j=0; j < image_count-i-1; ++j) {
            if ( windowCoords[j].x > windowCoords[j+1].x ) {
                temp = windowCoords[j];
                windowCoords[j] = windowCoords[j+1];
                windowCoords[j+1] = temp;
            }
        }
    }

    /*
     * minimized windows will have coordinates set to negative, and we only
     * want non-minimized windows.
     */
    for (base=0; base < image_count; ++ base) {
        if (windowCoords[base].x >= 0) {
            break;
        }
    }
    if (base >= image_count) {
        MemFree( windowCoords );
        return;
    }

    width = _wpi_getsystemmetrics( SM_CYCAPTION );
#ifndef __OS2_PM__
    _wpi_setintrectvalues(&proposed, new_origin->x, new_origin->y,
                                new_origin->x + width, new_origin->y + width);
#else
    _wpi_setrectvalues(&proposed, new_origin->x, new_origin->y - 1,
                                new_origin->x + width, new_origin->y + width-1);
#endif

    /*
     * try to place at the origin passed in if we can
     */
    if ( !_wpi_ptinrect(&proposed, windowCoords[base]) ) {
        MemFree( windowCoords );
        return;
    }

    for (i=base; i < image_count-1; ++i) {
        if ( windowCoords[i+1].x - windowCoords[i].x > 2*width ) {
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
    if ( !_wpi_ptinrect(&proposed, temp) ) {
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
 * SetMenus - sets the menu options for the new image.
  */
void SetMenus( img_node *node )
{
    HMENU       hmenu;

    hmenu = _wpi_getmenu( _wpi_getframe(HMainWindow) );
    _wpi_enablemenuitem( hmenu, IMGED_SNAP, TRUE, FALSE );
    _wpi_enablemenuitem( hmenu, IMGED_CUT, TRUE, FALSE );
    _wpi_enablemenuitem( hmenu, IMGED_COPY, TRUE, FALSE );
    _wpi_enablemenuitem( hmenu, IMGED_CLEAR, TRUE, FALSE );
#ifndef __OS2_PM__
    SetColourMenus( node );
#endif

    if (node->imgtype == BITMAP_IMG) {
        DisplayScreenClrs( FALSE );
        AddHotSpotTool( FALSE );
        _wpi_enablemenuitem( hmenu, IMGED_NEWIMG, FALSE, FALSE );
        _wpi_enablemenuitem( hmenu, IMGED_SELIMG, FALSE, FALSE );
        _wpi_enablemenuitem( hmenu, IMGED_DELIMG, FALSE, FALSE );
        _wpi_enablemenuitem( hmenu, IMGED_SIZE, TRUE, FALSE );
    } else if (node->imgtype == ICON_IMG) {
        DisplayScreenClrs( TRUE );
        AddHotSpotTool( FALSE );
        _wpi_enablemenuitem( hmenu, IMGED_SELIMG, TRUE, FALSE );
        if ( node->num_of_images < NUM_OF_ICONS ) {
            _wpi_enablemenuitem( hmenu, IMGED_NEWIMG, TRUE, FALSE );
        } else {
            _wpi_enablemenuitem( hmenu, IMGED_NEWIMG, FALSE, FALSE );
        }
        if ( node->num_of_images > 1 ) {
            _wpi_enablemenuitem( hmenu, IMGED_DELIMG, TRUE, FALSE );
            _wpi_enablemenuitem( hmenu, IMGED_SELIMG, TRUE, FALSE );
        } else {
            _wpi_enablemenuitem( hmenu, IMGED_DELIMG, FALSE, FALSE );
            _wpi_enablemenuitem( hmenu, IMGED_SELIMG, FALSE, FALSE );
        }
        _wpi_enablemenuitem( hmenu, IMGED_SIZE, FALSE, FALSE );
        SetIconInfo( node );
    } else if (node->imgtype == CURSOR_IMG) {
        DisplayScreenClrs( TRUE );
        AddHotSpotTool( TRUE );
        _wpi_enablemenuitem( hmenu, IMGED_NEWIMG, FALSE, FALSE );
        _wpi_enablemenuitem( hmenu, IMGED_SELIMG, FALSE, FALSE );
        _wpi_enablemenuitem( hmenu, IMGED_DELIMG, FALSE, FALSE );
        _wpi_enablemenuitem( hmenu, IMGED_SIZE, FALSE, FALSE );
    }
} /* SetMenus */

#if 0
/*      This routine is not necessary as far as I can see.
 * ConvertToDIBitmap - converts the device dependent bitmap to a DI bitmap
 */
void ConvertToDIBitmap( HBITMAP hbitmap )
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

    GetObject( hbitmap, sizeof(BITMAP), &bm );
    /*
     * Replace the fields on the info header with values for *this* bitmap.
     */
    bmi->bmiHeader.biWidth = bm.bmWidth;
    bmi->bmiHeader.biHeight = bm.bmHeight;
    if (bmi->bmiHeader.bmWidth > 32 && FALSE) {
        bmi->bmiHeader.biSizeImage = BITS_INTO_BYTES( bmi->bmiHeader.biBitCount *
                                                    bm.bmWidth, bm.bmHeight );
    } else {
        bmi->bmiHeader.biSizeImage = BITS_TO_BYTES( bmi->bmiHeader.biBitCount *
                                                    bm.bmWidth, bm.bmHeight );
    }
    bits = MemAlloc( bmi->bmiHeader.biSizeImage );

    hdc = GetDC( NULL );
    GetDIBits(hdc, hbitmap, 0, bmi->bmiHeader.biHeight, bits, bmi,
                                                        DIB_RGB_COLORS);
    SetDIBits( hdc, hbitmap, 0, bmi->bmiHeader.biHeight, bits, bmi,
                                                        DIB_RGB_COLORS);

    ReleaseDC( NULL, hdc );

    FreeDIBitmapInfo( bmi );
    MemFree( bits );
} /* ConvertToDIBitmap */
#endif
