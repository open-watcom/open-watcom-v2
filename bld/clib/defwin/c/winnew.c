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


#include "variety.h"
#include <string.h>
#include <stdio.h>
#include <stdarg.h>
#include "win.h"

/*
 * _NewWindow - create a new window
 */
unsigned _NewWindow( char *name, ... )
{
    HWND        hwnd;
    LPWDATA     w;
    WORD        xchar,ychar;
    HDC         dc;
    TEXTMETRIC  tm;
    char        str[80];
    int         x1,x2,y1,y2;
    va_list     al;

    _GetWindowNameAndCoords( name, str, &x1, &x2, &y1, &y2 );

    hwnd = CreateWindow(
        _ClassName,                         /* our class                  */
        str,                                /* Text for window title bar. */
        WS_CHILD |  WS_VSCROLL | WS_CAPTION | WS_THICKFRAME |
        WS_CLIPSIBLINGS | WS_SYSMENU | WS_MAXIMIZEBOX |
        WS_MINIMIZEBOX,                     /* Window style.       */
        0,                                  /* horizontal position.       */
        0,                                  /* vertical position.         */
        0,                                  /* width.                     */
        0,                                  /* height.                    */
        _MainWindow,                        /* parent                     */
        NULL,                               /* menu handle                */
        _MainWindowData->inst,              /* owner of window            */
        NULL                                /* extra data pointer         */
    );

    if( !hwnd ) return( FALSE );
    /*
     * allocate window data area
     */
    va_start( al, name );
    w = _AnotherWindowData( hwnd, al );

    /*
     * set up data
     */
    dc = GetDC( hwnd );
    w->text_color = BLACK;
    w->background_color = BRIGHT_WHITE;
    w->brush = CreateSolidBrush( _ColorMap[BRIGHT_WHITE] );
    _SetMyDC( dc, 0, 0 );
    GetTextMetrics( dc, &tm );
    ReleaseDC( hwnd, dc );
    xchar = tm.tmMaxCharWidth;
    ychar = tm.tmHeight;
    w->xchar = xchar;
    w->ychar = ychar+2;
    w->inst = _MainWindowData->inst;
    _PositionScrollThumb( w );
    AppendMenu( _SubMenuWindows, MF_ENABLED, MSG_WINDOWS+w->handles[0], str );

    MoveWindow( hwnd,x1*xchar,y1*ychar, x2*xchar, y2*ychar, TRUE );
    DeleteMenu( GetSystemMenu( hwnd, 0 ), SC_CLOSE, MF_BYCOMMAND );
    ShowWindow( hwnd, SW_NORMAL );
    UpdateWindow( hwnd );
    _MakeWindowActive( w );
    return( TRUE );

} /* _NewWindow */

/*
 * _ReleaseWindowResources - get rid of any resources
 */
void _ReleaseWindowResources( LPWDATA w )
{
    DeleteObject( w->brush );
    if( w->hascursor ) {
        DestroyCaret();
    }
    DeleteMenu( _SubMenuWindows, MSG_WINDOWS+w->handles[0], MF_BYCOMMAND );

} /* _ReleaseWindowResources */
