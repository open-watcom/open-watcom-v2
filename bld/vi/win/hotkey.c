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


#include "vi.h"
#include "subclass.h"
#include "hotkey.h"


/* Local Windows CALLBACK function prototypes */
WINEXPORT LRESULT CALLBACK HotkeyProc( HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam );

/*
 * HotkeyProc - pass hotkeys on to root for various modeless 'bars'
 */
WINEXPORT LRESULT CALLBACK HotkeyProc( HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam )
{
    vi_key      key;

    switch( msg ) {
    case WM_KEYDOWN:
        key = MapVirtualKeyToVIKey( wparam, HIWORD( lparam ) );
        if( key == VI_KEY( F7 ) ||
            key == VI_KEY( F8 ) ||
            key == VI_KEY( F9 ) ) {
            return( SendMessage( Root, msg, wparam, lparam ) );
        }
        break;
    }

    return( CallWindowProc( SubclassGenericFindOldProc( hwnd ), hwnd, msg, wparam, lparam ) );
}
