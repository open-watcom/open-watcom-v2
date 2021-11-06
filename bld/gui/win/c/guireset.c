/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2021 The Open Watcom Contributors. All Rights Reserved.
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


#include "guiwind.h"
#include "guimenus.h"
#include "guixutil.h"
#include "guihook.h"

#define NUM_SYSTEM_MENUS        9

static void FreeSystemMenu( gui_window *wnd )
{
    int             num_items;
    int             i;
    HMENU           hsysmenu;
    HWND            frame;

    frame = GUIGetParentFrameHWND( wnd );
    if( ( _wpi_getparent( frame ) != HWND_DESKTOP ) && (wnd->style & GUI_SYSTEM_MENU) ) {
        hsysmenu = _wpi_getsystemmenu( frame );
        if( hsysmenu != NULLHANDLE ) {
            num_items = _wpi_getmenuitemcount( hsysmenu );
            for( i = num_items; i >= NUM_SYSTEM_MENUS; i-- ) {
                _wpi_deletemenu( hsysmenu, i, TRUE );
            }
        }
    }
}

bool GUIAPI GUIResetMenus( gui_window *wnd, const gui_menu_items *menus )
{
    HMENU       hmenu;
    bool        success;
    HWND        parent;
    HWND        frame;

    GUIFiniHint( wnd, MENU_HINT );
    success = false;
    frame = GUIGetParentFrameHWND( wnd );
    parent = _wpi_getparent( frame );
    if( ( parent == HWND_DESKTOP ) || (wnd->style & GUI_POPUP) ) {
        if( menus->num_items > 0 ) {
            if( GUICreateMenus( wnd, menus, &hmenu ) ) {
                GUISetMenu( wnd, hmenu );
                _wpi_drawmenubar( frame );
                success = true;
            }
        } else {
            GUISetMenu( wnd, NULLHANDLE );
            GUIFreePopupList( wnd );
        }
    } else {
        FreeSystemMenu( wnd );
        success = GUIAddToSystemMenu( wnd, frame, menus, wnd->style );
    }
    if( success ) {
        GUIMDIResetMenus( wnd, wnd->parent, menus );
        GUIInitHint( wnd, menus, MENU_HINT );
    }
    return( success );
}
