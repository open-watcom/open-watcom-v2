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

static short    snapWidth;
static short    snapHeight;
static int      firstTime;
static POINT    prevTopLeft;
static int      prevToolType;
static int      previousState;
static HCURSOR  prevCursor;
static POINT    topLeft;
static BOOL     rectExists = FALSE;
#ifdef __NT__
static HWND     deskTopWindow;
#endif

// the following static variables are use to fix the snap bug
static HWND     snapWindow;

void AbortSnap( HWND hwnd );

#ifdef __NT__

/*
 * deskTopWindowHook
 */
static bool deskTopWindowHook( HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam )
{
    hwnd = hwnd;
    lparam = lparam;

    if( msg == WM_ACTIVATE ) {
        if( !LOWORD( wparam ) ) {
            AbortSnap( snapWindow );
        }
    }
    return( false );

} /* deskTopWindowHook */

#endif

/*
 * redrawPrevRegion - redraw the previously selected region
 */
static void redrawPrevRegion( void )
{
    int         prevROP2;
    HDC         hdc;
    HBRUSH      holdbrush;
    HPEN        holdpen;

    hdc = GetDC( NULL );
    prevROP2 = SetROP2( hdc, R2_XORPEN );

    holdbrush = SelectObject( hdc, GetStockObject( WHITE_BRUSH ) );
    holdpen = SelectObject( hdc, GetStockObject( WHITE_PEN ) );
    Rectangle( hdc, prevTopLeft.x, prevTopLeft.y,
                    prevTopLeft.x + snapWidth,
                    prevTopLeft.y + snapHeight );
    SelectObject( hdc, holdpen );
    SelectObject( hdc, holdbrush );

    SetROP2( hdc, prevROP2 );
    ReleaseDC( NULL, hdc );

} /* redrawPrevRegion */

/*
 * OutlineSnap - outline the area to snap
 */
void OutlineSnap( void )
{
    POINT       topleft;
    int         prevROP2;
    HBRUSH      hbrush;
    HBRUSH      holdbrush;
    HPEN        hwhitepen;
    HPEN        holdpen;
    HDC         hdc;

    if( firstTime == 2 ) {
        firstTime--;
        return;
    }

    if( firstTime == 0 ) {
        redrawPrevRegion();
    }

    hdc = GetDC( NULL );
    GetCursorPos( &topleft );
    prevROP2 = SetROP2( hdc, R2_XORPEN );

    hbrush = GetStockObject( WHITE_BRUSH );
    holdbrush = SelectObject( hdc, hbrush );
    hwhitepen = GetStockObject( WHITE_PEN );
    holdpen = SelectObject( hdc, hwhitepen );
    Rectangle( hdc, topleft.x, topleft.y, topleft.x + snapWidth, topleft.y + snapHeight );
    SelectObject( hdc, holdpen );
    SelectObject( hdc, holdbrush );

    SetROP2( hdc, prevROP2 );
    ReleaseDC( NULL, hdc );

    if( firstTime != 0 ) {
        firstTime--;
    }

    prevTopLeft = topleft;

} /* OutlineSnap */

/*
 * SnapPicture - minimize the image editor and prepare to snap the picture
 */
void SnapPicture( void )
{
    img_node    *node;
    POINT       pt;
#ifndef __NT__
    HWND        hwnd;
#endif
    RECT        cliprect;

    node = GetCurrentNode();
    if( node == NULL ) {
        return;
    }

    firstTime = 2;

    GetCursorPos( &pt );

    if( DoesRectExist( &cliprect ) ) {
        snapWidth = cliprect.right - cliprect.left;
        snapHeight = cliprect.bottom - cliprect.top;
        topLeft.x = cliprect.left;
        topLeft.y = cliprect.top;
        rectExists = TRUE;
    } else {
        snapWidth = node->width;
        snapHeight = node->height;
        topLeft.x = 0;
        topLeft.y = 0;
        rectExists = FALSE;
    }

    if( IsZoomed( HMainWindow ) ) {
        previousState = SW_SHOWMAXIMIZED;
    } else {
        previousState = SW_SHOWNORMAL;
    }

    snapWindow = node->hwnd;
#ifdef __NT__
    RegisterSnapClass( Instance );
    ShowWindow( HMainWindow, SW_SHOWMINIMIZED );
    ShowWindow( HMainWindow, SW_HIDE );
    InvalidateRect( HWND_DESKTOP, NULL, TRUE );
    // force the desktop window to be redrawn
    RedrawWindow( HWND_DESKTOP, NULL, NULL,
                  RDW_ERASE | RDW_UPDATENOW | RDW_ERASENOW |
                  RDW_ALLCHILDREN | RDW_INVALIDATE );
    SetDeskTopHook( deskTopWindowHook );
    deskTopWindow = DisplayDesktop( HMainWindow );
#else
    SetCapture( node->hwnd );
    ShowWindow( HMainWindow, SW_SHOWMINIMIZED );
    ShowWindow( HMainWindow, SW_HIDE );

    hwnd = WindowFromPoint( pt );
#if 0
    if( hwnd == GetDesktopWindow() ) {
        return;
    }
#endif
    // this code makes sure that the window under the cursor is redrawn
    // Why? gets rid of the XOR turd
    RedrawWindow( hwnd, NULL, NULL,
                  RDW_ERASE | RDW_UPDATENOW | RDW_ALLCHILDREN | RDW_INVALIDATE );
#endif

    prevToolType = SetToolType( IMGED_SNAP );
    prevCursor = SetCursor( NULL );

#ifdef __NT__
    SetCapture( node->hwnd );
#endif

    PostMessage( node->hwnd, WM_MOUSEMOVE, (WPARAM)0, MAKELPARAM( pt.x, pt.y ) );
    Yield();

} /* SnapPicture */

/*
 * TransferImage - transfer the image in the outlined box to the view window
 */
void TransferImage( HWND hwnd )
{
    POINT       topleft_desk;
    HDC         hdc;
    HDC         memdc;
    HBITMAP     oldbitmap;
    img_node    *node;

#ifdef __NT__
    SetDeskTopHook( NULL );
#endif
    redrawPrevRegion();

    GetCursorPos( &topleft_desk );
    node = SelectImage( hwnd );
    hdc = GetDC( NULL );
    memdc = CreateCompatibleDC( hdc );

    oldbitmap = SelectObject( memdc, node->handbitmap );
    PatBlt( memdc, topLeft.x, topLeft.y, snapWidth, snapHeight, BLACKNESS );

    SelectObject( memdc, node->hxorbitmap );
    BitBlt( memdc, topLeft.x, topLeft.y, snapWidth, snapHeight, hdc,
            topleft_desk.x, topleft_desk.y, SRCCOPY );
    ReleaseDC( NULL, hdc );
    SelectObject( memdc, oldbitmap );
    DeleteDC( memdc );
    ReleaseCapture();
    ShowWindow( HMainWindow, previousState );

#ifdef __NT__
    DestroyWindow( deskTopWindow );
#else
    InvalidateRect( node->viewhwnd, NULL, FALSE );
#endif
    RecordImage( hwnd );

    SetToolType( prevToolType );
    if( !DoKeepRect() ) {
        SetRectExists( FALSE );
    } else {
        SetRectExists( rectExists );
    }
#ifndef __NT__
    BlowupImage( node->hwnd, NULL );
#endif
    SetCursor( prevCursor );
    PrintHintTextByID( WIE_BMPSNAPPEDFROMDSKTOP, NULL );

} /* TransferImage */

/*
 * AbortSnap - abort the snapping of an image
 */
void AbortSnap( HWND hwnd )
{
    img_node    *node;

#ifdef __NT__
    SetDeskTopHook( NULL );
#endif

    node = SelectImage( hwnd );
    ReleaseCapture();
    ShowWindow( HMainWindow, previousState );

#ifdef __NT__
    DestroyWindow( deskTopWindow );
#else
    InvalidateRect( node->viewhwnd, NULL, FALSE );
#endif

    SetToolType( prevToolType );
    SetCursor( prevCursor );
    PrintHintTextByID( WIE_BMPSNAPABORTED, NULL );

} /* AbortSnap */
