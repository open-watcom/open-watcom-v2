/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2015-2016 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  File completion window.
*
****************************************************************************/


#include "vi.h"
#include "color.h"
#include "font.h"
#include "utils.h"
#include "wprocmap.h"
#include "winifini.h"


/* Local Windows CALLBACK function prototypes */
WINEXPORT LRESULT CALLBACK FileCompleteWindowProc( HWND, UINT, WPARAM, LPARAM );

window FileCompleteWindow = {
    &filecw_info,
    { 0, 0, 0, 0 }
};

static char *ClassName = "FileCompleteWindow";

bool FileCompleteWindowInit( void )
{
    WNDCLASS        wc;

    wc.style = CS_DBLCLKS;
    wc.lpfnWndProc = GetWndProc( FileCompleteWindowProc );
    wc.cbClsExtra = 0;
    wc.cbWndExtra = sizeof( LONG_PTR );
    wc.hInstance = InstanceHandle;
    wc.hIcon = LoadIcon( (HINSTANCE)NULLHANDLE, IDI_APPLICATION );
    wc.hCursor = LoadCursor( (HINSTANCE)NULLHANDLE, IDC_ARROW );
    wc.hbrBackground = (HBRUSH) COLOR_APPWORKSPACE;
    wc.lpszMenuName = NULL;
    wc.lpszClassName = ClassName;
    return( RegisterClass( &wc ) != 0 );
}

bool FileCompleteWindowFini( void )
{
    return( true );
}

WINEXPORT LRESULT CALLBACK FileCompleteWindowProc( HWND hwnd, UINT msg, WPARAM w, LPARAM l )
{
    switch( msg ) {
    case WM_KEYDOWN:
        if( !BAD_ID( command_window_id ) ) {
            SetFocus( command_window_id );
            SendMessage( command_window_id, msg, w, l );
            return( 0 );
        }
        break;
    case WM_ERASEBKGND:
        return( TRUE );
    case WM_CREATE:
        SET_WNDINFO( hwnd, (LONG_PTR)&FileCompleteWindow );
        break;
    case WM_LBUTTONDBLCLK:
    case WM_MBUTTONDBLCLK:
    case WM_RBUTTONDBLCLK:
        FileCompleteMouseClick( hwnd, (int)(short)LOWORD( l ), (int)(short)HIWORD( l ), true );
        break;
    case WM_LBUTTONUP:
    case WM_MBUTTONUP:
    case WM_RBUTTONUP:
        FileCompleteMouseClick( hwnd, (int)(short)LOWORD( l ), (int)(short)HIWORD( l ), false );
        break;
    }
    return( DefWindowProc( hwnd, msg, w, l ) );
}

window_id NewFileCompleteWindow( void )
{
    RECT        *size;
    window_id   wid;
    POINT       p;

    size = &FileCompleteWindow.def_area;
    p.x = size->left;
    p.y = size->top;
    ClientToScreen( root_window_id, &p );
    wid = CreateWindow( ClassName, "File Complete",
                        WS_POPUPWINDOW | WS_CLIPSIBLINGS | WS_BORDER,
                        p.x, p.y,
                        size->right - size->left, size->bottom - size->top, root_window_id,
                        (HMENU)NULLHANDLE, InstanceHandle, NULL );
    ShowWindow( wid, SW_SHOWNORMAL );
    UpdateWindow( wid );
    return( wid );
}
