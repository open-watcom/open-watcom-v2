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


#include <assert.h>
#include "winvi.h"
#include "color.h"
#include "font.h"
#include "utils.h"
#include "watcom.h"

static BOOL Init( window *, void * );
static BOOL Fini( window *, void * );

window FileCompleteWindow = {
    &filecw_info,
    { 0, 0, 0, 0 },
    Init,
    Fini
};

extern void FileCompleteMouseClick( HWND, int, int, BOOL );
LONG WINEXP FileCompleteWindowProc( HWND, unsigned, UINT, LONG );

static char *ClassName = "FileCompleteWindow";

static BOOL Init( window *w, void *parm )
{
    WNDCLASS        wc;

    w = w;
    parm = parm;
    wc.style = CS_DBLCLKS;
    wc.lpfnWndProc = (LPVOID) FileCompleteWindowProc;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = sizeof( LPVOID );
    wc.hInstance = InstanceHandle;
    wc.hIcon = LoadIcon( (HINSTANCE)NULL, IDI_APPLICATION );
    wc.hCursor = LoadCursor( (HINSTANCE)NULL, IDC_ARROW );
    wc.hbrBackground = (HBRUSH) COLOR_APPWORKSPACE;
    wc.lpszMenuName = NULL;
    wc.lpszClassName = ClassName;
    return( RegisterClass( &wc ) );
}

static BOOL Fini( window *w, void *parm )
{
    w = w;
    parm = parm;
    return( TRUE );
}

LONG WINEXP FileCompleteWindowProc( HWND hwnd, unsigned msg, UINT w, LONG l )
{
    switch( msg ) {
    case WM_KEYDOWN:
        if( CommandId != NO_WINDOW ) {
            SetFocus( CommandId );
            SendMessage( CommandId, msg, w, l );
            return( TRUE );
        }
        break;
    case WM_ERASEBKGND:
        return( TRUE );
    case WM_CREATE:
        SetWindowLong( hwnd, 0, (LONG)(LPSTR)&FileCompleteWindow );
        break;
    case WM_LBUTTONDBLCLK:
    case WM_MBUTTONDBLCLK:
    case WM_RBUTTONDBLCLK:
        FileCompleteMouseClick( hwnd, (int)(signed_16)LOWORD( l ),
                                (int)(signed_16)HIWORD( l ), TRUE );
        break;
    case WM_LBUTTONUP:
    case WM_MBUTTONUP:
    case WM_RBUTTONUP:
        FileCompleteMouseClick( hwnd, (int)(signed_16)LOWORD( l ),
                                (int)(signed_16)HIWORD( l ), FALSE );
        break;
    }
    return( DefWindowProc( hwnd, msg, w, l ) );
}

window_id NewFileCompleteWindow( void )
{
    RECT        *size;
    HWND        dir;
    POINT       p;

    size = &FileCompleteWindow.area;
    p.x = size->left;
    p.y = size->top;
    ClientToScreen( Root, &p );
    dir = CreateWindow( ClassName, "File Complete",
        WS_POPUPWINDOW | WS_CLIPSIBLINGS | WS_BORDER,
        p.x, p.y,
        size->right - size->left, size->bottom - size->top, Root,
        (HMENU)NULL, InstanceHandle, NULL );
    ShowWindow( dir, SW_SHOWNORMAL );
    UpdateWindow( dir );
    return( dir );
}
