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

WINEXPORT LRESULT CALLBACK ContainerWindowProc( HWND, UINT, WPARAM, LPARAM );


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
    SET_WNDINFO( container, 0 );
    oldContainerProc = (WNDPROC)GET_WNDPROC( container );
    SET_WNDPROC( container, (LONG_PTR)MakeProcInstance( (FARPROC)ContainerWindowProc, InstanceHandle ) );
    SetScrollRange( container, SB_VERT, 1, 1, FALSE );
    SetScrollRange( container, SB_HORZ, 1, 1, FALSE );
    return( container );

} /* CreateContainerWindow */

/*
 * ContainerWindowProc - window procedure for container
 */
WINEXPORT LRESULT CALLBACK ContainerWindowProc( HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam )
{
    switch( msg ) {
    case WM_KEYDOWN:
        return( SendMessage( Root, msg, wparam, lparam ) );
        break;
    }
    return( CallWindowProc( (WNDPROC)oldContainerProc, hwnd, msg, wparam, lparam ) );

} /* ContainerWindowProc */
