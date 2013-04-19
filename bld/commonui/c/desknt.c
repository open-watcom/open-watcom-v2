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
* Description:  Display desktop background bitmap.
*
****************************************************************************/


#include "precomp.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "wi163264.h"
#include "desknt.h"

static HBITMAP  deskTopBitmap;
static short    screenWidth;
static short    screenHeight;
static HANDLE   thisInstance;

static deskNThook deskTopHook = NULL;

void SetDeskTopHook( deskNThook hook )
{
    deskTopHook = hook;
}

/*
 * DesktopProc - creates and displays the bitmap for the desktop (NT version).
 */
WINEXPORT LRESULT CALLBACK DesktopProc( HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam )
{
    static HBITMAP      oldbmp;
    PAINTSTRUCT         ps;
    HDC                 hdc;
    HDC                 memdc;

    if( deskTopHook ) {
        if( deskTopHook( hwnd, msg, wparam, lparam ) ) {
            return( 0 );
        }
    }

    switch ( msg ) {
    case WM_ERASEBKGND:
        break;
    case WM_CREATE:
        hdc = GetDC( hwnd );
        memdc = CreateCompatibleDC( hdc );
        oldbmp = SelectObject( memdc, deskTopBitmap );
        BitBlt( hdc, 0, 0, screenWidth, screenHeight, memdc, 0, 0, SRCCOPY );
        SelectObject( memdc, oldbmp );
        ReleaseDC( hwnd, hdc );
        DeleteDC( memdc );
        break;
    case WM_PAINT:
        hdc = BeginPaint( hwnd, &ps );
        memdc = CreateCompatibleDC( hdc );
        oldbmp = SelectObject( memdc, deskTopBitmap );
        BitBlt( hdc, 0, 0, screenWidth, screenHeight, memdc, 0, 0, SRCCOPY );
        SelectObject( memdc, oldbmp );
        DeleteDC( memdc );
        EndPaint( hwnd, &ps );
        break;
    case WM_DESTROY:
        DeleteObject( deskTopBitmap );
        break;
    default:
        return( DefWindowProc( hwnd, msg, wparam, lparam ) );
    }
    return( 0 );

} /* DesktopProc */

/*
 * RegisterSnapClass - register the window class used to display the desktop
 */
BOOL RegisterSnapClass( HANDLE instance )
{
    WNDCLASS    wc;

    thisInstance = instance;
    wc.style = 0L;
#if defined( __WINDOWS_386__ )
    wc.lpfnWndProc = (LPVOID)DesktopProc;
#else
    wc.lpfnWndProc = DesktopProc;
#endif
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hInstance = thisInstance;
    wc.hIcon = (HICON)NULL;
    wc.hCursor = (HCURSOR)NULL;
    wc.hbrBackground = (HBRUSH)NULL;
    wc.lpszMenuName = NULL;
    wc.lpszClassName = "SnapClass";
    if( !RegisterClass( &wc ) ) {
        return( FALSE );
    }
    return( TRUE );

} /* RegisterSnapClass */

/*
 * getDesktopBitmap - returns the bitmap for the entire desktop
 */
static void getDesktopBitmap( void )
{
    HDC         hdc;
    HDC         memdc;
    HBITMAP     oldbmp;

    hdc = GetDC( NULL );
    memdc = CreateCompatibleDC( hdc );

    deskTopBitmap = CreateCompatibleBitmap( hdc, screenWidth, screenHeight );
    oldbmp = SelectObject( memdc, deskTopBitmap );
    BitBlt( memdc, 0, 0, screenWidth, screenHeight, hdc, 0, 0, SRCCOPY );

    SelectObject( memdc, oldbmp );
    DeleteDC( memdc );
    ReleaseDC( NULL, hdc );

} /* getDesktopBitmap */

/*
 * createDeskWindow - create a window the size of the entire screen that
 *                    gets the desktop bitmap blted on it
 */
static HWND createDeskWindow( HWND hparent )
{
    HWND        hwnd;

    hwnd = CreateWindow(
        "SnapClass",                            /* Window class name */
        "",                                     /* Window caption */
        WS_POPUP | WS_VISIBLE,
        0,                                      /* Initial X position */
        0,                                      /* Initial Y position */
        screenWidth,                            /* Initial X size */
        screenHeight,                           /* Initial Y size */
        hparent,                                /* Parent window handle */
        (HMENU)NULL,                            /* Window menu handle */
        thisInstance,                           /* Program instance handle */
        NULL );                                 /* Create parameters */

    return( hwnd );

} /* createDeskWindow */

/*
 * DisplayDesktop - display the desktop on the screen
 */
HWND DisplayDesktop( HWND hparent )
{
    HWND        desktopwindow;
    HCURSOR     prevcursor;

    screenWidth = (short)GetSystemMetrics( SM_CXSCREEN );
    screenHeight = (short)GetSystemMetrics( SM_CYSCREEN );

    prevcursor = SetCursor( LoadCursor(NULL, IDC_WAIT) );
    getDesktopBitmap();

    desktopwindow = createDeskWindow( hparent );

    RedrawWindow( desktopwindow, NULL, NULL, RDW_UPDATENOW | RDW_ALLCHILDREN );
    SetCursor( prevcursor );
    SetWindowPos( desktopwindow, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE );
    return( desktopwindow );

} /* DisplayDesktop */
