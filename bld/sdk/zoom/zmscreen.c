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


#include "wzoom.h"
#include <string.h>


ScreenBitmap *InitScreenBitmap( HWND display ) {

    ScreenBitmap        *info;
    HDC                 dc;

    info = MemAlloc( sizeof( ScreenBitmap ) );
    memset( info, 0, sizeof( ScreenBitmap ) );
    dc = GetDC( display );
    info->memdc = CreateCompatibleDC( dc );
    info->hwnd = display;
    ReleaseDC( display, dc );
    return( info );
}

/*
 * trimScreen - blacken any areas of the bitmap that came from off the edge
 *              of the screen
 */
static void trimScreen( ScreenBitmap *info, POINT *pos, POINT *size ) {

    int         screenx;
    int         screeny;

    screenx = GetSystemMetrics( SM_CXSCREEN );
    screeny = GetSystemMetrics( SM_CYSCREEN );
    if( pos->x < 0 ) {
        BitBlt( info->memdc, 0, 0, -pos->x, size->y, NULL, 0, 0, BLACKNESS );
    }
    if( pos->y < 0 ) {
        BitBlt( info->memdc, 0, 0, size->x, -pos->y, NULL, 0, 0, BLACKNESS );
    }
    if( pos->x + size->x > screenx ) {
        BitBlt( info->memdc, screenx - pos->x, 0,
                size->x + pos->x - screenx, size->y , NULL, 0, 0, BLACKNESS );
    }
    if( pos->y + size->y > screeny ) {
        BitBlt( info->memdc, 0, screeny - pos->y,
                size->x, size->y + pos->y - screeny , NULL, 0, 0, BLACKNESS );
    }
}

/*
 * GetScreen - get a bitmap representation of the screen
 */
void GetScreen( ScreenBitmap *info, POINT *pos, POINT *size, BOOL trim ) {

    HBITMAP     old;
    HBITMAP     old_screen;
    HDC         dc;

    dc = GetDC( NULL );
    old_screen = info->screen;
    info->screen = CreateCompatibleBitmap( dc, size->x, size->y );
    old = SelectObject( info->memdc, info->screen );
    BitBlt( info->memdc, 0, 0, size->x, size->y, dc, pos->x, pos->y,
            SRCCOPY );
    ReleaseDC( NULL, dc );
    if( old_screen != NULL ) DeleteObject( old_screen );
    if( info->old_bitmap == NULL ) info->old_bitmap = old;
    if( trim ) {
        trimScreen( info, pos, size );
    }
}

/*
 * DrawScreen - draw the screen bitmap into a window
 */

void DrawScreen( ScreenBitmap *info, HDC dc, POINT *dstpos, POINT *dstsize,
                POINT *srcpos, POINT *srcsize ) {

    BOOL        own_dc;

    own_dc = FALSE;
    if( dc == NULL ) {
        dc = GetDC( info->hwnd );
        own_dc = TRUE;
    }
    StretchBlt( dc, dstpos->x, dstpos->y, dstsize->x, dstsize->y,
                info->memdc, srcpos->x, srcpos->y,
                srcsize->x, srcsize->y, SRCCOPY );
    if( own_dc ) ReleaseDC( info->hwnd, dc );
}

/*
 * CopyToClipBoard - copy a bitmap to the clip board
 */

void CopyToClipBoard( MainWndInfo *info ) {

    HBITMAP     copy;
    HBITMAP     old_bitmap;
    HDC         dc;
    HDC         mdc;
    POINT       origin = { 0, 0 };



    /* clone the bitmap */
    dc = info->screen->memdc;
    mdc = CreateCompatibleDC( dc );
    copy = CreateCompatibleBitmap( dc, info->wndsize.x, info->wndsize.y );
    old_bitmap = SelectObject( mdc, copy );

    DrawScreen( info->screen, mdc, &origin, &info->wndsize,
                    &origin, &info->magsize );

    SelectObject( mdc, old_bitmap );
    DeleteDC( mdc );

    /* add the bitmap */
    OpenClipboard( info->screen->hwnd );
    EmptyClipboard();
    SetClipboardData( CF_BITMAP, copy );
    CloseClipboard();
}

/*
 * FiniScreenBitmap -clean up
 */

void FiniScreenBitmap( ScreenBitmap *info ) {

    if( info->old_bitmap != NULL ) {
        SelectObject( info->memdc, info->old_bitmap );
    }
    if( info->screen != NULL ) {
        DeleteObject( info->screen );
    }
    DeleteDC( info->memdc );
    MemFree( info );
}
