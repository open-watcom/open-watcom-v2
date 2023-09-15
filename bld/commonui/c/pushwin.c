/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2015-2023 The Open Watcom Contributors. All Rights Reserved.
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


#include "commonui.h"
#include <string.h>
#include "watcom.h"
#include "wpi.h"
#include "bool.h"
#include "cguimem.h"
#include "pushwin.h"


/* Window callback functions prototypes */
WINEXPORT LRESULT CALLBACK PushWinProc( HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam );

#define WPI_GET_WNDINFO( w )    ((PushWinInfo *)_wpi_getwindowlongptr( w, 0 ))
#define WPI_SET_WNDINFO( w, d ) (_wpi_setwindowlongptr( w, 0, d ))

/*
 * PushWinProc - push window procedure
 */
LRESULT CALLBACK PushWinProc( HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam )
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

/*
 * RegPushWin - register the push window class
 */
bool RegPushWin( HANDLE instance )
{
    WNDCLASS    wc;

    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = GetWndProc( PushWinProc );
    wc.cbClsExtra = 0;
    wc.cbWndExtra = sizeof( LONG_PTR );
    wc.hInstance = instance;
    wc.hIcon = NULLHANDLE;
    wc.hCursor = LoadCursor( NULLHANDLE, IDC_ARROW );
    wc.hbrBackground = (HBRUSH)(pointer_uint)(COLOR_WINDOW + 1);
    wc.lpszMenuName = NULL;
    wc.lpszClassName = "PushWin";
    return( RegisterClass( &wc ) != 0 );

} /* RegPushWin */

/*
 * CreatePushWin - create a push window
 */
HWND CreatePushWin( HWND parent, char *txt, int id, HFONT font, HANDLE inst )
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
        NULLHANDLE,             /* Window menu handle */
        inst,                   /* Program instance handle */
        (LPVOID)info );         /* Create parameters */
    return( hwnd );

} /* CreatePushWin */
