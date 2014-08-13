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
#ifdef __NT__
    #include "desknt.h"
#endif

static UINT     prevState;
static RECT     bmpRegion;
static BOOL     notDestroyed = TRUE;
#ifdef __NT__
static HWND     deskTopWindow;
#endif

/*
 * checkRectBounds - make sure that the rectangle isn't bigger than 512x512
  */
static void checkRectBounds( RECT *rect )
{
    short       width;
    short       height;

    if( rect->right > rect->left ) {
        width = (short)(rect->right - rect->left);
        if( width > MAX_DIM ) {
            rect->right = rect->left + MAX_DIM;
        }
    } else {
        width = (short)(rect->left - rect->right);
        if( width > MAX_DIM ) {
            rect->right = rect->left - MAX_DIM;
        }
    }

    if( rect->bottom > rect->top ) {
        height = (short)(rect->bottom - rect->top);
        if( height > MAX_DIM ) {
            rect->bottom = rect->top + MAX_DIM;
        }
    } else {
        height = (short)(rect->top - rect->bottom);
        if( height > MAX_DIM ) {
            rect->bottom = rect->top - MAX_DIM;
        }
    }

} /* checkRectBounds */

/*
 * SelectDynamicBitmap - let the user select the bitmap from the screen
 */
BOOL SelectDynamicBitmap( img_node *node, int imgcount, char *filename )
{
    HDC         hdc;
    HDC         memdc;
    HBITMAP     oldbitmap;
    HWND        bitmappickwindow;
    MSG         msg;
    RECT        screen_coords;

    if( IsZoomed( HMainWindow ) ) {
        prevState = SW_SHOWMAXIMIZED;
    } else {
        prevState = SW_SHOWNORMAL;
    }
#ifdef __NT__
    RegisterSnapClass( Instance );
    ShowWindow( HMainWindow, SW_SHOWMINIMIZED );
    ShowWindow( HMainWindow, SW_HIDE );
    deskTopWindow = DisplayDesktop( HMainWindow );
#else
    ShowWindow( HMainWindow, SW_SHOWMINIMIZED );
    ShowWindow( HMainWindow, SW_HIDE );
#endif

    bitmappickwindow = CreateWindow(
        BitmapPickClass,    /* Window class name */
        "",                 /* Window caption */
        WS_CHILDWINDOW,     /* Window style */
        0,                  /* Initial X position */
        0,                  /* Initial Y position */
        0,                  /* Initial X size */
        0,                  /* Initial Y size */
        HMainWindow,        /* Parent window handle */
        (HMENU)NULL,        /* Window menu handle */
        Instance,           /* Program instance handle */
        NULL );             /* Create parameters */

    if( bitmappickwindow == NULL ) {
        return( FALSE );
    }

    while( notDestroyed && GetMessage( &msg, bitmappickwindow, 0, 0 ) ) {
        TranslateMessage( &msg );
        DispatchMessage( &msg );
    }

    if( bmpRegion.right > bmpRegion.left ) {
        screen_coords.left = bmpRegion.left;
        screen_coords.right = bmpRegion.right;
    } else {
        screen_coords.left = bmpRegion.right;
        screen_coords.right = bmpRegion.left;
    }
    if( bmpRegion.bottom > bmpRegion.top ) {
        screen_coords.bottom = bmpRegion.bottom;
        screen_coords.top = bmpRegion.top;
    } else {
        screen_coords.bottom = bmpRegion.top;
        screen_coords.top = bmpRegion.bottom;
    }
    if( screen_coords.right - screen_coords.left == 0 ||
        screen_coords.bottom - screen_coords.top == 0 ) {
        ShowWindow( HMainWindow, prevState );
#ifdef __NT__
        DestroyWindow( deskTopWindow );
#endif
        IEDisplayErrorMsg( WIE_APPNAME, WIE_INVALIDREGIONSELECTED,
                           MB_OK | MB_ICONINFORMATION );
        notDestroyed = TRUE;
        return( FALSE );
    }

    node->imgtype = BITMAP_IMG;
    node->width = (short)(screen_coords.right-screen_coords.left);
    node->height = (short)(screen_coords.bottom-screen_coords.top);
    node->bitcount = 4;                 // ?????
    node->hotspot.x = 0;
    node->hotspot.y = 0;
    node->num_of_images = 1;
    node->nexticon = NULL;
    node->issaved = FALSE;
    node->next = NULL;
    if( filename != NULL ) {
        strcpy( node->fname, filename );
    } else {
        sprintf( node->fname, "%s (%d)", IEImageUntitled, imgcount );
    }

    MakeBitmap( node, TRUE );

    hdc = GetDC( NULL );
    memdc = CreateCompatibleDC( hdc );

    oldbitmap = SelectObject( memdc, node->handbitmap );
    PatBlt( memdc, 0, 0, node->width, node->height, BLACKNESS );

    SelectObject( memdc, node->hxorbitmap );
    BitBlt( memdc, 0, 0, node->width, node->height, hdc, screen_coords.left,
            screen_coords.top, SRCCOPY );
    ReleaseDC( NULL, hdc );
    SelectObject( memdc, oldbitmap );
    DeleteDC( memdc );

    ShowWindow( HMainWindow, prevState );
#ifdef __NT__
    DestroyWindow( deskTopWindow );
#endif
    notDestroyed = TRUE;
    return( TRUE );

} /* SelectDynamicBitmap */

/*
 * BitmapPickProc - handle messages for choosing the region to begin the
 *                  editing session
 */
LRESULT CALLBACK BitmapPickProc( HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam )
{
    static POINT        topleft;
    static POINT        bottomright;
    static RECT         prevpos;
    static bool         firsttime = true;
    static BOOL         buttondown;
    static HCURSOR      prevcursor;
    static HCURSOR      crosshairs;
    HDC                 hdc;

    switch ( msg ) {
    case WM_CREATE:
        crosshairs = LoadCursor( Instance, CROSSHAIRSCUR );
        prevcursor = SetCursor( crosshairs );
        SetCapture( hwnd );
        firsttime = true;
        SetRect( &prevpos, 0, 0, 0, 0 );
        break;

    case WM_LBUTTONDOWN:
        GetCursorPos( &topleft );
        buttondown = TRUE;
        break;

    case WM_LBUTTONUP:
        GetCursorPos( &bottomright );
        SetRect( &bmpRegion, topleft.x, topleft.y, bottomright.x, bottomright.y );
        checkRectBounds( &bmpRegion );
        hdc = GetDC( NULL );
        OutlineRectangle( true, hdc, &prevpos, &bmpRegion );
        ReleaseDC( NULL, hdc );
        buttondown = FALSE;
        ReleaseCapture();
        SendMessage( hwnd, WM_CLOSE, 0, 0L );
        break;

    case WM_MOUSEMOVE:
        if( buttondown ) {
            GetCursorPos( &bottomright );
            SetRect( &bmpRegion, topleft.x, topleft.y, bottomright.x, bottomright.y );
            checkRectBounds( &bmpRegion );
            hdc = GetDC( NULL );
            OutlineRectangle( firsttime, hdc, &prevpos, &bmpRegion );
            ReleaseDC( NULL, hdc );
            firsttime = false;
            prevpos = bmpRegion;
        }
        break;

    case WM_DESTROY:
        notDestroyed = FALSE;
        SetCursor( prevcursor );
        DestroyCursor( crosshairs );
        break;

    default:
        return( DefWindowProc( hwnd, msg, wparam, lparam ) );
    }
    return( 0 );

} /* BitmapPickProc */
