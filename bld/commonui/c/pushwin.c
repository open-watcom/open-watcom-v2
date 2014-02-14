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
* Description:  Push window class.
*
****************************************************************************/


#include "precomp.h"
#include <string.h>
#include "wpi.h"
#include "mem.h"
#include "pushwin.h"

#define WPI_GET_WNDINFO( w )    ((PushWinInfo *)_wpi_getwindowlongptr( w, 0 ))
#define WPI_SET_WNDINFO( w, d ) (_wpi_setwindowlongptr( w, 0, d ))

/*
 * PushWinProc - push window procedure
 */
WINEXPORT LRESULT CALLBACK PushWinProc( HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam )
{
    PushWinInfo         *info;
    PAINTSTRUCT         paint;
    HFONT               old_font;
    RECT                area;

    info = WPI_GET_WNDINFO( hwnd );
    switch( msg ) {
    case WM_CREATE:
        info = (PushWinInfo *)((CREATESTRUCT *)lparam)->lpCreateParams;
        WPI_SET_WNDINFO( hwnd, (LONG_PTR)info );
        break;
    case WM_PAINT:
        BeginPaint( hwnd, &paint );
        old_font = SelectObject( paint.hdc, info->font );
        GetWindowRect( hwnd, &area );
        area.bottom -= area.top;
        area.top = 0;
        area.right -= area.left;
        area.left = 0;
        DrawText( paint.hdc, info->str, -1, &area, DT_LEFT );
        SelectObject( paint.hdc, old_font );
        EndPaint( hwnd, &paint );
        break;
    case WM_LBUTTONDOWN:
        SendMessage( info->parent, WM_COMMAND, info->id, 0L );
        break;
    case WM_DESTROY:
        MemFree( info );
        break;
    default:
        return( DefWindowProc( hwnd, msg, wparam, lparam ) );
    }
    return( TRUE );

} /* PushWinProc */

static BOOL _RegPushWin( HANDLE instance, WNDPROCx fn )
{
    WNDCLASS    wc;

    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = (WNDPROC)fn;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = sizeof( LONG_PTR );
    wc.hInstance = instance;
    wc.hIcon = NULL;
    wc.hCursor = LoadCursor( NULL, IDC_ARROW );
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wc.lpszMenuName = NULL;
    wc.lpszClassName = "PushWin";
    if( !RegisterClass( &wc ) ) {
        return( FALSE );
    }
    return( TRUE );

} /* _RegPushWin */

/*
 * RegPushWin - register the push window class
 */
BOOL RegPushWin( HANDLE instance )
{
    return( _RegPushWin( instance, PushWinProc ) );
}

/*
 * CreatePushWin - create a push window
 */
HWND CreatePushWin( HWND parent, char *txt, WORD id, HFONT font, HANDLE inst )
{
    HWND                hwnd;
    PushWinInfo         *info;
    size_t              len;

    len = strlen( txt ) + 1;
    info = MemAlloc( sizeof( PushWinInfo ) + len );
    info->id = id;
    info->font = font;
    info->parent = parent;
    strcpy( info->str, txt );
    hwnd = CreateWindow(
        "PushWin",              /* Window class name */
        "",                     /* Window caption */
        WS_CHILD,               /* Window style */
        0,                      /* Initial X position */
        0,                      /* Initial Y position */
        0,                      /* Initial X size */
        0,                      /* Initial Y size */
        parent,                 /* Parent window handle */
        NULL,                   /* Window menu handle */
        inst,                   /* Program instance handle */
        info );                 /* Create parameters */
    return( hwnd );

} /* CreatePushWin */
