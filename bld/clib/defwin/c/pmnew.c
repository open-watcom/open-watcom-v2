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
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <limits.h>
#define INCL_GPI
#include "win.h"
#include "pmmenu.h"

#define DISPLAY(x)      WinMessageBox( HWND_DESKTOP, NULL, x, "Error", 0, MB_APPLMODAL | MB_NOICON | MB_OK | MB_MOVEABLE );

extern void _CreateFont( LPWDATA w );
static HWND menuHandle;
/*
 * _NewWindow - create a new window
 */
unsigned _NewWindow( char *name, ... )
{
    LPWDATA     w;
    MENUITEM    menus;
    HWND        hwnd,frame,temp;
    char        str[80];
    int         x1,x2,y1,y2;
    ULONG       style;
    RECTL       rcl;
    va_list     al;

    _GetWindowNameAndCoords( name, str, &x1, &x2, &y1, &y2 );

    style = FCF_TITLEBAR | FCF_SYSMENU | FCF_SIZEBORDER | FCF_MINMAX |
            FCF_VERTSCROLL;
    frame = WinCreateStdWindow( _MainWindow,
                WS_VISIBLE | WS_CLIPSIBLINGS,
                &style, _ClassName, str, 0, NULL, 0, &hwnd );
    if( frame == 0 ) return( FALSE );
    WinSetOwner( hwnd, _MainWindow );

    va_start( al, name );
    w = _AnotherWindowData( hwnd, al );
    w->frame = frame;
    w->text_color = CLR_WHITE;
    w->background_color = CLR_BLACK;
    WinSendMsg( frame, WM_SETICON,
        MPFROMLONG( WinQuerySysPointer( HWND_DESKTOP, SPTR_APPICON, TRUE ) ), 0 );
    WinSetWindowBits( WinWindowFromID( w->frame, FID_VERTSCROLL ), QWL_STYLE,
                        SBS_AUTOTRACK, SBS_AUTOTRACK );

    _CreateFont( w );
    _PositionScrollThumb( w );
    WinQueryWindowRect( _MainWindow, &rcl );
    WinSetWindowPos( frame, HWND_TOP,
                x1*w->xchar,
                (rcl.yTop - rcl.yBottom)-y1*w->ychar-y2*w->ychar,
                x2*w->xchar,
                y2*w->ychar,
                SWP_SIZE | SWP_MOVE | SWP_ZORDER );

    menus.iPosition = _MainWindowData->window_count - 1;
    menus.afStyle = MIS_TEXT;
    menus.afAttribute = 0;
    menus.id = DID_WIND_STDIO + w->handles[0];
    menus.hwndSubMenu = NULL;
    menus.hItem = 0;
    if ( MIT_ERROR == (BOOL)WinSendMsg( menuHandle, ( ULONG )MM_INSERTITEM, MPFROMP( &menus ), MPFROMP( str ) ) ) abort();
    temp = WinWindowFromID( frame, FID_SYSMENU );
    WinSendMsg( temp, MM_QUERYITEM, MPFROM2SHORT(SC_SYSMENU, TRUE),
                               MPFROMP((PSZ)&menus) );
    WinSendMsg( menus.hwndSubMenu, MM_DELETEITEM, MPFROM2SHORT( SC_CLOSE, TRUE ), 0 );
    WinUpdateWindow( hwnd );
    WinSetFocus( HWND_DESKTOP, hwnd );
    return( TRUE );

} /* _NewWindow */

/*
 * _ReleaseWindowResources - get rid of any resources
 */
void _ReleaseWindowResources( LPWDATA w )
{
        w = w;
} /* _ReleaseWindowResources */

/*
 * _SetWinMenuHandle - Sets the internal submenu handle.
 */
void _SetWinMenuHandle( HWND hmenu ) {

    menuHandle = hmenu;
}

HWND _GetWinMenuHandle( void ) {

    return( menuHandle );
}
