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
* Description:  Display a bitmap resource.
*
****************************************************************************/


#include <stdio.h>
#include <windows.h>
#include "restest.h"
#include "resname.h"

#define BITMAP_WND_CLASS                "BitmapWindowClass"
static char             bitmapName[256];
static HDC              memDc;
static HBITMAP          oldBm;
static BITMAP           bmInfo;

BOOL __export FAR PASCAL BitmapWndProc( HWND hwnd, UINT msg, UINT wparam,
                                    LONG lparam )
{
    HBITMAP     new;
    PAINTSTRUCT paintinfo;

    switch( msg ) {
    case WM_PAINT:
        BeginPaint( hwnd, &paintinfo );
        BitBlt( paintinfo.hdc, 0, 0, bmInfo.bmWidth, bmInfo.bmHeight,
                memDc, 0, 0, SRCCOPY );
        EndPaint( hwnd, &paintinfo );
        break;
    case WM_DESTROY:
        new = SelectObject( memDc, oldBm );
        DeleteDC( memDc );
        DeleteObject( new );
        break;
    default:
        return( DefWindowProc( hwnd, msg, wparam, lparam ) );
    }
    return( FALSE );
}

BOOL RegisterBitmapClass( void ) {
    WNDCLASS    wc;

    /* fixed window */
    wc.style = 0L;
    wc.lpfnWndProc = (LPVOID) BitmapWndProc;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 4;
    wc.hInstance = Instance;
    wc.hIcon = NULL;
    wc.hCursor = LoadCursor( NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH) (COLOR_WINDOW + 1);
    wc.lpszMenuName = NULL;
    wc.lpszClassName = BITMAP_WND_CLASS;
    if( !RegisterClass( &wc ) ) return( FALSE );

    return( TRUE );
}

BOOL CALLBACK GetBitmapNameDlgProc( HWND hwnd, UINT msg, UINT wparam, DWORD lparam )
{
    lparam = lparam;
    switch( msg ) {
    case WM_COMMAND:
        if( LOWORD( wparam ) == IDOK ) {
            GetDlgItemText( hwnd, INPUT_FIELD, bitmapName, sizeof( bitmapName ) );
            EndDialog( hwnd, 0 );
        }
        break;
    default:
        return( FALSE );
        break;
    }
    return( TRUE );
}

void doDisplayBitmap( HBITMAP newbm )
{
    HDC         windc;

    windc = GetDC( MainHwnd );
    memDc = CreateCompatibleDC( windc );
    oldBm = SelectObject( memDc, newbm );
    ReleaseDC( MainHwnd, windc );
    GetObject( newbm, sizeof( BITMAP ), &bmInfo );
}

void DisplayBitmap( void )
{
    FARPROC     fp;
    HBITMAP     bitmap;
    char        buf[256];
    HWND        hwnd;

    fp = MakeProcInstance( (FARPROC)GetBitmapNameDlgProc, Instance );
    DialogBox( Instance, "GET_RES_NAME_DLG" , NULL, (DLGPROC)fp );
    FreeProcInstance( fp );
    bitmap = LoadBitmap( Instance, bitmapName );
    if( bitmap == NULL ) {
        sprintf( buf, "Can't Load bitmap %s", bitmapName );
        Error( "bitmap", buf );
        return;
    }
    doDisplayBitmap( bitmap );
    hwnd = CreateWindow(
        BITMAP_WND_CLASS,       /* Window class name */
        "Bitmap display window",/* Window caption */
        WS_OVERLAPPEDWINDOW,    /* Window style */
        CW_USEDEFAULT,          /* Initial X position */
        CW_USEDEFAULT,          /* Initial Y position */
        500,                    /* Initial X size */
        200,                    /* Initial Y size */
        NULL,                   /* Parent window handle */
        NULL,                   /* Window menu handle */
        Instance,               /* Program instance handle */
        NULL );                 /* Create parameters */
    if( hwnd == NULL ) return;
    ShowWindow( hwnd, SW_SHOW );
    UpdateWindow( hwnd );
}
