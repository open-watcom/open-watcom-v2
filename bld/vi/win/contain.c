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
* Description:  MDI container window.
*
****************************************************************************/


#include "vi.h"
#include "window.h"

static char *className = "MDICLIENT";
WNDPROC oldContainerProc;

LONG WINEXP ContainerWindowProc( HWND, unsigned, UINT, LONG );



/*
 * CreateContainerWindow
 */
window_id CreateContainerWindow( RECT *size )
{
    HWND    container;
    CLIENTCREATESTRUCT client;

    client.hWindowMenu = (HMENU)NULLHANDLE;
    client.idFirstChild = 3000; // some arbitrary number that doesn't conflict

    container = CreateWindow( className, "Container",
                WS_CHILD | WS_CLIPCHILDREN | WS_HSCROLL | WS_VSCROLL | WS_VISIBLE,
                size->left, size->top,
                size->right - size->left, size->bottom - size->top,
                Root, (HMENU)NULLHANDLE, InstanceHandle, (LPVOID)&client );
    SetWindowLong( container, 0, 0 );
    oldContainerProc =(WNDPROC)GetWindowLong( container, GWL_WNDPROC );
    SetWindowLong( container, GWL_WNDPROC,
                   (LONG)MakeProcInstance( (FARPROC)ContainerWindowProc,
                                           InstanceHandle ) );
    SetScrollRange( container, SB_VERT, 1, 1, FALSE );
    SetScrollRange( container, SB_HORZ, 1, 1, FALSE );
    return( container );

} /* CreateContainerWindow */

/*
 * ContainerWindowProc - window procedure for container
 */
LONG WINEXP ContainerWindowProc( HWND hwnd, unsigned msg, UINT wparam, LONG lparam )
{
    switch( msg ) {
    case WM_KEYDOWN:
        return( SendMessage( Root, msg, wparam, lparam ) );
        break;
    }
    return( CallWindowProc( (WNDPROC)oldContainerProc, hwnd, msg, wparam, lparam ) );

} /* ContainerWindowProc */
